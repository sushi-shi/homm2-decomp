# `bool` crossing an inline boundary defeats the constant fold

**Trigger.** An inlined member call stores a compile-time constant into an
`i32`/`b32` member, and retail materialises the constant into a register
*before* reloading the inline `this` temp:

```
mov eax, 1
mov ecx, [ebp-<this temp>]
mov [ecx+<off>], eax
```

Every spelling that keeps the value an `int` folds the immediate into the
store instead, and reloads `this` first:

```
mov eax, [ebp-<this temp>]
mov [eax+<off>], 1
```

The register form costs one extra slot in VC6's `eax -> ecx -> edx` rotation,
so the whole rest of the function shifts by one register — which is how the
divergence usually announces itself (a `push ebx` in our prologue that retail
does not have).

## The measured divergence

`swapManager::Close` at RVA 0x000a2750. Retail (`build/delink/SOURCE/SWAPMGR.c.obj`)
writes the *same* member offset twice, and only the inlined one is unfolded:

```
53e: 8b 4d d8            mov  ecx, [ebp-0x28]         ; this (swapManager)
541: c7 41 32 00 00 00 00 mov dword ptr [ecx+0x32], 0 ; m_active = false  <- FOLDED
548: 8b 15 <gpAdvManager> mov edx, [gpAdvManager]
54e: 89 55 dc            mov  [ebp-0x24], edx         ; inline `this` temp
551: b8 01 00 00 00      mov  eax, 1                  ; <- value FIRST, in a register
556: 8b 4d dc            mov  ecx, [ebp-0x24]
559: 89 41 32            mov  dword ptr [ecx+0x32], eax
```

Ours, from `void Activate(void) { m_active = true; }`:

```
     8b 15 <gpAdvManager> mov edx, [gpAdvManager]
     89 55 dc            mov  [ebp-0x24], edx
     8b 45 dc            mov  eax, [ebp-0x24]
     c7 40 32 01 00 00 00 mov dword ptr [eax+0x32], 1
```

## What made it match

The value has to cross a function boundary **as a `bool`**. Adding a defaulted
`bool` parameter to the inline accessor does it, with the call site unchanged:

```cpp
// include/BASE/baseManager.h
void Activate(bool on = true) {
    m_active = on;          // m_active is b32
}
```

`gpAdvManager->Activate();` then compiles to retail's three instructions, the
frame loses nothing, and the `push ebx`/`pop ebx` pair disappears.

## Probe matrix (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT`)

Thirty-four spellings were compiled in four probe TUs. **Folded** (wrong):
`m_active = true`, `= 1`, `= 1u`, `= (b32)1`, `= !0`, `= (0,1)`, `= (1&&1)`,
`= (1==1)`, `= true?1:0`, `= (bool)true`, `= bool(1)`, `= kTrue` (file-scope
`static const int`), `= ONE_MACRO()`, `= enumerator`, `= *&kOne`, `= kArr[0]`,
`this->m_active = true`, `return m_active = true`, out-of-class `inline`,
32-bit bit-field member, `volatile` member, reference accessor
(`Active() = true`), pointer accessor (`*ActivePtr() = true`), an `int`-valued
parameter with a constant argument, an `int`-returning `inline` free function,
an `int`-returning static member, an `int`-returning member.

**Unfolded** (retail's shape): a `bool`-returning `inline` free function, a
`bool`-returning `static` member, a `bool`-returning member, a `bool`
parameter (explicit argument *or* default argument). A non-inlined `int`
function also produces it — the call clobbers the registers — but leaves a
real `call`.

So the discriminator is not "is it a call" and not "is it a constant": it is
the `bool -> int` conversion node that VC6's front end refuses to fold when
the value passes through a function's parameter or return slot.

## Cross-version note

PoL 2.0's `swapManager::Close` is byte-exact from `m_active = true;` under
MSVC 4.2 (its retail bytes carry the *folded* store at the same site), so this
is a real 2.0 -> 2.1 source edit, not a compiler difference. See
`docs/version-changes.md`.

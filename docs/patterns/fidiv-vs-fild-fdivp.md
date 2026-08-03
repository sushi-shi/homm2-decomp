# `fidiv` vs `fild`+`fdivp`: the divide's own type, not the cast spelling

**Trigger.** A float-valued expression `intA / intB`. VC6 `/Od` picks

- `fild A; fidiv B`      when the *division* is done in **float** precision, and
- `fild A; fild B; fdivp st(1),st` when the division is done in **double** precision.

The casts you write on the operands do not decide it; the type of the division does.

Measured on `advManager::DoVisions` (RVA 0x154ac).

## The divergence

```
retail                                        ours
--------------------------------------------- ---------------------------------------------
89 85 fc fe ff ff  mov  [ebp-0x104], eax      89 85 fc fe ff ff  mov  [ebp-0x104], eax
db 85 fc fe ff ff  fild dword [ebp-0x104]     db 85 fc fe ff ff  fild dword [ebp-0x104]
...                                           ...
89 8d f8 fe ff ff  mov  [ebp-0x108], ecx      89 8d f8 fe ff ff  mov  [ebp-0x108], ecx
db 85 f8 fe ff ff  fild dword [ebp-0x108]     da b5 f8 fe ff ff  fidiv dword [ebp-0x108]
de f9              fdivp st(1), st
d9 5d e8           fstp dword [ebp-0x18]      d9 9d 28 ff ff ff  fstp dword [ebp-0xd8]
```

Same operands, same temp slots, one instruction apart.

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

```cpp
float p1(void){ int a=gA(); return static_cast<float>(a)  / static_cast<float>(gCnt*gFv[1]); }
float q7(void){ int a=gA(); return static_cast<double>(a) / (gCnt*gFv[1]);                   }
```

```
p1:  db 45 fc  fild [ebp-4]   ...  da 75 f8  fidiv [ebp-8]          <- float divide
q7:  db 45 fc  fild [ebp-4]   ...  db 45 f8  fild  [ebp-8]
                                   de f9     fdivrp st, st(1)       <- double divide
```

Every `float`-typed arm (`(float)a/(float)b`, `(float)a/b`, `a/(float)b`) produced
`fidiv`; every `double`-typed arm (`(double)a/(double)b`, `(double)a/b`, `a/(double)b`,
`1.0*a/b`) produced `fild`+`fdivp`. Precision of the *divide*, nothing else.

## What made it match

```cpp
    fRatio = static_cast<float>(gpPhilAI->FightValueOfStack(...))     // ours, fidiv
             / static_cast<float>(gMonsterDatabase[IDX(type)].fightValue * count);
```
```cpp
    fRatio = static_cast<double>(gpPhilAI->FightValueOfStack(...))    // retail, fild+fdivp
             / static_cast<double>(count * gMonsterDatabase[IDX(type)].fightValue);
```

`fRatio` stays a `float`; only the division is evaluated in double, so the store is
still `fstp dword`. (The operand swap in the same line is the separate
`inline-call-operand-index-first` axis: `/Od` lowers `a * b` as `mov reg,a; imul reg,b`,
so the retail order of the two loads names the source order.)

## Independent confirmation (SOURCE/EVENTS)

The same rule was re-derived from scratch on `advManager::PlayerMonsterInteract`
(0x47fff) and `advManager::ComputerMonsterInteract` (0x48730), which carry the
same army-strength ratio.

```
ours   (float)/(float)                     retail
------------------------------------       ------------------------------------
db 85 b0 fe ff ff  fildl  -0x150(%ebp)     db 85 b0 fe ff ff  fildl  -0x150(%ebp)
...                                        ...
89 85 ac fe ff ff  movl %eax,-0x154(%ebp)  89 85 ac fe ff ff  movl %eax,-0x154(%ebp)
da b5 ac fe ff ff  fidivl -0x154(%ebp)     db 85 ac fe ff ff  fildl  -0x154(%ebp)
                                           de f9              fdivrp %st, %st(1)
d9 5d f8           fstps  -0x8(%ebp)       d9 5d f0           fstps  -0x10(%ebp)
```

A 13-arm `batch_source_variants` matrix (`ratio_expr`) separates the two classes
cleanly: the four `double` arms all land on size 1841, which is retail's size
(99.810814%), and the four `float` arms on 1839 (99.522520%). Which operand
carries the cast does not matter, only the type of the division.

**Cross-version note.** PoL 2.0's retail bytes at the same site are also
`fild; fild; fdivrp`, but the 2.0 reconstruction reaches them from
`(float)a / (float)b` under MSVC 4.2, which does not perform the `fidiv` fold at
all. The spelling divergence is therefore a compiler-version artifact rather
than a source difference, and stays OPEN in `docs/cross-version-spellings.md`
until the 2.0 tree re-tests the `double` spelling.

## Third confirmation, and the sharper statement of the rule (SOURCE/GAME)

`game::SetupPuzzlePieces` (0x4cb1e) hit it again, and a 9-arm matrix over the
*divisor* spelling pins down which cast actually matters:

| divisor spelling            | body size | lowering |
| :--                         | --:       | :--      |
| `int` (no cast)             | 539       | `fidiv`  |
| `static_cast<float>(int)`   | 539       | `fidiv`  |
| `static_cast<i32>(int)`     | 539       | `fidiv`  |
| `static_cast<double>(int)`  | 541       | `fild` + `fdivp` — retail |

So an explicit `static_cast<float>` on the operand does **not** stop the fold:
the cast is absorbed into the same coercion node, and `/Od` still folds the
integer memory operand into `fidiv`. Only raising the *division* to `double`
puts the divisor on the x87 stack first. Consistent with the arms measured in
EVENTS, where `static_cast<double>(a) / b` also lands on the retail form — it
is the common type of the division that decides, not which side carries the
cast.

Narrowing back on assignment is unaffected: the store stays a non-popping
`d9 55 fc  fst dword ptr [ebp-4]`.

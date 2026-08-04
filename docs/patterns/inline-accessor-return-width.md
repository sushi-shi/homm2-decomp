# inline-accessor-return-width

**Symptom.** A one-byte member is read through an in-class accessor. Retail
widens it with a single `movsx r32, byte [reg+off]`; ours stores the byte into
a frame slot first and widens out of that slot. The extra slot pushes every
deeper local down, so the whole frame is one dword too big and a large,
otherwise-perfect function reads as a mess of wrong `-0xN(%ebp)`.

**Rule (VC6 SP5 `/Od /Ob1`, byte-proven).** `/Ob1` expands the accessor body in
place and materialises a temporary of the accessor's *declared return type*:

| accessor returns | emitted at the call site |
|---|---|
| `i8` | `mov r8,[..]` + `mov [ebp-N],r8` + `movsx r32,[ebp-N]` — a frame slot |
| `i32` | `movsx r32,[..]` — **no** slot, byte-identical to a direct member read |
| n/a (direct member read into a `short` context) | `66 0f be` (`movsbw`) |

So `movsx` of a one-byte member with NO frame temp means the accessor returns
`i32`. A byte temp means it returns `i8`. A `movsbw` means the site read the
member directly and the target is narrower than `int`.

Casting at the call site does not help: `static_cast<i32>(p->Acc())` still
materialises the `i8` temp first, because the cast applies to the already-typed
inline result.

## The divergence, measured

`playerData::CurrentTown()` returning `m_currentTown` (offset 0x45), read at
`advManager::Main` (RVA 0x27cf), the `INPUT_SCAN_T` hot-key arm:

```
ours: i8 CurrentTown(void)                       retail
------------------------------------------------ ------------------------------------------------
8b 15 <gpCurPlayer>  movl  gpCurPlayer, %edx     8b 15 <gpCurPlayer>  movl  gpCurPlayer, %edx
8a 42 45             movb  0x45(%edx), %al       0f be 42 45          movsbl 0x45(%edx), %eax
88 45 c8             movb  %al, -0x38(%ebp)      83 f8 ff             cmpl  $-0x1, %eax
0f be 4d c8          movsbl -0x38(%ebp), %ecx    75 0f                jne   ...
83 f9 ff             cmpl  $-0x1, %ecx
75 0f                jne   ...
```

```
ours: i32 CurrentTown(void)   ==  retail, byte for byte
------------------------------------------------
8b 15 <gpCurPlayer>  movl  gpCurPlayer, %edx
0f be 42 45          movsbl 0x45(%edx), %eax
83 f8 ff             cmpl  $-0x1, %eax
```

The same holds for an accessor **with a parameter** — `/Ob1` does not spill the
parameter either, so an `i32` accessor with an index argument is still
byte-identical to the subscript written out. `playerData::TownId(i32 index)`
reading `m_townIds[index]` (base 0x47) in the same loop:

```
ours: i8 TownId(i32)                             ours: i32 TownId(i32)  == retail
------------------------------------------------ ------------------------------------------------
8b 0d <gpCurPlayer>  movl gpCurPlayer, %ecx      a1 <gpCurPlayer>     movl gpCurPlayer, %eax
03 4d e8             addl -0x18(%ebp), %ecx      03 45 e8             addl -0x18(%ebp), %eax
8a 51 47             movb 0x47(%ecx), %dl        0f be 48 47          movsbl 0x47(%eax), %ecx
88 55 c4             movb %dl, -0x3c(%ebp)
0f be 45 c4          movsbl -0x3c(%ebp), %eax
```

## Census recipe

The `i8`-accessor spill is a fixed three-instruction idiom, so it can be counted
across the whole tree rather than hunted per function: scan both objects for
`movb %r8, -0xN(%ebp)` immediately followed by `mov[sz]bl -0xN(%ebp), %r32`
with the SAME displacement, and compare the per-unit counts. Retail's own count
is not zero — it has genuine `char` locals and `char` parameters, and
`hero::GetSSLevel` really does return `char` — so only the *delta* is signal.
Before this sweep `playerData`'s narrow accessors accounted for 7 spurious
spills (ADVMGR 5, Castle 1, HERO 1); after widening them the delta is zero.

## Which sites are decided by this, and which are not

Once an accessor returns `i32` its expansion is byte-identical to the member
read, so **bytes can no longer distinguish `p->Acc()` from `p->m_field`** at an
`i32`-valued site. The two remaining discriminators are:

- a narrower target (a `short` parameter): the member read gives `movsbw`, the
  accessor gives `movsbl` + `push` — see the closing note in
  [narrow-inline-accessor-spill](narrow-inline-accessor-spill.md);
- **operand evaluation order**: an inlined call on one side of a binary
  operator is evaluated *last* — see
  [inline-call-operand-index-first](inline-call-operand-index-first.md).

## Closes

`playerData::CurrentTown/Color/TownId/AvailableHeroId` widened `i8` -> `i32`;
`advManager::Main` 82.39 -> 83.32, `advManager::ProcessSelect` 78.41 -> 78.72,
`SetupHeroView` 94.86 -> 95.30, `townManager::SetupCastle` 97.16 -> 98.21 (then
exact), `CastleHandler` 22.34 -> 25.53. `playerData::CurrentHero` (i8 -> i32)
and `game::GetPlayerColor` (i8 -> i32) are the earlier witnesses.

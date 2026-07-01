# tu-cumulative-eval-order — a residual that OTHER functions fix (early-stop signal)

> **This is a "reverse pattern."** The rest of `docs/patterns/*` are *fix-it* patterns
> (symptom → the source spelling that matches). This one is the inverse: a symptom that
> is **NOT steerable from this function's source**, so the move is to **`@early-stop` it**
> and let it resolve as *sibling* functions in the same TU get reconstructed.
> Documenting it tells us **when to stop grinding a diff.**

## Fingerprint

Logic byte-exact and **frame slots byte-exact** (verify with `(%ebp)` VISIBLE — see
`od-hash-slots.md`); objdiff ~90–99%. The ONLY residual is **which of two commutative
operands `/Od` materializes into a register first.** Real example — `game::WriteDiffHeaderInfo`
at the third `flags |= (word >> 16)` merge (objdiff 94%):

```
;                          RETAIL — accumulator-first (reg,reg OR)
  33 c0              xorl  %eax, %eax
  8a 45 fc           movb  -0x4(%ebp), %al      ; load flags(-0x4) into AL FIRST
  8b 4d ec           movl  -0x14(%ebp), %ecx    ; THEN compute the RHS in ECX
  81 e1 00 00 2f 00  andl  $0x2f0000, %ecx
  c1 e9 10           shrl  $0x10, %ecx
  0a c1              orb   %cl, %al              ; OR reg,reg
  88 45 fc           movb  %al, -0x4(%ebp)

;                          OURS — value-first (reg,mem OR)   ← the whole diff
  8b 45 ec           movl  -0x14(%ebp), %eax    ; compute the RHS in EAX FIRST
  25 00 00 2f 00     andl  $0x2f0000, %eax
  c1 e8 10           shrl  $0x10, %eax
  0a 45 fc           orb   -0x4(%ebp), %al      ; OR reg,mem (folds the flags load)
  88 45 fc           movb  %al, -0x4(%ebp)
```

Same C (`flags |= (v & 0x2f0000) >> 16`), same result; retail spills the accumulator to
a register before computing the RHS, we fold the flag load into the OR's memory operand.
Note the *first two* `flags |=` in the same function match both ways — only this third
one (RHS is a multi-instruction value) diverges. Other guises of the same class: a
loop-test operand (`mov fv;cmp bestFV` vs `mov bestFV;cmp fv`), a `y*WIDTH` term, an
FPU store-vs-compare schedule (`fcoms;fstps` vs `fsts;fcomps`).

## Root cause

`/Od` operand selection carries a **TU-global parity**: the register-vs-memory / load-order
choice for a commutative operand depends on cumulative compiler state that shifts as
**other functions in the same compiland are added**. So the SAME source that mismatches
inside a *partial* TU compiles byte-exact **standalone** and byte-exact once the TU is
**fuller**.

## What made it match in the end

**Non-local (this pattern's core) — a sibling function landing.** Measured this session,
source of the parked function UNCHANGED, flipped only by reconstructing other functions in
the TU:

| function | before | after | what changed |
| :--- | :--- | :--- | :--- |
| `game::GetNumThievesGuilds` | 92% (GAME batch 2) | **100%** | 18 sibling GAME funcs bodied in batch 3 |
| `philAI::SetupRelativeHeroStrengths` | 98.9% (PHILAI b2) | **100%** | PHILAI b3 siblings bodied |
| `game::ExperienceValueOfStack` | 96% | 99% | partial parity shift (still climbing) |

So the "fix" is **not in this function** — park it and re-check after the TU fills.

**Local escape hatch — ONE variant IS fixable: the SIB array index.** When the diff is an
array subscript's base-vs-index load order, the **commuted subscript** `i[(T*)p]` (instead
of `((T*)p)[i]`) forces the index operand first and yields retail's fused address. Retail
target in `ExperienceValueOfStack`:

```
  0f be 04 08        movsbl (%eax,%ecx), %eax     ; base+index, index materialized first
```

`((signed char*)group)[i]` emitted the base first (mismatch); `i[(signed char*)group]`
matched. Try this before parking a SIB-index diff. (The `field = val | 0` trick can
similarly force a value-first bitfield write — see GAME `RandomizeBarrier`.) Everything
else in the class — loop-test operand, flag-OR, FPU schedule — is **not** source-steerable.

## Confirm-then-park checklist

1. `(%ebp)`-visible diff → every local on the retail slot (rule out a slot-hash miss).
2. The only diff is a commutative operand's order, and it survives every
   `<`/`>`/`<=`/`else`/`continue`/extra-temp/reversed-compare/`k*i` spelling.
3. (If cheap) the function compiles byte-exact **standalone** — proves the source is right.

All three ⇒ tu-cumulative. Mark it a **soft / revisit** early-stop (distinct from a
permanent delinker/reloc artifact — expect 100% for free later, then drop the marker):

    // @early-stop
    // ~9x%: logic + frame slots byte-exact; residual is tu-cumulative /Od operand-load
    // order (docs/patterns/tu-cumulative-eval-order.md) — not source-steerable, resolves
    // as the rest of <TU> is reconstructed. Matched 100% standalone.
    VA(0x........, 0x..)

## Still parked awaiting a fuller TU (re-check these)

`game::{MakeAllWaterVisible, SetupAdjacentMons, CountShrines, WriteDiffHeaderInfo, RestoreCell}`,
`philAI::{DetermineHeroToMove, ValueOfTown, DoDimensionDoor}`.

Related: `od-hash-slots.md` (prove slots first), `od-debug-build.md`, `inline-accessors.md`.

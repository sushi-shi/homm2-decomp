# TU-cumulative evaluation order — diagnose, steer, then defer only if proved

> A cumulative-state fingerprint is a diagnosis, not immediate permission to park a
> function. First reproduce the combined root/header state, process predecessors in
> source order, simplify the affected source, and try audited exact-preserving predecessor
> variants. See [o2-tu-cumulative-register-steering.md](o2-tu-cumulative-register-steering.md)
> for the byte-exact BITMAP case and the required rejection criteria.

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

## What made historical cases match

**Non-local (this pattern's core) — a sibling function landing.** Measured this session,
source of the parked function UNCHANGED, flipped only by reconstructing other functions in
the TU:

| function | before | after | what changed |
| :--- | :--- | :--- | :--- |
| `game::GetNumThievesGuilds` | 92% (GAME batch 2) | **100%** | 18 sibling GAME funcs bodied in batch 3 |
| `philAI::SetupRelativeHeroStrengths` | 98.9% (PHILAI b2) | **100%** | PHILAI b3 siblings bodied |
| `game::ExperienceValueOfStack` | 96% | 99% | partial parity shift (still climbing) |

These cases prove cumulative state exists; they do not prove a new residual is locally
unsteerable. Rebuild after the TU fills, then perform the steering sequence below.

**Local escape hatch — ONE variant IS fixable: the SIB array index.** When the diff is an
array subscript's base-vs-index load order, the **commuted subscript** `i[(T*)p]` (instead
of `((T*)p)[i]`) forces the index operand first and yields retail's fused address. Retail
target in `ExperienceValueOfStack`:

```
  0f be 04 08        movsbl (%eax,%ecx), %eax     ; base+index, index materialized first
```

`((signed char*)group)[i]` emitted the base first (mismatch); `i[(signed char*)group]`
matched. Try this during local steering. (The `field = val | 0` trick can similarly force
a value-first bitfield write — see GAME `RandomizeBarrier`.) Loop tests, flag ORs, and FPU
schedules may also move when exact predecessors or header state changes; do not classify
them as walls before a combined-root source-order audit.

## Confirm, steer, then optionally soft-defer

1. Reproduce the integrated root/header state; a stale worker score is not evidence.
2. Process all predecessors in source order and prove their raw bytes and relocations.
3. For `/Od`, expose every `(%ebp)` displacement and rule out a slot-hash miss.
4. Try audited local expression, alias, temporary, condition, and CFG spellings.
5. At 96-97% or better, use only the libclang AST permuter and audit every retained
   mutation; never use the regex permuter.
6. Try exact-preserving AST variants in predecessors. Reject any variant that changes a
   predecessor byte or relocation, and retest every candidate on the combined root.
7. If the residual still consists solely of operand/register order, document all of the
   above and use a **soft revisit**, not a permanent compiler-wall claim:

    // @early-stop
    // ~9x%: logic + frame slots byte-exact; residual is TU-cumulative operand order.
    // Combined-root local and exact-predecessor steering were exhausted; revisit after
    // later <TU> source/header changes. Matched 100% standalone.
    VA(0x........, 0x..)

## Soft-deferred cases to re-check after each material TU change

`game::{MakeAllWaterVisible, SetupAdjacentMons, CountShrines, WriteDiffHeaderInfo, RestoreCell}`,
`philAI::{DetermineHeroToMove, ValueOfTown, DoDimensionDoor}`.

Related: [o2-tu-cumulative-register-steering.md](o2-tu-cumulative-register-steering.md),
`od-hash-slots.md` (prove slots first), `od-debug-build.md`, `inline-accessors.md`.

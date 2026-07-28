# /Od goto stubs, dead structural jmps, and inline continuations

**tags:** `topic:od` `toolchain:vc42` `cpp:goto` `cpp:inline`

The 2026-07-28 sweep of "retail has extra `jmp`" residuals closed ten functions
(ClaimMine, ClaimTown, RemoteCleanup, TransmitAndWait, UpdateHeroLocator,
UpdBottomView, SetupTown, ShowMoraleInfo, recruitUnit::Main, CalcBaseScore) with
five byte-proven producers. None of these are reachable by TU-state islands —
they are statement-level source facts.

## 1. Forward `goto` owns an end-of-function stub

`goto L;` where `L:` is later in the function emits `jmp STUB`, and the stub —
`jmp L` — is appended after the function body's final `jmp` to the epilogue.
Stubs accumulate in reverse source order (first goto → last stub). A `return`
does NOT stub: mid-function `return` jumps straight to the epilogue
(RemoteCleanup's guards), so retail guards that land past the end-of-body
`jmp $+0` are early returns, and guards that hop through a stub are gotos
(TransmitAndWait's transmit guard).

## 2. The goto's dead structural jmps

- `if (c) goto L;` (guard form) emits `[jmp STUB][jmp L]` — the second jmp is
  dead but always present (TransmitAndWait, ShowMoraleInfo).
- `if (c) { ...; goto L; } else X;` emits `[then][jmp STUB][jmp L][jmp after-else]`
  — stub, dead label jmp, dead skip-else (CalcBaseScore's three tiers,
  ComputerMonsterInteract's flee/fight pair). Dropping the `else` drops exactly
  one dead jmp; a braced `{ goto L; }` without else drops two.

Count the trailing jmps at a block end and their targets to read the construct
back: `N same-target trailing jmps` distinguishes goto+else / goto / break.

## 3. `default: break;` emits its own case-exit jmp

A switch whose case bodies each end in `break` emits one `jmp after-switch` per
case; an explicit `default: break;` adds one more identical jmp at the end of
the case-body run (RemoteCleanup's driver switch, recruitUnit::Main's command
dispatch). One extra jmp-to-after-switch in retail = the devs wrote the default.

## 4. /Ob1 inline continuations are position-sensitive

Every expanded inline call site emits a continuation `jmp $+0` (see
`inline-accessors.md`), and the placement depends on the call's syntactic slot:

- value-returning accessor anywhere in a statement (argument, condition,
  assignment RHS): the continuation lands at the STATEMENT START — visually a
  bare `jmp $+0` right after the previous statement, or the branch-entry jmp
  when the statement opens a then/else block (ClaimMine's else entry,
  UpdBottomView's else-if condition, SetupTown's if condition).
- void out-param accessor (`Cell(cell, x, y)`): body then continuation — but
  argument expressions materialize into temps, so it only matches sites whose
  bytes show the temps.
- empty body: the call survives as exactly one bare `jmp $+0`
  (`include/BASE/DebugCheck.h`; UpdateHeroLocator).

So a retail-only bare `jmp $+0` before a statement usually means that statement
reads a member DIRECTLY in our source where retail went through an in-class
accessor (`GetPlayerColor`, `GetHero`, `CurrentHero`, `Color`,
`town::OccupyingHero`, ...). Spell the read through the accessor; the
substitution is byte-exact for simple bodies.

## 5. Inlined callee names are slot-visible

Renaming an inline callee (or moving its declaration between a TU-local
definition and a header) can permute the CALLER's /Od local slots even though
`od_slots.py` predicts layout from local names alone — the callee's identifier
participates in the scope state. When adding a hook or accessor flips a
function's slot pairing, fix it with local renames as usual; and A/B any new
header declaration before trusting cached deltas (msvc42-tu-declaration-state).

## 6. Condition-spelling truth table (2026-07-29 sweep)

The jcc mnemonic and immediate pin the source spelling exactly; the sweep closed
or branch-cleaned a dozen more functions on these:

- `cmp x, -1` + `je`  vs retail `cmp x, 0` + `jl`: retail tests the SIGN, not the
  sentinel — spell `>= 0`, not `!= COMBAT_SIDE_NONE` / `!= PURCHASE_NONE`
  (DoHydraAttack, DoAttack, CheckBuyStuff).
- `test x, x` + `jle` vs retail `cmp x, 1` + `jl`: spell `>= 1`, not `> 0`
  (InitCampaignMap).
- `je` vs `jne` with the same cmp and SWAPPED arms: the if/else is written with
  the other polarity — retail often writes the DISABLE/muted/hidden arm first
  (UpdateSystemOptions, SetupHeroView, RemoteMain's transport chain,
  CampaignHandler's `m_active == 1`).
- inc + `cmp x, COUNT; jne / mov 0` wrap vs retail idiv: spell
  `x = (x + 1) % COUNT` (CheckLevel).
- A loop guard whose false path jumps to a LOCAL trampoline right before the
  body is a `continue` statement (`if (...) continue;`); one that jumps to the
  far after-body is the positive wrap `if (...) { body }`. Retail uses both —
  read the target (SetupAndLoadObstacles' three continues under one nested
  terrain gate; ConvertObject's bounds continue; EffectSpellCure's positive
  wrap where we had a continue). The trailing-jmp count at the previous block
  end distinguishes them before you compile.

One exception is now probe-proven: an INLINE ACCESSOR on one side of a
relational PINS the canonical cmp direction across TU states, and which side is
inlined selects the jcc (right-side accessor -> jge form, left-side -> jle
form; 8-state probe, 2026-07-29). The expansion usually costs the statement's
continuation `jmp $+0`, so it closes a mirror site only when retail also shows
that jmp - but the jmp is context-dependent (ClaimTown's GetCell emitted none),
so a wall site with a natural accessor is worth one compile either way.

Otherwise, none of the at-state mirrors move for any spelling: relational operand order,
global-vs-local cmp direction, int add/or term order, and imul factorization
all canonicalize per TU state (A/B: tradpost byte-identical under operand
swap). Read the cmp reloc/slot ORDER first — if both sides load different
operands first it can still be the canonicalizer, not the source.

## 7. Multi-dimensional subscript term order is TU state, not spelling

`&m_armies[side][index]` can emit the SIDE term first or the INDEX term first.
A clean-TU probe gives side-first for every natural spelling — plain 2D
subscript, row pointer + index, explicit row local, and with or without the
`IDX()` conversion (which is a constexpr call the compiler folds away, proven by
an unchanged score when it is replaced by direct casts). Only hoisting the side
INDEX into a local flips the probe to index-first. In a large TU the order
follows cumulative state instead (GetCommand, HandleAppSpecificMenuCommands, and
CycleCombatScreen all show index-first against retail's side-first with correct
source). Treat it as at-state (tu-cumulative-eval-order.md), not as a bug to
respell.

## Diagnosis workflow

`homm2 sema disasm <rva> --diff` shows the inserted `+jmp` rows;
`--branches --diff` proves the control flow otherwise agrees; then read the
extra jmps' TARGETS in raw retail disasm (`homm2 sema disasm <rva>`): same
target repeated = goto/dead/skip cluster or default:break, `$+0` = inline
continuation, hop-through-stub = forward goto.

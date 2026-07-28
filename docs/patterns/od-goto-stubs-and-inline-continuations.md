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

## Diagnosis workflow

`homm2 sema disasm <rva> --diff` shows the inserted `+jmp` rows;
`--branches --diff` proves the control flow otherwise agrees; then read the
extra jmps' TARGETS in raw retail disasm (`homm2 sema disasm <rva>`): same
target repeated = goto/dead/skip cluster or default:break, `$+0` = inline
continuation, hop-through-stub = forward goto.

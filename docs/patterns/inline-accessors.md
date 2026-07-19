# Inlined accessors & the `jmp $+0` fingerprint (`/Ob1`)

**tags:** `topic:codegen` `cpp:inline` `toolchain:vc42` `flag:Ob1`

## symptom
- The retail function is littered with **`jmp $+0`** — `e9 00 00 00 00`, a near
  jump whose target is the very next instruction (a no-op jump). They cluster at
  function entry, after stores, and sometimes **two in a row**.
- Your reconstruction has the right logic and even byte-matches large stretches,
  but plateaus (~85–92%) and the diff is almost entirely **`jmp` lines retail has
  that you don't** (plus a trailing alignment `nop`).
- Plain `/Od` (`/Ob0`) **never** emits `jmp $+0`; `/G5`/`/GB`/`/Z7` don't change it.

## cause
The retail build is **`/Od /Ob1`**: unoptimized *but with inline expansion on*.
`/Ob` is a **separate axis** from `/Od`↔`/O2`. Each inlined callee's `return`
becomes a jump to its continuation point; at `/Od` that lands as `jmp $+0`. So a
cluster of `jmp $+0` around repeated field/array access means the original wrote
**inline accessor methods** and `/Ob1` spliced them into every call site. Fully
inlined → the accessor has no out-of-line copy → **CodeView lists no such method**
(consistent with, not contradicted by, the recovered class).

Proof on EDITOR/mapcell: a probe `caller()` calling two in-class accessors twice
each emits `6 call / 1 jmp$+0` at `/Ob0` vs `2 call / 5 jmp$+0` at `/Ob1` — inlining
trades 4 calls for 4 `jmp $+0`, exactly the retail shape.

## fix
1. Build with `/Ob1` (now global in `config/units.toml`). `/Ob1` not `/Ob2`: retail
   still emits real `call`s to out-of-line methods (e.g. `GetNewCellExtraIndex`),
   which `/Ob1` leaves alone; `/Ob2` would auto-inline more.
2. **Reconstruct the inline accessor instead of hand-inlining the expression.** Raw
   `(cells + width*y)[x].extra` byte-matches a subset but can never reach 100% — it
   lacks the per-call `jmp $+0`. An inline `mapCell *Row(int y){ return cells+width*y; }`
   used as `Row(y)[x].extra` restores them.
3. **Match the accessor's return shape to the addressing.** A *row-pointer* accessor
   (`return cells + width*y`, caller does `[x]`) keeps the row base in a register and
   **defers `[x]` to the load** — `0xa(%eax,%ecx,4)`, what retail emits. A *cell*
   accessor that returns `(cells+width*y)+x` (pointer) or even `(cells+width*y)[x]`
   (reference) lets the compiler reassociate to `width*y*12 + x*12` then `+cells`,
   giving the wrong `0xa(%eax,%ecx)` (no scale). Pick the form that reproduces the
   retail addressing mode, not just the right value.

## counting & placing the jumps (this is NOT a wall)
The `jmp $+0` are understood, not mysterious — use them as a checksum:
- **Count first.** Roughly **one `jmp $+0` per inlined call site** in a region (plus
  one per branch landing-pad: function entry, `jne`-target). If your count is SHORT,
  you're missing an inline call (you hand-inlined it, or skipped an accessor) — add
  it. `llvm-objdump -d <obj> | grep -c "e9 00 00 00 00"` both sides.
- **Don't be fooled by the alignment `nop`s.** The delinked target carries trailing
  `nop` padding (`90`); objdiff IGNORES it for scoring (100%-matched functions have
  them too), so it is never the blocker — don't chase it.
- **Then placement.** Once counts match, the only residual is the LEADING-vs-TRAILING
  position of a given inline bracket (e.g. a `cell = &Row(y)[x]` assignment brackets
  *after* the store in retail but *before* the body in our build). This is an opaque
  C2.EXE block-layout choice; it resisted every accessor/statement-shape variant
  tried. If that is genuinely all that is left, keep it as a live residual and record an ordinary
  comment only when the accessor-continuation fact helps explain the source. Verify with
  displacements-on diffing first (see od-hash-slots.md): a wrong
  slot can masquerade as "just jmps" behind the fuzzy.

On mapcell this lifted `GetNewCellExtra{Object,Overlay}` from a structurally-capped
~91% to ~97% (addressing exact, jmp$+0 count 10/10, frame slots fixed via od_slots);
the only residual is ~1 inline-bracket placement. `/Ob1` alone lifted the untouched
`ChangeTilesetIndex` 96→99%.

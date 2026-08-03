# VC6 SP5 /Od pattern catalog (Buka line)

Byte-proven codegen idioms for this branch's toolchain. Every entry carries
the real asm of the divergence and the exact source spelling that closed it.
Founded 2026-08-02; nothing here is ported from the PoL VC4.2 catalog.

| pattern | one-line trigger |
| :-- | :-- |
| [user-dtor-out-of-line](user-dtor-out-of-line.md) | delete-site calls `??1X` instead of inlining it → X has a user-declared dtor defined out of line |
| [member-static-guarded-teardown](member-static-guarded-teardown.md) | atexit teardown with shared flag byte + bit per object → statics are class members, not file statics |
| [while1-literal-test](while1-literal-test.md) | `mov ecx,1; test ecx,ecx; je end` at loop head → source spells `while (1)`, not `for (;;)` |
| [loop-exit-in-header-chain](loop-exit-in-header-chain.md) | direct jcc to loop exit (no jmp stub) → break condition lives in the loop header chain |
| [od-slot-solve](od-slot-solve.md) | byte-identical except swapped ebp displacements → solve local-name buckets, never grind |
| [if-else-two-jmp-backedge](if-else-two-jmp-backedge.md) | jmp-to-jmp at a loop back edge → if/else inside the loop, not `continue` |
- [inline-call-operand-index-first](inline-call-operand-index-first.md) - ptr-side-first vs inline-accessor index-first; zero-arm ternary sbb
- [od-constant-ternary-lowering](od-constant-ternary-lowering.md) - `setcc/dec/and/add` select: `dec` (not `sub reg,1`) means a real `?:`, and the source condition is the INVERSE of the emitted setcc
- [two-backedge-jmps-in-one-loop](two-backedge-jmps-in-one-loop.md) - two separate `jmp <increment>` blocks in one loop body → an explicit `continue` guard plus the natural body end, never one `&&` chain
- [paren-suppresses-fp-commute](paren-suppresses-fp-commute.md) - `fld`/`fmul` on the wrong float operands and no source order helps → parenthesise the indexed operand

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

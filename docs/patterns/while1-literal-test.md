# while1-literal-test

**Symptom.** Retail loop head materializes a constant test:

    b9 01000000   mov ecx, 1
    85 c9         test ecx, ecx
    0f84 <exit>   je <loop exit>      ; dead branch, always falls through

`for (;;)` under VC6 /Od emits NO head test at all — block counts then
disagree by one and the skeleton diverges at the loop entry.

**Close.** Spell the loop `while (1)`. Proven in font::ExtractLine at
0x4c3b20 (flow SAME, 217/217 blocks after the change). Commit e4852e58.

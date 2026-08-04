# while1-literal-test

**Symptom.** Retail loop head materializes a constant test:

    b9 01000000   mov ecx, 1
    85 c9         test ecx, ecx
    0f84 <exit>   je <loop exit>      ; dead branch, always falls through

`for (;;)` under VC6 /Od emits NO head test at all — block counts then
disagree by one and the skeleton diverges at the loop entry.

**Close.** Spell the loop `while (1)`. Proven in font::ExtractLine at
0x4c3b20 (flow SAME, 217/217 blocks after the change). Commit e4852e58.

## Converse (proven on this branch)

The rule reads in both directions. `game::GetRandomArtifactId` (RVA 0x5a9e8)
has **no** head test at all — the back edge jumps straight at the first body
instruction:

```
retail                                   ours (`while (1)`)
---------------------------------------  ---------------------------------------
                                         b8 01 00 00 00  movl  $1, %eax
                                         85 c0           testl %eax, %eax
                                         0f 84 ....      je    <exit>
0f be 05 <xIsExpansionMap> movsbl ...    0f be 0d <xIsExpansionMap> movsbl ...
```

Spelling the loop `for (;;)` removed the three-instruction head and closed the
function (87.14% -> EXACT). Read the retail loop head first; do not assume
either spelling.

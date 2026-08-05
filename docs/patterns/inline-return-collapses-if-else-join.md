# Arms that jump STRAIGHT to the join are inlined `return`s, not a nested if/else

**Symptom.** A constant-select chain is byte-identical to retail except for the
displacement of ONE `jmp`: ours jumps to the next `jmp` (a jump-to-jump), retail
jumps directly to the join.

`?PlayMilesSample@@YIXPAVsample@@@Z` (RVA 0xcddf0), the DIG_F format select:

```
retail                                  ours (nested if/else)
--------------------------------------- ---------------------------------------
c7 45 ec 03 00 00 00  movl $3,-0x14      c7 45 ec 03 00 00 00  movl $3,-0x14
eb 22                 jmp <JOIN>         eb 07                 jmp <the eb 19>
c7 45 ec 01 00 00 00  movl $1,-0x14      c7 45 ec 01 00 00 00  movl $1,-0x14
eb 19                 jmp <JOIN>         eb 19                 jmp <JOIN>
8b 45 fc              movl -0x4,%eax     8b 45 fc              movl -0x4,%eax
83 78 14 00           cmpl $0,0x14(%eax) 83 78 14 00           cmpl $0,0x14(%eax)
74 09                 je  <L3>           74 09                 je  <L3>
c7 45 ec 02 00 00 00  movl $2,-0x14      c7 45 ec 02 00 00 00  movl $2,-0x14
eb 07                 jmp <JOIN>         eb 07                 jmp <JOIN>
c7 45 ec 00 00 00 00  movl $0,-0x14      c7 45 ec 00 00 00 00  movl $0,-0x14
                      JOIN:                                    JOIN:
```

One byte, 99.97%.

## Why the shapes differ

VC6 `/Od` lowers `if (c) S1 else S2` as `S1; jmp Lend; Lelse: S2; Lend:`. When
the whole if/else is the last statement of an ENCLOSING if's then-block, the
inner `Lend` sits on top of the outer `jmp Lend_outer`, and VC6 does **not**
chain them: the inner then-arm's jump targets the inner label, i.e. the outer
jump. That is our `eb 07`. Measured: `if/else if/else`, `else { if/else }`,
dangling-else and fully braced spellings all emit `eb 07`
(`build/scratch-m3/probe1.cpp` v1..v5).

Retail's arms each terminate with their own jump to the single join. That shape
comes from an exit statement, not from nesting. Measured
(`build/scratch-m3/probe2.cpp`, `probe3.cpp`, `probe4.cpp`):

| source | result |
| :-- | :-- |
| nested `if/else` (any bracing) | `eb 07` - jump-to-jump |
| `goto done;` per arm | matches the jumps but parks one 2-byte stub per `goto` before the epilogue |
| `do { ... break; } while (0)` with the LAST arm falling through | correct jumps + an extra `xor eax,eax; test eax,eax; jne <top>` loop test |
| `switch (0) { default: ... break; }` | correct jumps + a switch-selector frame slot and `movl $0,-0xc` |
| `for (;;) { ... break; }` / `do { ... break; } while (0)` with EVERY arm breaking | exact |
| **`inline` helper returning the value, `/Ob1`-expanded** | **exact** |

The inline helper is the only one of those that is ordinary source. Its
`return`s become stores to the expansion's result temp followed by a jump to the
end of the expansion, which is the join - and when the result initialises a
local, VC6 merges the temp with that local, so the expansion costs no extra
instruction and no extra frame slot (`sub esp,8` either way).

## Close

```cpp
namespace {
    // The Miles DIG_F_* sample format code: bit 0 selects 16-bit samples and
    // bit 1 selects stereo.
    inline i32 MilesSampleFormat(SamplePlaybackData* sampleData) {
        if (sampleData->sampleFormat != FORMAT_8_BIT) {
            if (sampleData->stereo != 0)
                return 3;
            else
                return 1;
        } else if (sampleData->stereo != 0) {
            return 2;
        } else {
            return 0;
        }
    }
}
...
i32 formatMode = MilesSampleFormat(sampleData);
AIL_set_sample_type(handle, formatMode, 0);
```

`PlayMilesSample` 99.97% -> EXACT.

**Do not read this as "hoist every select into an inline".** The open-coded
if/else is retail's shape wherever the arm jumps are chained; the jump
displacement is the evidence, per call site.

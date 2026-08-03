# if-else-two-jmp-backedge

**Symptom.** Our loop body ends in `continue;` → single backward `jmp` to the
loop head. Retail shows a FORWARD `jmp` to a trampoline that itself jumps
back: the if/else two-jmp shape.

    ours:   e9 <loop head ^>            retail:  e9 <B215 v>
                                                 B215: e9 <loop head ^>

**Close.** The overflow handling is the `if (maxWidth < width) { ... }` block
and the delimiter handling is its `else { ... }`; the if-block's natural exit
jumps past the else (the forward jmp), and the loop end jumps back. No
`continue`. font::ExtractLine, commit e4852e58.

**Second case — an EMPTY then-arm** (RandomizeMine 0x5a40e). The tile-skip test
is a two-condition `&&` whose "skip" path must land on the loop body's own end
jump, not on the increment:

    ours (`if (A) if (B) continue;`)      retail
    ------------------------------------ ------------------------------------
    7f 05           jg  <stores>          7f 05           jg  <stores>
    e9 76 ff ff ff  jmp <increment ^>     e9 85 00 00 00  jmp <body end v>

`if (!(A && B)) { stores }` collapses to a single `jle <stores>` (VC6 applies
De Morgan), so the two-jmp shape needs the positive test with an empty then:

    if (A && B) {
        // a lower-numbered trigger already owns this tile
    } else {
        stores
    }

Measured: `continue` and `!(A && B)` both 1314 bytes but wrong jumps; the
empty-then if/else is EXACT.


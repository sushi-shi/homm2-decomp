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

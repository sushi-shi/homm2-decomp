# A near `jmp` straight to the epilogue from inside the last loop is `return`, not `break`

**Trigger.** The function's final statement is a loop, and the loop body exits
early. `break` targets the loop-exit label, which VC6 `/Od` materialises as its
own 5-byte `jmp <epilogue>`; the `break` itself is then a 2-byte `jmp` to that
label. `return` skips the label and jumps to the epilogue directly.

Same rule at function level: an `if (a == b) { } else { body }` whose empty then-arm
jumps to the post-`else` join lands BEFORE the parked `goto` stub island, while
`if (a == b) return;` jumps past the stubs to the epilogue.

Measured on `combatManager::DoCompAI` (RVA 0x1628b) and `hero::CheckLevel` (0x620c8).

## The divergence (DoCompAI, the `finish:` scan loop)

```
ours   ... break;                                  retail   ... return;
--------------------------------------------      --------------------------------------------
0eb1 8b 8d 64 ff.. movl -0x9c(%ebp), %ecx          0eb5 8b 8d 64 ff.. movl -0x9c(%ebp), %ecx
0eb7 89 0d ......  movl %ecx, <giNextActionGrid>   0ebb 89 0d ......  movl %ecx, <giNextActionGrid>
0ebd eb 02         jmp  0xec1     <- loop-exit lbl 0ec1 e9 d4 00 00 00 jmp  0xf9a  <- epilogue
0ebf eb 8b         jmp  0xe4c     <- back edge     0ec6 eb 88         jmp  0xe50   <- back edge
0ec1 e9 cd 00 00 00 jmp 0xf93     <- exit -> epi   0ec8 e9 cd 00 00 00 jmp 0xf9a   <- exit -> epi
```

Ours is 3 bytes shorter, which also flips an earlier `jne` from the near form
(`0f 85 81 00 00 00`) to the short form (`75 7e`) — a 7-byte total size delta from
one keyword.

## CheckLevel: the same rule one scope out

```
ours   if (newLevel == m_level) { } else { ... }   retail   if (newLevel == m_level) return;
--------------------------------------------      --------------------------------------------
0034 75 05          jne  0x3b                      0034 75 05          jne  0x3b
0036 e9 5d 06 00 00 jmp  0x698   <- end-of-if      0036 e9 64 06 00 00 jmp  0x69f  <- epilogue
...                                                ...
0698 eb 05          jmp  0x69f   (skips the stub)  0698 eb 05          jmp  0x69f
069a e9 65 fd ff ff jmp  0x404   <- goto stub      069a e9 65 fd ff ff jmp  0x404
069f 8b e5 5d c3    epilogue                       069f 8b e5 5d c3    epilogue
```

## What made it match

```cpp
    newLevel = GetLevel(m_experience);
    if (newLevel == m_level)
        return;
    /* body unindented; no else */
```
```cpp
                giNextActionGridIndex = adjCell;
                return;
```

`hero::CheckLevel` and `combatManager::DoCompAI` both closed on this as the last
residual. Related: [goto-label-jump-stub](goto-label-jump-stub.md) (which parks the
stubs) and [switch-goto-tail-stub-island](switch-goto-tail-stub-island.md).

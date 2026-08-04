# `p[i] += 1` reads pointer-first; `p[i]++` reads index-first

**Trigger.** A read-modify-write of one array element where retail and ours emit the
*same* instructions in a different order, and only on the READ half: retail evaluates
the pointer/base subtree first and the subscript second, ours does the reverse. The
store half is identical in both (index first).

Measured on `game::PerDay` (RVA 0x574c6), the three AI resource-bonus statements
`m_players[player].m_resources[m_day - 1]`.

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gi- /GX`)

```cpp
struct PD { char pad[0x8b]; int res[8]; char pad2[0x11c-0x8b-32]; };
struct G { char head[0x496]; unsigned short day; char gap[4]; PD players[6];
           void f1(int player); void f2(int player); };

void G::f1(int player) { players[player].res[day - 1]++; }      // ours
void G::f2(int player) { players[player].res[day - 1] += 1; }   // retail
```

```
f1  x[i]++                                     f2  x[i] += 1
---------------------------------------------  ---------------------------------------------
8b 45 fc  movl -0x4(%ebp), %eax                8b 45 08  movl 0x8(%ebp), %eax      <<< player
33 c9     xorl %ecx, %ecx                      69 c0 ..  imull $0x11c, %eax, %eax
66 8b 88 96 04 00 00  movw 0x496(%eax), %cx    8b 4d fc  movl -0x4(%ebp), %ecx
8b 55 08  movl 0x8(%ebp), %edx     <<< player  8d 94 01 9c 04 00 00  leal 0x49c(%ecx,%eax), %edx
69 d2 ..  imull $0x11c, %edx, %edx             8b 45 fc  movl -0x4(%ebp), %eax
8b 45 fc  movl -0x4(%ebp), %eax                33 c9     xorl %ecx, %ecx
8d 94 10 9c 04 00 00  leal 0x49c(%eax,%edx), %edx   66 8b 88 96 04 00 00  movw 0x496(%eax), %cx
8b 84 8a 88 00 00 00  movl 0x88(%edx,%ecx,4), %eax  8b 94 8a 88 00 00 00  movl 0x88(%edx,%ecx,4), %edx
83 c0 01  addl $0x1, %eax                      83 c2 01  addl $0x1, %edx
   ... store: index first in BOTH forms           ... store: index first in BOTH forms
```

`x[i] = x[i] + 1` (`f3`) also reads pointer-first, but its *store* re-evaluates the
subscript one instruction later than retail, so it is not the same body.

## What made it match

```cpp
                m_players[player].m_resources[m_day - 1] += 1;
```

(previously `...[m_day - 1]++;`). Three sites in `game::PerDay`; with the frame solve
and two operand-order fixes the function went 95.77% -> EXACT.

Related: [inline-call-operand-index-first](inline-call-operand-index-first.md) — the
general rule that VC6 /Od evaluates the pointer-typed operand's subtree first. `++`
on a subscript is a second, source-visible exception to it.

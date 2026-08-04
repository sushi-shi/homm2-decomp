# adjacent-enum-ternary-setcc

**Symptom.** A select between two enum constants that differ by exactly one lowers to a
bare `setcc` plus an `add`, with no `dec`/`and` pair:

    xor  eax, eax
    cmp  <a>, <b>
    setne al
    add  eax, 0x5          <-- ENABLE(5) / DISABLE(6)
    mov  dword ptr [ebp - 0x18], eax

`od-constant-ternary-lowering` describes the general `setcc / dec / and M / add K`
shape. When `A - B` is 1 the mask degenerates, VC6 drops both the `dec` and the
`and`, and what is left looks deceptively like hand-written arithmetic.

## The divergence

`game::UpdateNewGameWindow` (RVA 0x76e3b) had three of these written as `if`/`else`:

```
retail                                       ours (if / else)
-------------------------------------------- --------------------------------------------
8b 4d d4  movl -0x2c(%ebp), %ecx             8b 4d d4  movl -0x2c(%ebp), %ecx
0f be 91 79 04 00 00 movsbl 0x479(%ecx),%edx 0f be 91 79 04 00 00 movsbl 0x479(%ecx),%edx
33 c0     xorl %eax, %eax                    3b 55 dc  cmpl -0x24(%ebp), %edx
39 55 dc  cmpl %edx, -0x24(%ebp)             75 0b     jne  <else>
0f 95 c0  setne %al                          c7 45 e8 05 00 00 00 movl $0x5, -0x18(%ebp)
83 c0 05  addl $0x5, %eax                    eb 09     jmp  <end>
89 45 e8  movl %eax, -0x18(%ebp)             c7 45 e8 06 00 00 00 movl $0x6, -0x18(%ebp)
```

## The trap: `enum_constant + bool` masks, `int_literal + bool` does not

Writing the arithmetic form directly gets close but leaves an extra instruction,
because adding an **enumerator** to a `bool` makes VC6 normalise the bool:

```
ours: NEW_GAME_WIDGET_ENABLE + (playerIndex3 != m_selectedSetupPlayer)
      33 c0     xorl %eax, %eax
      39 55 dc  cmpl %edx, -0x24(%ebp)
      0f 95 c0  setne %al
      25 ff 00 00 00 andl $0xff, %eax      <<< ours only
      83 c0 05  addl $0x5, %eax
```

Probe (same flags), `enum { WA = 5 }; typedef int WCmd;`:

```cpp
void f1(int i, int*  o) { *o = 5  + (i != gp->sel); }   // setne; add           - no mask
void g1(int i, WCmd* o) { *o = WA + (i != gp->sel); }   // setne; and 0xff; add - masked
void h1(int i, WCmd* o) { *o = WA + (int)(i != gp->sel); }        // no mask
void h4(int i, WCmd* o) { *o = (i == gp->sel) ? WA : WB; }        // no mask
```

## What made it match

The ternary, not the arithmetic - it reproduces retail exactly and reads like the
rest of the file:

```cpp
    messageTemp.payload.widget.command = playerIndex3 == m_selectedSetupPlayer
                                             ? NEW_GAME_WIDGET_ENABLE
                                             : NEW_GAME_WIDGET_DISABLE;
```

Note the compare order: the operand that needs a register is materialised first
and lands as the **second** `cmp` operand, so `cmp -0x24(%ebp), %edx` means the
local is on the left of the source relational and the widened field on the right.

`UpdateNewGameWindow` 87.78% -> EXACT with these three sites converted.

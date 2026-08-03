# od-constant-ternary-lowering

**Symptom.** A branchless select over two compile-time constants:

    xor  ecx, ecx
    cmp  <a>, <b>
    setcc cl
    dec  ecx                  <-- or `sub ecx, 1`
    and  ecx, M
    add  ecx, K

Two independent things are readable off those seven instructions.

## 1. `dec ecx` is the compiler's own `?:` lowering; `sub ecx, 1` is source arithmetic

`combatManager::DoLuck` (0x9ffec) passed the bolt colour through a macro that
spelled the select as arithmetic, `REVERSE + (((targetX >= startX) - 1) & (FORWARD - REVERSE))`:

    ours (arithmetic macro)                    retail
    8b 45 f4  movl -0xc(%ebp), %eax            8b 45 f8  movl -0x8(%ebp), %eax
    33 c9     xorl %ecx, %ecx                  33 c9     xorl %ecx, %ecx
    3b 45 ec  cmpl -0x14(%ebp), %eax           3b 45 f0  cmpl -0x10(%ebp), %eax
    0f 9d c1  setge %cl                        0f 9e c1  setle %cl
    83 e9 01  subl $0x1, %ecx        <<<       49        decl %ecx           <<<
    83 e1 fd  andl $-0x3, %ecx                 83 e1 fd  andl $-0x3, %ecx
    81 c1 2f 01 00 00 addl $0x12f, %ecx        81 c1 2f 01 00 00 addl $0x12f, %ecx

The `- 1` a source expression writes becomes `sub reg, 1` (`83 e9 01`, 3 bytes).
VC6's own conditional-constant lowering emits `dec reg` (`49`, 1 byte). A retail
`dec` inside this shape therefore means the source held a real `?:` with two
constant arms, not hand-written mask arithmetic. Deleting the helper and writing
the ternary at the call site produced the `dec`.

## 2. The condition in the source is the INVERSE of the emitted `setcc`

VC6 lowers `cond ? A : B` as

    set!cond cl ; dec ecx ; and ecx, (A - B) ; add ecx, B

so `setcc` carries `!cond`, the `and` mask is `A - B` and the `add` constant is `B`.
Writing the arms in source order gives the mirror of what you read:

    source `startX <= targetX ? REVERSE : FORWARD`   ->  setg  / and 0x3  / add 0x12c
    retail                                               setle / and -0x3 / add 0x12f

Reading retail back through the rule: `!cond = le` so `cond = g`, `B = 0x12f = REVERSE`,
`A = M + B = -3 + 0x12f = 0x12c = FORWARD`.

**Close.**

```cpp
DoBolt(0, startX_b, startY, targetX, targetY_l, 0, 0, LUCK_BOLT_WIDTH, LUCK_BOLT_WIDTH,
       startX_b > targetX ? BOLT_COLOR_RAINBOW_FORWARD : BOLT_COLOR_RAINBOW_REVERSE,
       ...);
```

`combatManager::DoLuck` 98.70% -> EXACT. The `#if H2_STRICT_ENUMS` inline helper /
arithmetic-macro pair it replaced is gone: a ternary between two enumerators of one
enum already type-checks under both build modes.

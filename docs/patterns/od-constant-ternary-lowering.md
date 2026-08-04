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

## 3. Arms that differ by exactly 1: no `dec`, no `and`, and the setcc is NOT inverted

The seven-instruction shape above is the general case. When `A - B == 1` VC6 drops the
mask entirely and emits the condition **as written**, followed by `add reg, B` (or
`inc reg` when B is 1, or nothing when B is 0):

```
source                                        emitted (VC6 SP5 /Od)
--------------------------------------------- ---------------------------------------------
c ? 7 : 6                                     xor eax,eax ; cmp ; sete al ; add eax,6
c ? 2 : 1                                     ...         ; setne cl ; inc ecx
c ? 1 : 0                                     xor ecx,ecx ; cmp ; setne cl        (no add)
```

Measured on `combatManager::SetCombatDirections` (0x2bc31) and
`combatManager::CheckWin` (0x2d369).

```
ours  6 + (dir == D_WIDE_W)                    retail  dir == D_WIDE_W ? D_WIDE_E : D_WIDE_W
--------------------------------------------- ---------------------------------------------
33 c0        xorl %eax, %eax                  33 c0        xorl %eax, %eax
83 7d f8 06  cmpl $0x6, -0x8(%ebp)            83 7d f8 06  cmpl $0x6, -0x8(%ebp)
0f 94 c0     sete %al                         0f 94 c0     sete %al
25 ff 00 00 00  andl $0xff, %eax   <<<
83 c0 06     addl $0x6, %eax                  83 c0 06     addl $0x6, %eax
```

The `and 0xff` is the bool->int promotion of an arithmetic `enum + (a == b)`; the real
`?:` has no promotion node at all. (`(int)D + (a==b)` and `D + (int)(a==b)` both keep
the mask if `D` is an *enumerator*; `6 + (a==b)` with a literal does not. The `?:`
form is unconditionally clean.)

## 4. `i8`-typed arms: `setne`/`inc`/`movsbl` with no `xor`

If the two constants are `i8`-typed, only the low byte is live, so VC6 skips the
register clear and truncates at the end:

```
ours  (i8)((f & 2) ? 2 : 1)                    retail  (i8)((f & 2) ? (i8)2 : (i8)1)
--------------------------------------------- ---------------------------------------------
83 e0 02     andl $0x2, %eax                  83 e0 02     andl $0x2, %eax
f7 d8        negl %eax                        85 c0        testl %eax, %eax
1b c0        sbbl %eax, %eax                  0f 95 c1     setne %cl
f7 d8        negl %eax                        41           incl %ecx
83 c0 01     addl $0x1, %eax                  0f be d1     movsbl %cl, %edx
0f be c8     movsbl %al, %ecx
```

`neg/sbb/neg` is the int-truthiness normalisation VC6 inserts when an `int`-typed
select has to force a bitwise-AND result to 0/1; casting **each arm** to `i8` removes
it. Closed `combatManager::GetCommand`:

```cpp
                    command = CombatMessageCommand(
                        HAS(..., MONSTER_FLAGS_FLYING)
                            ? static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_FLY))
                            : static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_MOVE))
                    );
```

## 5. `neg/sbb` instead of `setcc`: the source condition is an int, not a comparison

The same normalisation identifies the *condition* spelling. `neg reg; sbb reg,reg;
and M; add B` means the source tested a plain integer for truth; `setcc` means it
tested a relational. `combatManager::ShowDeadArmies` (0x2ec2c):

```
retail  y = side ? CASUALTY_DEFENDER_Y : CASUALTY_ATTACKER_Y
8b 95 a0 fe ff ff  movl -0x160(%ebp), %edx
f7 da              negl %edx
1b d2              sbbl %edx, %edx
83 e2 43           andl $0x43, %edx        ; 0x15a - 0x117
81 c2 17 01 00 00  addl $0x117, %edx
```

(`if (side == IDX(COMBAT_ATTACKER_SIDE)) y = A; else y = B;` produced a branch pair.)

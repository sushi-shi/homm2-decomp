# `lea K(base,index)`: the SIB base names the LEFT operand of the source `+`

**Trigger.** Two objects agree on every instruction and register, but one `leal`
encodes `0x10(%eax,%edx)` where the other encodes `0x10(%edx,%eax)`. The registers
hold the same values and were computed in the same order, so this is not regalloc:
it is the source order of the addition.

Measured on `combatManager::ShowDeadArmies` (RVA 0x2ec2c), twice (the casualty icon
X and the casualty quantity X).

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

```cpp
void l1(int p,int q,int r){ g = p*q + r + 16; }
void l2(int p,int q,int r){ g = r + p*q + 16; }
```

```
l1: p*q + r + 16                              l2: r + p*q + 16
--------------------------------------------- ---------------------------------------------
8b 45 fc  movl -0x4(%ebp), %eax               8b 45 fc  movl -0x4(%ebp), %eax
0f af 45 f8  imull -0x8(%ebp), %eax           0f af 45 f8  imull -0x8(%ebp), %eax
8b 4d 08  movl 0x8(%ebp), %ecx                8b 4d 08  movl 0x8(%ebp), %ecx
8d 54 08 10  leal 0x10(%eax,%ecx), %edx       8d 54 01 10  leal 0x10(%ecx,%eax), %edx
                  ^^ base = product                             ^^ base = r
```

Both spellings evaluate the multiply FIRST and load `r` second - the instruction
sequence is identical - so only the SIB byte (`08` vs `01`) separates them. The base
register is whichever operand was written on the **left** of the `+`; the constant
folds into the displacement either way.

## What made it match

```cpp
                    startX_0 + spacing_6 * armyIndex + CASUALTY_ICON_CELL_WIDTH / 2
```
```cpp
                    startX_0 + spacing_6 * armyIndex + CASUALTY_QUANTITY_X_OFFSET,
```

(previously `armyIndex * spacing_6 + startX_0 + ...`, which also had the multiply's
operands the wrong way round - `/Od` lowers `a * b` as `mov reg,a; imul reg,b`, so
`imull -0x164(%ebp), %edx` after `movl -0x10(%ebp), %edx` names `spacing * armyIndex`.)

`combatManager::ShowDeadArmies` 95.75% -> EXACT (with the other axes on the same
function).

## The same rule without a `lea`: `add reg,[mem]` vs `mov reg,[mem]; add reg,reg`

When the sum is not folded into an address, the left operand of the `+` still
decides — and here it costs an instruction, so the function's instruction count
moves too.

```cpp
int Make(int,int);  int gres;
void o_exprfirst(int i, int base) { gres = Make(0, i * 0x30 + base); }   // ours
void o_localfirst(int i, int base){ gres = Make(0, base + i * 0x30); }   // retail
```

```
o_exprfirst  (ours)                           o_localfirst  (retail)
--------------------------------------------- ---------------------------------------------
8b 55 fc     movl -0x4(%ebp), %edx            8b 45 fc     movl -0x4(%ebp), %eax
6b d2 30     imull $0x30, %edx, %edx          6b c0 30     imull $0x30, %eax, %eax
03 55 f8     addl -0x8(%ebp), %edx            8b 55 f8     movl -0x8(%ebp), %edx
                                              03 d0        addl %eax, %edx
```

`/Od` evaluates the multiply first in BOTH spellings; what changes is whether the
named operand is the accumulator (`mov` it, then `add` the product into it) or the
memory operand of a single `add`. Measured on `SetupDynamicWindow` (RVA 0x6e35e):
`leftOffset + columnIndex * TILE_SIZE` and `topOffsetNum + tileRowPos * TILE_SIZE`
closed a 562-vs-564 instruction-count gap, and the `lea` form of the same rule fixed
`*contentLeft + columnsSize * TILE_SIZE - 1` plus four border-tile coordinates.

### Confirmed on real sites (SOURCE/ARMY)


The same rule reads off a plain two-term `int` sum whose operands are a computed
value and a simple lvalue. `/Od` evaluates the COMPLEX operand first into a
register, then materialises the simple one, and the ADD's **destination register
holds the LEFT operand of the source `+`**:

```
ours   ArmyFacingRearHexOffset(m_facing) + m_hex      retail   m_hex + ArmyFacingRearHexOffset(m_facing)
------------------------------------------------     ------------------------------------------------
8b 82 86 00 00 00  movl 0x86(%edx), %eax  ; facing    8b 88 86 00 00 00  movl 0x86(%eax), %ecx
83 e8 01           subl $0x1, %eax                    83 e9 01           subl $0x1, %ecx
f7 d8              negl %eax                          f7 d9              negl %ecx
1b c0              sbbl %eax, %eax                    1b c9              sbbl %ecx, %ecx
83 e0 fe           andl $-0x2, %eax                   83 e1 fe           andl $-0x2, %ecx
83 c0 01           addl $0x1, %eax                    83 c1 01           addl $0x1, %ecx
8b 4d e8           movl -0x18(%ebp), %ecx             8b 55 e8           movl -0x18(%ebp), %edx
03 41 7a           addl 0x7a(%ecx), %eax   <<<        8b 42 7a           movl 0x7a(%edx), %eax   <<<
                                                      03 c1              addl %ecx, %eax
```

`add eax, [ecx+0x7a]` keeps the *computed* value as the accumulator, so the
computed value is the source's LEFT operand. Retail loads `m_hex` into a fresh
register and adds the computed one, so `m_hex` is the LEFT operand. One
instruction longer, and it is the retail form at every `ArmyFacingRearHexOffset`
site in `SOURCE/ARMY` (`army::Walk`, `army::ProcessDeath`, `army::DamageEnemy`).

The same reading applies when the simple operand is a *parameter*:

```
ours   m_frameInfo.spellEffectX * rear + x     retail   x + m_frameInfo.spellEffectX * rear
0f bf 82 2a 01 00 00 movswl 0x12a(%edx),%eax   0f bf 82 2a 01 00 00 movswl 0x12a(%edx),%eax
...                                            ...
0f af c2             imull %edx, %eax          0f af c2             imull %edx, %eax
03 45 08             addl 0x8(%ebp), %eax      8b 4d 08             movl 0x8(%ebp), %ecx
                                               03 c8                addl %eax, %ecx
```

`army::DrawToBuffer` 96.66% -> EXACT once every such sum was rewritten with the
simple operand on the left.

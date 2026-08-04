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

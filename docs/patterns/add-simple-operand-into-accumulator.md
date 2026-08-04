# `mov scratch,<product>` + `mov acc,<local>` + `add acc,scratch` names the LEFT operand

**Trigger.** Same instructions, same registers-modulo-rotation, but ours fuses the
add into the accumulator (`mov acc,X; shl/imul acc; add acc,mem`) while retail keeps
three moves (`mov scratch,X; shl/imul scratch; mov acc,mem; add acc,scratch`). The
extra `mov` is not noise: VC6 `/Od` evaluates the operand that needs a register FIRST,
into a scratch, and then loads the *simple* operand into the accumulator. So the
memory operand that ends up in the ACCUMULATOR is the source's **left** operand.

This is the non-`lea` sibling of `lea-base-names-add-order`.

## The divergence

`advManager::HeroQuickView` (RVA 0xc322), detailed-army icon X argument:

```
retail                                          ours
----------------------------------------------- -----------------------------------------------
8b 4d ac           movl -0x54(%ebp), %ecx  ; ii  8b 75 ac           movl -0x54(%ebp), %esi
c1 e1 05           shll $0x5, %ecx               c1 e6 05           shll $0x5, %esi
8b b5 50 ff ff ff  movl -0xb0(%ebp), %esi ;start 03 b5 50 ff ff ff  addl -0xb0(%ebp), %esi
03 f1              addl %ecx, %esi
```

and the `imul` form, `TownQuickView` (RVA 0xd1f9) second-row text X:

```
retail                                          ours
----------------------------------------------- -----------------------------------------------
8b 85 40 ff ff ff  movl -0xc0(%ebp), %eax ;strid 8b 45 ac           movl -0x54(%ebp), %eax
0f af 45 ac        imull -0x54(%ebp), %eax       0f af 85 40 ff ff ff  imull -0xc0(%ebp), %eax
83 c0 16           addl $0x16, %eax              83 c0 16           addl $0x16, %eax
```

The second form also fixes the MULTIPLY order: `mov r,a; imul r,b` is `a * b`, so
retail multiplies `stride * index`, not `index * stride`.

## What made it match

```cpp
    static_cast<i16>(ii * ARMY_QUICK_ICON_SIZE + armyStart - ...)     // ours
    static_cast<i16>(armyStart + ii * ARMY_QUICK_ICON_SIZE - ...)     // retail

    static_cast<i16>((wIndex - topRow) * stride + basePos - ...)      // ours
    static_cast<i16>(basePos + stride * (wIndex - topRow) - ...)      // retail
```

`HeroQuickView` 96.05% -> EXACT and `TownQuickView` 96.47% -> EXACT once every
`<product> + <local>` in the widget-placement arguments was rewritten
`<local> + <product>` (six sites in each).

**Caveat: it is per-site, not per-function.** `ComboDraw` (RVA 0xf30a) has both
orders in the same body - every guard and `GetCell`/`DrawCell` argument is
`originX + drawX` (param into the accumulator), but all three
`GetCloudLookup(drawX + originX, drawY + originY)` calls keep the local first.
Read the add order off each site.

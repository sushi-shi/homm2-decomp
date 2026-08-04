# `int`-cast of a relational suppresses the /G5 byte widen

**Trigger.** Retail materialises a comparison as a value with
`xor reg,reg; setcc reg_l` and uses it directly in arithmetic. Ours emits the
same `xor`/`setcc` but inserts an extra `and reg,0xff` before the arithmetic.

In C++ a relational yields `bool` (one byte), so `int + (a <= b)` runs the
bool through an integral promotion, and VC6 `/G5` spells that promotion as the
AND-mask zero-extension. An explicit `(int)`/`static_cast<i32>` on the
relational makes the conversion part of the operand instead of a separate
promotion node, and the mask disappears — the `xor reg,reg` already guarantees
the high bytes.

## The divergence

`game::UpdateSpellWidgets` (RVA 0x548cf), mana-digit widget command:

```
retail                                       ours (bare relational)
-------------------------------------------  -------------------------------------------
33 c9              xor   ecx, ecx            33 c9              xor   ecx, ecx
83 7d fc 63        cmpl  $0x63, -0x4(%ebp)   83 7d fc 63        cmpl  $0x63, -0x4(%ebp)
0f 9e c1           setle %cl                 0f 9e c1           setle %cl
                                             81 e1 ff 00 00 00  andl  $0xff, %ecx
83 c1 05           addl  $0x5, %ecx          83 c1 05           addl  $0x5, %ecx
89 4d e0           movl  %ecx, -0x20(%ebp)   89 4d e0           movl  %ecx, -0x20(%ebp)
```

## What made it match

```cpp
message.payload.widget.command = static_cast<BaseWidgetCommand>(
    IDX(WIDGET_COMMAND_SET_FLAGS)
    + static_cast<i32>(spellPoints0 <= VIEW_SPELL_MANA_HUNDREDS_THRESHOLD)
);
```

Both mana-digit sites closed with it; `UpdateSpellWidgets` went 99.31% ->
EXACT (size 1179 -> 1167 = retail's 0x48f). A `? 1 : 0` ternary spelling
produces the same bytes; the bare relational and an operand swap do not.

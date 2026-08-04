# `Column(x)[W*y]` vs a flat `base + x + W*y`: which term is emitted first

**Trigger.** A row/column address built from three terms - a pointer member, a
column term scaled by `sizeof(element)`, and a row term also scaled by
`sizeof(element)` - where retail emits **the row product first, the column
product second and the base last**, then adds column then row. No flat
association of `+` reproduces that order: every spelling of
`nodes + x + MAP_WIDTH * y` (and its 12 reassociations, parenthesisations and
`&nodes[x][...]` forms) emits the base *between* the two products.

This is the `getcell-vs-row-plus-index` rule one level down: the order comes
from an **inline accessor whose body indexes a nested accessor**, not from the
call site's spelling.

## The divergence (`searchArray::BuildPath` RVA 0x916c0, `SeedPosition` 0x917d6)

`sizeof(searchNode) == 9`, `MAP_WIDTH` is a global.

```
ours   m_storage.nodes + destinationX + MAP_WIDTH * destinationY
 32: 8b 4d 10        movl 0x10(%ebp), %ecx        ; destinationX
 35: 6b c9 09        imull $0x9, %ecx, %ecx
 38: 8b 55 f0        movl -0x10(%ebp), %edx       ; this
 3b: 8b 82 14240000  movl 0x2414(%edx), %eax      ; m_storage.nodes
 41: 03 c1           addl %ecx, %eax
 43: 8b 0d <W>       movl MAP_WIDTH, %ecx
 49: 0f af 4d 14     imull 0x14(%ebp), %ecx
 4d: 6b c9 09        imull $0x9, %ecx, %ecx
 50: 03 c1           addl %ecx, %eax

retail
 32: 8b 0d <W>       movl MAP_WIDTH, %ecx        ; ROW product first
 38: 0f af 4d 14     imull 0x14(%ebp), %ecx
 3c: 6b c9 09        imull $0x9, %ecx, %ecx
 3f: 8b 55 10        movl 0x10(%ebp), %edx       ; column product second
 42: 6b d2 09        imull $0x9, %edx, %edx
 45: 8b 45 f0        movl -0x10(%ebp), %eax      ; base LAST
 48: 8b 80 14240000  movl 0x2414(%eax), %eax
 4e: 03 c2           addl %edx, %eax
 50: 03 c1           addl %ecx, %eax
```

## What made it match

Add the one-argument column accessor and index it with the row term:

```cpp
    searchNode* GetColumn(i32 x) {                    /* include/SOURCE/searchArray.h */
        return m_storage.nodes + x;
    }
```
```cpp
    searchNode* node = &GetColumn(destinationX)[MAP_WIDTH * destinationY];   /* BuildPath   */
    s_currentNode    = GetColumn(targetX)[MAP_WIDTH * targetY];              /* SeedPosition */
    s_neighborNode   = &GetColumn(s_neighborX)[MAP_WIDTH * s_neighborY];
```

The index operand of `[]` is evaluated before the inlined accessor body, and
inside the body the non-foldable `+ x` is evaluated before `m_storage.nodes` -
so the emission order is row, column, base, exactly as retail. `BuildPath`
88.92% -> EXACT; `SeedPosition` 61.26% -> EXACT.

**Byte arrays do not need it.** `mapExtra` is `u8*`, so `+ x` folds into the
`add` and the base *is* emitted first:

```
retail   *(mapExtra + targetX + MAP_WIDTH * targetY)
 174: a1 <mapExtra>     movl mapExtra, %eax
 179: 03 45 28          addl 0x28(%ebp), %eax        ; + targetX (foldable)
 17c: 8b 0d <W>         movl MAP_WIDTH, %ecx
 182: 0f af 4d 2c       imull 0x2c(%ebp), %ecx
 188: 8a 14 08          movb (%eax,%ecx), %dl
```

So the accessor form is only indicated when *both* index terms need their own
register, i.e. when the element size is not 1.

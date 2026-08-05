# `a = b = K` (chained store) vs two independent `= K` stores

**Trigger.** Two adjacent members of the same object are both set to one constant.
Ours emits, for the second store, a reload of the object pointer, a load of the
field just written, and a register store; retail emits a second `movl $K, off(reg)`.
Three extra instructions and no immediate in the second store is the whole tell.

Measured on `advManager::DoAdvCommand` (RVA 0x20d7), the hover reset at the end of
the body, and again on `advManager::SetHeroContext` (0xe374, two pairs) and
`advManager::SetInitialMapOrigin` (0x12a12).

## The divergence

`m_selectedCell` is 0x36, `m_lastHoverCell` 0x1e6 and `m_hoverCellY` 0x1ea.

```
ours   m_hoverCellY = CURSOR_INVALID_POSITION;      retail
       m_lastHoverCell = m_hoverCellY;
--------------------------------------------------  --------------------------------------------------
8b 45 84           movl -0x7c(%ebp), %eax           8b 45 84           movl -0x7c(%ebp), %eax
c7 40 36 ff ff ff ff  movl $-1, 0x36(%eax)          c7 40 36 ff ff ff ff  movl $-1, 0x36(%eax)
8b 4d 84           movl -0x7c(%ebp), %ecx           8b 4d 84           movl -0x7c(%ebp), %ecx
c7 81 ea 01 00 00 ff ff ff ff                       c7 81 ea 01 00 00 ff ff ff ff
                   movl $-1, 0x1ea(%ecx)                               movl $-1, 0x1ea(%ecx)
8b 55 84           movl -0x7c(%ebp), %edx           8b 55 84           movl -0x7c(%ebp), %edx
8b 45 84           movl -0x7c(%ebp), %eax           c7 82 e6 01 00 00 ff ff ff ff
8b 88 ea 01 00 00  movl 0x1ea(%eax), %ecx                              movl $-1, 0x1e6(%edx)
89 8a e6 01 00 00  movl %ecx, 0x1e6(%edx)
```

`/Od` never forwards the value it just stored: a source `x = y;` always reloads `y`
from memory, so the copy form is three instructions longer *and* leaves the second
store without an immediate. A retail `movl $K, off(reg)` in the second slot means the
source repeated the constant.

## What made it match

```cpp
    m_hoverCellY = CURSOR_INVALID_POSITION;      // retail
    m_lastHoverCell = CURSOR_INVALID_POSITION;
```
```cpp
    m_hoverCellY = CURSOR_INVALID_POSITION;      // ours
    m_lastHoverCell = m_hoverCellY;
```

`DoAdvCommand` closed EXACT with that plus the frame-slot solve. `SetHeroContext`
needed the same on both `m_cursorMapX/Y = VIEW_CENTER_CELL` and
`m_previousCursorMapX/Y = CURSOR_INVALID_POSITION`; `SetInitialMapOrigin` on
`m_hoverCellY`/`m_lastHoverCell`.

**Corollary.** The reverse reading is just as usable: when retail *does* carry the
reload-and-copy triple, the source really is the chained `a = b;` form — see the
`CompleteDraw` all-black origin reset row in docs/cross-version-spellings.md, where
PoL 2.0 spells the chain and Buka spells two literals.

## The genuine chained EXPRESSION `a = b = K` is the literal form, not the copy form

The two spellings above are `a = K; b = K;` and `a = K; b = a;` — two *statements*.
The third spelling, a single chained assignment expression, is a distinct source form
and VC6 folds it to the **literal** shape: the inner assignment's value is the
constant, not a reload of `b`.

`heroWindow::heroWindow(void)` (0xba5c0), `heroWindow(i32,i32,i32,i32,WindowFlag)`
(0xba660), `heroWindow(i32,i32,char*)` (0xba700) and `heroWindow::RemoveWidget`
(0xbb0d0) each carry two or three such pairs; retail stores immediates in both slots
and the store ORDER is inner-then-outer, which is what names the spelling:

```
ours   m_prevWindow = NULL;                    retail (and the fix)
       m_nextWindow = m_prevWindow;                   m_nextWindow = m_prevWindow = NULL;
------------------------------------------     ------------------------------------------
8b 4d fc        movl -0x4(%ebp), %ecx          8b 4d fc        movl -0x4(%ebp), %ecx
c7 41 08 00..   movl $0x0, 0x8(%ecx)           c7 41 08 00..   movl $0x0, 0x8(%ecx)
8b 55 fc        movl -0x4(%ebp), %edx          8b 55 fc        movl -0x4(%ebp), %edx
8b 45 fc        movl -0x4(%ebp), %eax          c7 42 04 00..   movl $0x0, 0x4(%edx)
8b 48 08        movl 0x8(%eax), %ecx
89 4a 04        movl %ecx, 0x4(%edx)
```

`m_prevWindow` is 0x8 and `m_nextWindow` is 0x4, so retail writes 0x8 first: the
INNER target of `m_nextWindow = m_prevWindow = NULL`. Reading the store order
therefore recovers which name the source put on the right. The same evidence gives
`m_posX = m_posY = 0` (0x2c before 0x28) and
`m_widgetListTail = m_widgetListHead = NULL` (0x3c before 0x38).

Consequence for the census: a pair of immediate stores does NOT prove two independent
statements. Two statements repeat the constant in source order; the chained
expression writes the RIGHTMOST target first. Both are byte-identical only when the
two fields happen to be stored in the other order.

All four functions closed EXACT with this plus the `w == m_widgetListTail` operand
order in `RemoveWidget`.

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

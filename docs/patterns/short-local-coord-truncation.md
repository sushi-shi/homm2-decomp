# Short-local coordinate truncation (/O2 widget parent-relative coords)

**Tags:** o2, widget, short, truncation, regalloc
**Symptom:** a widget draw/dim/blit that computes a parent-relative screen coord
`(short)(ownerWindow->offset + this->pos)` sits at 30–90% — the arithmetic is there
but the truncation/register use is scheduled differently from retail.

## The tell (Ghidra)

```c
DimBitmapArea(gpWindowManager->field_0x46,
              (int)(short)((short)*(undefined4 *)(*(int *)(this + 4) + 0x28) + *(short *)(this + 0x18)),
              (int)(short)(*(short *)(this + 0x1a) + *(short *)(*(int *)(this + 4) + 0x2c)),
              ...);
```

The retail truncates the **sum** to `short` and *also* truncates the wide operand
(`(short)win->field_0x28`) before adding — i.e. the value lives in a `short` slot, not
an `int` temporary. This only happens when the source stored it in a **`short` local**.

## Fix — use a short LOCAL, not an inline cast

```cpp
// WRONG — 38% : casts the whole int sum, keeps it in an int temp
DimBitmapArea(gpWindowManager->field_0x46,
              static_cast<short>(field_0x4->field_0x28 + field_0x18),
              static_cast<short>(field_0x1a + field_0x4->field_0x2c),
              field_0x1c, field_0x1e, 0);

// RIGHT — 100% exact : the short local reproduces the (short)op + op truncation
short x = field_0x4->field_0x28 + field_0x18;
short y = field_0x1a + field_0x4->field_0x2c;
DimBitmapArea(gpWindowManager->field_0x46, x, y, field_0x1c, field_0x1e, 0);
```

The compiler, knowing `x` is `short`, is free to truncate the wide operand (`win->field`)
before the add (value-range narrowing) and keeps the result in a 16-bit-flavored temp —
exactly retail's `movsx` dance. The inline `static_cast<short>` forces an `int` sum first.

Verified: `widget::Dim` (0x004de1e0) 38% → 100% by this change alone. Same shape appears in
`button::Select`/`Deselect`, `textWidget::Draw` — reach for the short local whenever a
parent-relative coord is passed as an `int` arg but computed through a `short`.

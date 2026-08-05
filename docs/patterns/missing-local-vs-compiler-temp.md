# missing-local-vs-compiler-temp

Retail allocates a frame slot we do not. There are two completely different
causes with opposite fixes, and **which one it is depends only on where the slot
sits relative to the register-argument spill**.

## The rule

/Od lays a frame out in three regions, shallowest first:

```
  -0x04 ...          named locals            (hashed: bucket, then LIFO by declaration)
  ...                register arguments      (`this` first, then argument order, unhashed)
  ...deepest         compiler temporaries
```

So for an unexplained slot:

| where | what it is | fix |
| :-- | :-- | :-- |
| **above** the argument spill | a declaration our source lost | declare it |
| **below** the argument spill | a materialised sub-expression | inline the expression into its use site |

Acting on the wrong one fabricates a local that retail never had. In a function
with no register arguments (a `void`-argument free function) there is no divider,
so fall back on whether anything reads the slot: retail referencing it means a
declaration, a pure gap means either.

## Below the spill: a temp — `dropListWidget::Draw`

```
ours                                     retail
sub esp, 0x8                         |   sub esp, 0x8
mov dword ptr [ebp - 0x8], ecx       |   mov dword ptr [ebp - 0x4], ecx      ; `this`
```

Both reserve 8 bytes and both hold exactly one named thing, but ours puts `this`
at `-0x8` and retail at `-0x4`. `this` is the deepest NAMED slot, so retail's
`-0x8` is below it: a temp. Our source had hoisted the colour choice into a local
that retail never declared:

```c
        FontDrawMode color = FONT_DRAW_DIMMED;              // ours: a named local at -0x4
        if (!HAS(m_flags, WIDGET_FLAG_DIMMED))
            color = m_normalColor;
        m_font->DrawBoundedString(..., color, m_alignment);
```

```c
        m_font->DrawBoundedString(                          // retail: a temp at -0x8
            ...,
            HAS(m_flags, WIDGET_FLAG_DIMMED) ? FONT_DRAW_DIMMED : m_normalColor,
            m_alignment
        );
```

Inlining it closed the function. Declaring an extra local instead would have
moved `this` further down and made the frame worse.

## Above the spill: a declaration — `advManager::RecruitEvent`

Retail leaves `-0x14` empty between `recruitDlg` (`-0x10`) and `availableCount`,
which it puts at `-0x18` where ours has `-0x14`. `this` is at `-0x44`, so the gap
is well inside the named region: a declaration.

The trap is that retail's frame is the SAME SIZE as ours, so simply adding a
local is wrong - it grows the frame by 4 and the residual gets bigger, which is
what happened on the first attempt. The frame balances only if the fabricated
local that our reconstruction had put at the bottom goes away at the same time:

```c
    tag_message message;
    i32 unusedEventWord;      // the slot retail leaves at -0x14, bucket 7 puts it there
    i16 availableCount = ...;
    baseManager* recruitDlg = ...;
    i32 dialogResult;         // REMOVED - our own dead local, bucket 14, sat at the bottom
```

Same frame size, `availableCount` moves to `-0x18`, function EXACT. Both locals
are dead storage; only their buckets differ, and only one of them is retail's.

## Consequence for the missing-declaration lever

`docs/campaign-state.md` records that retail allocating storage we never declared
is a live class (the ours-larger negative result does not cover it). That stands,
but read the slot's position first: `dpProcessMessages` really was a missing
declaration (`unusedDpWord[2]`, and that function has no register arguments so
the divider question does not arise), while `dropListWidget::Draw` looked
identical from the frame size alone and was the opposite.

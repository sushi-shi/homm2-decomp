# inline-accessor-return-temp

**Symptom.** A function is structurally right, every field offset is right, and
yet the frame is 4 bytes bigger *per call site* of one inline member accessor,
so `this` and every compiler temp below it sit one slot deeper and the whole
body mismatches.

`dropListWidget::Read` (0x4cff40) read 252 differing bytes with `sub esp,0x24`
against retail's `sub esp,0x18` — exactly 3 x 4 bytes for its three uses of
`icon::Entries()`.

## What the bytes say

`icon::Entries()` is the inline accessor

```cpp
struct IconEntry* Entries(void) { return reinterpret_cast<struct IconEntry*>(m_data); }
```

Under `/Od /Ob1` VC6 does expand it, but it materialises the **return value into
a frame temp** and reloads it. Retail has no such temp: it dereferences the
member in place and completes the address with a single `add`.

```
retail (?Read@dropListWidget@@QAEXXZ)        ours, with m_icon->Entries()
-------------------------------------------- --------------------------------------------
8b 55 e8   movl -0x18(%ebp), %edx             8b 55 dc   movl -0x24(%ebp), %edx
8b 42 24   movl 0x24(%edx), %eax   ; m_icon   8b 42 24   movl 0x24(%edx), %eax   ; m_icon
8b 4d e8   movl -0x18(%ebp), %ecx             8b 48 12   movl 0x12(%eax), %ecx   ; m_data
0f bf 51 48 movswl 0x48(%ecx), %edx           89 4d e8   movl %ecx, -0x18(%ebp)  ; <- temp
6b d2 0d   imull $0xd, %edx, %edx             8b 55 dc   movl -0x24(%ebp), %edx
8b 52 12   movl 0x12(%edx), %edx  ; m_data    0f bf 42 48 movswl 0x48(%edx), %eax
03 d1      addl %ecx, %edx                    6b c0 0d   imull $0xd, %eax, %eax
89 55 fc   movl %edx, -0x4(%ebp)              8b 4d e8   movl -0x18(%ebp), %ecx  ; <- reload
                                              03 c8      addl %eax, %ecx
                                              89 4d fc   movl %ecx, -0x4(%ebp)
```

The tell is the pair `movl <member>, reg` / `movl reg, -0xN(%ebp)` immediately
followed by a reload of the same slot, plus a frame that is a clean multiple of
4 bytes too large.

## What made it match

Spell the member read directly and cast it, instead of calling the accessor:

```cpp
entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_closedContentFrame;
```

`dropListWidget::Read` 79.94% -> EXACT; the same edit is what let
`ProcessSelectDialog` (six such sites) close.

## Reading it the other way

This is the counterpart of the `/Ob1` inline-accessor lever, not a contradiction
of it: an inline accessor whose result feeds *directly* into one addressing mode
still costs nothing, but one whose result is an operand of further arithmetic
buys a frame slot. When retail's frame is exactly right and the arithmetic is
in-place, retail read the member; when retail carries the extra slot and the
reload, retail called the accessor.

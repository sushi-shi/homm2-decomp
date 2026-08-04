# `sub esp,8` with no local slots -> the working state is file statics

## Trigger

A large function opens with a frame that holds nothing but the two `__fastcall`
argument spills, and every value the body reads or writes goes through a
`IMAGE_REL_I386_DIR32` relocation instead of an `ebp`-relative slot.

```
0: 55              pushl  %ebp
1: 8b ec           movl   %esp, %ebp
3: 83 ec 08        subl   $0x8, %esp        <-- 8 bytes: ecx + edx spills, nothing else
6: 89 55 f8        movl   %edx, -0x8(%ebp)
9: 89 4d fc        movl   %ecx, -0x4(%ebp)
```

`/Od` never elides a local, so a 0x750-byte body with a 8-byte frame has **zero**
locals. Any reconstruction that declares locals is structurally wrong no matter
how the arithmetic reads.

## Retail vs ours

`?IconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z` (RVA 0xc14c0), loop head.

retail (`build/delink/BASE/Icon2b.c.obj`):

```
10e: 8b 0d 00 00 00 00   movl 0x0, %ecx      DIR32 const_0013611c   ; s_src
114: 33 d2               xorl %edx, %edx
116: 8a 11               movb (%ecx), %dl
118: 89 15 00 00 00 00   movl %edx, 0x0      DIR32 const_0013612c   ; s_run
11e: a1 00 00 00 00      movl 0x0, %eax      DIR32 const_0013611c
123: 83 c0 01            addl $0x1, %eax
126: a3 00 00 00 00      movl %eax, 0x0      DIR32 const_0013611c
12b: 0f be 0d 00 00 00 00 movsbl 0x0, %ecx   DIR32 const_0013612c
132: 85 c9               testl %ecx, %ecx
```

ours, before (locals `u8* data; IconEntry* entries; i32 command; u8* row;` ...):

```
3: 83 ec 0c            subl $0xc, %esp                 <-- frame too big
c: 8b 45 f8            movl -0x8(%ebp), %eax
f: 8b 48 12            movl 0x12(%eax), %ecx
12: 89 4d fc           movl %ecx, -0x4(%ebp)           <-- Entries() return temp
```

## What closed it

Delete every local and declare each one as a file static in the same TU:

```c
static u8* s_src;
static i32 s_run;
...
s_run = *s_src;
s_src++;
if (static_cast<i8>(s_run) < 0) {
```

Two consequences fall out of the same evidence and are needed for the match:

- **No inline accessor may appear in the expression.** `srcIcon->Entries() + frame`
  materialises the accessor's return value into a frame temp (the `-0x4` slot
  above) and evaluates the pointer before the index; the direct
  `reinterpret_cast<IconEntry*>(srcIcon->m_data) + frame` is a leaf and matches.
- **No helper with a local may be used for the cursor read.** `s_run = *s_src;
  s_src++;` as two statements is the only shape that both keeps the frame at 8
  and emits load-before-increment; `*s_src++` or an inline
  `ReadIconRleByte(u8*&)` reverses the order or needs a temp.

Closed eleven functions in one family: `IconToBitmap`, `FlipIconToBitmap`,
`IconToBitmapColorTable`, `FlipIconToBitmapColorTable`, `IconToBitmapYModify`,
`FlipIconToBitmapYModify`, `DimIconToBitmap`, `FlipDimIconToBitmap`,
`MonoIconToBitmap`, `FlipMonoIconToBitmap` (21-58% -> exact).

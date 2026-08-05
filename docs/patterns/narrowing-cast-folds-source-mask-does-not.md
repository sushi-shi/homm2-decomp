# narrowing-cast-folds-source-mask-does-not

**Symptom.** Retail materialises an `int` member/lvalue into a 32-bit register and
masks it (`mov r32,<mem>; and r32,0xffff`) before a 16-bit store or a 16-bit `or`.
Ours reads the same lvalue with a 16-bit access and never emits the mask - and no
`static_cast<i16>` / `static_cast<u16>` / `LOWORD` spelling changes that.

Measured on `widget::Main` (RVA 0xd4180), `iconWidget::Main` (RVA 0xbba10) and
`iconWidget::Read` (RVA 0xbb890).

## The rule

At `/Od`, VC6 folds a **conversion** to a narrow type into the memory access itself:
`(i16)p->intField` and `(u16)p->intField` both become `mov cx, word ptr [p+off]`,
because the low half of the dword is already the answer. `LOWORD` is
`((WORD)(l))` in VC6's `WINDEF.H`, so it folds too.

An arithmetic `& 0xffff` written in the source cannot fold: it is a real `int`
operation, so the compiler must load the dword and `and` it. **A single
`and r32,0xffff` in front of a narrow store is a source-level mask, not a cast.**

(The mirror of `hiword-macro-double-mask`: there the *doubled* mask names the
`HIWORD` macro. Here a *single* mask on an unshifted value names a bare `& 0xffff`.)

## The bytes

`widget::Main`, `WIDGET_COMMAND_SET_FLAGS`, the `m_flags |= ...` statement:

```
ours: m_flags |= static_cast<i16>(message.payload.widget.data.value);
--------------------------------------------------------------------
20e: 8b 45 f0              mov  eax, [ebp-0x10]        ; this
211: 8b 4d 08              mov  ecx, [ebp+0x8]         ; &message
214: 66 8b 50 16           mov  dx,  [eax+0x16]        ; m_flags
218: 66 0b 51 18           or   dx,  [ecx+0x18]        ; <-- folded 16-bit operand
21c: 8b 45 f0              mov  eax, [ebp-0x10]
21f: 66 89 50 16           mov  [eax+0x16], dx

retail
--------------------------------------------------------------------
214: 8b 45 08              mov  eax, [ebp+0x8]         ; &message   (RHS first)
217: 8b 48 18              mov  ecx, [eax+0x18]        ; data.value (32-bit!)
21a: 81 e1 ff ff 00 00     and  ecx, 0xffff            ; <-- the source mask
220: 8b 55 f0              mov  edx, [ebp-0x10]
223: 66 8b 42 16           mov  ax,  [edx+0x16]
227: 66 0b c1              or   ax,  cx
22a: 8b 4d f0              mov  ecx, [ebp-0x10]
22d: 66 89 41 16           mov  [ecx+0x16], ax
```

`iconWidget::Main`, `WIDGET_COMMAND_SET_FRAME`, the local's initialiser:

```
ours: i16 frame = static_cast<i16>(msg.payload.widget.data.value);
--------------------------------------------------------------------
451: 8b 45 08              mov  eax, [ebp+0x8]
454: 66 8b 48 18           mov  cx,  [eax+0x18]        ; <-- folded 16-bit load
458: 66 89 4d f4           mov  [ebp-0xc], cx

retail
--------------------------------------------------------------------
4e1: 8b 45 08              mov  eax, [ebp+0x8]
4e4: 8b 48 18              mov  ecx, [eax+0x18]
4e7: 81 e1 ff ff 00 00     and  ecx, 0xffff
4ed: 66 89 4d f4           mov  [ebp-0xc], cx
```

The same function proves the negative in the neighbouring arm: `m_x = value;`
(`widget::Main`, `WIDGET_COMMAND_SET_X`) really is a plain narrowing assignment and
retail emits `mov dx,[ecx+0x18]; mov [eax+0x18],dx` with **no** mask. So the mask is
not a compiler-inserted truncation for narrow stores; it only appears where the
source wrote one.

## What made it match

```cpp
m_flags |= static_cast<WidgetFlag>(
    message.payload.widget.data.value & WIDGET_FLAG_MASK      /* 0xffff */
);

H2_ENUM_STORAGE(WidgetFlag, i16) flags =
    static_cast<H2_ENUM_STORAGE(WidgetFlag, i16)>(
        message.payload.widget.data.value & WIDGET_FLAG_MASK
    );

i16 frame = static_cast<i16>(msg.payload.widget.data.value & FRAME_INDEX_MASK);
```

The `static_cast` that survives is only there for the strict-enum build; it is
byte-neutral in production once the `&` is present. `iconWidget::Read`'s
`m_orientation` store carries the same shape at byte width
(`movsx edx,ax; and edx,0xff; mov [eax+0x26],dl` = `ReadWord() & 0xff`).

`widget::Main` 98.61% -> EXACT, `iconWidget::Main` 99.61% -> EXACT,
`iconWidget::Read` 96.88% -> EXACT.

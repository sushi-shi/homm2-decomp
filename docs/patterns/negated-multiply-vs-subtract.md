# negated-multiply-vs-subtract

**Symptom.** One arithmetic statement, three instructions on each side, but ours
carries an `imul reg,-K` where retail has a `shl`/`imul reg,K` followed by a
`sub`. The result is the same number, the frame is the same, the operand
registers are the same - only the sign moved from the multiply to the join.

`textEntryWidget::textEntryWidget(short,...)` (RVA 0xd1d50), the
`TEXT_ENTRY_LAYOUT_INSET` arm.

## The bytes

```
ours: m_innerW = m_width + -HORIZONTAL_INSET_SIDE_COUNT * horizontalInset;
--------------------------------------------------------------------------
1c5: 8b 45 ec           movl    -0x14(%ebp), %eax     ; this
1c8: 0f bf 48 1c        movswl  0x1c(%eax), %ecx      ; m_width
1cc: 8b 55 3c           movl    0x3c(%ebp), %edx      ; horizontalInset
1cf: 6b d2 fe           imull   $-0x2, %edx, %edx     ; <-- signed constant
1d2: 03 ca              addl    %edx, %ecx            ; <-- join is an ADD
1d4: 8b 45 ec           movl    -0x14(%ebp), %eax
1d7: 66 89 48 3d        movw    %cx, 0x3d(%eax)

retail
--------------------------------------------------------------------------
1f7: 8b 45 ec           movl    -0x14(%ebp), %eax     ; this
1fa: 0f bf 48 1c        movswl  0x1c(%eax), %ecx      ; m_width
1fe: 8b 55 3c           movl    0x3c(%ebp), %edx      ; horizontalInset
201: d1 e2              shll    %edx                  ; <-- *2 as a shift
203: 2b ca              subl    %edx, %ecx            ; <-- join is a SUB
205: 8b 45 ec           movl    -0x14(%ebp), %eax
208: 66 89 48 3d        movw    %cx, 0x3d(%eax)
```

## The rule

VC6 `/Od` never rewrites the sign of a multiply. The constant in the `imul` is
the literal the source wrote, and the join opcode (`add` vs `sub`) is the
source's operator:

- `a + -K * v`  ->  `imul r, -K` + `add`
- `a - K * v`   ->  strength-reduced multiply + `sub`

The second consequence is the strength reduction. `/Od` still turns a
multiply by a small positive power of two into a shift (`* 2` -> `d1 e2`,
`shl reg,1`), but it will **not** do that for a negative constant, so a
`-2 *` spelling is visible twice over: the wrong opcode class *and* the wrong
join. Seeing `imul` with a negative immediate anywhere in a `/Od` object is
therefore near-proof that the source folded the sign into the constant when
retail did not.

## What made it match

```cpp
m_innerW = m_width - HORIZONTAL_INSET_SIDE_COUNT * horizontalInset;
```

`textEntryWidget::textEntryWidget(short,short,short,short,short,char*,char*,
short,char*,short,short,short,short,int,int)` 82.64% -> EXACT.

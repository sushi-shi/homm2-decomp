# `for (; c; step)` leaves a `jmp` over a step block; `while` does not

**Trigger.** The loop entry. If the instruction right before the loop test is
an unconditional **`jmp` forward over a short block**, and that block is
exactly the loop step, the source used a `for` with a third clause. A `while`
whose body *ends* with the same step falls straight into the test and keeps the
step inline at the bottom of the body.

The two shapes execute identically and differ by one `jmp` (2 bytes) plus the
position of the step relative to the last body statement — so a `for`/`while`
mix-up also **reorders** the tail: `for` puts the step after everything in the
body, `while` puts it wherever the source wrote it.

## Probe (VC6 SP5, `/nologo /c /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT`)

```cpp
void qC(int n, int step) { int y = 0; for (; y < n; y += step) { sink1(y); } }
void qD(int n, int step) { int y = 0; while (y < n) { sink1(y); y += step; } }
```

```
qC   for (; y < n; y += step)                  qD   while (y < n) { ...; y += step; }
---------------------------------------------  ---------------------------------------------
c7 45 fc 00 00 00 00  movl $0x0, -0x4(%ebp)    c7 45 fc 00 00 00 00  movl $0x0, -0x4(%ebp)
eb 09                 jmp  <test>              (falls through)
8b 45 fc              movl -0x4(%ebp), %eax    8b 45 fc   movl -0x4(%ebp), %eax   <test>
03 45 f4              addl -0xc(%ebp), %eax    3b 45 f8   cmpl -0x8(%ebp), %eax
89 45 fc              movl %eax, -0x4(%ebp)    7d 13      jge  <end>
8b 4d fc              movl -0x4(%ebp), %ecx    8b 4d fc   movl -0x4(%ebp), %ecx
3b 4d f8              cmpl -0x8(%ebp), %ecx    e8 ..      calll sink1
7d 0a                 jge  <end>               8b 4d fc   movl -0x4(%ebp), %ecx
8b 4d fc              movl -0x4(%ebp), %ecx    03 4d f4   addl -0xc(%ebp), %ecx
e8 ..                 calll sink1              89 4d fc   movl %ecx, -0x4(%ebp)
eb e5                 jmp  <step>              eb e5      jmp  <test>
```

## Retail, `font::DrawBoundedString` (RVA 0xc4b70)

The statement before the loop is `m_suppressDraw = 0`; the test follows it with
no `jmp` between, and the height step sits in the body ahead of the
`lineWidth` reset:

```
15e5: 8b 4d bc              movl  -0x44(%ebp), %ecx    ; m_suppressDraw = 0
15e8: c7 41 18 00 00 00 00  movl  $0x0, 0x18(%ecx)
15ef: 8b 55 d4              movl  -0x2c(%ebp), %edx    ; <test>   pos
15f2: 3b 55 e0              cmpl  -0x20(%ebp), %edx    ;          < len
15f5: 0f 8d fd 00 00 00     jge   0x16f8
      ...
16e0: 8b 4d bc              movl  -0x44(%ebp), %ecx    ; yPosition += m_height
16e3: 8b 55 f0              movl  -0x10(%ebp), %edx
16e6: 03 51 10              addl  0x10(%ecx), %edx
16e9: 89 55 f0              movl  %edx, -0x10(%ebp)
16ec: c7 45 d8 00 00 00 00  movl  $0x0, -0x28(%ebp)    ; lw = 0
16f3: e9 f7 fe ff ff        jmp   0x15ef
```

Ours (a `for` with `yOffset += m_height` in the third clause) had the leading
`eb 0c jmp <test>`, a separate step block, and emitted `lineWidth = 0` *before*
the step.

## What made it match

```cpp
while (pos < len && line[pos] != 0 && (yPosition + m_height <= h || yPosition == 0)) {
    ...
    DrawStringExecute(line, xPosition + x, yPosition + y, drawMode, x, y, w, h);
    yPosition += m_height;
    lw = 0;
}
```

`font::DrawBoundedString` -> EXACT. Compare `while1-literal-test.md`, which is
the same question for an infinite loop (`while (1)` vs `for (;;)`).

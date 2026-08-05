# Where the constant sits in an index expression is byte-visible

**Trigger.** Retail addresses an array element with a **non-zero SIB
displacement** (`movb $K, 0x1(%eax,%edx)`) where ours computes the same address
with an explicit `add`/`sub` of the constant and a zero displacement
(`addl $0x1, %eax` ... `movb $K, (%edx,%eax)`), one extra instruction per site.

`/Od` evaluates an additive index chain strictly left to right and folds a
trailing integer literal into the addressing mode — but only if it is *last*.
Put the literal earlier and it becomes a real add before the other terms are
even loaded.

This is the same doctrine as `flat-index-grouping-is-not-a-trick.md`: the term
order of an index expression is retail evidence, not style.

## Probe (VC6 SP5, `/nologo /c /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT`)

```cpp
void pC(char* line, int pos, int* position) { line[pos + 1 - *position] = '-'; }
void pD(char* line, int pos, int* position) { line[pos - *position + 1] = '-'; }
```

```
pC   line[pos + 1 - *position]                pD   line[pos - *position + 1]
--------------------------------------------  --------------------------------------------
8b 45 f8        movl  -0x8(%ebp), %eax        8b 45 08        movl  0x8(%ebp), %eax
83 c0 01        addl  $0x1, %eax                                (position loaded FIRST)
8b 4d 08        movl  0x8(%ebp), %ecx         8b 4d f8        movl  -0x8(%ebp), %ecx
2b 01           subl  (%ecx), %eax            2b 08           subl  (%eax), %ecx
8b 55 fc        movl  -0x4(%ebp), %edx        8b 55 fc        movl  -0x4(%ebp), %edx
c6 04 02 2d     movb  $0x2d, (%edx,%eax)      c6 44 0a 01 2d  movb  $0x2d, 0x1(%edx,%ecx)
```

Note the *second* side effect: which operand is materialised first flips too.
For `a - *p` VC6 loads `p` into a register before `a`; the leading `+ 1` in
`pC` forces `a` out first instead. So one misplaced literal moves three rows.

## Retail, `font::ExtractLine` (RVA 0xc3b20, hyphen insertion at 0xebb)

```
ebb: 8b 4d 10              movl  0x10(%ebp), %ecx     ; position
ebe: 8b 55 f4              movl  -0xc(%ebp), %edx     ; curPos
ec1: 2b 11                 subl  (%ecx), %edx
ec3: 8b 45 0c              movl  0xc(%ebp), %eax      ; line
ec6: c6 44 10 01 2d        movb  $0x2d, 0x1(%eax,%edx)
ecb: 8b 4d 10              movl  0x10(%ebp), %ecx
ece: 8b 55 f4              movl  -0xc(%ebp), %edx
ed1: 2b 11                 subl  (%ecx), %edx
ed3: 8b 45 0c              movl  0xc(%ebp), %eax
ed6: c6 44 10 02 00        movb  $0x0, 0x2(%eax,%edx)
```

## What made it match

```cpp
line[curPos - *position + 1] = '-';       // was line[curPos + 1 - *position]
line[curPos - *position + 2] = 0;         // was line[curPos + 2 - *position]
```

Five sites in `font::ExtractLine`. The already-exact neighbours in the same
function carried the rule the whole time — `line[wStart - *position - 1]`
assembles as `movb $0x0, -0x1(%ecx,%eax)`, displacement `-1`, and
`line[curPos - *position]` as a plain `(reg,reg)` — which is the cheapest way
to spot the convention a function uses: read the sites that already agree.

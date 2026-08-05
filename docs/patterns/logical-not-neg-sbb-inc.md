# `neg`/`sbb`/`inc` is `!x` — and its operand load names the flag's width

**Trigger.** A global "toggle" flag is flipped and retail spends five
instructions on it:

```
33 c0                xorl %eax, %eax
a0 00 00 00 00       movb  gbEveryOtherCycle, %al     <- /G5 zero-extension
f7 d8                negl  %eax
1b c0                sbbl  %eax, %eax
40                   incl  %eax
a2 00 00 00 00       movb  %al, gbEveryOtherCycle
```

Ours, from the arithmetic spelling `g = 1 - g;` on an `i32` global, emits a
completely different pair:

```
b8 01 00 00 00       movl  $0x1, %eax
8b 0d 00 00 00 00    movl  gbEveryOtherCycle, %ecx
2b c1                subl  %ecx, %eax
a3 00 00 00 00       movl  %eax, gbEveryOtherCycle
```

`negl` sets CF = (operand != 0); `sbbl %eax,%eax` turns that into 0/-1;
`incl` turns it into 1/0. The triple is VC6's canonical lowering of the
**logical negation operator** applied to an integer, and nothing else
produces it. `1 - x` is an ordinary subtraction and never does.

## The same block also pins the flag's TYPE

Two independent facts fall out of the operand encodings, and both are needed
before the block will match:

- the read is `xorl %eax,%eax; movb g,%al` — `/G5`'s zero-extension idiom,
  emitted only for an **unsigned one-byte** value promoted to `int`
  (see [unsigned-byte-flag-zero-extend](unsigned-byte-flag-zero-extend.md));
  a signed `i8`/`char` gives `movsbl`, an `i32` gives a plain `movl`;
- the write-back is `movb %al, g`, so the storage is one byte wide.

So the retail declaration is `u8`, not the `b32` the PoL 2.0 reconstruction
carries. Changing the header alias is not cosmetic: with `b32` the `!` form
still mismatches (dword load, dword store).

## What made it match

```cpp
extern u8 gbEveryOtherCycle;                    // was b32
...
    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT)
        gbEveryOtherCycle = true;
    else
        gbEveryOtherCycle = !gbEveryOtherCycle; // was `1 - gbEveryOtherCycle`
```

`CycleColors` (0xb6b80, BASE/WINMGR) 80.72% -> EXACT with this plus the
guard-clause and `goto` restructuring.

**Corollary for the census.** When retail *tests* the same flag it emits
`xorl %ecx,%ecx; movb g,%cl; testl %ecx,%ecx`, i.e. the zero-extension
survives into the comparison. A `cmpb $0, g` instead would mean the source
compared the byte directly without promoting it.

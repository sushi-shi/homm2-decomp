# `shr` where the natural expression gives `sar` -> an unsigned conversion

## Trigger

A right shift of a value that the reconstruction models as `i32` comes out as
`shrl` (logical) in retail, while our compile emits `sarl` (arithmetic) for the
same source. Nothing else in the expression differs.

## Retail

`?IconToBitmapColorTable@@YIXPAVicon@@PAVbitmap@@HHHHHHHHHPAEH@Z` (RVA 0xc0ca0)
selecting the dim palette level:

```
3e4: 8b 0d 00 00 00 00   movl 0x0, %ecx      DIR32 const_001360f8   ; s_run
3ea: 83 e1 3c            andl $0x3c, %ecx
3ed: c1 e9 02            shrl $0x2, %ecx                            ; LOGICAL
3f0: c1 e1 08            shll $0x8, %ecx
3f3: 81 c1 00 00 00 00   addl $0x0, %ecx     DIR32 const_000f8d10   ; uDimPal
3f9: 89 0d 00 00 00 00   movl %ecx, 0x0      DIR32 const_001360dc   ; s_dimPal
```

`s_run` is provably signed elsewhere in the same function - every
`s_x + s_run` comparison uses `jle`/`jg`, not `jbe`/`ja`:

```
233: 8b 05 ... movl s_x, %eax
238: 03 05 ... addl s_run, %eax
23e: 3b 45 18  cmpl 0x18(%ebp), %eax
241: 7e ...    jle  ...                       ; signed
```

so `(s_run & 0x3c) >> 2` on an `i32` would have to be `sarl`.

## What closed it

An explicit widening to unsigned at that one site:

```c
s_dimPal = reinterpret_cast<u8*>(uDimPal)
           + (static_cast<u32>(s_run & ICON_RLE_DIM_LEVEL_MASK) >> 2)
                 * DIM_PALETTE_COLOR_COUNT;
```

`static_cast<u32>` applied to the masked value (not to `s_run` itself, and not
to the whole expression) is what puts the `andl` before a `shrl`. The sibling
decoders that fold the same level into a single `* 0x40` keep the signed
spelling and emit only `shll $0x6` - the cast is needed exactly where retail
splits the scale into `>> 2` then `<< 8`.

`IconToBitmapColorTable` 27.59% -> exact on the first compile with this
spelling.

Compare `cast-widens-relational-to-unsigned.md`: same class of evidence (an
unsigned conversion the reconstruction dropped), different instruction family
(`jb`/`jbe` there, `shr` here).

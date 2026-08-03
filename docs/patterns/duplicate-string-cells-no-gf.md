# Duplicate string cells inside a unit block: that TU compiled without `/Gf`

**Trigger.** The delinked target object names one literal's cells
`$anon_str_<contenthash>_0 .. _N` while our object has a single symbol for the same
content. Retail kept one `.rdata` cell **per occurrence** - i.e. the unit was compiled
without string pooling. Flip that unit to the `base_nogf` profile in
`config/units.toml`; it is code-byte neutral and fixes the relocation identity.

Measured on `SOURCE/ADVMGR`.

## The evidence

`advManager::Open` (RVA 0x12f2) references `"advmice.mse"` twice, in the two arms of one
`if`:

```
retail                                                    ours
--------------------------------------------------------- ---------------------------------------------------------
68 00 00 00 00   push  $anon_str_6a2c7177..._0            68 00 00 00 00   push  $anon_str_6a2c7177..._0
...                                                       ...
68 00 00 00 00   push  $anon_str_6a2c7177..._1            68 00 00 00 00   push  $anon_str_6a2c7177..._0
                       ^^^ second cell, same content                            ^^^ pooled: one cell
```

Same code bytes; different reloc target. The image confirms it - `"advmice.mse"` occurs
9 times inside ADVMGR's block (file offsets 0xef218, 0xef224, 0xefed8, 0xeff3c, 0xf00b4,
0xf0134, 0xf015c, 0xf0174, 0xf0238) and the TU has exactly 9 source occurrences.
`"scroll.icn"` is duplicated the same way inside `Open` alone.

## What made it match

```toml
[[unit]]
unit = "SOURCE/ADVMGR"
source = "src/SOURCE/ADVMGR.cpp"
flags = "base_nogf"     # was "base"
```

`homm2 relocs 0x12f2` went `only-base=2` -> `only-base=0`; unit fuzzy and exact counts
were bit-for-bit unchanged (89.008194% / 41 of 94 before and after), so the flag only
moves the literal pool.

VC6 emits string literals as content-named COMDATs under `/Gf`, which folds identical
literals TU-wide; without `/Gf` each occurrence gets its own `$SG` cell. That is the
whole mechanism - there is no source spelling that unpools a literal.

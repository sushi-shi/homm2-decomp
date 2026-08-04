# switch-table-tail-in-function-span

**Symptom.** Every ordinary byte of a `switch`-carrying function matches, the
frame matches, the relocations match - and objdiff still reports 96-99%.

`army::DrawToBuffer` (0x19095) was byte-for-byte identical over the whole
claimed span and read 96.66%.

## What the bytes say

VC6 emits the jump table and the byte index map for a dense `switch` into
`.text` immediately after the function's `ret`. The dispatch names them with
displacements relative to the function symbol:

```
retail                                     ours (before)
------------------------------------------ ------------------------------------------
8a 81 ec 06 00 00  movb 0x6ec(%ecx), %al   8a 81 ec 06 00 00  movb 0x6ec(%ecx), %al
ff 24 85 e4 06 00 00 jmpl *0x6e4(,%eax,4)  ff 24 85 e4 06 00 00 jmpl *0x6e4(,%eax,4)
```

`0x6e4` is the *claimed size* of the function in `config/retail_functions.csv`
(1764). The tables therefore start exactly where the claim ends, and the two DIR32
sites for them are already in `config/delink_relocs.tsv` (`0x19779`, `0x1977d`).
The gap to the next claimed function is exactly their size:

```
next function (army::Wince)  0x19790
DrawToBuffer + claimed size  0x19095 + 0x6e4 = 0x19779
gap                          23 bytes = 2 jump-table entries + 15 index bytes
```

Under the short claim the delinker drops those 23 bytes, so the target object's
`?DrawToBuffer@...` symbol is 23 bytes shorter than ours and the tail never pairs.

## What made it match

Widen the source marker to the real span - the claimed size plus the compiler's
own tables, i.e. up to the next function's RVA:

```cpp
VA(0x00419095, 0x6fb)          // was 0x6e4
void army::DrawToBuffer(i32 x, i32 y, i32 effectsOnly) {
```

`army::DrawToBuffer` 96.66% -> EXACT. Same refinement on `army::DoAttack`
(0x1bddb, `0x13f6` -> `0x1445`; 79 bytes of tables) took it 80.17% -> 98.96%.

This is a size-claim refinement, not a new function: the tables are the
compiler's storage for the body that already owns the span. It needs
`homm2 redelink` to take effect. Check for it whenever
`grep VA(0x` shows a gap between a `switch`-carrying function's end and the next
claimed RVA.

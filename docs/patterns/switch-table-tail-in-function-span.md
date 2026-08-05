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

## Variant: the dense byte-index table, and the 0.00% symptom

A *dense* `switch` (VC6's `sub imm; cmp imm; ja default; movzx idx; jmp *table`)
stores **two** tables: 4 bytes per distinct target, then one index byte per case
value in the range. Both are addressed as displacements off the function symbol,
so the dispatch names the exact sizes:

```
42c: 8a 90 1c 03 00 00     movb  0x31c(%eax), %dl      DIR32 ?Main@iconWidget@@...
432: ff 24 95 08 03 00 00  jmpl  *0x308(,%edx,4)       DIR32 ?Main@iconWidget@@...
```

`0x308` is the claimed size (where the jump table starts), `0x31c` is where the
index table starts, so the jump table is `0x31c-0x308 = 0x14` = 5 entries and the
index table is `cmp` bound + 1 = `0x39` bytes. Reviewed size = `0x308 + 0x14 +
0x39 = 0x355`; the 11 bytes of `0xcc` up to the next function are handled by the
`$fnpad` boundary symbol (`docs/data-symbol-normalization.md`).

**When the truncated span is long enough to swallow the next function's entry
point, objdiff reports no score at all** - `homm2 sema match` prints `--` and the
report's `fuzzy_match_percent` is `null`, not a low number. `iconWidget::Main`
read `--`, not 0.4%, because the delinker gave it exactly `0x308` bytes and put
`?Draw@iconWidget@@UAEXXZ` on top of its own jump table.

Three spans in the widget-dispatch family closed this way:

| function | RVA | claimed | reviewed | tables |
| :-- | :-- | :-- | :-- | :-- |
| `iconWidget::Main` | 0xbba10 | 0x308 | 0x355 | 5 entries + 0x39 index |
| `border::Main` | 0xcb390 | 0x199 | 0x1de | 3 entries + 0x39 index |
| `widget::Main` | 0xd4180 | 0x35d | 0x3b5 | 7 entries + 0x3c index |

The index-table *contents* are matching evidence in their own right: they encode
the case-value -> target-slot map, so a byte-identical index table with shifted
jump-table addends means the case labels are right and only the body layout moved.
## Stop at the tables, not at the next function: the linker pad is NOT the span

The "widen to the next claimed RVA" recipe overshoots whenever LINK.EXE inserted
alignment padding between the tables and the next function. `heroWindow::heroWindow(
i32, i32, char*)` (0xba700) is the witness: the dispatch reads
`jmpl *0x6aa(,%edx,4)` and `movb 0x6c6(%eax)`, so the tails are

```
0x6aa  7 dword entries   (6 cases + default)   -> 0x6c6
0x6c6  0x41 index bytes  (values 0 .. 0x40)    -> 0x707
0x707  6 dword entries   (0x201 .. 0x206)      -> 0x71f
```

and the next claimed function `heroWindow::Open` is at 0xbae20 = +0x720. Claiming the
full gap (`VA(0x004ba700, 0x720)`) leaves exactly ONE differing byte and reads 99.78%:

```
ours   .. 06 00 00 2e 05 00 00 | 55 8b ec ..     (next function follows immediately)
retail .. 06 00 00 2e 05 00 00 | cc | 55 8b ec .. (0xcc = LINK.EXE inter-function pad)
```

`VA(0x004ba700, 0x71f)` — the compiler's own output size, tables included, pad
excluded — is EXACT. Compute the span from the dispatch displacements and the table
geometry, then check the residue against the next RVA; a 1..15-byte remainder of
`0xcc` (or `0x90`) is padding and belongs to no function.

## Variant: two nested switches leave two table groups

The tail is not one block. Every dense `switch` in the function contributes its
own tables, in the order the switches are *emitted*, and the span is named by
the **last** group, not the first.

`textEntryWidget::Main` (RVA 0xd22a0, claimed `0xa85`) carries two of them - the
scan-code switch in the edit loop and the numeric-keypad switch nested inside its
`default` arm:

```
a45: 8b 95 f8 ef ff ff     movl  -0x1008(%ebp), %edx
a4b: 33 c9                 xorl  %ecx, %ecx
a4d: 8a 8a 99 0a 00 00     movb  0xa99(%edx), %cl      DIR32 ?Main@textEntryWidget@@...
a53: ff 24 8d 85 0a 00 00  jmpl  *0xa85(,%ecx,4)       DIR32 ?Main@textEntryWidget@@...
...
d0d: 8b 95 f4 ef ff ff     movl  -0x100c(%ebp), %edx
d13: ff 24 95 ec 0a 00 00  jmpl  *0xaec(,%edx,4)       DIR32 ?Main@textEntryWidget@@...
```

Read straight off the displacements:

```
+0xa85  jump table  #1   (0xa99-0xa85)/4 = 5 targets
+0xa99  index table #1   `cmp $0x52` bound + 1 = 0x53 bytes
+0xaec  jump table  #2   12 targets (`sub $0x47; cmp $0xb`), no index table
+0xb1c  end
```

Stopping at `0xaec` - the end of the first switch's tables - loses 48 bytes and
leaves the second table unpaired. The reviewed size is `0xb1c`; the image
confirms it, with `cc cc cc cc` filling 0xd2dbc..0xd2dbf up to `?Draw@...` at
0xd2dc0.

Two rules follow. A *dense* switch (`sub`/`cmp`/`ja`/`movb idx`/`jmpl`) emits an
index table; a switch whose case values are already contiguous
(`INPUT_SCAN_NUMPAD_7..NUMPAD_0` = 0x47..0x52 here) emits the jump table alone.
And the tables sit in emission order, so **walk every `jmpl *disp(,reg,4)` in the
function and take the largest displacement** before claiming a size.

`textEntryWidget::Main` 55.97% -> EXACT (`0xa85` -> `0xb1c`).

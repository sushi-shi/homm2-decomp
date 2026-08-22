# `SOURCE/wingraph` data closure

After a forced redelink and full build, all compared wingraph payload sections are
byte-exact:

- `.CRT$XCU`: 4/4 bytes
- `.bss`: 1,312/1,312 bytes
- `.data`: 3,804/3,804 bytes
- total data: 5,120/5,120 bytes

The linked relocation audit resolves every wingraph-owned string and ordinary data
target. Its eight remaining diagnostics are import-address cells for
`ClientToScreen`, `OffsetRect`, `MessageBeep`, `CreatePalette`, and `AnimatePalette`.
Those belong to the project-wide import identity model, not this TU's data ownership.

The raw whole-object command

```sh
homm2 data-relocs --unit SOURCE/wingraph \
  build/objdiff/base/SOURCE/wingraph.obj \
  build/delink/SOURCE/wingraph.c.obj \
  -o build/wingraph-data-relocs.json
```

retains one `.CRT$XCU` owner/owner-offset classification residual. Candidate and target each
contain one relocation at offset zero to the same compiler initializer body; the target
uses the reviewed compiler-function owner while the candidate COFF cell is owned by the
local CRT section symbol. The source data payload, site, relocation type, and destination
are closed. The remaining name/owner normalization belongs to compiler metadata.

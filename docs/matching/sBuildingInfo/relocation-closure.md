# `sBuildingInfo` relocation closure

`SOURCE/TOWNMGR` already emitted the retail `sBuildingInfo` payload.  Its
candidate `.data` section had no relocations, but the delinked retail object
carried seven `DIR32` records within the packed numeric table:

```
0x118c0c  0x118c10  0x118cb4  0x118d44
0x118dc0  0x118fc8  0x119038
```

`SBuildingInfo` is a packed nine-byte record containing one `i8` and four
`i16` fields.  At each site, an aligned four-byte window across adjacent
numeric fields happens to form an address inside the retail image.  For
example, the bytes at `0x118c0c` are `00 00 45 00`, or `0x00450000` as a
little-endian dword.  The generic image-only relocation sweep therefore
classified these values as pointers and the delinker replaced them with
symbol-relative addends.

The candidate COFF has no `.data` relocations, the table's typed source has no
pointer fields, and removing the seven false-positive sites makes the retail
and candidate `.data` payloads byte-identical.  The sites are retained in
`config/delink_reloc_exclusions.tsv` so future generated sweeps cannot
reintroduce them.

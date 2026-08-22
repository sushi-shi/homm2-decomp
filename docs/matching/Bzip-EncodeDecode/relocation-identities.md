# EncodeData / DecodeData relocation identities

`EncodeData` (RVA `0x000ca980`) and `DecodeData` (RVA `0x000cabe0`) were
already byte-shape and CFG exact. Their residuals consisted only of the same
four relocation identities: a private `"H2C"` cell and calls to the CRT
`_open`, `_write`, and `_close` APIs.

The retail PE stores the two independent `H2C\0` allocations at VAs
`0x00520100` and `0x00520114`; the function-relative relocation sites select
the former from `EncodeData` and the latter from `DecodeData`. Those cells are
now recorded as separate `DATA_COMPGEN` identities.

The retail function assignments at RVAs `0x000d8c6e`, `0x000d8760`, and
`0x000d8b8e` are backed by VC6 `LIBCMT.LIB` members `open.obj`, `write.obj`,
and `close.obj`. Direct archive symbol-table inspection proves their public
COFF spellings are `__open`, `__write`, and `__close`. Other exact callers of
the same linked entries use the synthetic target's single-underscore owners, so
the archive spellings are recorded as reviewed same-RVA aliases and applied
only at paired sites. No source declaration changes are needed.

Disposition: retain the exact physical literal owners and exact archive public
symbols. After a clean redelink and normalized build, require complete ordered
relocation streams and exact reviewed spans for both functions.

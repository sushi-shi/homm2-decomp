# `SOURCE/Wsnetwin` data placement closure

## Residual

`SOURCE/Wsnetwin` already had exact initialized data and all 12 functions
exact, but objdiff matched only 1,342 / 2,071 data bytes. The entire 729-byte
residual was the `.bss` section: its candidate and delinked extents were both
`0x2d9`, while two ordinary global definitions lacked Buka `DATA` claims.
Without those reviewed semantic boundaries, Vostok could preserve the
uninitialized extent but could not reproduce the target symbol topology.

## Placement evidence

`iAddrLen` is the four-byte global immediately after the anchored `iRc` at
`0x005344e8` and immediately before the anchored `gIn_addrIP` at `0x005344f0`.
Its candidate COFF offset is `.bss+0x4`, proving `DATA(0x005344ec)`.

`saddr_remote` is the 16-byte `sockaddr_in` immediately after the anchored
`cWSTextBuffer` extent and immediately before the anchored `bHostFound` at
`0x005347b0`. Its candidate COFF offset is `.bss+0x2b8`, proving
`DATA(0x005347a0)` and the full extent through `0x005347af`.

These are ordinary source globals. No `DATA_COMPGEN` identity or raw-offset
substitute is involved.

## Result

The source now carries the two missing placement claims:

```cpp
DATA(0x005347a0) struct sockaddr_in saddr_remote;
DATA(0x005344ec) i32 iAddrLen;
```

After `homm2 redelink` and `homm2 build`:

- `SOURCE/Wsnetwin` is 2,071 / 2,071 data bytes exact;
- `.CRT$XCU` is 4 / 4, `.bss` is 729 / 729, and `.data` is 1,338 / 1,338;
- all 12 / 12 functions remain exact;
- candidate and target define `iAddrLen` at `.bss+0x4` and `saddr_remote` at
  `.bss+0x2b8`, with the same external storage class;
- campaign-wide data comparison moves from 290,529 / 291,980 bytes (99.503%,
  72 / 97 data-bearing units exact) to 291,258 / 291,980 bytes (99.753%,
  73 / 97 units exact).

The focused normalized relocation audit reports zero policy errors, zero
provenance diagnostics, and zero parse errors. Its single residual is the
pre-existing `.CRT$XCU` owner presentation difference for `_$E19`: both sides
have one `DIR32` relocation at offset zero to the same symbol with addend zero,
but the base attributes the site to the preceding function boundary while the
target attributes it to `_$E19`. This compiler-metadata diagnostic does not
represent an unmatched source datum.

## Verification

- forced `homm2 redelink` after adding the source placement claims;
- `homm2 build` and refreshed `homm2 status`;
- normalized section-size and symbol-offset comparison;
- focused `homm2 data-relocs --homm2-root .` review;
- `python3 -m unittest scripts.homm2.build.test_annotated_data`;
- `git diff --check`.

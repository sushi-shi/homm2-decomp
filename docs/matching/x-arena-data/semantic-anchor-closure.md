# `SOURCE/x_arena` semantic-anchor data closure

## Residual

`SOURCE/x_arena` had all 5 functions exact and exact `.CRT$XCU`/`.bss`, but
only 24 / 249 total data bytes matched. The candidate `.data` section was
`0xdd` bytes; the delinked target was `0xe1` bytes and diverged at `+0xd0`,
where the candidate emitted the second copy of `"xprimary.icn"`.

The source compiler symbol for that occurrence is `$SG55805` at
`.data+0xd0`, with meaningful size `0xd`. The reviewed retail relocation at
`UpdateArenaIcons+0xd6` targets RVA `0x0011b0a8`, whose 13 bytes are exactly
`"xprimary.icn\0"`. This is an occurrence-specific placement proof even
though the same payload also occurs at RVA `0x0011b088` in `DoArenaDialog`.

## Cause and correction

The relocation proof collector checked candidate definitions before public
retail identities. Consequently the same-TU public globals `arenaWinPtr`,
`skillWidget`, and `choice` were recorded as private placement proposals
instead of validation anchors. The EH relocation omissions made the complete
candidate/retail site sequences differ, so the absence of a counted public
anchor caused the exact `$SG55805` site proof to be discarded.

Public identity now takes precedence: a symbol with an authoritative retail
public RVA validates the pairing even when that symbol is also defined in the
candidate TU. Only definitions without public identity become inferred private
allocations. A regression test covers both identical-site and validated-
sequence proof paths.

The regenerated manifest therefore enrolls `$SG55805` automatically as:

```text
SOURCE\x_arena.c  RVA 0x0011b0a8  size 0xd  .data+0xd0
candidate-COFF-string:aligned-relocation-addend
```

No source edit and no additional `DATA_COMPGEN` claim are required.

## Result

After forced `homm2 redelink` and `homm2 build`:

- `SOURCE/x_arena` is 245 / 245 total data bytes exact;
- `.CRT$XCU` is 4 / 4, `.bss` is 20 / 20, and `.data` is 221 / 221;
- both normalized `.data` payloads have SHA-256
  `f3e36cf21a2b84fd277cb61ad59c4c03369af2de1cf4bff8865b0b6cc5a92b8e`;
- all 5 / 5 functions remain exact;
- automatic compiler-string enrollment increases from 1,105 to 1,106 rows;
- campaign-wide data comparison moves from 291,258 / 291,980 bytes (99.753%,
  73 / 97 data-bearing units exact) to 291,479 / 291,976 bytes (99.830%,
  74 / 97 units exact).

The focused relocation audit has zero policy, provenance, and parse errors.
Its seven residual sites are independent compiler metadata: one pre-existing
`.CRT$XCU` owner-boundary presentation difference and six candidate-only
`.xdata$x` EH records. They do not represent unmatched source data.

## Verification

- 323 tests from `python3 -m unittest discover -s scripts/homm2/build` pass;
- forced `homm2 redelink` and full normalized rebuild;
- exact normalized `.data` size, byte comparison, and SHA-256;
- focused `homm2 data-relocs --homm2-root .` review;
- refreshed `homm2 status`;
- `git diff --check`.

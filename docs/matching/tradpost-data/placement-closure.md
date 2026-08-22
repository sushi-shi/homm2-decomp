# `SOURCE/tradpost` data placement closure

## Residual

`SOURCE/tradpost` had all 8 functions exact and identical section extents, but
only 465 / 626 data bytes matched. The entire 161-byte residual was `.bss`:
one ordinary global definition had no Buka `DATA` claim, so Vostok could retain
the physical uninitialized extent but not its complete semantic symbol
topology.

## Placement evidence

The candidate defines `maxUnitsToTrade` at `.bss+0x90`. The `.bss` stream
begins with the anchored `fTradingPostEfficiency` at `0x00533ed8`, which gives
the same retail address `0x00533f68`. Its immediate anchored neighbors provide
an independent extent proof:

- `rightResource` is at `.bss+0x8c`, RVA `0x00533f64`;
- `bIsMarketPlace` is at `.bss+0x94`, RVA `0x00533f6c`.

The four-byte cell between them is therefore exactly `maxUnitsToTrade`, with
no padding or overlap. This is an ordinary source global, not compiler-
generated data.

## Result

The retained reconstruction adds the missing placement claim:

```cpp
DATA(0x00533f68) i32 maxUnitsToTrade;
```

After forced `homm2 redelink` and `homm2 build`:

- `SOURCE/tradpost` is 626 / 626 total data bytes exact;
- `.CRT$XCU` is 4 / 4, `.bss` is 161 / 161, `.data` is 449 / 449, and
  `.rdata` is 12 / 12;
- candidate and target both define `maxUnitsToTrade` at `.bss+0x90` with
  external storage;
- both normalized `.data` payloads have SHA-256
  `4beb17a09b21b3017636d562d78edfe6fc859aa8bbe6e078df47bd893f35230d`;
- all 8 / 8 functions remain exact;
- campaign-wide data comparison moves from 291,479 / 291,976 bytes (99.830%,
  74 / 97 data-bearing units exact) to 291,640 / 291,976 bytes (99.885%,
  75 / 97 units exact).

The focused relocation audit has zero policy, provenance, and parse errors.
Its four residual sites are independent compiler metadata: one pre-existing
`.CRT$XCU` owner-boundary presentation difference and three candidate-only
`.xdata$x` EH records.

## Verification

- forced `homm2 redelink` after adding the source placement claim;
- `homm2 build` and refreshed `homm2 status`;
- exact normalized `.data` byte comparison and SHA-256;
- normalized `.bss` section-size and symbol-offset comparison;
- focused `homm2 data-relocs --homm2-root .` review;
- `python3 -m unittest scripts.homm2.build.test_annotated_data`;
- `git diff --check`.

# `SOURCE/kbwin` data placement closure

## Residual

The candidate and delinked `kbwin` objects already had the same physical
section topology:

- `.CRT$XCU`: `0x4` bytes;
- `.bss`: `0x450` bytes;
- `.data`: `0xef` bytes.

Nevertheless, objdiff matched only 4 / 1,347 data bytes because three ordinary
global definitions had no Buka `DATA` claims. Vostok preserved their physical
space as manifest gaps but could not emit their semantic symbols or, for the
initialized object, its retail payload.

## Placement evidence

`lLastCycleColorsTickCount` and `bRestartMusic` are four-byte globals between
the anchored `lLastGTimerTickCount` and `lLastGetMessage` definitions. Their
candidate `.bss` offsets are `0x43c` and `0x440`; the surrounding anchored
offsets are `0x438` and `0x444`. This proves the Buka VAs `0x00526ce4` and
`0x00526ce8`. The same declaration order and values occur in the predecessor
layout.

`iLastMusic` is the four-byte initialized object at candidate `.data+0x24`.
The retail PE maps `.data` RVA and file offsets directly in this band, and bytes
at RVA `0x115800` are `ff ff ff ff`, followed immediately by the localized
`"Герои II"` compiler string at `0x115804`. This independently proves both
`DATA(0x00515800)` and the source initializer `-1`.

An exploratory removal of `iLastMusic` was rejected: VC6 moved the following
string pool four bytes earlier and reduced `.data` from `0xef` to `0xeb`.

## Result

The source now carries three ordinary data claims:

```cpp
DATA(0x00526ce4) i32l lLastCycleColorsTickCount = 0;
DATA(0x00526ce8) i32 bRestartMusic = 0;
DATA(0x00515800) i32 iLastMusic = -1;
```

After `homm2 redelink` and `homm2 build`:

- `SOURCE/kbwin` is 1,347 / 1,347 data bytes exact;
- all 18 / 18 functions remain exact;
- candidate and target `.data` have the same 239 bytes and SHA-256
  `1a7bb8d05cbabe2d9f99c28c5fd522a1bf8694476aa37c21f06ef176cc45f49a`;
- the target defines all three symbols at object offsets `0x43c`, `0x440`, and
  `0x24`, respectively;
- the focused data-relocation audit has zero policy, provenance, and parse
  errors.

Its one remaining topology diagnostic is the pre-existing `.CRT$XCU` owner
presentation difference for `_$E19`; it is compiler/linker metadata and does
not represent an unmatched source datum.

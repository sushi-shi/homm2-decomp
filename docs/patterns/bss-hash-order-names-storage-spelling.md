# `.bss` order names the storage spelling

Measured with the pinned VC6 SP5 compiler on `SOURCE/SEARCH` (`/Od`, `/Gy`),
2026-10-06.

## Rule

Within one object, VC6 emits uninitialized `.bss` definitions in ascending
`check16(name) & 0x3ff`, where `check16` is the name-intern fold documented in
`docs/compiler-re-allocation-order.md` §3 (`h = c + (h >> 4) + h * 4`, then
`(h >> 16) ^ (h & 0xffff)`). The hashed name is the identifier of a
file-scope definition and the decorated name, without the leading underscore,
of a function-local static, for example
`?s_direction@?1??SeedPosition@searchArray@@QAEXHHHHHHHHHHHH@Z@4HA`.
Type, size and source position do not matter. The prediction held for all 29
`SeedPosition` statics, both before and after renaming.

Two further allocation facts close the layout:

- A record (struct/class) of 8 bytes or more starts 8-byte aligned; this is the
  hole before `searchNode` at retail `0x00533dcc`.
- An unreferenced function-local static keeps its slot in the hash-ordered run
  but VC6 emits no symbol for it. The non-alignment hole at retail
  `0x00533db8`, between two 4-byte owners, is such an object.

## Use

Sort a unit's claimed `.bss` owners by retail address. Every adjacent pair
must have ascending keys; a readable name whose key falls outside the window
left by its retail neighbours is not the compiled spelling. Keep the readable
name in source and supply the compiled spelling as a retail-only alias, as the
repository already does for name-hashed `/Od` locals:

```cpp
#if H2_RETAIL_COMPILER
#define s_hasTarget s_hasTarget_a
#endif
```

The `DATA()` inventory resolves such an alias at each marker
(`homm2.build.annotated_data.retail_spelling`), so claims bind to the
compiled symbol while Clang and the clean export see the readable name.

A hole that is neither alignment nor a claimed owner is evidence of an
unreferenced object; declare one at that place rather than padding a record.

## Does not establish

The key orders definitions; it does not recover original names. Spellings
chosen to fit a window are compiler input, not evidence of what the
developers called the objects.

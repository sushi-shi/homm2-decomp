# homm2-icon

Decoder and blitter for the Heroes of Might and Magic II icon (ICN) sprite
format, plus a reader for the `.AGG` archive the icons ship in. It lives under
`tools/` because it is an independent typed model and differential testbed, not
the rendering implementation used by the game.

`#![no_std]`, no allocator, zero dependencies, zero `unsafe`
(`unsafe_code = "forbid"`).

## Zero-copy without unsafe

Casting a byte slice straight to a struct reference is what normally drags in
`unsafe` or a crate like `bytemuck`/`zerocopy`. This crate does not need it,
because the two halves of "zero-copy" are separated:

- **Bulk data is genuinely borrowed.** `Op::Literal(&'a [u8])` points straight
  into the caller's buffer; the 27.7 M literal pixels in a retail archive are
  never copied. Archive member names and payloads are borrowed too. Slicing is
  safe by construction.
- **Fixed headers are parsed, not cast.** A 13-byte frame header becomes a
  `Frame` via `u16::from_le_bytes`. That copies 13 bytes and costs nothing.

Casting the header would be wrong anyway: `data_offset` sits at wire offset 9,
so a `#[repr(C)]` Rust struct pads to 16 bytes and does not match the format;
`repr(C, packed)` fixes the size but then every field read must copy regardless,
since references to misaligned fields are UB. And a raw cast reinterprets
*native*-endian, which would silently produce garbage on a big-endian target,
whereas `from_le_bytes` is correct everywhere. For a `no_std` crate that is not
a hypothetical.

## Why it looks like this

The original ships **eleven** near-identical blitters, one per translation unit,
made by cloning two ancestors and hand-editing the copies. The clone tree is
visible in what diverged: one variant lost a parameter, two ignore an argument
they accept, and a clipping defect appears in exactly the five mirrored decoders
and none of the six forward ones.

They are not eleven algorithms. They are configurations of two engines:

```
  dialect          direction     pixel operation        modifiers
  ---------------  -----------   --------------------   ----------------------
  Color  ------->  Forward   x   Direct | Remap(table)   shear table,
                   Mirrored                              highlight colour,
                                                         shadow mode
  Mask   ------->  Forward   x   Solid(colour) | Dim
                   Mirrored
```

Direction is genuinely orthogonal. The pixel operation is **not** - it depends on
the dialect, which is why `Pixels` and `MaskPaint` are two disjoint enums rather
than one flat set. A `MaskBlit` cannot be handed a remap table, and the compiler
enforces that. Eight combinations exist, not sixteen.

Retail has a fourth axis this crate deliberately collapses: a clip mode whose
`NO_CLIP` setting takes a separate, entirely unchecked path. Drawing here is
always clipped, so that axis is gone rather than factored - see Deviations.

`src/retail.rs` maps all eleven original symbols, with their RVAs, onto one-line
configurations.

## Pipeline

```
  bytes ---> decode ---> place ---> paint ---> surface
             (pure)      (clip)     (pixels)

  decode   ColorOps / MaskOps      opcode -> Op, no geometry at all
  place    Direction::clip         run + cursor + clip -> Span, or nothing
  paint    solid / dim / literal   Span -> pixels
```

The split matters. In the original, clipping is written out once per variant and
the horizontal test is supplied by the caller as a macro argument, so a handler
can be invoked without one - and passing the wrong predicate produces a `memset`
whose length underflows. Here a `Span` is **only** obtainable from
`Direction::clip`, so a run that was never tested against the clip rectangle
cannot reach the surface. The invariant is structural rather than conventional.

## Deviations from the original

Deliberate, and all in the direction of not corrupting memory:

- Frame indices, stream offsets and terminators are validated, not trusted.
- Drawing is always clipped to the surface. The original's unclipped path has no
  bounds test of any kind.
- Shadow levels are range-checked. A stream can encode sixteen levels; the retail
  palette table holds twelve - and the shipped archive really does contain 207
  runs at levels 12 and 13, where retail reads past the table into the adjacent
  remap tables. Those frames render differently here.
- The downscaler honours its clip rectangle; retail ignores it except on the
  `scale == 32` pass-through.

One retail defect is reproducible on request rather than silently fixed:

```rust
ColorBlit::new().mirrored(true).quirks(Quirks::retail())
```

makes mirrored solid, shadow and mask runs that straddle a clip edge vanish
entirely instead of clamping, matching the shipped game.

## Validated against shipped assets

`examples/survey.rs` decodes every icon in a real archive and checks the model
against the frame headers:

```sh
cargo run --release --example survey -- /path/to/HEROES2.AGG
```

Against retail `HEROES2.AGG` (846 icons, 14,444 frames):

| check | result |
| --- | --- |
| streams that failed to decode | 0 |
| decoded row count != declared height | 0 |
| decoded column extent > declared width | 0 |

`examples/render.rs` draws one frame, prints an ASCII preview and writes a PPM:

```sh
cargo run --example render -- HEROES2.AGG FONT.ICN 33
cargo run --example render -- HEROES2.AGG FONT.ICN 38 --mirrored
```

```
FONT.ICN frame 38 of 96
  size 15x13  offset (0, 0)  kind 0  data_offset 0x11e3

| ..........|          |.......... |
| ..........|          |     ..  . |
| .  ..     |          |     .. .. |
| .. ..     |   -->    |  .....    |     --mirrored
|    .....  |          |  .....    |
|    ..     |          |     ..    |
| .....     |          |     ..... |
```

## Layout

| module | responsibility |
| --- | --- |
| `frame` | container and frame table |
| `decode` | opcodes to runs, both dialects |
| `geometry` | direction, clipping, shear |
| `paint` | the four per-pixel operations |
| `blit` | the two engines |
| `scale` | nearest-neighbour downscale |
| `retail` | the eleven original entry points |
| `agg` | archive reader |

## Robustness

`tests/robustness.rs` is a dependency-free stand-in for `cargo fuzz`: a seeded
xorshift stream drives ~145,000 pathological inputs through the decoders, the
container parser and all three blitters, with random pens, clip rectangles,
mirroring and quirk settings. The surface is wrapped in a canary border, so any
write that escapes it fails the test.

It also asserts it did real work - 20,000 of 20,000 cases reach the blitter and
~45,000 pixels get painted - because a robustness suite that silently skips
everything is worse than none. One case in five uses deliberately degenerate
coordinates (`i32::MIN`/`MAX` pens, full-range `i16` frame offsets); the first
version of the suite drew everything from +/-40 and so missed four reachable
overflow panics.

A real fuzzer would explore far better. This is the cheap 90%.

## Development

```sh
cargo test --all-targets      # 31 integration tests
cargo test --doc              # 2 doctests
cargo clippy --all-targets    # pedantic, zero warnings
cargo fmt --check
```

The checked-in `rust-toolchain.toml` selects the exact Rust nightly used by this
repository. The Nix shell resolves that file through `oxalica/rust-overlay`,
which reproducibly packages the official Rust binary distribution. The C++
parity harness uses the enclosing repository by default; set
`HOMM2_SOURCE_DIR` only when testing against another compatible source tree.
The crate is covered by the repository's CC0 dedication.

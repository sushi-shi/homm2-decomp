# Image decoder validation

`icon` retains the ICN body length. Loading checks the declared length against
the selected AGG member, then validates all 13-byte frame records and their
self-terminating streams. Zero-sized empty frames are legal; negative extents,
missing terminators, incomplete opcodes, and runs outside the frame are rejected.
Offsets may share stream suffixes and need not be sorted.

The live drawing entry points use `src/BASE/ImageDecode.cpp`. The decoder checks
the requested dialect before painting, then intersects every run with the
destination surface, including `ICON_DRAW_NO_CLIP` calls. Geometry uses widened
arithmetic and invisible rows never form pixel pointers. Shear reads use a
span; the combat adapter supplies its actual 480-row allocation contract.

The mirrored partial-run and previous-row shear quirks remain intentional.
Scaled drawing keeps the retail sample positions, rejects invalid scales and
clips its writes. The native `image_decode` CTest exercises the production
validator and all eleven entry points, including the audit's missing-stream
input, truncations, malformed frames, boundary coordinates, dim levels, and
5,000 deterministic mutations. Run that target with the native sanitizer build
to check memory accesses as well as rejection results.

The Rust crate remains an independent model. Its generated parity cases call
the production C++ variants with explicit source and destination capacities.
The optional retail check also runs the production loader validator and
compares entire rendered surfaces for every installed frame. Retail files are
neither committed nor required by asset-free checks:

```sh
nix develop .#icon
cd tools/homm2-icon-rs
HOMM2_RETAIL_AGG=/path/to/HEROES2.AGG:/path/to/HEROES2X.AGG \
  cargo test --release --test cxx_parity retail_frames_agree -- --ignored --nocapture
```

The September 2026 Gold/PoL corpus passed 15,765 frames and 94,552 surface
comparisons across normal/flipped colour, remap, shear, mask-fill and mask-dim
variants. This is parser and pixel evidence, not a full combat playthrough.

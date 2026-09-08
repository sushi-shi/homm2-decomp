# BMP and TIL bounds

BMP and TIL constructors check dimensions and payload size against the selected
AGG member before allocating. BMP dimensions must be positive signed 16-bit
values; supported types are 0 and 33. TIL counts and dimensions must be nonzero.
The checked size calculation rejects products outside the member or the
engine's signed 32-bit transfer/allocation limit before multiplication can wrap.
Runtime memory bitmaps may be empty, with no pixel allocation.

Tiles retain their payload size and validate every requested tile index and
span. Tile, bitmap copy, fill and dim operations clip against actual destination
dimensions using widened coordinate arithmetic. Copies also clip the source
while retaining the relationship between source and destination positions.
Overlapping copies preserve the original source pixels, and `bitmap::CopyTo`
uses the actual row strides. Backdrop loading uses the validated BMP constructor
and copy path.

The explicit fill-rectangle clip helper retains retail's existing exclusion at
the first/last clip edge. This update does not change that visible rule.

The `raster` CTest links the production BMP/TIL constructors, bitmap copy methods
and rendering functions; only resource byte I/O and host services are supplied
by its fixture. It covers truncated/invalid headers, overflowing products,
undersized payloads, tile indices and flips, clipped and overlapping copies,
different row strides, empty bitmaps, palette levels, and extreme coordinates.
The same test can load installed archives for pixel comparisons:

```sh
ctest --test-dir build -R '^raster$' --output-on-failure
./build/homm2_raster_test /path/to/HEROES2.AGG /path/to/HEROES2X.AGG
```

Retail assets stay outside the repository and asset-free checks. Run the target
in the native ASan/UBSan build to verify accesses as well as output values.

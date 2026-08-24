// VC6 SP3 /Od /Ob1 cross-version source/state matrices.
// Overlay: build/source-variant-batch/get-new-cell-extra-overlay/results.json
// Object:  build/source-variant-batch/get-new-cell-extra-object/results.json
// Manifests: build/get-new-cell-extra-{overlay,object}-manifest.json
// Complete: 204/204 variants per function; no generator/wall-time truncation.
//
// Cartesian arms:
//   Row(y)[x] or Column(x)[y * width]
//   Extra(index) or &extras[index] for the two Buka direct-access sites
//
// Result (identical maxima for Overlay and Object):
//   Row + Extra: 100%; clean exact, 467 bytes, 22 blocks, 2/2 relocations.
//   Row + direct extras: 98.639460%, 457 bytes, 20 blocks.
//   Column + Extra: 87.047620%, 467 bytes, 22 blocks.
//   Column + direct extras: 85.687070%, 457 bytes, 20 blocks.
//
// Disposition: do not port the Buka call sites. Retain PoL Row + Extra; the
// shared Column accessor remains independently justified by its other callers.

// VC6 SP5 measured C66/S60 products. Parent 2b905597.
// Source record, not a replacement TU; no full TU-state census claim.
// Full proof: docs/reconstruction/C66-S60-B81.md.

// c66-path 0x7ce90
// build/source-variant-batch/c66-path/results.json
// Complete2/2; elapsed 1.391865s; input restored.
// Disposition: retain implicit assignment; complete final native/retail proof.
// trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind
// 0 / 269 / 1b3394a8099aca34 / ee259db5b8cc28fd / 18:18 / 18,0,0,0
// 1 / 269 / 1b3394a8099aca34 / ee259db5b8cc28fd / 18:18 / 18,0,0,0
/* AXIS restore_signed_byte_speed
BEFORE
static_cast<i8>(savedSpeed2)
AFTER implicit_assignment
savedSpeed2
*/

// s60-path 0x7cf9d
// build/source-variant-batch/s60-path/results.json
// Complete2/2; elapsed 1.397737s; input restored.
// Disposition: retain original unused scalar; deletion changes native text.
// trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind
// 0 / 128 / c3ea353e6ce94827 / f92fb571e0c3488b / 8:8 / 8,0,0,0
// 1 / 128 / 1fdf08fb6565a4c7 / f92fb571e0c3488b / 8:8 / 8,0,0,0
/* AXIS unused_scalar
BEFORE
    i32 H2_UNUSED(extra);

AFTER remove

*/

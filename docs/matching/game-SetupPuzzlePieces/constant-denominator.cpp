// C35: build/c35-{axes,manifest}.json and build/c35-batch/results.json.
// Complete2-arm family,2/2 in3.756s, restored, no AST/state probes/truncation.
// Original: (fraction * fraction + fraction)
//             / static_cast<float>(PUZZLE_INTERPOLATION_TERM_COUNT)
// Applied:  (fraction * fraction + fraction)
//             / IDX(PUZZLE_INTERPOLATION_TERM_COUNT)
// Constant2 is exactly representable; IDX supports the strong-enum build.
// Both541 bytes/20 relocs,36 blocks/155 instructions, identical native bytes.
// Both matrix strict verdicts false: private float operand+53 unresolved.
// Independent ordinary bytes/DIR32/19 targets PASS; focused constant checker
// proves remaining fdiv m32 at+51/+53 ->4ea4e4 bytes00000040, zero addend.
// Full102-function GAME native comparison includes all allocated sections and
// ordered relocation/addend semantics; PASS. Retain real runtime conversions.

// VC6-measured complete addend-order matrix for fileRequester::DoKnob,
// RVA 0x0049085b. The two float addends are semantically commutative, but
// their source order controls the ordered fGutterMinY/fGutterTravelLength
// relocations at both the upper-bound comparison and its clamping assignment.
//
// Artifacts: build/request-doknob-reloc-manifest.json
//            build/source-variant-batch/request-doknob-relocs/results.json
//
// travel first:  99.880240%, size 0x25b, relocs 31/31
// minimum first: 100.000000%, size 0x25b, relocs 31/31, 20/20 blocks
//
// The product was a complete two-arm exact-span axis with no AST or TU-state
// dimension. Retained at both sites:
//   knobOffset + fGutterMinY + fGutterTravelLength

// Experiment: independently express both dim loops as do-while, while, or
// condition-only for loops.
// Artifact: build/match-variants/iconf2bc-dim-loop-forms-clean-20260726/
// Coverage: complete 3 x 3 matrix, nine clean source structures.
// Result: every arm compiled byte-identically at 87.035260%, size 1341,
// relocations 84/83.
// Disposition: exhausted; VC4.2 canonicalizes all three forms because the
// surrounding positive-count guard proves one iteration before loop entry.

#if 0
do {
    DimPixelBody();
} while (count != 0);

while (count != 0) {
    DimPixelBody();
}

for (; count != 0;) {
    DimPixelBody();
}
#endif

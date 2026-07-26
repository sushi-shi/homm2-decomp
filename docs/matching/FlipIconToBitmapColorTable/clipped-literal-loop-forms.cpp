// Experiment: clipped literal-copy loop count ownership and direction.
// Artifact: build/match-variants/iconf2bc-clipped-loop-forms-clean-20260726/
// Arms: cn do-while, cn while, cn for, copyCount countdown, and copyCount
// forward index.
// Result: all five source structures compiled byte-identically at 87.035260%,
// size 1341, relocations 84/83.
// Disposition: exhausted; VC4.2 canonicalizes both counter identities and all
// tested loop directions under the existing positive copyCount guard.

#if 0
do {
    CopyMappedPixel();
    cn--;
} while (cn != 0);

do {
    CopyMappedPixel();
    copyCount--;
} while (copyCount != 0);

for (i32 k = 0; k < copyCount; k++) {
    CopyMappedPixel();
}
#endif

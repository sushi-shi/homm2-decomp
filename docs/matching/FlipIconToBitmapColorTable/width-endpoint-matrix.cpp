// Experiment: preserve or mutate icon width while constructing the endpoint.
// Artifact: build/match-variants/iconf2bc-width-endpoint-clean-20260726/
// Coverage: three endpoint arms crossed with entry-width/global-width clip tests.
// Best: 86.153656%. Disposition: rejected.

#if 0
// Arm 1: mutate width into the exclusive endpoint.
w = w + x0;
i32 X = (gFCXEnd = w - 1);

// Arm 2: preserve width with a chained publication.
i32 X = (gFCXEnd = w + x0 - 1);

// Arm 3: preserve width with split publication.
i32 X = w + x0 - 1;
gFCXEnd = X;

// Clip-width source A.
if (clipW + clipX < entry->w + x0) {
}

// Clip-width source B.
if (clipW + clipX < x0 + w) {
}
#endif

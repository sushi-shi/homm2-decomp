// Experiment: dim-loop decoder read/advance ownership.
// Artifact: build/match-variants/iconf2bc-read-advance-islands-20260726/
// Best: 86.380356%. Disposition: rejected.

#if 0
// Arm 1: split read, update, and pointer advance.
i32 px = *dp;
count--;
dp++;
gFCDimDst = dp;
dp[-1] = dimPalette[px];

// Arm 2: read through postincrement.
i32 px = *dp++;
count--;
gFCDimDst = dp;
dp[-1] = dimPalette[px];
#endif

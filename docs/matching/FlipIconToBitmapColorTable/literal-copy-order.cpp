// Experiment: order independent right-clipped literal-run assignments.
// Artifact: build/match-variants/iconf2bc-copy-order-islands-20260726-r2/
// Coverage: this source arm crossed with 120 TU-state trials.
// Best: 86.644840% at trial 5. Disposition: rejected.

#if 0
// Baseline order.
skip = gFCClipR + ((cmd - X) - clipW);
cn = clipW;

// Permuted order.
cn = clipW;
skip = gFCClipR + ((cmd - X) - clipW);
#endif

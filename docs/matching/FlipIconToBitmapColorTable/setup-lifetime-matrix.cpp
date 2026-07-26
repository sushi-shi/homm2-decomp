// Experiment: setup declaration and lifetime order.
// Artifact: build/match-variants/iconf2bc-setup-lifetimes-islands-20260726/
// Coverage: four source shapes crossed with TU-state probes.
// Best: 86.430730%. Disposition: rejected.

#if 0
// Arm 1: X before an uninitialized width.
i32 x0 = x;
i32 w;
w = entries[frame].w;
x0 = x0 - entries[frame].x;
x0 = x0 - w;

// Arm 2: width declaration before X.
i32 w;
i32 x0 = x;
w = entries[frame].w;
x0 = x0 - entries[frame].x;
x0 = x0 - w;

// Arm 3: initialize width, then initialize X from the packed field.
i32 w = entries[frame].w;
i32 x0 = x - entries[frame].x;
x0 = x0 - w;

// Arm 4: initialize width, split the X operations.
i32 w = entries[frame].w;
i32 x0 = x;
x0 = x0 - entries[frame].x;
x0 = x0 - w;
#endif

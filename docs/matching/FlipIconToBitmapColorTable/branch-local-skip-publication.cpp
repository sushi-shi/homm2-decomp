// Experiment: remove set_skip/skip_set by publishing the computed skip in
// each clipped-literal branch where it is produced.
// Artifacts:
//   build/match-variants/iconf2bc-no-set-skip-clean-20260726/
//   build/tu-state-noise/iconf2bc-branch-local-skip-50-20260726/
// Clean result: 87.035260%, size 1341, relocations 84/83.
// Official generated status after the full build: 87.0479%.
// Blocks: 76/76, 67 exact and 9 size-only.
// Census: 50 trials, 36 unique states; clean source remained best.
// Disposition: retained as plausible source and a new isolated maximum.

#if 0
if (X <= gFCClipR) {
    selectedDst = gFCRow + X;
    gFCDst = selectedDst;
    if (clipX <= left) {
        gFCSkip = 0;
        cn = cmd;
    } else {
        cn = (X - clipX) + 1;
        skip = cmd - cn;
        gFCSkip = skip;
    }
} else {
    i32 right = gFCClipR;
    src = src + (X - right);
    selectedDst = gFCRow + right;
    gFCDst = selectedDst;
    if (clipX <= (X - cmd)) {
        gFCSkip = 0;
        cn = (cmd - X) + gFCClipR;
    } else {
        cn = clipW;
        skip = gFCClipR + ((cmd - X) - clipW);
        gFCSkip = skip;
    }
}

i32 copyCount = cn;
#endif

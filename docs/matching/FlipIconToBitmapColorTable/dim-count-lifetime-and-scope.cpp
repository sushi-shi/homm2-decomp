// Experiment: force the retail-looking distinct dimCount register through
// assignment placement and declaration scope.
// Artifacts:
//   build/match-variants/iconf2bc-dim-count-lifetimes-clean-20260726/
//   build/match-variants/iconf2bc-dim-count-scope-clean-20260726/
//   build/tu-state-noise/iconf2bc-clipped-root-dimcount-50-20260726/
// The 4 x 4 near-use matrix (initialized/split/guard assignment in each path)
// was entirely byte-identical at 87.035260%.
// Hoisting the unclipped local alone was byte-neutral. Hoisting the clipped
// local, both locals, or two distinct root locals produced the same credible
// lower island: 86.934510%, size 1341, relocations 84/83, blocks 76/76.
// The clipped-root form received 50 trials (38 states); trial 14 returned to
// 87.035260%, but none beat official MAX 87.0479%.
// Disposition: retained block-local declarations on master. The clipped-root
// shape remains an active branch seed for deeper structural descendants.

#if 0
// Near-use assignment arm.
i32 dimCount;
gFCDimDst = dp;
if ((dimCount = count) > 0) {
    gFCCnt = dimCount;
}

// Credible lower declaration-scope arm.
i32 cmd;
i32 dimCount;
for (;;) {
    // ...
    gFCDimDst = dp;
    dimCount = count;
    gFCCnt = 0;
    if (dimCount > 0) {
        gFCCnt = dimCount;
    }
}
#endif

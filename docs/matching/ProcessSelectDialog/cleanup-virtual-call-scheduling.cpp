/*
dropListWidget::ProcessSelectDialog, RVA 0x000dcb10: cleanup virtual-call
scheduling (2026-07-27).

In the retained trial-15 island the sole residual is one cleanup block.  Both
objects perform the same operations, but retail preloads the Draw virtual-call
receiver and vtable before clearing m_savedBackground:

  retail:   mov eax,[ebx]; mov ecx,ebx; mov [ebx+0xb0],0; call [eax]
  candidate:mov [ebx+0xb0],0; mov eax,[ebx]; mov ecx,ebx; call [eax]

The baseline and three developer-plausible scope/sequence spellings below
compiled to identical clean code.  A saved-pointer local opened a worse
94.003360% island without improving topology and was rejected.
*/

#if 0
// Baseline.
if (m_savedBackground != NULL)
    delete m_savedBackground;
m_savedBackground = NULL;
Draw();

// Braced delete.
if (m_savedBackground != NULL) {
    delete m_savedBackground;
}
m_savedBackground = NULL;
Draw();

// One sequenced expression.
if (m_savedBackground != NULL)
    delete m_savedBackground;
m_savedBackground = NULL, Draw();

// Assignment in both branch arms.
if (m_savedBackground != NULL) {
    delete m_savedBackground;
    m_savedBackground = NULL;
} else {
    m_savedBackground = NULL;
}
Draw();

// Rejected alias.
bitmap *savedBackground = m_savedBackground;
if (savedBackground != NULL)
    delete savedBackground;
m_savedBackground = NULL;
Draw();
#endif

/*
Artifacts:

  build/droplist-process-select-cleanup-axes.json
  build/droplist-process-select-cleanup-clean.json
  build/match-variants/droplist-process-select-cleanup-clean-20260727/

The first four arms are codegen-equivalent in the clean object.  The retained
baseline spelling was already covered by the clean plus 200 forest census
recorded in saved-background-width-branch.cpp.  No raw dispatch, fake helper,
or semantic reordering was introduced merely to force the final schedule.

Cross-version follow-up, 2026-08-05:

The exact Gold/Buka body calls `RestoreDropBackground()` at this point. To
subtract the later compiler and surrounding-loop changes, a four-arm matrix
crossed the open PoL body or later call with an ordinary or explicitly inline
definition of the existing helper. The clean paired inline-definition/call
arm was byte-identical to the open body; the ordinary call was structurally
wrong. A complete 4 x 51 forest/top matrix then showed that the paired inline
form remained byte-identical to the open form in every corresponding state.
The best current census state was 99.488260%, size 2079, exact 100-block flow,
and 36/36 relocations; no audited exact closure appeared. The historical
99.6225% MAX remains higher and is not lowered.

Artifacts:

  build/process-select-cross-version-restore-helper-axes.json
  build/process-select-cross-version-restore-helper-clean-manifest.json
  build/source-variant-batch/process-select-cross-version-restore-helper-clean/results.json
  build/process-select-cross-version-restore-helper-states-manifest.json
  build/source-variant-batch/process-select-cross-version-restore-helper-states-complete/results.json

Disposition: retain the open PoL body. The later helper boundary is plausible
original structure but is codegen-neutral under VC4.2 and does not improve MAX.
*/

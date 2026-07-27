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
*/

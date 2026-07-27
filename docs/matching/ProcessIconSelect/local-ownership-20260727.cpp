// ProcessIconSelect local-ownership experiment, 2026-07-27.
//
// Target: SOURCE/Overview, RVA 0x0000ab6c, retail size 0x4fa.
// Matrix: /tmp/processiconselect-local-ownership-manifest.json
// Artifacts: build/match-variants/processiconselect-local-ownership
//
// The current typed block locals produced the correct 57-block CFG, instruction
// sequence, and 55 relocations, but allocated an 0x2c-byte frame instead of the
// retail 0x24-byte frame.  This matrix independently assigned the skill level
// and selected artifact either to a distinct typed block local or to the
// existing function-scoped selectionIndex2 slot.
//
// Results in the clean TU state:
//   typed skill, typed artifact:   99.925380%, frame 0x2c
//   typed skill, shared artifact:  99.934326%
//   shared skill, shared artifact: 100.000000%, frame 0x24, 55/55 relocs
//
// Exact closure appeared before TU-state probes were needed.  The matrix was
// interrupted after the exact clean-state result so no generated declaration
// forest was retained.  The accepted structure keeps the proven skill-level
// temporary evaluation while giving it, and the artifact value, the shared
// selection slot evidenced by the retail frame.

i32 game::ProcessIconSelect_local_ownership_excerpt(
    hero* selectedHero13,
    i32 widgetId,
    i32 quickView,
    i32& selectionIndex2
) {
    if (widgetId >= HERO_SKILL_FIRST && widgetId <= HERO_SKILL_LAST) {
        selectionIndex2 = selectedHero13->GetNthSS(widgetId - HERO_SKILL_FIRST);
        selectedHero13->DoSSLevelDialog(selectionIndex2, quickView);
    }

    if (widgetId >= HERO_ARTIFACT_FIRST && widgetId <= HERO_ARTIFACT_LAST) {
        selectionIndex2 =
            selectedHero13->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
        if (selectionIndex2 == IDX(ARTIFACT_MAGIC_BOOK)) {
            gpGame->ViewSpells(selectedHero13, SPELL_TYPE_ALL, ViewSpecialHandler, 1);
        } else {
            NormalDialog(
                gArtifactDesc[selectionIndex2],
                quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
                NORMAL_DIALOG_NO_VALUE,
                NORMAL_DIALOG_MAX_TOP,
                NORMAL_DIALOG_NO_RESOURCE,
                0,
                NORMAL_DIALOG_NO_RESOURCE,
                0,
                NORMAL_DIALOG_NO_VALUE,
                0
            );
        }
    }
    return 0;
}

// VC6 SP5 Overview source family r24-overview-select-roles; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r24-overview-select-roles/results.json; RVA 0x7c9b3
// Complete 2 arms; elapsed 1.738495s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1187 / e41fb732eacaba9b / 4a868ea98d5d2bce / 59:55 / 55,0,0,0 / False
// 1 / {'source_family': 'skill_artifact', 'candidate': 'baseline'} / 1187 / b3be75eba1ad0841 / ca0152b6b7321fa0 / 59:55 / 55,0,0,0 / False

// AXIS source_family
// BEFORE (complete exact span):
// | #else
// |                 item = curHero->GetNthSS(widgetId - HERO_SKILL_FIRST);
// |                 curHero->DoSSLevelDialog(item, quickView);
// | #endif
// |             }
// |             if (widgetId >= HERO_ARTIFACT_FIRST
// |                 && widgetId <= HERO_ARTIFACT_LAST) {
// | #if H2_STRICT_ENUMS
// |                 ArtifactType selectedArtifact =
// |                     curHero->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
// | #else
// |                 item = curHero->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
// | #endif
// |                 if (
// | #if H2_STRICT_ENUMS
// |                     selectedArtifact == ARTIFACT_MAGIC_BOOK
// | #else
// |                     item == IDX(ARTIFACT_MAGIC_BOOK)
// | #endif
// |                 ) {
// |                     gpGame->ViewSpells(curHero, SPELL_TYPE_ALL, ViewSpecialHandler, 1);
// |                 } else {
// |                     curHero->ViewArtifact(
// | #if H2_STRICT_ENUMS
// |                         selectedArtifact,
// | #else
// |                         item,
// | #endif
// ALTERNATIVE skill_artifact (unified source delta from BEFORE):
// | --- before
// | +++ skill_artifact
// | @@ -1,6 +1,6 @@
// |  #else
// | -                item = curHero->GetNthSS(widgetId - HERO_SKILL_FIRST);
// | -                curHero->DoSSLevelDialog(item, quickView);
// | +                i32 skillId = curHero->GetNthSS(widgetId - HERO_SKILL_FIRST);
// | +                curHero->DoSSLevelDialog(skillId, quickView);
// |  #endif
// |              }
// |              if (widgetId >= HERO_ARTIFACT_FIRST
// | @@ -9,13 +9,13 @@
// |                  ArtifactType selectedArtifact =
// |                      curHero->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
// |  #else
// | -                item = curHero->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
// | +                i32 artifactId = curHero->m_artifacts[widgetId - HERO_ARTIFACT_FIRST];
// |  #endif
// |                  if (
// |  #if H2_STRICT_ENUMS
// |                      selectedArtifact == ARTIFACT_MAGIC_BOOK
// |  #else
// | -                    item == IDX(ARTIFACT_MAGIC_BOOK)
// | +                    artifactId == IDX(ARTIFACT_MAGIC_BOOK)
// |  #endif
// |                  ) {
// |                      gpGame->ViewSpells(curHero, SPELL_TYPE_ALL, ViewSpecialHandler, 1);
// | @@ -24,5 +24,5 @@
// |  #if H2_STRICT_ENUMS
// |                          selectedArtifact,
// |  #else
// | -                        item,
// | +                        artifactId,
// |  #endif

// VC6 SP5 source product c79-events-human-scroll; RVA 0x3b640.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.085131s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-human-scroll/results.json and build/c79-events-human-scroll-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'cast0': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False
// 1 / {'cast0': 'recover', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / 824852a69d125abf / 557:561 / 556,0,0,1,4 / False

// AXIS cast0
// BEFORE (complete exact span):
// |                     IDX(artifact_g),
// |                     -1,
// |                     0,
// |                     -1
// |                 );
// |                 GiveArtifact(eventHero2, artifact_g, true, static_cast<i8>(cell->m_objectMetadata));
// |                 eraseObject_l = 1;
// |                 fizzleType_k = true;
// |                 break;
// |             }
// |
// |             if (artifact_g == ARTIFACT_SPELL_SCROLL) {
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -3,7 +3,7 @@
// |                      0,
// |                      -1
// |                  );
// | -                GiveArtifact(eventHero2, artifact_g, true, static_cast<i8>(cell->m_objectMetadata));
// | +                GiveArtifact(eventHero2, artifact_g, true, cell->m_objectMetadata);
// |                  eraseObject_l = 1;
// |                  fizzleType_k = true;
// |                  break;
// EXTRA BEFORE:
// |                     IDX(artifact_g),
// |                     -1,
// |                     0,
// |                     -1
// |                 );
// |                 GiveArtifact(eventHero2, artifact_g, true, static_cast<i8>(cell->m_objectMetadata));
// |                 eraseObject_l = 1;
// |                 fizzleType_k = true;
// |                 break;
// |             }
// |
// |             if (cell->m_objectMetadata & ARTIFACT_EVENT_GUARDED_FLAG) {
// EXTRA REPLACEMENT:
// |                     IDX(artifact_g),
// |                     -1,
// |                     0,
// |                     -1
// |                 );
// |                 GiveArtifact(eventHero2, artifact_g, true, cell->m_objectMetadata);
// |                 eraseObject_l = 1;
// |                 fizzleType_k = true;
// |                 break;
// |             }
// |
// |             if (cell->m_objectMetadata & ARTIFACT_EVENT_GUARDED_FLAG) {

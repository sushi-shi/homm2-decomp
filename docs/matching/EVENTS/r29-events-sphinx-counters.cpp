// VC6 SP5 source product r29-events-sphinx-counters; RVA 0x3b640.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 4 arms, 5.206047s; no compile failure or timeout; source restored.
// build/source-variant-batch/r29-events-sphinx-counters/results.json and build/r29-events-sphinx-counters-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'answerIndex': 'keep', 'resourceIndex': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False
// 1 / {'answerIndex': 'keep', 'resourceIndex': 'recover', 'candidate': 'baseline'} / 24557 / 986795e956ee3046 / bc18f8580a7edb0f / 557:561 / 556,0,0,1,4 / False
// 2 / {'answerIndex': 'recover', 'resourceIndex': 'keep', 'candidate': 'baseline'} / 24536 / 73c896897f1088b8 / 085676214d77f44e / 557:561 / 556,0,0,1,4 / False
// 3 / {'answerIndex': 'recover', 'resourceIndex': 'recover', 'candidate': 'baseline'} / 24572 / 7fd68d5363696634 / 22083a6ca888d140 / 557:561 / 556,0,0,1,4 / False

// AXIS answerIndex
// BEFORE (complete exact span):
// |                     for (eventValue1 = 0; eventValue1 < eventExtra_o->answerCount; eventValue1++) {
// |                         if (RiddleStringsEqual(sphinxAnswer_a, eventExtra_o->answers[eventValue1]))
// |                             correctAnswer_e = true;
// |                     }
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,4 +1,5 @@
// | -                    for (eventValue1 = 0; eventValue1 < eventExtra_o->answerCount; eventValue1++) {
// | -                        if (RiddleStringsEqual(sphinxAnswer_a, eventExtra_o->answers[eventValue1]))
// | +                    i32 answerIndex;
// | +                    for (answerIndex = 0; answerIndex < eventExtra_o->answerCount; answerIndex++) {
// | +                        if (RiddleStringsEqual(sphinxAnswer_a, eventExtra_o->answers[answerIndex]))
// |                              correctAnswer_e = true;
// |                      }

// AXIS resourceIndex
// BEFORE (complete exact span):
// |                         for (eventValue1 = 0; eventValue1 < SPHINX_RESOURCE_COUNT;
// |                              eventValue1++) {
// |                             gpGame->m_players[giCurPlayer].m_resources[eventValue1] +=
// |                                 eventExtra_o->resources[eventValue1];
// |                             if (gpGame->m_players[giCurPlayer].m_resources[eventValue1] < 0)
// |                                 gpGame->m_players[giCurPlayer].m_resources[eventValue1] = 0;
// |                             if (eventExtra_o->resources[eventValue1] != 0) {
// |                                 if (primaryReward_e != MAP_EVENT_REWARD_NONE) {
// |                                     secondaryReward_k = primaryReward_e;
// |                                     secondaryAmount_j = primaryAmount_j;
// |                                 }
// |                                 primaryReward_e = eventValue1;
// |                                 primaryAmount_j = eventExtra_o->resources[eventValue1];
// |                             }
// |                         }
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,15 +1,16 @@
// | -                        for (eventValue1 = 0; eventValue1 < SPHINX_RESOURCE_COUNT;
// | -                             eventValue1++) {
// | -                            gpGame->m_players[giCurPlayer].m_resources[eventValue1] +=
// | -                                eventExtra_o->resources[eventValue1];
// | -                            if (gpGame->m_players[giCurPlayer].m_resources[eventValue1] < 0)
// | -                                gpGame->m_players[giCurPlayer].m_resources[eventValue1] = 0;
// | -                            if (eventExtra_o->resources[eventValue1] != 0) {
// | +                    i32 resourceIndex;
// | +                        for (resourceIndex = 0; resourceIndex < SPHINX_RESOURCE_COUNT;
// | +                             resourceIndex++) {
// | +                            gpGame->m_players[giCurPlayer].m_resources[resourceIndex] +=
// | +                                eventExtra_o->resources[resourceIndex];
// | +                            if (gpGame->m_players[giCurPlayer].m_resources[resourceIndex] < 0)
// | +                                gpGame->m_players[giCurPlayer].m_resources[resourceIndex] = 0;
// | +                            if (eventExtra_o->resources[resourceIndex] != 0) {
// |                                  if (primaryReward_e != MAP_EVENT_REWARD_NONE) {
// |                                      secondaryReward_k = primaryReward_e;
// |                                      secondaryAmount_j = primaryAmount_j;
// |                                  }
// | -                                primaryReward_e = eventValue1;
// | -                                primaryAmount_j = eventExtra_o->resources[eventValue1];
// | +                                primaryReward_e = resourceIndex;
// | +                                primaryAmount_j = eventExtra_o->resources[resourceIndex];
// |                              }
// |                          }

// VC6 SP5 source product c79-events-teleport-ai; RVA 0x44d73.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.995589s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-teleport-ai/results.json and build/c79-events-teleport-ai-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'bounded-trigger': 'keep', 'candidate': 'baseline'} / 11116 / fe51168989c69b33 / c73a53b52e3d49b1 / 367:374 / 366,0,0,1,7 / False
// 1 / {'bounded-trigger': 'recover', 'candidate': 'baseline'} / 11104 / c15d415b13b2e15a / 20daa080e1bc1ddf / 367:374 / 364,2,0,1,7 / False

// AXIS bounded-trigger
// BEFORE (complete exact span):
// |                             == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                         && gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_objectIndex
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,2 +1,2 @@
// | -                            == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
// | +                            == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                          && gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_objectIndex
// EXTRA BEFORE:
// |                                 == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                             && gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_objectIndex
// EXTRA REPLACEMENT:
// |                                 == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                             && gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_objectIndex

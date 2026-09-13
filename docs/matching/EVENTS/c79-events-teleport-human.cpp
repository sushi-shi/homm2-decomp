// VC6 SP5 source product c79-events-teleport-human; RVA 0x3b640.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.158641s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-teleport-human/results.json and build/c79-events-teleport-human-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'bounded-trigger': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False
// 1 / {'bounded-trigger': 'recover', 'candidate': 'baseline'} / 24509 / d503e8fa48e4afba / ec4d45d84bd6d616 / 557:561 / 554,2,0,1,4 / False

// AXIS bounded-trigger
// BEFORE (complete exact span):
// |                             == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                         && (gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_objectIndex
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,2 +1,2 @@
// | -                            == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
// | +                            == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                          && (gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_objectIndex
// EXTRA BEFORE:
// |                                 == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                             && (gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_objectIndex
// EXTRA REPLACEMENT:
// |                                 == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
// |                             && (gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_objectIndex

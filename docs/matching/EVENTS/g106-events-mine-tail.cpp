// VC6 SP5 source product g106-events-mine-tail; RVA 0x3b640.
// Parent: 3a80a103 (unmodified).
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.218353s; no compile failure or timeout; source restored.
// build/source-variant-batch/g106-events-mine-tail/results.json and build/g106-events-mine-tail-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / f00329f724dc8285 / 557:561 / 556,0,0,1,4 / False
// 1 / {'family': 'recovered', 'candidate': 'baseline'} / 24604 / 19f547737a326c0e / 5b01fc979dc4a839 / 556:561 / 117,5,101,333,5 / False

// AXIS family
// BEFORE (complete exact span):
// | case MAP_OBJECT_MINE:
// |             if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
// |                 break;
// |             if (gpGame->m_mines[cell->m_objectMetadata].guardianType != CREATURE_NONE) {
// |                 mineCombatResult_b = CombatMonsterEvent(
// |                     eventHero2,
// |                     gpGame->m_mines[cell->m_objectMetadata].guardianType,
// |                     gpGame->m_mines[cell->m_objectMetadata].guardianCount,
// |                     cell,
// |                     x,
// |                     y,
// |                     0,
// |                     x,
// |                     y,
// |                     CREATURE_NONE,
// |                     0,
// |                     0,
// |                     CREATURE_NONE,
// |                     0,
// |                     0
// |                 );
// |                 if (mineCombatResult_b == COMBAT_RESULT_ATTACKER) {
// |                     gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
// |                     eventHero2->CheckLevel();
// |                 } else {
// |                     break;
// |                 }
// |             }
// |             EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |             if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_GOLD)
// |                 resourceAmount_o = MINE_GOLD_INCOME;
// |             else if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_ORE)
// |                 resourceAmount_o = MINE_ORE_INCOME;
// |             else
// |                 resourceAmount_o = 1;
// |             EventWindow(
// |                 IDX(gpGame->m_mines[cell->m_objectMetadata].resourceType)
// |                     + MINE_RESOURCE_ICON_OFFSET,
// |                 NORMAL_DIALOG_INFO,
// |                 "",
// |                 IDX(gpGame->m_mines[cell->m_objectMetadata].resourceType),
// |                 -resourceAmount_o,
// |                 -1,
// |                 0,
// |                 -1
// |             );
// |             goto claimMine;
// OPTION keep
// OPTION recovered
// | --- before
// | +++ recovered
// | @@ -44,4 +44,5 @@
// |                  0,
// |                  -1
// |              );
// | -            goto claimMine;
// | +            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
// | +            break;
// EXTRA BEFORE:
// | case MAP_OBJECT_ALCHEMIST_LAB:
// |             if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
// |                 break;
// |             EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |             EventWindow(
// |                 EVENT_TEXT_ALCHEMIST_CAPTURED,
// |                 NORMAL_DIALOG_INFO,
// |                 "",
// |                 IDX(RES_MERCURY),
// |                 NORMAL_DIALOG_NO_VALUE,
// |                 -1,
// |                 0,
// |                 -1
// |             );
// |             goto claimMine;
// EXTRA REPLACEMENT:
// | case MAP_OBJECT_ALCHEMIST_LAB:
// |             if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
// |                 break;
// |             EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |             EventWindow(
// |                 EVENT_TEXT_ALCHEMIST_CAPTURED,
// |                 NORMAL_DIALOG_INFO,
// |                 "",
// |                 IDX(RES_MERCURY),
// |                 NORMAL_DIALOG_NO_VALUE,
// |                 -1,
// |                 0,
// |                 -1
// |             );
// |             gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
// |             break;
// EXTRA BEFORE:
// |         claimMine:
// EXTRA REPLACEMENT:

// Parent 7d4ffd56; 512 complete arms in 365.8164061168209 seconds.
// Selected trial 3: {'source_shape': 'omit0_casts3', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (1607, '1a73668dfacafd6d', '492766c49fd7c8e5').
// Same size and masked payload as baseline: ['omit0_casts0', 'omit0_casts1', 'omit0_casts2', 'omit0_casts3'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/c46-s48-well-manifest.json; build/source-variant-batch/c46-s48-well/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// void townManager::SetupWell(heroWindow* window) {
//     i16 H2_UNUSED(unusedFirstIcon_d) = 1;
//     i16 H2_UNUSED(unusedFirstName_b) = TOWN_WELL_FIRST_NAME_CONTROL;
//     i16 H2_UNUSED(unusedFirstMonsterIcon_p) = TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
//     i16 H2_UNUSED(unusedFirstCreature_f) = TOWN_WELL_FIRST_CREATURE_CONTROL;
//     i16 H2_UNUSED(unusedFirstDetail_g) = TOWN_WELL_FIRST_DETAIL_CONTROL;
//     i16 H2_UNUSED(unusedFirstAvailable) = TOWN_WELL_FIRST_AVAILABLE_CONTROL;
//     i16 H2_UNUSED(unusedFirstAvailableCount) = TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
//     u8 dwellingTypes_c[WELL_DWELLING_TYPE_STORAGE_COUNT];
//     i32 available_e;
//     i32 dwellingResult_a;
//     tag_message message_i;
//     char iconName_b[TOWN_OBJECT_FILENAME_SIZE];
//     char detailText_i[WELL_DETAIL_TEXT_CAPACITY];
//     tag_monsterInfo monsterInfo_f;
//     i32 growth_a;
//
//     for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
//         if (dwellingResult_a == TOWN_WELL_DWELLING_COUNT - 1
//             && (m_town->m_buildings & (1L << TOWN_WELL_LAST_UPGRADE_BUILDING))) {
//             dwellingTypes_c[dwellingResult_a] = WELL_ALTERNATE_UPGRADE_INDEX;
//         } else if (dwellingResult_a >= 1
//                    && (m_town->m_buildings
//                        & (1L << (dwellingResult_a + TOWN_WELL_FIRST_UPGRADE_BUILDING)))) {
//             dwellingTypes_c[dwellingResult_a] =
//                 static_cast<u8>(dwellingResult_a + TOWN_WELL_FIRST_UPGRADE_OFFSET);
//         } else {
//             dwellingTypes_c[dwellingResult_a] = static_cast<u8>(dwellingResult_a);
//         }
//     }
//
//     message_i.type = MESSAGE_WIDGET;
//     message_i.payload.widget.command = WIDGET_COMMAND_SET_ICON;
//     sprintf(iconName_b, "cstl%s.icn", cHeroTypeShortName[IDX(m_town->m_type)]);
//     message_i.payload.widget.data.text = iconName_b;
//     for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
//         message_i.payload.widget.id = dwellingResult_a + 1;
//         window->BroadcastMessage(message_i);
//     }
//
//     for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
//         message_i.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
//         message_i.payload.widget.id = dwellingResult_a + 1;
//         message_i.payload.widget.data.value =
//             dwellingTypes_c[dwellingResult_a] + IDX(BUILDING_SLOT_DWELLING_FIRST);
//         window->BroadcastMessage(message_i);
//         sprintf(
//             gText,
//             "monh%04d.icn",
//             IDX(gDwellingType[IDX(m_town->m_type)][dwellingTypes_c[dwellingResult_a]])
//         );
//         message_i.payload.widget.command = WIDGET_COMMAND_SET_ICON;
//         message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
//         message_i.payload.widget.data.text = gText;
//         window->BroadcastMessage(message_i);
//     }
//
//     message_i.type = MESSAGE_WIDGET;
//     message_i.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
//     for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
//         sprintf(
//             gText,
//             GetBuildingName(
//                 m_town->m_type,
//                 BuildingSlotType(
//                     dwellingTypes_c[dwellingResult_a] + IDX(BUILDING_SLOT_DWELLING_FIRST)
//                 )
//             )
//         );
//         message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_NAME_CONTROL;
//         message_i.payload.widget.data.text = gText;
//         window->BroadcastMessage(message_i);
//
//         if (m_town->m_buildings
//             & (1L << (dwellingTypes_c[dwellingResult_a] + IDX(BUILDING_SLOT_DWELLING_FIRST)))) {
//             available_e = m_town->m_garrison[dwellingTypes_c[dwellingResult_a]];
//             sprintf(gText, "\xc4\xee\xf1\xf2\xf3\xef\xed\xee:" /* "Доступно:" */);
//             message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_AVAILABLE_CONTROL;
//             message_i.payload.widget.data.text = gText;
//             window->BroadcastMessage(message_i);
//             sprintf(gText, "%d", available_e);
//             message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
//             message_i.payload.widget.data.text = gText;
//             window->BroadcastMessage(message_i);
//         }
//
//         message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_CREATURE_CONTROL;
//         strcpy(
//             gText,
//             gArmyNames[IDX(gDwellingType[IDX(m_town->m_type)][dwellingTypes_c[dwellingResult_a]])]
//         );
//         char upperFirst;
//         if (static_cast<u8>(gText[0]) >= 'a' && static_cast<u8>(gText[0]) <= 'z')
//             upperFirst = static_cast<u8>(gText[0]) - ' ';
//         else if (static_cast<u8>(gText[0]) >= 0xe0 && static_cast<u8>(gText[0]) <= 0xff)
//             upperFirst = static_cast<u8>(gText[0]) - ' ';
//         else if (static_cast<u8>(gText[0]) == 0xb8)
//             upperFirst = '\xa8';
//         else
//             upperFirst = gText[0];
//         gText[0] = upperFirst;
//         message_i.payload.widget.data.text = gText;
//         window->BroadcastMessage(message_i);
//     }
//
//     for (dwellingResult_a = 0; dwellingResult_a < TOWN_WELL_DWELLING_COUNT; ++dwellingResult_a) {
//         monsterInfo_f = gMonsterDatabase[IDX(
//             gDwellingType[IDX(m_town->m_type)][dwellingTypes_c[dwellingResult_a]]
//         )];
//         strcpy(
//             gText,
//             ""
//         );
//         sprintf(detailText_i, "%s%d", cWellDetail[WELL_DETAIL_ATTACK], monsterInfo_f.attack);
//         strcat(gText, detailText_i);
//         sprintf(detailText_i, "\n%s%d", cWellDetail[WELL_DETAIL_DEFENSE], monsterInfo_f.defense);
//         strcat(gText, detailText_i);
//         sprintf(detailText_i, "\n%s%d", cWellDetail[WELL_DETAIL_DAMAGE], monsterInfo_f.damageMin);
//         strcat(gText, detailText_i);
//         if (monsterInfo_f.damageMin != monsterInfo_f.damageMax) {
//             sprintf(detailText_i, "-%d", monsterInfo_f.damageMax);
//             strcat(gText, detailText_i);
//         }
//         sprintf(detailText_i, "\n%s%d", cWellDetail[WELL_DETAIL_HIT_POINTS], monsterInfo_f.hitPoints);
//         strcat(gText, detailText_i);
//         sprintf(detailText_i, cWellDetail[WELL_DETAIL_SPEED], speedText[monsterInfo_f.speed]);
//         strcat(gText, detailText_i);
//         if (m_town->m_buildings
//             & (1L << (dwellingTypes_c[dwellingResult_a] + IDX(BUILDING_SLOT_DWELLING_FIRST)))) {
//             growth_a = gMonsterDatabase[IDX(gDwellingType[IDX(m_town->m_type)]
//                                                         [dwellingTypes_c[dwellingResult_a]])]
//                           .growth;
//             growth_a += TOWN_WELL_BASE_GROWTH_BONUS;
//             if (dwellingResult_a == 0
//                 && (m_town->m_buildings & (1L << TOWN_WELL_FIRST_DWELLING_GROWTH_BUILDING)))
//                 growth_a += TOWN_WELL_FIRST_DWELLING_GROWTH_BONUS;
//             sprintf(detailText_i, cWellDetail[WELL_DETAIL_GROWTH], growth_a);
//             strcat(gText, detailText_i);
//         }
//         message_i.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
//         message_i.payload.widget.id = dwellingResult_a + TOWN_WELL_FIRST_DETAIL_CONTROL;
//         message_i.payload.widget.data.text = gText;
//         window->BroadcastMessage(message_i);
//     }
// }
//
// Exact independent storage edits (MASK=0..127):
// [
//   {
//     "bit": 0,
//     "find": "    i16 H2_UNUSED(unusedFirstIcon_d) = 1;\n",
//     "replace": ""
//   },
//   {
//     "bit": 1,
//     "find": "    i16 H2_UNUSED(unusedFirstName_b) = TOWN_WELL_FIRST_NAME_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 2,
//     "find": "    i16 H2_UNUSED(unusedFirstMonsterIcon_p) = TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 3,
//     "find": "    i16 H2_UNUSED(unusedFirstCreature_f) = TOWN_WELL_FIRST_CREATURE_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 4,
//     "find": "    i16 H2_UNUSED(unusedFirstDetail_g) = TOWN_WELL_FIRST_DETAIL_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 5,
//     "find": "    i16 H2_UNUSED(unusedFirstAvailable) = TOWN_WELL_FIRST_AVAILABLE_CONTROL;\n",
//     "replace": ""
//   },
//   {
//     "bit": 6,
//     "find": "    i16 H2_UNUSED(unusedFirstAvailableCount) = TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;\n",
//     "replace": ""
//   }
// ]
// Exact independent cast edits (CASTS=0..3):
// [
//   {
//     "bit": 0,
//     "find": "static_cast<u8>(dwellingResult_a + TOWN_WELL_FIRST_UPGRADE_OFFSET)",
//     "replace": "dwellingResult_a + TOWN_WELL_FIRST_UPGRADE_OFFSET"
//   },
//   {
//     "bit": 1,
//     "find": "static_cast<u8>(dwellingResult_a)",
//     "replace": "dwellingResult_a"
//   }
// ]
// Every option omitMASK_castsCASTS applies both masks; all 512 option strings replayed exactly.

// Readability structural matrices: ReadabilityFormulas
// Not build input. All matrices use --min-depth 0 --max-depth 0
// --state-trials 0 and --limit equal to the complete exact-axis product.
// Exact flags are the tool audit results, not rounded fuzzy percentages.
// Preserving a pre-existing retail residual is not a new exact closure.
// Macro/helper declarations are in their corresponding owner headers.

// h43-formula: src/SOURCE/DRAWING.cpp, RVA 0x39058
// Artifacts: build/readability/h43-formula-{axes,manifest}.json;
//            build/readability/h43-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 63, "candidate_size": 1376, "choices": {"candidate": "baseline", "h43-formula": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "a0aab71572097e63", "retail_relocs": 60, "text_sha": "9d5c35ecf8315e11"}
// {"candidate_relocs": 63, "candidate_size": 1376, "choices": {"candidate": "baseline", "h43-formula": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "a0aab71572097e63", "retail_relocs": 60, "text_sha": "9d5c35ecf8315e11"}
// Axis: h43-formula
// Arm: explicit
hexIndex < 0 || hexIndex >= COMBAT_HEX_COUNT || hexIndex % COMBAT_GRID_ROW_LENGTH == 0
        || hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1

// Arm: helper
!IS_INTERIOR_COMBAT_HEX(hexIndex)

// h44-formula: src/SOURCE/ARMY.cpp, RVA 0x1a1d9
// Artifacts: build/readability/h44-formula-{axes,manifest}.json;
//            build/readability/h44-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 180, "candidate_size": 5727, "choices": {"candidate": "baseline", "h44-formula": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "2d3c689fe1a264a8", "retail_relocs": 176, "text_sha": "21ab4b5e167a5228"}
// {"candidate_relocs": 180, "candidate_size": 5727, "choices": {"candidate": "baseline", "h44-formula": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "2d3c689fe1a264a8", "retail_relocs": 176, "text_sha": "21ab4b5e167a5228"}
// Axis: h44-formula
// Arm: explicit
static_cast<i32>(
        sqrt(static_cast<double>(xStretch * xStretch + yStretch * yStretch))
    )

// Arm: helper
INTEGER_VECTOR_LENGTH(xStretch, yStretch)

// h56-formula: src/SOURCE/ARMY.cpp, RVA 0x20238
// Artifacts: build/readability/h56-formula-{axes,manifest}.json;
//            build/readability/h56-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 24, "candidate_size": 861, "choices": {"candidate": "baseline", "h56-formula": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "22cb15b8781584c9", "retail_relocs": 24, "text_sha": "f50b176c9ff19239"}
// {"candidate_relocs": 24, "candidate_size": 861, "choices": {"candidate": "baseline", "h56-formula": "helper"}, "exact": true, "exact_rejections": [], "reloc_sha": "22cb15b8781584c9", "retail_relocs": 24, "text_sha": "f50b176c9ff19239"}
// Axis: h56-formula
// Arm: explicit
adjacentCell->m_occupantSide == m_targetSide
                            && adjacentCell->m_occupantIndex == m_targetIndex

// Arm: helper
HEX_HAS_OCCUPANT(*adjacentCell, m_targetSide, m_targetIndex)

// h58-negative-formula: src/SOURCE/COMMAND.cpp, RVA 0x2bb0e
// Artifacts: build/readability/h58-negative-formula-{axes,manifest}.json;
//            build/readability/h58-negative-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 6, "candidate_size": 291, "choices": {"candidate": "baseline", "h58-negative-formula": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "9a56f27adf60ddbd", "retail_relocs": 6, "text_sha": "118e449d28ef1a57"}
// {"candidate_relocs": 6, "candidate_size": 291, "choices": {"candidate": "baseline", "h58-negative-formula": "helper"}, "exact": true, "exact_rejections": [], "reloc_sha": "9a56f27adf60ddbd", "retail_relocs": 6, "text_sha": "118e449d28ef1a57"}
// Axis: h58-negative-formula
// Arm: explicit
(gpCombatManager->m_inCastleCombat == 0
                  || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
                  || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
                      && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
                          || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                     .m_occupantSide
                                 != COMBAT_SIDE_NONE
                          || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                     .m_deadOccupantCount
                                 != 0)))

// Arm: helper
!CAN_PASS_CASTLE_GATE(hexIndex)

// h58-positive-formula: src/SOURCE/PATH.cpp, RVA 0x7d13b
// Artifacts: build/readability/h58-positive-formula-{axes,manifest}.json;
//            build/readability/h58-positive-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 20, "candidate_size": 550, "choices": {"candidate": "baseline", "h58-positive-formula": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "f8a862f461a103b0", "retail_relocs": 20, "text_sha": "4f44c17c7d5418d0"}
// {"candidate_relocs": 20, "candidate_size": 550, "choices": {"candidate": "baseline", "h58-positive-formula": "helper"}, "exact": true, "exact_rejections": [], "reloc_sha": "f8a862f461a103b0", "retail_relocs": 20, "text_sha": "4f44c17c7d5418d0"}
// Axis: h58-positive-formula
// Arm: explicit
(gpCombatManager->m_inCastleCombat
                && (destHexNext == COMBAT_CASTLE_GATE_APPROACH_HEX
                    || destHexNext == IDX(COMBAT_CASTLE_HEX_GATE))
                && (gpCombatManager->m_drawbridgeState != COMBAT_DRAWBRIDGE_RAISED
                    || (gpCombatManager->m_currentSide == COMBAT_DEFENDER_SIDE
                        && gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                   .m_occupantSide
                               == COMBAT_SIDE_NONE
                        && gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                   .m_deadOccupantCount
                               == 0)))

// Arm: helper
CAN_PASS_CASTLE_GATE(destHexNext)

// h63-formula: src/SOURCE/ADVMGR.cpp, RVA 0x10f50
// Artifacts: build/readability/h63-formula-{axes,manifest}.json;
//            build/readability/h63-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 44, "candidate_size": 963, "choices": {"candidate": "baseline", "h63-formula": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "54f8a11de99c7ad1", "retail_relocs": 44, "text_sha": "efa4086529c13978"}
// {"candidate_relocs": 44, "candidate_size": 963, "choices": {"candidate": "baseline", "h63-formula": "helper"}, "exact": true, "exact_rejections": [], "reloc_sha": "54f8a11de99c7ad1", "retail_relocs": 44, "text_sha": "efa4086529c13978"}
// Axis: h63-formula
// Arm: explicit
giVisRange[IDX(mapHero->m_secondarySkills[IDX(HERO_SKILL_SCOUTING)])]
            + (mapHero->HasArtifact(ARTIFACT_TELESCOPE) != 0)

// Arm: helper
HERO_SCOUTING_VISIBILITY_RADIUS(*mapHero)

// h72-formula: src/SOURCE/Castle.cpp, RVA 0x24320
// Artifacts: build/readability/h72-formula-{axes,manifest}.json;
//            build/readability/h72-formula-matrix/results.json
// Complete: 3/3
// {"candidate_relocs": 142, "candidate_size": 3856, "choices": {"candidate": "baseline", "h72-formula": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "3a6ab4e1c449b006", "retail_relocs": 139, "text_sha": "c3f48f8d1e052b80"}
// {"candidate_relocs": 142, "candidate_size": 3856, "choices": {"candidate": "baseline", "h72-formula": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "3a6ab4e1c449b006", "retail_relocs": 139, "text_sha": "c3f48f8d1e052b80"}
// {"candidate_relocs": 142, "candidate_size": 3861, "choices": {"candidate": "baseline", "h72-formula": "inline"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 3861, retail 3856", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "7eb0cc17221e9370", "retail_relocs": 139, "text_sha": "80f0c9747843c2fd"}
// Axis: h72-formula
// Arm: explicit
m_town->m_buildState + 1 < TOWN_MAGE_GUILD_MAX_LEVEL ? m_town->m_buildState + 1
                                                                     : TOWN_MAGE_GUILD_MAX_LEVEL

// Arm: helper
NEXT_MAGE_GUILD_LEVEL(m_town->m_buildState)

// Arm: inline
NextMageGuildLevel(m_town->m_buildState)

// h73-formula: src/SOURCE/TOWNMGR.cpp, RVA 0xa986a
// Artifacts: build/readability/h73-formula-{axes,manifest}.json;
//            build/readability/h73-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 45, "candidate_size": 929, "choices": {"candidate": "baseline", "h73-formula": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "01311810057f2657", "retail_relocs": 45, "text_sha": "90cbf264cc426e82"}
// {"candidate_relocs": 45, "candidate_size": 929, "choices": {"candidate": "baseline", "h73-formula": "helper"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "01311810057f2657", "retail_relocs": 45, "text_sha": "90cbf264cc426e82"}
// Axis: h73-formula
// Arm: explicit
HAS(m_town->m_buildings, BIT(building))
        && (building != BUILDING_SLOT_MAGE_GUILD
            || m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)

// Arm: helper
TOWN_BUILDING_COMPLETE(*m_town, building)

// h73-long-formula: src/SOURCE/Castle.cpp, RVA 0x24320
// Artifacts: build/readability/h73-long-formula-{axes,manifest}.json;
//            build/readability/h73-long-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 142, "candidate_size": 3856, "choices": {"candidate": "baseline", "h73-long-formula": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "3a6ab4e1c449b006", "retail_relocs": 139, "text_sha": "c3f48f8d1e052b80"}
// {"candidate_relocs": 142, "candidate_size": 3856, "choices": {"candidate": "baseline", "h73-long-formula": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "3a6ab4e1c449b006", "retail_relocs": 139, "text_sha": "c3f48f8d1e052b80"}
// Axis: h73-long-formula
// Arm: explicit
HAS(m_town->m_buildings, (1L << IDX(castleSlotsUse[slotNum])))
            && (castleSlotsUse[slotNum] != CASTLE_MAGE_GUILD
                || m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)

// Arm: helper
TOWN_BUILDING_COMPLETE(*m_town, castleSlotsUse[slotNum])

// h87-formula: src/SOURCE/GAME.cpp, RVA 0x4e8d9
// Artifacts: build/readability/h87-formula-{axes,manifest}.json;
//            build/readability/h87-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 158, "candidate_size": 2681, "choices": {"candidate": "baseline", "h87-formula": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "3a2b749ed9275c4d", "retail_relocs": 158, "text_sha": "cd864eec504ad031"}
// {"candidate_relocs": 158, "candidate_size": 2681, "choices": {"candidate": "baseline", "h87-formula": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "3a2b749ed9275c4d", "retail_relocs": 158, "text_sha": "cd864eec504ad031"}
// Axis: h87-formula
// Arm: explicit
    giCurTurn = m_day + (m_week - 1) * EVENT_DAYS_PER_WEEK
                + (m_month - 1) * EVENT_DAYS_PER_MONTH;

// Arm: helper
    giCurTurn = GAME_DAY_NUMBER(*this);

// h92-formula: src/SOURCE/EVENTS.cpp, RVA 0x41fbc
// Artifacts: build/readability/h92-formula-{axes,manifest}.json;
//            build/readability/h92-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 14, "candidate_size": 293, "choices": {"candidate": "baseline", "h92-formula": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "fd8d7ea24e980b21", "retail_relocs": 14, "text_sha": "c17e05a02a074b98"}
// {"candidate_relocs": 14, "candidate_size": 293, "choices": {"candidate": "baseline", "h92-formula": "helper"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "fd8d7ea24e980b21", "retail_relocs": 14, "text_sha": "c17e05a02a074b98"}
// Axis: h92-formula
// Arm: explicit
gpCurPlayer->m_barrierTents & (1 << colorIndex)

// Arm: helper
PLAYER_HAS_VISITED_TENT(*gpCurPlayer, colorIndex)

// h93-formula: src/SOURCE/EVENTS.cpp, RVA 0x42952
// Artifacts: build/readability/h93-formula-{axes,manifest}.json;
//            build/readability/h93-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 13, "candidate_size": 368, "choices": {"candidate": "baseline", "h93-formula": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "8914fc685160182d", "retail_relocs": 13, "text_sha": "648c564f141b1309"}
// {"candidate_relocs": 13, "candidate_size": 362, "choices": {"candidate": "baseline", "h93-formula": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 362, retail 368", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "58f9fe5dc9bd28ae", "retail_relocs": 13, "text_sha": "3c99926b31041155"}
// Axis: h93-formula
// Arm: explicit
    quantity = static_cast<i16>(cell->m_objectMetadata);
    quantity >>= EVENT_RECRUIT_COUNT_SHIFT;

// Arm: helper
    quantity = RecruitSiteCount(static_cast<i16>(cell->m_objectMetadata));

// h96-formula: src/SOURCE/ADVMGR.cpp, RVA 0x115f5
// Artifacts: build/readability/h96-formula-{axes,manifest}.json;
//            build/readability/h96-formula-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 67, "candidate_size": 1032, "choices": {"candidate": "baseline", "h96-formula": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "d3a96cfdbd5628ff", "retail_relocs": 64, "text_sha": "9d80ed2c2cf29be1"}
// {"candidate_relocs": 67, "candidate_size": 1032, "choices": {"candidate": "baseline", "h96-formula": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "d3a96cfdbd5628ff", "retail_relocs": 64, "text_sha": "9d80ed2c2cf29be1"}
// Axis: h96-formula
// Arm: explicit
abs(

                            gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_x
                            - targetHero->m_x
                        )
                        + abs(
                            gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_y
                            - targetHero->m_y
                        )

// Arm: helper
MANHATTAN_LENGTH(gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_x
                            - targetHero->m_x, gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]].m_y
                            - targetHero->m_y)

// Disposition: see docs/readability/implementation.md for retained subsets,
// caller boundaries and the combined original-snapshot object comparison.

// Rejected inline declarations, removed from owner headers after measurement:
inline i32 NextMageGuildLevel(i32 level) {
    return level + 1 < TOWN_MAGE_GUILD_LEVEL_COUNT ? level + 1 : TOWN_MAGE_GUILD_LEVEL_COUNT;
}
inline i16 RecruitSiteCount(i16 packed) {
    return static_cast<i16>(packed >> EVENT_RECRUIT_COUNT_SHIFT);
}

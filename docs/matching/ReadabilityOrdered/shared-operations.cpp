// Readability structural matrices: ReadabilityOrdered
// Not build input. All matrices use --min-depth 0 --max-depth 0
// --state-trials 0 and --limit equal to the complete exact-axis product.
// Exact flags are the tool audit results, not rounded fuzzy percentages.
// Preserving a pre-existing retail residual is not a new exact closure.
// Macro/helper declarations are in their corresponding owner headers.

// h32-ordered: src/SOURCE/ADVMGR.cpp, RVA 0x11515
// Artifacts: build/readability/h32-ordered-{axes,manifest}.json;
//            build/readability/h32-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 11, "candidate_size": 224, "choices": {"candidate": "baseline", "h32-ordered": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "dd8ecee5b2376b1f", "retail_relocs": 11, "text_sha": "99339e4b3baecd6a"}
// {"candidate_relocs": 11, "candidate_size": 224, "choices": {"candidate": "baseline", "h32-ordered": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "dd8ecee5b2376b1f", "retail_relocs": 11, "text_sha": "99339e4b3baecd6a"}
// Axis: h32-ordered
// Arm: explicit
gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = TOWN_PORTAL_CLOSE_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;

// Arm: macro
FINISH_DIALOG_MESSAGE(message);

// h34-ordered: src/SOURCE/ARMY.cpp, RVA 0x197b2
// Artifacts: build/readability/h34-ordered-{axes,manifest}.json;
//            build/readability/h34-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 145, "candidate_size": 2599, "choices": {"candidate": "baseline", "h34-ordered": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "ea76d213923cdb3b", "retail_relocs": 145, "text_sha": "e3a14c8b9e239088"}
// {"candidate_relocs": 145, "candidate_size": 2609, "choices": {"candidate": "baseline", "h34-ordered": "macro"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 2609, retail 2599", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "ea76d213923cdb3b", "retail_relocs": 145, "text_sha": "2c71224201ea3dfd"}
// Axis: h34-ordered
// Arm: explicit
if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMinExtentY < 0)
        giMinExtentY = 0;
    if (giMaxExtentX > ARMY_COMBAT_MAX_X)
        giMaxExtentX = ARMY_COMBAT_MAX_X;
    if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
        giMaxExtentY = ARMY_COMBAT_MAX_Y;

// Arm: macro
CLIP_COMBAT_EXTENTS();

// h35-ordered: src/SOURCE/ARMY.cpp, RVA 0x197b2
// Artifacts: build/readability/h35-ordered-{axes,manifest}.json;
//            build/readability/h35-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 145, "candidate_size": 2599, "choices": {"candidate": "baseline", "h35-ordered": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "ea76d213923cdb3b", "retail_relocs": 145, "text_sha": "e3a14c8b9e239088"}
// {"candidate_relocs": 145, "candidate_size": 2599, "choices": {"candidate": "baseline", "h35-ordered": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "ea76d213923cdb3b", "retail_relocs": 145, "text_sha": "e3a14c8b9e239088"}
// Axis: h35-ordered
// Arm: explicit
gpWindowManager->UpdateScreenRegion(
                tempLeft,
                tempTop,
                tempRight - tempLeft + 1,
                tempBottom - tempTop + 1
            )

// Arm: macro
UPDATE_INCLUSIVE_REGION(tempLeft, tempTop, tempRight, tempBottom)

// h36-ordered: src/SOURCE/ARMY.cpp, RVA 0x1a1d9
// Artifacts: build/readability/h36-ordered-{axes,manifest}.json;
//            build/readability/h36-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 180, "candidate_size": 5727, "choices": {"candidate": "baseline", "h36-ordered": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "88e9897216820dbc", "retail_relocs": 176, "text_sha": "21ab4b5e167a5228"}
// {"candidate_relocs": 180, "candidate_size": 5727, "choices": {"candidate": "baseline", "h36-ordered": "macro"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "88e9897216820dbc", "retail_relocs": 176, "text_sha": "21ab4b5e167a5228"}
// Axis: h36-ordered
// Arm: explicit
static_cast<i32>(
                KBTickCount() + shotDelay * gfCombatSpeedMod[gConfig.combatSpeed]
            )

// Arm: macro
COMBAT_DEADLINE(shotDelay)

// h38-ordered: src/SOURCE/ARMY.cpp, RVA 0x1bddb
// Artifacts: build/readability/h38-ordered-{axes,manifest}.json;
//            build/readability/h38-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 170, "candidate_size": 5189, "choices": {"candidate": "baseline", "h38-ordered": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "55c46874fd8ffb1a", "retail_relocs": 168, "text_sha": "845c9255be5ff4aa"}
// {"candidate_relocs": 170, "candidate_size": 5189, "choices": {"candidate": "baseline", "h38-ordered": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "55c46874fd8ffb1a", "retail_relocs": 168, "text_sha": "845c9255be5ff4aa"}
// Axis: h38-ordered
// Arm: explicit
m_quantity <= 1 ? gArmyNames[IDX(m_monsterType)]
                            : gArmyNamesPlural[IDX(m_monsterType)]

// Arm: macro
CREATURE_DISPLAY_NAME(m_monsterType, m_quantity)

// h40-ordered: src/SOURCE/ARMY.cpp, RVA 0x1f1a0
// Artifacts: build/readability/h40-ordered-{axes,manifest}.json;
//            build/readability/h40-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 20, "candidate_size": 863, "choices": {"candidate": "baseline", "h40-ordered": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "a9e172fc095b3ae8", "retail_relocs": 19, "text_sha": "7852a07dd4038333"}
// {"candidate_relocs": 20, "candidate_size": 863, "choices": {"candidate": "baseline", "h40-ordered": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "a9e172fc095b3ae8", "retail_relocs": 19, "text_sha": "7852a07dd4038333"}
// Axis: h40-ordered
// Arm: explicit
frontCell_1->m_occupantSide = COMBAT_SIDE_NONE;
            frontCell_1->m_occupantIndex = -1;

// Arm: macro
CLEAR_HEX_OCCUPANT(*frontCell_1);

// h53-ordered: src/SOURCE/AI.cpp, RVA 0x1628b
// Artifacts: build/readability/h53-ordered-{axes,manifest}.json;
//            build/readability/h53-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 92, "candidate_size": 4000, "choices": {"candidate": "baseline", "h53-ordered": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "e5ede5c89b245cc1", "retail_relocs": 92, "text_sha": "147ff2d01b0462bb"}
// {"candidate_relocs": 92, "candidate_size": 4000, "choices": {"candidate": "baseline", "h53-ordered": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "e5ede5c89b245cc1", "retail_relocs": 92, "text_sha": "147ff2d01b0462bb"}
// Axis: h53-ordered
// Arm: explicit
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;

// Arm: macro
                SET_NEXT_COMBAT_MOVE((m_armies[IDX(sideEnemy)] + best)
                        ->m_hex);

// h57-ordered: src/SOURCE/ARMY.cpp, RVA 0x18650
// Artifacts: build/readability/h57-ordered-{axes,manifest}.json;
//            build/readability/h57-ordered-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 0, "candidate_size": 194, "choices": {"candidate": "baseline", "h57-ordered": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "e3b0c44298fc1c14", "retail_relocs": 0, "text_sha": "05a783845a057a60"}
// {"candidate_relocs": 0, "candidate_size": 194, "choices": {"candidate": "baseline", "h57-ordered": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "e3b0c44298fc1c14", "retail_relocs": 0, "text_sha": "05a783845a057a60"}
// Axis: h57-ordered
// Arm: explicit
    m_targetSide = COMBAT_SIDE_NONE;
    m_targetIndex = -1;

// Arm: macro
    CLEAR_ARMY_TARGET(*this);

// Disposition: see docs/readability/implementation.md for retained subsets,
// caller boundaries and the combined original-snapshot object comparison.

// Rejected H34 declaration, present only during the two-arm matrix:
#define CLIP_COMBAT_EXTENTS() \
    do { \
        if (giMinExtentX < 0) giMinExtentX = 0; \
        if (giMinExtentY < 0) giMinExtentY = 0; \
        if (giMaxExtentX > COMBAT_MAX_EXTENT_X) giMaxExtentX = COMBAT_MAX_EXTENT_X; \
        if (giMaxExtentY > COMBAT_MAX_EXTENT_Y) giMaxExtentY = COMBAT_MAX_EXTENT_Y; \
    } while (0)

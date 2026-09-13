// Readability structural matrices: ReadabilityDomains
// Not build input. All matrices use --min-depth 0 --max-depth 0
// --state-trials 0 and --limit equal to the complete exact-axis product.
// Exact flags are the tool audit results, not rounded fuzzy percentages.
// Preserving a pre-existing retail residual is not a new exact closure.
// Macro/helper declarations are in their corresponding owner headers.

// h20-building: src/SOURCE/ARMYGRP.cpp, RVA 0x21802
// Artifacts: build/readability/h20-building-{axes,manifest}.json;
//            build/readability/h20-building-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 11, "candidate_size": 561, "choices": {"candidate": "baseline", "h20-building": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "17a8dc6483d9e4fa", "retail_relocs": 11, "text_sha": "abf4ee01cc6e6957"}
// {"candidate_relocs": 11, "candidate_size": 561, "choices": {"candidate": "baseline", "h20-building": "has"}, "exact": true, "exact_rejections": [], "reloc_sha": "17a8dc6483d9e4fa", "retail_relocs": 11, "text_sha": "abf4ee01cc6e6957"}
// Axis: h20-building
// Arm: explicit
occupiedTown->m_buildings & IDX(TOWN_BUILDING_TAVERN)

// Arm: has
HAS(occupiedTown->m_buildings, IDX(TOWN_BUILDING_TAVERN))

// h20-call-grouping: src/SOURCE/ARMYGRP.cpp, RVA 0x21802
// Artifacts: build/readability/h20-call-grouping-{axes,manifest}.json;
//            build/readability/h20-call-grouping-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 11, "candidate_size": 561, "choices": {"call_grouping": "grouped", "candidate": "baseline"}, "exact": true, "exact_rejections": [], "reloc_sha": "17a8dc6483d9e4fa", "retail_relocs": 11, "text_sha": "abf4ee01cc6e6957"}
// {"candidate_relocs": 11, "candidate_size": 561, "choices": {"call_grouping": "primary", "candidate": "baseline"}, "exact": true, "exact_rejections": [], "reloc_sha": "17a8dc6483d9e4fa", "retail_relocs": 11, "text_sha": "abf4ee01cc6e6957"}
// Axis: call_grouping
// Arm: grouped
(HAS(occupiedTown->m_buildings, IDX(TOWN_BUILDING_TAVERN)))

// Arm: primary
HAS(occupiedTown->m_buildings, IDX(TOWN_BUILDING_TAVERN))

// h21-troop: src/SOURCE/ARMYGRP.cpp, RVA 0x2174e
// Artifacts: build/readability/h21-troop-{axes,manifest}.json;
//            build/readability/h21-troop-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 1, "candidate_size": 90, "choices": {"candidate": "baseline", "h21-troop": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "4fdbc0d0f2e5edbf", "retail_relocs": 1, "text_sha": "9109923f77f69cb8"}
// {"candidate_relocs": 1, "candidate_size": 101, "choices": {"candidate": "baseline", "h21-troop": "method"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 101, retail 90", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "4fdbc0d0f2e5edbf", "retail_relocs": 1, "text_sha": "46067dd721a5cad1"}
// Axis: h21-troop
// Arm: explicit
        if (m_creatureTypes[slot] != CREATURE_NONE

// Arm: method
        if (HasTroop(slot)

// h24-terrain: src/SOURCE/FINDPATH.cpp, RVA 0x4a136
// Artifacts: build/readability/h24-terrain-{axes,manifest}.json;
//            build/readability/h24-terrain-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 98, "candidate_size": 1369, "choices": {"candidate": "baseline", "h24-terrain": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "ee9cbf6a1e04c867", "retail_relocs": 98, "text_sha": "f9b5b989ecafd846"}
// {"candidate_relocs": 98, "candidate_size": 1369, "choices": {"candidate": "baseline", "h24-terrain": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "ee9cbf6a1e04c867", "retail_relocs": 98, "text_sha": "f9b5b989ecafd846"}
// Axis: h24-terrain
// Arm: explicit
giGroundToTerrain[gSearchNextCell->m_terrainImageIndex]

// Arm: macro
CELL_TERRAIN(gSearchNextCell)

// h25-sprite: src/SOURCE/FINDPATH.cpp, RVA 0x4a136
// Artifacts: build/readability/h25-sprite-{axes,manifest}.json;
//            build/readability/h25-sprite-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 98, "candidate_size": 1369, "choices": {"candidate": "baseline", "h25-sprite": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "ee9cbf6a1e04c867", "retail_relocs": 98, "text_sha": "f9b5b989ecafd846"}
// {"candidate_relocs": 98, "candidate_size": 1369, "choices": {"candidate": "baseline", "h25-sprite": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "ee9cbf6a1e04c867", "retail_relocs": 98, "text_sha": "f9b5b989ecafd846"}
// Axis: h25-sprite
// Arm: explicit
gSearchCurrentCell->m_objectIndex != SEARCH_NO_OBJECT
                && gSearchCurrentCell->m_objectTileset != TILESET_DUMMY
                && (gSearchCurrentCell->m_flags & SEARCH_CELL_BLOCKED) == 0

// Arm: macro
CELL_HAS_NON_SHADOW_OBJECT(gSearchCurrentCell)

// h28-army: src/SOURCE/HEXCELL.cpp, RVA 0x64d06
// Artifacts: build/readability/h28-army-{axes,manifest}.json;
//            build/readability/h28-army-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 2, "candidate_size": 161, "choices": {"candidate": "baseline", "h28-army": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "32d1f9f80d947b89", "retail_relocs": 2, "text_sha": "3dbe81a120602f68"}
// {"candidate_relocs": 2, "candidate_size": 161, "choices": {"candidate": "baseline", "h28-army": "method"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "32d1f9f80d947b89", "retail_relocs": 2, "text_sha": "4423b3ef3b07671d"}
// Axis: h28-army
// Arm: explicit
                &gpCombatManager->m_armies[IDX(m_deadOccupantSides[i])][m_deadOccupantIndices[i]];

// Arm: method
                &gpCombatManager->GetArmy(IDX(m_deadOccupantSides[i]), m_deadOccupantIndices[i]);

// h29-capacity: src/SOURCE/HERO.cpp, RVA 0x63b0f
// Artifacts: build/readability/h29-capacity-{axes,manifest}.json;
//            build/readability/h29-capacity-matrix/results.json
// Complete: 3/3
// {"candidate_relocs": 143, "candidate_size": 2310, "choices": {"candidate": "baseline", "h29-capacity": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "34388de3666fd1d9", "retail_relocs": 143, "text_sha": "f2be759ecfd28f3e"}
// {"candidate_relocs": 143, "candidate_size": 2310, "choices": {"candidate": "baseline", "h29-capacity": "method"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "34388de3666fd1d9", "retail_relocs": 143, "text_sha": "d6cd34b4dc8cff68"}
// {"candidate_relocs": 143, "candidate_size": 2310, "choices": {"candidate": "baseline", "h29-capacity": "macro"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "34388de3666fd1d9", "retail_relocs": 143, "text_sha": "f2be759ecfd28f3e"}
// Axis: h29-capacity
// Arm: explicit
gpHVHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE

// Arm: method
gpHVHero->NormalSpellPointCapacity()

// Arm: macro
HERO_NORMAL_SPELL_POINTS(*gpHVHero)

// h33-elemental: src/SOURCE/ARMY.cpp, RVA 0x187f1
// Artifacts: build/readability/h33-elemental-{axes,manifest}.json;
//            build/readability/h33-elemental-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 20, "candidate_size": 908, "choices": {"candidate": "baseline", "h33-elemental": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "9b4313c8de15bd72", "retail_relocs": 20, "text_sha": "3ed9d8c7afe6b7bd"}
// {"candidate_relocs": 20, "candidate_size": 908, "choices": {"candidate": "baseline", "h33-elemental": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "9b4313c8de15bd72", "retail_relocs": 20, "text_sha": "3ed9d8c7afe6b7bd"}
// Axis: h33-elemental
// Arm: explicit
m_monsterType == CREATURE_EARTH_ELEMENTAL || m_monsterType == CREATURE_AIR_ELEMENTAL
        || m_monsterType == CREATURE_FIRE_ELEMENTAL || m_monsterType == CREATURE_WATER_ELEMENTAL

// Arm: macro
IS_ELEMENTAL_CREATURE(m_monsterType)

// h33-elemental-negative: src/SOURCE/CMBTMGR.cpp, RVA 0x271ad
// Artifacts: build/readability/h33-elemental-negative-{axes,manifest}.json;
//            build/readability/h33-elemental-negative-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 3, "candidate_size": 705, "choices": {"candidate": "baseline", "h33-elemental-negative": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "84a601dd1a07ccb8", "retail_relocs": 3, "text_sha": "209b5018c7005920"}
// {"candidate_relocs": 3, "candidate_size": 705, "choices": {"candidate": "baseline", "h33-elemental-negative": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "84a601dd1a07ccb8", "retail_relocs": 3, "text_sha": "209b5018c7005920"}
// Axis: h33-elemental-negative
// Arm: explicit
m_armies[IDX(side)][index].m_monsterType != CREATURE_EARTH_ELEMENTAL
                     && m_armies[IDX(side)][index].m_monsterType != CREATURE_AIR_ELEMENTAL
                     && m_armies[IDX(side)][index].m_monsterType != CREATURE_FIRE_ELEMENTAL
                     && m_armies[IDX(side)][index].m_monsterType != CREATURE_WATER_ELEMENTAL

// Arm: macro
!IS_ELEMENTAL_CREATURE(m_armies[IDX(side)][index].m_monsterType)

// h47-golem: src/SOURCE/SPELLS.cpp, RVA 0x9bdf2
// Artifacts: build/readability/h47-golem-{axes,manifest}.json;
//            build/readability/h47-golem-matrix/results.json
// Complete: 3/3
// {"candidate_relocs": 83, "candidate_size": 2660, "choices": {"candidate": "baseline", "h47-golem": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "9ce3c95795255cf8", "retail_relocs": 80, "text_sha": "021d3ab00602affa"}
// {"candidate_relocs": 83, "candidate_size": 2660, "choices": {"candidate": "baseline", "h47-golem": "macro"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "9ce3c95795255cf8", "retail_relocs": 80, "text_sha": "021d3ab00602affa"}
// {"candidate_relocs": 83, "candidate_size": 2713, "choices": {"candidate": "baseline", "h47-golem": "inline"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 2713, retail 2660", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "31ffbcaf093e534e", "retail_relocs": 80, "text_sha": "759e1778a5d44605"}
// Axis: h47-golem
// Arm: explicit
target1->m_monsterType == CREATURE_IRON_GOLEM
                    || target1->m_monsterType == CREATURE_STEEL_GOLEM

// Arm: macro
IS_GOLEM_CREATURE(target1->m_monsterType)

// Arm: inline
IsGolemCreature(target1->m_monsterType)

// h48-dragon: src/SOURCE/ARMY.cpp, RVA 0x1d8ee
// Artifacts: build/readability/h48-dragon-{axes,manifest}.json;
//            build/readability/h48-dragon-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 21, "candidate_size": 1092, "choices": {"candidate": "baseline", "h48-dragon": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "0781b6ac98541049", "retail_relocs": 21, "text_sha": "3ecd4772f6881c26"}
// {"candidate_relocs": 21, "candidate_size": 1092, "choices": {"candidate": "baseline", "h48-dragon": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "0781b6ac98541049", "retail_relocs": 21, "text_sha": "3ecd4772f6881c26"}
// Axis: h48-dragon
// Arm: explicit
target->m_monsterType == CREATURE_GREEN_DRAGON
            || target->m_monsterType == CREATURE_RED_DRAGON
            || target->m_monsterType == CREATURE_BLACK_DRAGON
            || target->m_monsterType == CREATURE_BONE_DRAGON

// Arm: macro
IS_DRAGON_CREATURE(target->m_monsterType)

// h55-lich: src/SOURCE/ARMY.cpp, RVA 0x18b7d
// Artifacts: build/readability/h55-lich-{axes,manifest}.json;
//            build/readability/h55-lich-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 87, "candidate_size": 1101, "choices": {"candidate": "baseline", "h55-lich": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "7e3457b377e41e1f", "retail_relocs": 87, "text_sha": "77dbdcec7318c25c"}
// {"candidate_relocs": 87, "candidate_size": 1101, "choices": {"candidate": "baseline", "h55-lich": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "7e3457b377e41e1f", "retail_relocs": 87, "text_sha": "77dbdcec7318c25c"}
// Axis: h55-lich
// Arm: explicit
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {

// Arm: macro
    if (IS_LICH_CREATURE(m_monsterType)) {

// h55-vampire: src/SOURCE/FLY.cpp, RVA 0x4b5ce
// Artifacts: build/readability/h55-vampire-{axes,manifest}.json;
//            build/readability/h55-vampire-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 111, "candidate_size": 2530, "choices": {"candidate": "baseline", "h55-vampire": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "312ebdb91bbda449", "retail_relocs": 105, "text_sha": "221742905cbb15e9"}
// {"candidate_relocs": 111, "candidate_size": 2530, "choices": {"candidate": "baseline", "h55-vampire": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "312ebdb91bbda449", "retail_relocs": 105, "text_sha": "221742905cbb15e9"}
// Axis: h55-vampire
// Arm: explicit
m_monsterType == CREATURE_VAMPIRE
                         || m_monsterType == CREATURE_VAMPIRE_LORD

// Arm: macro
IS_VAMPIRE_CREATURE(m_monsterType)

// h55-troll: src/SOURCE/COMMAND.cpp, RVA 0x2d1a5
// Artifacts: build/readability/h55-troll-{axes,manifest}.json;
//            build/readability/h55-troll-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 8, "candidate_size": 452, "choices": {"candidate": "baseline", "h55-troll": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "09785f71bd87c130", "retail_relocs": 8, "text_sha": "8324a4b5c296349c"}
// {"candidate_relocs": 8, "candidate_size": 452, "choices": {"candidate": "baseline", "h55-troll": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "09785f71bd87c130", "retail_relocs": 8, "text_sha": "8324a4b5c296349c"}
// Axis: h55-troll
// Arm: explicit
currentArmy->m_monsterType == CREATURE_TROLL
                    || currentArmy->m_monsterType == CREATURE_WAR_TROLL

// Arm: macro
IS_TROLL_CREATURE(currentArmy->m_monsterType)

// h49-influence: src/SOURCE/ARMY.cpp, RVA 0x1a1d9
// Artifacts: build/readability/h49-influence-{axes,manifest}.json;
//            build/readability/h49-influence-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 180, "candidate_size": 5727, "choices": {"candidate": "baseline", "h49-influence": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "414b6a7be8f94021", "retail_relocs": 176, "text_sha": "21ab4b5e167a5228"}
// {"candidate_relocs": 180, "candidate_size": 5727, "choices": {"candidate": "baseline", "h49-influence": "macro"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "414b6a7be8f94021", "retail_relocs": 176, "text_sha": "21ab4b5e167a5228"}
// Axis: h49-influence
// Arm: explicit
m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
        || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]

// Arm: macro
ARMY_HAS_BERSERK_OR_HYPNOTIZE(*this)

// h50-influence: src/SOURCE/SPELLAI.cpp, RVA 0x95fb4
// Artifacts: build/readability/h50-influence-{axes,manifest}.json;
//            build/readability/h50-influence-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 128, "candidate_size": 3918, "choices": {"candidate": "baseline", "h50-influence": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "b9db0a9370bdfedb", "retail_relocs": 128, "text_sha": "d43307b80feb663d"}
// {"candidate_relocs": 128, "candidate_size": 3918, "choices": {"candidate": "baseline", "h50-influence": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "b9db0a9370bdfedb", "retail_relocs": 128, "text_sha": "d43307b80feb663d"}
// Axis: h50-influence
// Arm: explicit
targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || targetCreature->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]

// Arm: macro
ARMY_HAS_INCAPACITATING_SPELL(*targetCreature)

// h49-influence-negative: src/SOURCE/AI.cpp, RVA 0x17569
// Artifacts: build/readability/h49-influence-negative-{axes,manifest}.json;
//            build/readability/h49-influence-negative-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 0, "candidate_size": 265, "choices": {"candidate": "baseline", "h49-influence-negative": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "e3b0c44298fc1c14", "retail_relocs": 0, "text_sha": "e80be3785311d067"}
// {"candidate_relocs": 0, "candidate_size": 265, "choices": {"candidate": "baseline", "h49-influence-negative": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "e3b0c44298fc1c14", "retail_relocs": 0, "text_sha": "e80be3785311d067"}
// Axis: h49-influence-negative
// Arm: explicit
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] == 0)

// Arm: macro
            && !ARMY_HAS_BERSERK_OR_HYPNOTIZE(*currentArmy))

// h50-influence-negative: src/SOURCE/AI.cpp, RVA 0x17569
// Artifacts: build/readability/h50-influence-negative-{axes,manifest}.json;
//            build/readability/h50-influence-negative-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 0, "candidate_size": 265, "choices": {"candidate": "baseline", "h50-influence-negative": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "e3b0c44298fc1c14", "retail_relocs": 0, "text_sha": "e80be3785311d067"}
// {"candidate_relocs": 0, "candidate_size": 265, "choices": {"candidate": "baseline", "h50-influence-negative": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "e3b0c44298fc1c14", "retail_relocs": 0, "text_sha": "e80be3785311d067"}
// Axis: h50-influence-negative
// Arm: explicit
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0

// Arm: macro
            && !ARMY_HAS_INCAPACITATING_SPELL(*currentArmy)

// h76-positive: src/SOURCE/Overview.cpp, RVA 0x79ba0
// Artifacts: build/readability/h76-positive-{axes,manifest}.json;
//            build/readability/h76-positive-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 303, "candidate_size": 7750, "choices": {"candidate": "baseline", "h76-positive": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "9b466284ccd3c5de", "retail_relocs": 300, "text_sha": "628178f0823444cb"}
// {"candidate_relocs": 303, "candidate_size": 7750, "choices": {"candidate": "baseline", "h76-positive": "macro"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "9b466284ccd3c5de", "retail_relocs": 300, "text_sha": "628178f0823444cb"}
// Axis: h76-positive
// Arm: explicit
record->m_army.m_creatureTypes[i] != CREATURE_NONE
                        && record->m_army.m_creatureCounts[i] > 0

// Arm: macro
ARMY_GROUP_HAS_POSITIVE_STACK(record->m_army, i)

// h78-selection: src/SOURCE/SETUP.cpp, RVA 0x93b73
// Artifacts: build/readability/h78-selection-{axes,manifest}.json;
//            build/readability/h78-selection-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 6, "candidate_size": 254, "choices": {"candidate": "baseline", "h78-selection": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "1ca060256215e758", "retail_relocs": 6, "text_sha": "6c40d334df77d9ed"}
// {"candidate_relocs": 6, "candidate_size": 254, "choices": {"candidate": "baseline", "h78-selection": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "1ca060256215e758", "retail_relocs": 6, "text_sha": "6c40d334df77d9ed"}
// Axis: h78-selection
// Arm: explicit
        && (message.payload.widget.command == WIDGET_COMMAND_SELECT
            || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT)) {

// Arm: macro
        && (IS_WIDGET_SELECTION_COMMAND(message.payload.widget.command))) {

// Disposition: see docs/readability/implementation.md for retained subsets,
// caller boundaries and the combined original-snapshot object comparison.

// Rejected method/inline declarations, present during their source matrices:
// armyGroup owner: type-only presence is intentionally left explicit.
inline bool HasTroop(i32 slot) const {
    return m_creatureTypes[slot] != CREATURE_NONE;
}
// combatManager owner: same object, but different generated register use.
inline army& GetArmy(i32 side, i32 index) {
    return m_armies[side][index];
}
// hero owner: retain the expression macro instead.
inline i32 NormalSpellPointCapacity(void) {
    return Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
}
// Creature domain owner: retain the expression macro instead.
inline bool IsGolemCreature(H2_ENUM_PARAM(CreatureType, i32) type) {
    return type == CREATURE_IRON_GOLEM || type == CREATURE_STEEL_GOLEM;
}

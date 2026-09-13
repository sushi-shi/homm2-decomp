// Readability structural matrices: ReadabilityProtocols
// Not build input. All matrices use --min-depth 0 --max-depth 0
// --state-trials 0 and --limit equal to the complete exact-axis product.
// Exact flags are the tool audit results, not rounded fuzzy percentages.
// Preserving a pre-existing retail residual is not a new exact closure.
// Macro/helper declarations are in their corresponding owner headers.

// h45-protocol: src/SOURCE/ARMY.cpp, RVA 0x1f4ff
// Artifacts: build/readability/h45-protocol-{axes,manifest}.json;
//            build/readability/h45-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 54, "candidate_size": 863, "choices": {"candidate": "baseline", "h45-protocol": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "249930e5dc2a6d2e", "retail_relocs": 54, "text_sha": "5484cf7a24749234"}
// {"candidate_relocs": 54, "candidate_size": 869, "choices": {"candidate": "baseline", "h45-protocol": "statement-safe-block"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 869, retail 863", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "249930e5dc2a6d2e", "retail_relocs": 54, "text_sha": "92629da6297fc99b"}
// Axis: h45-protocol
// Arm: explicit
if (gCurLoadedSpellEffect != effect) {
            gpResourceManager->Dispose(gCurLoadedSpellIcon);
            gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFileId);
            gCurLoadedSpellEffect = effect;
        }

// Arm: statement-safe-block
do {
if (gCurLoadedSpellEffect != effect) {
            gpResourceManager->Dispose(gCurLoadedSpellIcon);
            gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFileId);
            gCurLoadedSpellEffect = effect;
        }
} while (0);

// h46-protocol: src/SOURCE/SPELLAI.cpp, RVA 0x95fb4
// Artifacts: build/readability/h46-protocol-{axes,manifest}.json;
//            build/readability/h46-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 128, "candidate_size": 3918, "choices": {"candidate": "baseline", "h46-protocol": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "00d2d446b096b34f", "retail_relocs": 128, "text_sha": "d43307b80feb663d"}
// {"candidate_relocs": 128, "candidate_size": 3924, "choices": {"candidate": "baseline", "h46-protocol": "statement-safe-block"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 3924, retail 3918", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "ef6c3556bdf84a52", "retail_relocs": 128, "text_sha": "418ed0a667680bda"}
// Axis: h46-protocol
// Arm: explicit
if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
                spellPowerWork += SPELL_HOURGLASS_POWER_BONUS;
            if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
                spellPowerWork += SPELL_WIZARD_HAT_POWER_BONUS;

// Arm: statement-safe-block
do {
if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
                spellPowerWork += SPELL_HOURGLASS_POWER_BONUS;
            if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
                spellPowerWork += SPELL_WIZARD_HAT_POWER_BONUS;
} while (0);

// h61-protocol: src/SOURCE/ADVMGR.cpp, RVA 0x5fc9
// Artifacts: build/readability/h61-protocol-{axes,manifest}.json;
//            build/readability/h61-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 554, "candidate_size": 9310, "choices": {"candidate": "baseline", "h61-protocol": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "acb15066bf798026", "retail_relocs": 554, "text_sha": "e8d928609afc1b2a"}
// {"candidate_relocs": 554, "candidate_size": 9316, "choices": {"candidate": "baseline", "h61-protocol": "statement-safe-block"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 9316, retail 9310", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "acb15066bf798026", "retail_relocs": 554, "text_sha": "96b144a1b1757b3f"}
// Axis: h61-protocol
// Arm: explicit
if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_51) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_56;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_50) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_57;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_49) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_58;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_47) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_55;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_46) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_55;
                                }

// Arm: statement-safe-block
do {
if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_51) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_56;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_50) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_57;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_49) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_58;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_47) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_55;
                                }
                                if (cursorFrame == HERO_SHADOW_SOURCE_FRAME_46) {
                                    cursorFrame = HERO_SHADOW_TARGET_FRAME_55;
                                }
} while (0);

// h71-protocol: src/SOURCE/GAME.cpp, RVA 0x53fd3
// Artifacts: build/readability/h71-protocol-{axes,manifest}.json;
//            build/readability/h71-protocol-matrix/results.json
// Complete: 3/3
// {"candidate_relocs": 9, "candidate_size": 775, "choices": {"candidate": "baseline", "h71-protocol": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "98274cff9d7567be", "retail_relocs": 9, "text_sha": "381afa20b054797d"}
// {"candidate_relocs": 10, "candidate_size": 767, "choices": {"candidate": "baseline", "h71-protocol": "existing-call"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 767, retail 775", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "aec2c3dc92dbdd77", "retail_relocs": 9, "text_sha": "448a5b922f8d4cef"}
// {"candidate_relocs": 9, "candidate_size": 783, "choices": {"candidate": "baseline", "h71-protocol": "inline"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 783, retail 775", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "98274cff9d7567be", "retail_relocs": 9, "text_sha": "f829e6d07afecb9d"}
// Axis: h71-protocol
// Arm: explicit
townRec4->m_army.m_creatureTypes[i] = CREATURE_NONE;
        townRec4->m_army.m_creatureCounts[i] = 0;

// Arm: existing-call
townRec4->m_army.Dismiss(i);

// Arm: inline
townRec4->m_army.DismissSlot(i);

// h74-protocol: src/SOURCE/ADVMGR.cpp, RVA 0x132d4
// Artifacts: build/readability/h74-protocol-{axes,manifest}.json;
//            build/readability/h74-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 7, "candidate_size": 200, "choices": {"candidate": "baseline", "h74-protocol": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "d09eabe93a46a60c", "retail_relocs": 7, "text_sha": "f32e00d1e0b47c19"}
// {"candidate_relocs": 7, "candidate_size": 200, "choices": {"candidate": "baseline", "h74-protocol": "helper"}, "exact": true, "exact_rejections": [], "reloc_sha": "d09eabe93a46a60c", "retail_relocs": 7, "text_sha": "f32e00d1e0b47c19"}
// Axis: h74-protocol
// Arm: explicit
msg.type = ADVMGR_BUTTON_MESSAGE;
    msg.payload.widget.command = ADVMGR_BUTTON_DISABLE;
    msg.payload.widget.data.value = BUTTON_TARGET;
    msg.payload.widget.id = BUTTON_SLOT_1;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_2;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_3;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_4;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_5;
    m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = BUTTON_SLOT_6;
    m_adventureWindow->BroadcastMessage(msg);

// Arm: helper
SET_ADVENTURE_BUTTON_FLAGS(msg, m_adventureWindow, ADVMGR_BUTTON_DISABLE);

// h79-protocol: src/SOURCE/Campaign.cpp, RVA 0x226d2
// Artifacts: build/readability/h79-protocol-{axes,manifest}.json;
//            build/readability/h79-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 67, "candidate_size": 1034, "choices": {"candidate": "baseline", "h79-protocol": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "41fdf33487acbbea", "retail_relocs": 64, "text_sha": "1d12751a3c8322ec"}
// {"candidate_relocs": 67, "candidate_size": 1034, "choices": {"candidate": "baseline", "h79-protocol": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "41fdf33487acbbea", "retail_relocs": 64, "text_sha": "1d12751a3c8322ec"}
// Axis: h79-protocol
// Arm: explicit
gpAdvManager->m_visibilityMapValid = false;
            giBottomViewOverride = BOTTOM_VIEW_NONE;
            gpWindowManager->FadeScreen(FADE_OUT, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
            gpAdvManager->SetInitialMapOrigin();
            gpAdvManager->RedrawAdvScreen(1, 0);
            gpWindowManager->FadeScreen(FADE_IN, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);

// Arm: helper
PRESENT_RESTARTED_CAMPAIGN_MAP();

// h80-protocol: src/SOURCE/Campaign.cpp, RVA 0x22adc
// Artifacts: build/readability/h80-protocol-{axes,manifest}.json;
//            build/readability/h80-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 196, "candidate_size": 2268, "choices": {"candidate": "baseline", "h80-protocol": "explicit"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "860701223b8c0c19", "retail_relocs": 196, "text_sha": "492dd832c89fd709"}
// {"candidate_relocs": 196, "candidate_size": 2278, "choices": {"candidate": "baseline", "h80-protocol": "statement-safe-block"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 2278, retail 2268", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "0f5968bab1fb923d", "retail_relocs": 196, "text_sha": "f90432d6aa3c1673"}
// Axis: h80-protocol
// Arm: explicit
message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_WIDGET_FIRST;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        if (!bCampaignViewOnly
            && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap])
            message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
        else
            message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
        campWin->BroadcastMessage(message);

        if (m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] == mapIndex)
            message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
        else
            message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.data.value = CAMPAIGN_WIDGET_REFRESH_FRAME;
        campWin->BroadcastMessage(message);

// Arm: statement-safe-block
do {
message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_WIDGET_FIRST;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        if (!bCampaignViewOnly
            && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap])
            message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
        else
            message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
        campWin->BroadcastMessage(message);

        if (m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] == mapIndex)
            message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
        else
            message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.data.value = CAMPAIGN_WIDGET_REFRESH_FRAME;
        campWin->BroadcastMessage(message);
} while (0);

// h81-protocol: src/SOURCE/Campaign.cpp, RVA 0x2387e
// Artifacts: build/readability/h81-protocol-{axes,manifest}.json;
//            build/readability/h81-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 85, "candidate_size": 2653, "choices": {"candidate": "baseline", "h81-protocol": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "97814edcf6edd5df", "retail_relocs": 79, "text_sha": "48b0bea1b939c5c0"}
// {"candidate_relocs": 85, "candidate_size": 2653, "choices": {"candidate": "baseline", "h81-protocol": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "97814edcf6edd5df", "retail_relocs": 79, "text_sha": "48b0bea1b939c5c0"}
// Axis: h81-protocol
// Arm: explicit
gpGame->GetHero(m_players[0].m_heroIds[0])->m_experience += choiceBest1->value;
                gpGame->GetHero(m_players[0].m_heroIds[0])->CheckLevel();

// Arm: macro
ADD_HERO_EXPERIENCE_AND_CHECK_LEVEL(*gpGame->GetHero(m_players[0].m_heroIds[0]), choiceBest1->value);

// h83-protocol: src/SOURCE/CURSOR.cpp, RVA 0x340ca
// Artifacts: build/readability/h83-protocol-{axes,manifest}.json;
//            build/readability/h83-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 159, "candidate_size": 4347, "choices": {"candidate": "baseline", "h83-protocol": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "f9aa10a16ded9ed3", "retail_relocs": 159, "text_sha": "5c5a2a2e1f1a35b4"}
// {"candidate_relocs": 153, "candidate_size": 4290, "choices": {"candidate": "baseline", "h83-protocol": "method"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 4290, retail 4347", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "59eacd7184384f9c", "retail_relocs": 159, "text_sha": "bc4a3e306f18ec71"}
// Axis: h83-protocol
// Arm: explicit
gpGame->m_players[giCurPlayer].m_resources[step_a] +=
                        mapEvent->resources[step_a];
                    if (gpGame->m_players[giCurPlayer].m_resources[step_a] < 0)
                        gpGame->m_players[giCurPlayer].m_resources[step_a] = 0;

// Arm: method
gpGame->m_players[giCurPlayer].ApplyEventResource(step_a, mapEvent->resources[step_a]);

// h86-protocol: src/SOURCE/ADVMGR.cpp, RVA 0x27cf
// Artifacts: build/readability/h86-protocol-{axes,manifest}.json;
//            build/readability/h86-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 229, "candidate_size": 3587, "choices": {"candidate": "baseline", "h86-protocol": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "839b710c4f828b8c", "retail_relocs": 229, "text_sha": "d4be3c1ef45ea45a"}
// {"candidate_relocs": 229, "candidate_size": 3593, "choices": {"candidate": "baseline", "h86-protocol": "statement-safe-block"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 3593, retail 3587", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "9506818ce3d8aa6f", "retail_relocs": 229, "text_sha": "a5fcdb8793c0fba5"}
// Axis: h86-protocol
// Arm: explicit
                                gpGame->m_cheated = 1;
                                if (gbInCampaign) {
                                    gpGame->m_campaignCheated = 1;
                                }

// Arm: statement-safe-block
                                do {
gpGame->m_cheated = 1;
                                if (gbInCampaign) {
                                    gpGame->m_campaignCheated = 1;
                                }
} while (0);

// h89-protocol: src/SOURCE/PHILAI.cpp, RVA 0x87fff
// Artifacts: build/readability/h89-protocol-{axes,manifest}.json;
//            build/readability/h89-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 27, "candidate_size": 645, "choices": {"candidate": "baseline", "h89-protocol": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "6061876a78f7fe4c", "retail_relocs": 27, "text_sha": "580216b35abd02ba"}
// {"candidate_relocs": 26, "candidate_size": 649, "choices": {"candidate": "baseline", "h89-protocol": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 649, retail 645", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "defdf537c284f179", "retail_relocs": 27, "text_sha": "234c0fbfe672bb0a"}
// Axis: h89-protocol
// Arm: explicit
for (slotIdx = 0; slotIdx < AI_PURCHASE_RESOURCE_COUNT; slotIdx++) {
        gpCurPlayer->m_resources[slotIdx] -= purchaseCount * monsterCosts[slotIdx];
    }

// Arm: helper
gpCurPlayer->SubtractCost(monsterCosts, purchaseCount);

// h90-protocol: src/SOURCE/ARMYGRP.cpp, RVA 0x21710
// Artifacts: build/readability/h90-protocol-{axes,manifest}.json;
//            build/readability/h90-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 2, "candidate_size": 49, "choices": {"candidate": "baseline", "h90-protocol": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "5d2141cb26521017", "retail_relocs": 2, "text_sha": "9b7f8924949cd839"}
// {"candidate_relocs": 2, "candidate_size": 49, "choices": {"candidate": "baseline", "h90-protocol": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "5d2141cb26521017", "retail_relocs": 2, "text_sha": "9b7f8924949cd839"}
// Axis: h90-protocol
// Arm: explicit
memset(m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, sizeof(m_creatureTypes));
    memset(m_creatureCounts, 0, sizeof(m_creatureCounts));

// Arm: macro
CLEAR_ARMY_GROUP(*this);

// h91-protocol: src/SOURCE/PHILAI.cpp, RVA 0x87687
// Artifacts: build/readability/h91-protocol-{axes,manifest}.json;
//            build/readability/h91-protocol-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 17, "candidate_size": 977, "choices": {"candidate": "baseline", "h91-protocol": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "36c22cf1874fc5ab", "retail_relocs": 17, "text_sha": "86628dc1eadd67e5"}
// {"candidate_relocs": 17, "candidate_size": 983, "choices": {"candidate": "baseline", "h91-protocol": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 983, retail 977", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "36c22cf1874fc5ab", "retail_relocs": 17, "text_sha": "5d3e3718a585e822"}
// Axis: h91-protocol
// Arm: explicit
army = 0;
            for (fromIdx = 0; fromIdx < AI_TOWN_ARMY_SLOTS; fromIdx++) {
                if (sourceArmy->m_creatureTypes[fromIdx] != CREATURE_NONE)
                    army += sourceArmy->m_quantities[fromIdx];
            }

// Arm: helper
army = sourceArmy->CountCreatures();

// Disposition: see docs/readability/implementation.md for retained subsets,
// caller boundaries and the combined original-snapshot object comparison.

// Rejected inline declarations, present during their matrices; removed afterward.
// armyGroup owner (the original out-of-line Dismiss ABI was never changed):
inline void DismissSlot(i32 slot) {
    m_creatureTypes[slot] = CREATURE_NONE;
    m_creatureCounts[slot] = 0;
}
inline i32 CountCreatures(void) const {
    i32 total = 0;
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
        if (m_creatureTypes[slot] != CREATURE_NONE)
            total += m_creatureCounts[slot];
    }
    return total;
}
// playerData owner:
inline void ApplyEventResource(i32 resource, i32 amount) {
    m_resources[resource] += amount;
    if (m_resources[resource] < 0)
        m_resources[resource] = 0;
}
inline void SubtractCost(const i32* unitCost, i32 count) {
    for (i32 resource = 0; resource < IDX(RES_COUNT); resource++)
        m_resources[resource] -= count * unitCost[resource];
}

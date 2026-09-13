// Parent 7d4ffd56; 8 complete arms in 6.338535942137241 seconds.
// Selected trial 0: {'source_shape': 'omit0', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (340, 'cde56e9c7c4c8e5b', 'e6d5fab432d44d0a').
// Same size and masked payload as baseline: ['omit0'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/s48-mage-manifest.json; build/source-variant-batch/s48-mage/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// MessageDispatchResult MageGuildHandler(tag_message& message) {
//     i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
//     i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
//     i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//     u32 quickView_i;
//     i32 spellSlot_b;
//     i32 level_d;
//     i32 slot_j;
//     SpellType spell_j;
//
//     if (message.type == MESSAGE_WIDGET) {
//         switch (message.payload.widget.command) {
//             case WIDGET_COMMAND_SELECT:
//             case WIDGET_COMMAND_ALTERNATE_SELECT:
//                 quickView_i = HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON);
//                 spellSlot_b = -1;
//                 if (message.payload.widget.id >= TOWN_MAGE_FIRST_SPELL_CONTROL
//                     && message.payload.widget.id
//                            < TOWN_MAGE_FIRST_SPELL_CONTROL
//                                  + TOWN_MAGE_GUILD_MAX_LEVEL * TOWN_MAGE_SPELLS_PER_LEVEL)
//                     spellSlot_b = message.payload.widget.id - TOWN_MAGE_FIRST_SPELL_CONTROL;
//                 if (message.payload.widget.id >= TOWN_MAGE_FIRST_ICON_CONTROL
//                     && message.payload.widget.id
//                            < TOWN_MAGE_FIRST_ICON_CONTROL
//                                  + TOWN_MAGE_GUILD_MAX_LEVEL * TOWN_MAGE_SPELLS_PER_LEVEL)
//                     spellSlot_b = message.payload.widget.id - TOWN_MAGE_FIRST_ICON_CONTROL;
//                 if (message.payload.widget.id >= TOWN_MAGE_FIRST_DESCRIPTION_CONTROL
//                     && message.payload.widget.id
//                            < TOWN_MAGE_FIRST_DESCRIPTION_CONTROL
//                                  + TOWN_MAGE_GUILD_MAX_LEVEL * TOWN_MAGE_SPELLS_PER_LEVEL)
//                     spellSlot_b = message.payload.widget.id - TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//                 if (spellSlot_b != -1) {
//                     level_d = spellSlot_b / TOWN_MAGE_SPELLS_PER_LEVEL;
//                     slot_j = spellSlot_b % TOWN_MAGE_SPELLS_PER_LEVEL;
//                     if (slot_j >= gpTownManager->m_town->m_spellCounts[level_d])
//                         return MESSAGE_DISPATCH_CONSUME;
//                     spell_j = gpTownManager->m_town->m_spells[level_d][slot_j];
//                     NormalDialog(
//                         gSpellDesc[IDX(spell_j)],
//                         quickView_i != 0 ? NORMAL_DIALOG_QUICK_VIEW : NORMAL_DIALOG_INFO,
//                         -1,
//                         -1,
//                         NORMAL_DIALOG_SPELL,
//                         IDX(spell_j),
//                         -1,
//                         0,
//                         -1,
//                         0
//                     );
//                     return MESSAGE_DISPATCH_CONSUME;
//                 }
//         }
//     }
//     return EventWindowHandler(message);
// }
//
// Option omit0:
//
//
// Option omit1:
// --- baseline
// +++ omit1
// @@ -1,5 +1,4 @@
//  MessageDispatchResult MageGuildHandler(tag_message& message) {
// -    i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
//      i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
//      i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//      u32 quickView_i;
//
// Option omit2:
// --- baseline
// +++ omit2
// @@ -1,6 +1,5 @@
//  MessageDispatchResult MageGuildHandler(tag_message& message) {
//      i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
// -    i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
//      i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//      u32 quickView_i;
//      i32 spellSlot_b;
//
// Option omit3:
// --- baseline
// +++ omit3
// @@ -1,6 +1,4 @@
//  MessageDispatchResult MageGuildHandler(tag_message& message) {
// -    i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
// -    i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
//      i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//      u32 quickView_i;
//      i32 spellSlot_b;
//
// Option omit4:
// --- baseline
// +++ omit4
// @@ -1,7 +1,6 @@
//  MessageDispatchResult MageGuildHandler(tag_message& message) {
//      i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
//      i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
// -    i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//      u32 quickView_i;
//      i32 spellSlot_b;
//      i32 level_d;
//
// Option omit5:
// --- baseline
// +++ omit5
// @@ -1,7 +1,5 @@
//  MessageDispatchResult MageGuildHandler(tag_message& message) {
// -    i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
//      i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
// -    i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//      u32 quickView_i;
//      i32 spellSlot_b;
//      i32 level_d;
//
// Option omit6:
// --- baseline
// +++ omit6
// @@ -1,7 +1,5 @@
//  MessageDispatchResult MageGuildHandler(tag_message& message) {
//      i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
// -    i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
// -    i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//      u32 quickView_i;
//      i32 spellSlot_b;
//      i32 level_d;
//
// Option omit7:
// --- baseline
// +++ omit7
// @@ -1,7 +1,4 @@
//  MessageDispatchResult MageGuildHandler(tag_message& message) {
// -    i16 H2_UNUSED(unusedFirstSpell_b) = TOWN_MAGE_FIRST_SPELL_CONTROL;
// -    i16 H2_UNUSED(unusedFirstIcon_c) = TOWN_MAGE_FIRST_ICON_CONTROL;
// -    i16 H2_UNUSED(unusedFirstDescription_h) = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
//      u32 quickView_i;
//      i32 spellSlot_b;
//      i32 level_d;
//

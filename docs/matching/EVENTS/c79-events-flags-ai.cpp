// VC6 SP5 source product c79-events-flags-ai; RVA 0x47989.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.641104s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-flags-ai/results.json and build/c79-events-flags-ai-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'flags': 'keep', 'candidate': 'baseline'} / 849 / 96a2ced660456cf4 / 7fa5b65d569be490 / 48:48 / 47,1,0,0,0 / True
// 1 / {'flags': 'recover', 'candidate': 'baseline'} / 849 / 96a2ced660456cf4 / d1c6a05c83880b47 / 48:48 / 47,1,0,0,0 / True

// AXIS flags
// BEFORE (complete exact span):
// |             if (!(eventHero->m_eventFlags & HERO_EVENT_ARENA)) {
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_ARENA)
// |                 );
// |                 switch (eventHero->m_cursorType) {
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,5 +1,3 @@
// |              if (!(eventHero->m_eventFlags & HERO_EVENT_ARENA)) {
// | -                eventHero->m_eventFlags = HeroEventFlag(
// | -                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_ARENA)
// | -                );
// | +                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_ARENA;
// |                  switch (eventHero->m_cursorType) {
// EXTRA BEFORE:
// |             if (!(eventHero->m_eventFlags & HERO_EVENT_MERMAID)) {
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_MERMAID)
// |                 );
// |                 eventHero->m_luck++;
// EXTRA REPLACEMENT:
// |             if (!(eventHero->m_eventFlags & HERO_EVENT_MERMAID)) {
// |                 eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MERMAID;
// |                 eventHero->m_luck++;
// EXTRA BEFORE:
// |                     GiveExperience(eventHero, armyValue7, 1);
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_SIRENS)
// |                 );
// |             }
// EXTRA REPLACEMENT:
// |                     GiveExperience(eventHero, armyValue7, 1);
// |                 eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_SIRENS;
// |             }
// EXTRA BEFORE:
// |             if (!(eventHero->m_eventFlags & HERO_EVENT_STABLES)) {
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_STABLES)
// |                 );
// |                 eventHero->m_mobility += EVENT_CREATURE_UPGRADE_MOBILITY;
// EXTRA REPLACEMENT:
// |             if (!(eventHero->m_eventFlags & HERO_EVENT_STABLES)) {
// |                 eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_STABLES;
// |                 eventHero->m_mobility += EVENT_CREATURE_UPGRADE_MOBILITY;

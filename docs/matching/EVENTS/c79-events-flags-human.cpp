// VC6 SP5 source product c79-events-flags-human; RVA 0x42256.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.592919s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-flags-human/results.json and build/c79-events-flags-human-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'flags': 'keep', 'candidate': 'baseline'} / 1788 / 41c0c86968f91750 / 23c61b993bdc9c20 / 71:72 / 70,0,0,1,1 / False
// 1 / {'flags': 'recover', 'candidate': 'baseline'} / 1788 / 41c0c86968f91750 / a1ea2440ba03a1f0 / 71:72 / 70,0,0,1,1 / False

// AXIS flags
// BEFORE (complete exact span):
// |                 );
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_ARENA)
// |                 );
// |                 primaryStat15 = DoArenaDialog();
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,5 +1,3 @@
// |                  );
// | -                eventHero->m_eventFlags = HeroEventFlag(
// | -                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_ARENA)
// | -                );
// | +                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_ARENA;
// |                  primaryStat15 = DoArenaDialog();
// EXTRA BEFORE:
// |                 );
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_MERMAID)
// |                 );
// |                 eventHero->m_luck++;
// EXTRA REPLACEMENT:
// |                 );
// |                 eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MERMAID;
// |                 eventHero->m_luck++;
// EXTRA BEFORE:
// |                 }
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_SIRENS)
// |                 );
// |             }
// EXTRA REPLACEMENT:
// |                 }
// |                 eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_SIRENS;
// |             }
// EXTRA BEFORE:
// |             if (HAS(eventHero->m_eventFlags, HERO_EVENT_STABLES) == 0) {
// |                 eventHero->m_eventFlags = HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_STABLES)
// |                 );
// |                 eventHero->m_mobility += SITE_STABLE_MOBILITY;
// EXTRA REPLACEMENT:
// |             if (HAS(eventHero->m_eventFlags, HERO_EVENT_STABLES) == 0) {
// |                 eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_STABLES;
// |                 eventHero->m_mobility += SITE_STABLE_MOBILITY;

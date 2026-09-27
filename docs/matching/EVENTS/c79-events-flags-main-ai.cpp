// VC6 SP5 source product c79-events-flags-main-ai; RVA 0x44d73.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 2.684702s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-flags-main-ai/results.json and build/c79-events-flags-main-ai-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'flags': 'keep', 'candidate': 'baseline'} / 11116 / fe51168989c69b33 / c73a53b52e3d49b1 / 367:374 / 366,0,0,1,7 / False
// 1 / {'flags': 'recover', 'candidate': 'baseline'} / 11116 / fe51168989c69b33 / 3dcb072cd2cd8e09 / 367:374 / 366,0,0,1,7 / False

// AXIS flags
// BEFORE (complete exact span):
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) & ~IDX(HERO_EVENT_EMBARKED)
// |                 )
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,3 +1 @@
// | -HeroEventFlag(
// | -                    static_cast<i32>(eventHero->m_eventFlags) & ~IDX(HERO_EVENT_EMBARKED)
// | -                )
// | +eventHero->m_eventFlags & ~HERO_EVENT_EMBARKED
// EXTRA BEFORE:
// | HeroEventFlag(
// |                 static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_EMBARKED)
// |             )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_EMBARKED
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_WATERING_HOLE)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_WATERING_HOLE
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_BUOY)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_BUOY
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_FAERIE_RING)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_FAERIE_RING
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_IDOL)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_IDOL
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_FOUNTAIN)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_FOUNTAIN
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_OASIS)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_OASIS
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_TEMPLE)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_TEMPLE
// EXTRA BEFORE:
// | HeroEventFlag(
// |                         static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_MAGIC_WELL)
// |                     )
// EXTRA REPLACEMENT:
// | eventHero->m_eventFlags | HERO_EVENT_MAGIC_WELL

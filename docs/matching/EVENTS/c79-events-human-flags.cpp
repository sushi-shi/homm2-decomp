// VC6 SP5 source product c79-events-human-flags; RVA 0x3b640.
// Parent: 3a80a103 (unmodified).
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.469958s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-human-flags/results.json and build/c79-events-human-flags-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / f00329f724dc8285 / 557:561 / 556,0,0,1,4 / False
// 1 / {'family': 'recovered', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False

// AXIS family
// BEFORE (complete exact span):
// | HeroEventFlag(
// |                         static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_MAGIC_WELL)
// |                     )
// OPTION keep
// OPTION recovered
// | --- before
// | +++ recovered
// | @@ -1,3 +1 @@
// | -HeroEventFlag(
// | -                        static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_MAGIC_WELL)
// | -                    )
// | +eventHero2->m_eventFlags | HERO_EVENT_MAGIC_WELL
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) & ~IDX(HERO_EVENT_EMBARKED)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags & ~HERO_EVENT_EMBARKED
// EXTRA BEFORE:
// | HeroEventFlag(
// |                 static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_EMBARKED)
// |             )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_EMBARKED
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_BUOY)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_BUOY
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_FAERIE_RING)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_FAERIE_RING
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_IDOL)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_IDOL
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_FOUNTAIN)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_FOUNTAIN
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_WATERING_HOLE)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_WATERING_HOLE
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_OASIS)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_OASIS
// EXTRA BEFORE:
// | HeroEventFlag(
// |                     static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_TEMPLE)
// |                 )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_TEMPLE
// EXTRA BEFORE:
// | HeroEventFlag(
// |                             static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_PYRAMID)
// |                         )
// EXTRA REPLACEMENT:
// | eventHero2->m_eventFlags | HERO_EVENT_PYRAMID

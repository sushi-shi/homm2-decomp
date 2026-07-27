/*
DoAIEvent, RVA 0x000b1e43: event-flag materialization experiments
(2026-07-27).

The clean candidate is 97.163475% fuzzy with 10623 candidate bytes versus
10816 retail bytes.  The retained current-hash MAX before this experiment was
97.2090%.  Both objects use an 0x150-byte frame, save ebx/esi/edi, and contain
356 relocations.

`disasm --blocks --diff --lite` currently raises IndexError on an empty block
in this function.  The separate block skeletons remain usable: the candidate
has 413 blocks and retail has 431.  Their first body-size divergence is B5,
the coast event's flag clear (26 candidate instructions versus 29 retail),
followed by the boat event's flag set at B7 (51 versus 54).  Raw diff shows
the repeated distinction:

  candidate: and/or dword ptr [eventHero + m_eventFlags], immediate
  retail:    load m_eventFlags, and/or register, reload eventHero, store

The source axes below cover all nine flag assignments in DoAIEvent as a
single authored family.  They preserve DATA_COMPGEN order and semantics.
*/

#if 0
// Axis 1: make the flag operand own the expression.
eventHero->m_eventFlags = ~HERO_EVENT_EMBARKED & eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_EMBARKED | eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_WATERING_HOLE | eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_BUOY | eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_FAERIE_RING | eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_IDOL | eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_FOUNTAIN | eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_OASIS | eventHero->m_eventFlags;
eventHero->m_eventFlags = HERO_EVENT_TEMPLE | eventHero->m_eventFlags;
#endif

/*
The complete current/flag-first Cartesian matrix is recorded at:

  build/doaievent-event-flags-manifest-20260727.json
  build/match-variants/doaievent-event-flags-20260727/

It compiled both source shapes against clean plus 50 declaration-forest
states (102 candidates).  Every flag-first result was byte-identical to its
member-first counterpart, so operand order is rejected as the explanation.
*/

#if 0
// Axis 2: explicitly materialize the enum-domain result before assignment.
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags & ~HERO_EVENT_EMBARKED
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_EMBARKED
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_WATERING_HOLE
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_BUOY
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_FAERIE_RING
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_IDOL
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_FOUNTAIN
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_OASIS
);
eventHero->m_eventFlags = static_cast<HeroEventFlag>(
    eventHero->m_eventFlags | HERO_EVENT_TEMPLE
);
#endif

/*
The explicit-result clean plus 50 matrix is recorded at:

  build/doaievent-event-flags-cast-manifest-20260727.json
  build/match-variants/doaievent-event-flags-cast-20260727/

It too was byte-identical to unchanged source for every paired state.  The
source was therefore restored after both attempts.

The matrices independently found unchanged-source forest trial 14 (also
trials 20, 30, and 41) at 97.229550%, size 10626, with 356/356 relocations.
Trial 14 was replayed through the official unchanged-source recorder:

  build/tu-state-noise/doaievent-trial14-record-max-20260727/
  build/doaievent-trial14-record-max-states-20260727.json

That replay raised current hash 7c815620edf8 from 97.2090% to 97.2296%.
No generated probe or source-shape change was retained.
*/

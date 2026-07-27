/*
 * ProcessOnMapHeroes structural and slot recovery, 2026-07-27.
 *
 * Initial evidence:
 *   live/MAX 98.005610/98.6748, 92 base blocks versus 91 retail blocks,
 *   frame 128 versus 124, and 22/23 relocations.
 *
 * Structural version retained in src/SOURCE/GAME.cpp:
 *
 *   heroClass6 = static_cast<FactionType>(
 *       cell5->m_objectIndex % MAP_HERO_FRAME_STRIDE
 *   );
 *   if (heroClass6 == MAP_HERO_RANDOM_FACTION_FRAME)
 *       heroClass6 = m_setupPlayerRace[...];
 *
 * Reusing heroClass6 for the frame remainder removed a reconstructed temporary.
 * This recovered the retail 124-byte frame and 91/91 block topology.  Replacing
 * the folded free-site literal 6604 with
 * processOnMapHeroesSourceLineBase + 0xdd recovered relocation 23/23.
 *
 * Loop-bound matrix:
 *   - mapY15 < MAP_HEIGHT / mapX0 < MAP_WIDTH
 *   - MAP_HEIGHT > mapY15 / MAP_WIDTH > mapX0
 *
 * Both reviewed orientations were crossed with clean plus 50 forest states
 * (102/102 cells).  Four normalized islands appeared.  The best unmodified
 * local layout was 99.842990% at retail size, proving the remaining residual
 * was compiler-state/slot interaction rather than CFG.
 *
 * A predicted four-identifier layout experiment renamed mapX0, townCell1,
 * mapY15, and recordSlot10 together and crossed its one complete source shape
 * with clean plus 50 states.  It reached 99.949530% at retail size.  Its only
 * differences were the three loop-index slots:
 *
 *   candidate: army -0x58, record -0x68, townCell -0x60
 *   retail:    army -0x68, record -0x60, townCell -0x58
 *
 * That negative result exposed the exact declaration/hash order.  The final
 * complete one-axis matrix renamed only the multi-purpose artifact/skill loop
 * index from recordSlot10 to the semantic recordPosition.  Its bucket places
 * the declarations in retail order:
 *
 *   townCell1, mapX0, recordPosition, mapY15, armySlot0
 *
 * Clean plus 50 forest states completed.  Trials 9 and 19 reached audited
 * 100.000000%, retail size 1908, 91/91 blocks, and 23/23 ordered relocations.
 * Official unchanged-source trial-9 replay advanced hash ca6a15e19c28 MAX from
 * 99.3271% to 100.0000%; no generated declarations are retained.
 *
 * Artifacts:
 *   build/process-on-map-heroes-loop-bounds-manifest-20260727.json
 *   build/match-variants/process-on-map-heroes-loop-bounds-20260727
 *   build/process-on-map-heroes-predicted-layout-manifest-20260727.json
 *   build/match-variants/process-on-map-heroes-predicted-layout-20260727
 *   build/process-on-map-heroes-record-position-manifest-20260727.json
 *   build/match-variants/process-on-map-heroes-record-position-20260727
 *   build/tu-state-noise/process-on-map-heroes-trial9-record-20260727
 *   build/process-on-map-heroes-trial9-record-states-20260727.json
 *
 * Disposition: retain the shared heroClass6 structure, source-line owner, and
 * recordPosition spelling.  Retain only the generated MAX record for the
 * disposable exact state.
 */

void ProcessOnMapHeroes_structural_and_slot_recovery_record() {
}

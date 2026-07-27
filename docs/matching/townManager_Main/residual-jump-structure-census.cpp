/*
 * townManager::Main residual jump-placement structural census.
 *
 * The retained source is 6192 bytes with a 592-byte frame and 215/215
 * relocation count.  Its normalized instruction diff has only four
 * unconditional-jump placements plus the resulting owner-relative
 * jump-table addends:
 *
 *   - one surplus jump at the marketplace loop head;
 *   - one surplus jump after the town-navigation remainder calculation;
 *   - one missing jump in the MESSAGE_KEY_UP switch tail;
 *   - one missing jump in the MESSAGE_KEY_DOWN switch tail.
 *
 * Four developer-plausible high-level alternatives were each compiled
 * against the clean state and all 50 forest TU states.  Every matrix
 * completed 51/51 without wall-time truncation and restored the source.
 *
 * 1. Marketplace `while` loop with a tail increment:
 *
 *      index_i = 0;
 *      while (index_i < gpCurPlayer->m_townCount) {
 *          if (gpGame->GetTown(gpCurPlayer->TownId(index_i))->m_buildings
 *              & IDX(TOWN_BUILDING_MARKETPLACE))
 *              ++marketplaceCount_m;
 *          ++index_i;
 *      }
 *
 *    Best: 99.254230%, 6187 bytes, 215/215 relocations, clean state.
 *    This remains a coherent lower loop island, but its shorter loop head
 *    contradicts the exact retail size and no TU state improved it.
 *
 * 2. Function-owned `townPosition`, with the navigation case unscoped:
 *
 *      townPosition = gpGame->TownIDToTownPos(gpCurPlayer, m_town->m_id);
 *      townPosition =
 *          (townPosition
 *           + (message.payload.widget.id == CONTROL_PREVIOUS_TOWN ? -1 : 1)
 *           + gpCurPlayer->m_townCount)
 *          % gpCurPlayer->m_townCount;
 *
 *    Best: 99.421890%, 6192 bytes, 215/215 relocations, clean state.
 *    The frame and size remain credible, so this is preserved as a lower
 *    scope island, but no TU state repaired the navigation continuation.
 *
 * 3. MESSAGE_KEY_UP expressed as an OR predicate rather than an inner switch:
 *
 *      if (message.payload.keyboard.keyCode == INPUT_SCAN_LEFT_SHIFT
 *          || message.payload.keyboard.keyCode == INPUT_SCAN_RIGHT_SHIFT)
 *          ShiftQualChange();
 *
 *    Best: 97.779106%, 6160 bytes, 215/215 relocations, clean state.
 *    The predicate removes 32 retail bytes and changes the dispatch family.
 *
 * 4. MESSAGE_KEY_DOWN expressed as an OR predicate plus escape `else if`:
 *
 *      if (message.payload.keyboard.keyCode == INPUT_SCAN_LEFT_SHIFT
 *          || message.payload.keyboard.keyCode == INPUT_SCAN_RIGHT_SHIFT) {
 *          ShiftQualChange();
 *      } else if (message.payload.keyboard.keyCode == INPUT_SCAN_ESCAPE) {
 *          ++exitTown_i;
 *      }
 *
 *    Best: 97.801490%, 6160 bytes, 215/215 relocations, clean state.
 *    It has the same 32-byte size contradiction and a different dispatch CFG.
 *
 * A fifth complete 51-state matrix replaced both `TownId(index)` calls with
 * direct `m_townIds[index]` reads while retaining `GetTown`.  It reached
 * 99.144775%, 6182 bytes, and 215/215 relocation count.  Direct-field,
 * direct-town-record, and fully direct clean diagnostics showed that the
 * apparent `jmp $+0` sites are not independently controlled by either nested
 * accessor.  Adding a keyboard-payload inline getter and adding lexical case
 * scopes were also byte-shape neutral at the four residual sites.  These
 * diagnostics reject accessor invention as a source-retained fix.
 *
 * Artifacts:
 *   build/townmain-marketplace-while-states-manifest-20260727.json
 *   build/match-variants/townmain-marketplace-while-states-20260727
 *   build/townmain-town-position-hoist-states-manifest-20260727.json
 *   build/match-variants/townmain-town-position-hoist-states-20260727
 *   build/townmain-key-up-predicate-states-manifest-20260727.json
 *   build/match-variants/townmain-key-up-predicate-states-20260727
 *   build/townmain-key-down-predicate-states-manifest-20260727.json
 *   build/match-variants/townmain-key-down-predicate-states-20260727
 *   build/townmain-direct-town-ids-states-manifest-20260727.json
 *   build/match-variants/townmain-direct-town-ids-states-20260727
 *
 * Disposition: retain the current nested switch, for-loop, case-local
 * navigation scope, and accessor source.  None of these structural families
 * closes the four-jump residual, so the durable 99.4468% MAX remains intact.
 */

void townManager_Main_residual_jump_structure_census_record() {
}

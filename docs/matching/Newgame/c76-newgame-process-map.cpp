// VC6 SP5 source family c76-newgame-process-map; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-process-map/results.json; RVA 0x75682
// Complete 2 arms; elapsed 1.792812s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 102 / 5e2b82df5719ee43 / 1377d0c4d4ac0b77 / 4:4 / 4,0,0,0 / True
// 1 / {'family': 'remove_1', 'candidate': 'baseline'} / 102 / 5e2b82df5719ee43 / 1377d0c4d4ac0b77 / 4:4 / 4,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x66)
// | void game::ProcessNewMap(struct SMapHeader* header) {
// |     m_newGameInitialized = false;
// |     m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// |     if (m_newGameWindow == NULL)
// |         return;
// |     CleanUpNewGameWindow();
// |     InitNewGame(header);
// |     InitNewGameWindow();
// |     UpdateNewGameWindow();
// |     m_newGameWindow->DrawWindow();
// | }
// |
// ALTERNATIVE remove_1 (unified delta from BEFORE):
// | --- before
// | +++ remove_1
// | @@ -1,7 +1,7 @@
// |   0x66)
// |  void game::ProcessNewMap(struct SMapHeader* header) {
// |      m_newGameInitialized = false;
// | -    m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// | +    m_newGameHumanCount = giNumHumanPlayers;
// |      if (m_newGameWindow == NULL)
// |          return;
// |      CleanUpNewGameWindow();

// VC6 SP5 source family c76-newgame-display; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-display/results.json; RVA 0x7850e
// Complete 2 arms; elapsed 1.855017s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 260 / b030ec80441c38dd / 0b12ada5fdc0af2c / 14:14 / 14,0,0,0 / True
// 1 / {'family': 'remove_1', 'candidate': 'baseline'} / 260 / b030ec80441c38dd / 0b12ada5fdc0af2c / 14:14 / 14,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x104)
// | void game::NGKPSetupDisplayString(char* text, u16 cursor) {
// |     if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
// |         return;
// |
// |     if (static_cast<i32>(KBTickCount()) > glTimers[0]) {
// |         NGKPcursorFlashOn = 1 - NGKPcursorFlashOn;
// |         glTimers[0] = KBTickCount() + GAME_CURSOR_FLASH_TICKS;
// |     }
// |
// |     if (cursor > 0)
// |         strncpy(cNGKPDisplay, text, cursor);
// |
// |     if (NGKPcursorFlashOn)
// |         cNGKPDisplay[cursor] = FONT_SPACER_CHAR;
// |     else
// |         cNGKPDisplay[cursor] = '_';
// |
// |     if (strlen(text) > cursor)
// |         strcpy(cNGKPDisplay + (cursor + 1), text + cursor);
// |     else
// |         cNGKPDisplay[cursor + 1] = 0;
// | }
// |
// ALTERNATIVE remove_1 (unified delta from BEFORE):
// | --- before
// | +++ remove_1
// | @@ -3,7 +3,7 @@
// |      if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
// |          return;
// |
// | -    if (static_cast<i32>(KBTickCount()) > glTimers[0]) {
// | +    if (KBTickCount() > glTimers[0]) {
// |          NGKPcursorFlashOn = 1 - NGKPcursorFlashOn;
// |          glTimers[0] = KBTickCount() + GAME_CURSOR_FLASH_TICKS;
// |      }

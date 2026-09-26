// VC6 SP5 source family c76-newgame-key-comparison; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-key-comparison/results.json; RVA 0x7811b
// Complete 2 arms; elapsed 1.858227s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1011 / ceddb9ebc33da089 / 211f5f1568700df6 / 48:48 / 47,1,0,0 / True
// 1 / {'family': 'usual_unsigned_conversion', 'candidate': 'baseline'} / 1011 / ceddb9ebc33da089 / 07cc215c5f50baf5 / 48:48 / 47,1,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x3f3)
// | i32 game::ProcessNGKeyPress(struct tag_message& message) {
// |     char buf[GAME_KEY_BUFFER_SIZE];
// |     char keyChar;
// |     i32 scanCode;
// |     i32 widthResult;
// |
// |     if (giNumHumanPlayers == 1 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
// |         return 0;
// |
// |     switch (message.payload.keyboard.keyCode) {
// |         case INPUT_SCAN_ESCAPE:
// |             if (!gbAllowTextEntryEscape)
// |                 break;
// |             strcpy(
// |                 cNGKPCore,
// |                 ""
// |             );
// |             break;
// |
// |         case INPUT_SCAN_NUMPAD_DELETE:
// |             if (static_cast<size_t>(NGKPcursorIndex) < strlen(cNGKPCore)) {
// |                 strcpy(gText, cNGKPCore + (NGKPcursorIndex + 1));
// |                 strcpy(cNGKPCore + NGKPcursorIndex, gText);
// |             }
// |             break;
// |
// |         case INPUT_SCAN_NUMPAD_4:
// |             if (NGKPcursorIndex > 0)
// |                 --NGKPcursorIndex;
// |             break;
// |
// |         case INPUT_SCAN_NUMPAD_6:
// |             if (static_cast<size_t>(NGKPcursorIndex) < strlen(cNGKPCore))
// |                 ++NGKPcursorIndex;
// |             break;
// |
// |         default:
// |             gpInputManager->AsciiConvert(message);
// |             if (message.payload.keyboard.keyCode == IDX(GAME_KEY_ENTER))
// |                 return 1;
// |
// |             if (message.payload.keyboard.keyCode == IDX(GAME_KEY_BACKSPACE)) {
// |                 if (NGKPcursorIndex > 0) {
// |                     strcpy(gText, cNGKPCore + NGKPcursorIndex);
// |                     strcpy(cNGKPCore + (NGKPcursorIndex - 1), gText);
// |                     --NGKPcursorIndex;
// |                 }
// |                 break;
// |             }
// |
// |             if (strlen(cNGKPCore) + 1 < GAME_CHAT_TEXT_LIMIT
// |                 && message.payload.keyboard.keyCode != 0) {
// |                 strcpy(buf, cNGKPCore);
// |                 keyChar = 0;
// |                 if (message.payload.keyboard.keyCode >= IDX(GAME_KEY_FIRST_EXTENDED)) {
// |                     scanCode = (message.payload.keyboard.keyCode & KEY_SCAN_CODE_MASK)
// |                         >> KEY_SCAN_CODE_SHIFT;
// |                     switch (static_cast<NewGameKeyCode>(scanCode)) {
// |                         case GAME_KEYPAD_INSERT:
// |                             keyChar = '0';
// |                             break;
// |                         case GAME_KEYPAD_END:
// |                             keyChar = '1';
// |                             break;
// |                         case GAME_KEYPAD_DOWN:
// |                             keyChar = '2';
// |                             break;
// |                         case GAME_KEYPAD_PAGE_DOWN:
// |                             keyChar = '3';
// |                             break;
// |                         case GAME_KEYPAD_LEFT:
// |                             keyChar = '4';
// |                             break;
// |                         case GAME_KEYPAD_CENTER:
// |                             keyChar = '5';
// |                             break;
// |                         case GAME_KEYPAD_RIGHT:
// |                             keyChar = '6';
// |                             break;
// |                         case GAME_KEYPAD_HOME:
// |                             keyChar = '7';
// |                             break;
// |                         case GAME_KEYPAD_UP:
// |                             keyChar = '8';
// |                             break;
// |                         case GAME_KEYPAD_PAGE_UP:
// |                             keyChar = '9';
// |                             break;
// |                     }
// |                 } else {
// |                     keyChar =
// |                         static_cast<char>(message.payload.keyboard.keyCode & KEY_ASCII_MASK);
// |
// |                     if (keyChar == '{' || keyChar == '}')
// |                         keyChar = 0;
// |                 }
// |
// |                 if (keyChar != 0) {
// |                     strcpy(gText, cNGKPCore);
// |                     gText[NGKPcursorIndex] = keyChar;
// |                     gText[NGKPcursorIndex + 1] = 0;
// |                     strcat(gText, cNGKPCore + NGKPcursorIndex);
// |                     strcpy(cNGKPCore, gText);
// |                     ++NGKPcursorIndex;
// |                     NGKPSetupDisplayString(cNGKPCore, static_cast<u16>(NGKPcursorIndex));
// |                     widthResult = smallFont->LineLength(cNGKPDisplay, GAME_CHAT_DRAW_WIDTH);
// |                     if (widthResult > GAME_CHAT_MAX_LINES) {
// |                         strcpy(cNGKPCore, buf);
// |                         --NGKPcursorIndex;
// |                     }
// |                 }
// |             }
// |             break;
// |     }
// |
// |     DrawNGKPDisplayString(1);
// |     return 0;
// | }
// |
// ALTERNATIVE usual_unsigned_conversion (unified delta from BEFORE):
// | --- before
// | +++ usual_unsigned_conversion
// | @@ -19,7 +19,7 @@
// |              break;
// |
// |          case INPUT_SCAN_NUMPAD_DELETE:
// | -            if (static_cast<size_t>(NGKPcursorIndex) < strlen(cNGKPCore)) {
// | +            if (NGKPcursorIndex < strlen(cNGKPCore)) {
// |                  strcpy(gText, cNGKPCore + (NGKPcursorIndex + 1));
// |                  strcpy(cNGKPCore + NGKPcursorIndex, gText);
// |              }
// | @@ -31,7 +31,7 @@
// |              break;
// |
// |          case INPUT_SCAN_NUMPAD_6:
// | -            if (static_cast<size_t>(NGKPcursorIndex) < strlen(cNGKPCore))
// | +            if (NGKPcursorIndex < strlen(cNGKPCore))
// |                  ++NGKPcursorIndex;
// |              break;
// |

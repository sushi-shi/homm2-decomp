// VC6 SP5 source family c76-newgame-init-widget5; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-init-widget5/results.json; RVA 0x766a7
// Complete 8 arms; elapsed 5.634585s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 5bcb53de2af7ca8e / 62:62 / 62,0,0,0 / True
// 1 / {'family': 'remove_001', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 2 / {'family': 'remove_010', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 3 / {'family': 'remove_011', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 4 / {'family': 'remove_100', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 5 / {'family': 'remove_101', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 6 / {'family': 'remove_110', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 7 / {'family': 'remove_111', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / e8e72c4a8045037d / 62:62 / 62,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// | new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
// |                 ),
// |                 PLAYER_COLOR_Y,
// |                 PLAYER_COLOR_WIDTH,
// |                 PLAYER_COLOR_HEIGHT,
// |                 "ngextra.icn",
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// |                                           : GAME_COLOR_WIDGET_SINGLE_FRAME
// |                 ),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// ALTERNATIVE remove_001 (unified delta from BEFORE):
// | --- before
// | +++ remove_001
// | @@ -11,7 +11,7 @@
// |                                            : GAME_COLOR_WIDGET_SINGLE_FRAME
// |                  ),
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// | +                playerCounter + NEW_GAME_COLOR_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_010 (unified delta from BEFORE):
// | --- before
// | +++ remove_010
// | @@ -6,10 +6,8 @@
// |                  PLAYER_COLOR_WIDTH,
// |                  PLAYER_COLOR_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_COLOR_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_COLOR_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// |                  WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE remove_011 (unified delta from BEFORE):
// | --- before
// | +++ remove_011
// | @@ -6,12 +6,10 @@
// |                  PLAYER_COLOR_WIDTH,
// |                  PLAYER_COLOR_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_COLOR_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_COLOR_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// | +                playerCounter + NEW_GAME_COLOR_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_100 (unified delta from BEFORE):
// | --- before
// | +++ remove_100
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET,
// |                  PLAYER_COLOR_Y,
// |                  PLAYER_COLOR_WIDTH,
// |                  PLAYER_COLOR_HEIGHT,
// ALTERNATIVE remove_101 (unified delta from BEFORE):
// | --- before
// | +++ remove_101
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET,
// |                  PLAYER_COLOR_Y,
// |                  PLAYER_COLOR_WIDTH,
// |                  PLAYER_COLOR_HEIGHT,
// | @@ -11,7 +9,7 @@
// |                                            : GAME_COLOR_WIDGET_SINGLE_FRAME
// |                  ),
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// | +                playerCounter + NEW_GAME_COLOR_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_110 (unified delta from BEFORE):
// | --- before
// | +++ remove_110
// | @@ -1,15 +1,11 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET,
// |                  PLAYER_COLOR_Y,
// |                  PLAYER_COLOR_WIDTH,
// |                  PLAYER_COLOR_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_COLOR_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_COLOR_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// |                  WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE remove_111 (unified delta from BEFORE):
// | --- before
// | +++ remove_111
// | @@ -1,17 +1,13 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET,
// |                  PLAYER_COLOR_Y,
// |                  PLAYER_COLOR_WIDTH,
// |                  PLAYER_COLOR_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_COLOR_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_COLOR_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// | +                playerCounter + NEW_GAME_COLOR_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );

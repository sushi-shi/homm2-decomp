// VC6 SP5 source family c76-newgame-init-widget3; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-init-widget3/results.json; RVA 0x766a7
// Complete 16 arms; elapsed 10.615653s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 5bcb53de2af7ca8e / 62:62 / 62,0,0,0 / True
// 1 / {'family': 'remove_0001', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 2 / {'family': 'remove_0010', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 3 / {'family': 'remove_0011', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 4 / {'family': 'remove_0100', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 5 / {'family': 'remove_0101', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 6 / {'family': 'remove_0110', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 7 / {'family': 'remove_0111', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / e8e72c4a8045037d / 62:62 / 62,0,0,0 / True
// 8 / {'family': 'remove_1000', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / b2891d20997c2734 / 62:62 / 62,0,0,0 / True
// 9 / {'family': 'remove_1001', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 10 / {'family': 'remove_1010', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 11 / {'family': 'remove_1011', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / e8e72c4a8045037d / 62:62 / 62,0,0,0 / True
// 12 / {'family': 'remove_1100', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 1cde1fcbd2bf0603 / 62:62 / 62,0,0,0 / True
// 13 / {'family': 'remove_1101', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / e8e72c4a8045037d / 62:62 / 62,0,0,0 / True
// 14 / {'family': 'remove_1110', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / e8e72c4a8045037d / 62:62 / 62,0,0,0 / True
// 15 / {'family': 'remove_1111', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / d5cab24d492f980f / 62:62 / 62,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// | new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// |                 ),
// |                 PLAYER_RACE_Y,
// |                 PLAYER_RACE_WIDTH,
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// |                                           : GAME_RACE_WIDGET_SINGLE_HEIGHT
// |                 ),
// |                 "ngextra.icn",
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// |                                           : GAME_RACE_WIDGET_SINGLE_FRAME
// |                 ),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// ALTERNATIVE remove_0001 (unified delta from BEFORE):
// | --- before
// | +++ remove_0001
// | @@ -14,7 +14,7 @@
// |                                            : GAME_RACE_WIDGET_SINGLE_FRAME
// |                  ),
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_0010 (unified delta from BEFORE):
// | --- before
// | +++ remove_0010
// | @@ -9,10 +9,8 @@
// |                                            : GAME_RACE_WIDGET_SINGLE_HEIGHT
// |                  ),
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// |                  WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE remove_0011 (unified delta from BEFORE):
// | --- before
// | +++ remove_0011
// | @@ -9,12 +9,10 @@
// |                                            : GAME_RACE_WIDGET_SINGLE_HEIGHT
// |                  ),
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_0100 (unified delta from BEFORE):
// | --- before
// | +++ remove_0100
// | @@ -4,10 +4,8 @@
// |                  ),
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// |                  static_cast<i16>(
// |                      giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// ALTERNATIVE remove_0101 (unified delta from BEFORE):
// | --- before
// | +++ remove_0101
// | @@ -4,17 +4,15 @@
// |                  ),
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// |                  static_cast<i16>(
// |                      giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// |                                            : GAME_RACE_WIDGET_SINGLE_FRAME
// |                  ),
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_0110 (unified delta from BEFORE):
// | --- before
// | +++ remove_0110
// | @@ -4,15 +4,11 @@
// |                  ),
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// |                  WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE remove_0111 (unified delta from BEFORE):
// | --- before
// | +++ remove_0111
// | @@ -4,17 +4,13 @@
// |                  ),
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_1000 (unified delta from BEFORE):
// | --- before
// | +++ remove_1000
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// |                  static_cast<i16>(
// ALTERNATIVE remove_1001 (unified delta from BEFORE):
// | --- before
// | +++ remove_1001
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// |                  static_cast<i16>(
// | @@ -14,7 +12,7 @@
// |                                            : GAME_RACE_WIDGET_SINGLE_FRAME
// |                  ),
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_1010 (unified delta from BEFORE):
// | --- before
// | +++ remove_1010
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// |                  static_cast<i16>(
// | @@ -9,10 +7,8 @@
// |                                            : GAME_RACE_WIDGET_SINGLE_HEIGHT
// |                  ),
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// |                  WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE remove_1011 (unified delta from BEFORE):
// | --- before
// | +++ remove_1011
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// |                  static_cast<i16>(
// | @@ -9,12 +7,10 @@
// |                                            : GAME_RACE_WIDGET_SINGLE_HEIGHT
// |                  ),
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_1100 (unified delta from BEFORE):
// | --- before
// | +++ remove_1100
// | @@ -1,13 +1,9 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// |                  static_cast<i16>(
// |                      giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// ALTERNATIVE remove_1101 (unified delta from BEFORE):
// | --- before
// | +++ remove_1101
// | @@ -1,20 +1,16 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// |                  static_cast<i16>(
// |                      giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// |                                            : GAME_RACE_WIDGET_SINGLE_FRAME
// |                  ),
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_1110 (unified delta from BEFORE):
// | --- before
// | +++ remove_1110
// | @@ -1,18 +1,12 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// |                  WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE remove_1111 (unified delta from BEFORE):
// | --- before
// | +++ remove_1111
// | @@ -1,20 +1,14 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET,
// |                  PLAYER_RACE_Y,
// |                  PLAYER_RACE_WIDTH,
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | -                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// | +                                          : GAME_RACE_WIDGET_SINGLE_HEIGHT,
// |                  "ngextra.icn",
// | -                static_cast<i16>(
// | -                    giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | -                                          : GAME_RACE_WIDGET_SINGLE_FRAME
// | -                ),
// | +                giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// | +                                          : GAME_RACE_WIDGET_SINGLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );

// VC6 SP5 source family c76-newgame-init-widget9; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-init-widget9/results.json; RVA 0x766a7
// Complete 8 arms; elapsed 5.505653s; source restored.
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
// |                     firstColumnX + playerStep * playerCounter
// |                     + PLAYER_RACE_CYCLE_X_OFFSET
// |                 ),
// |                 static_cast<i16>(
// |                     yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// |                 ),
// |                 PLAYER_RACE_CYCLE_WIDTH,
// |                 PLAYER_RACE_CYCLE_HEIGHT,
// |                 "ngextra.icn",
// |                 PLAYER_RACE_CYCLE_FRAME,
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// ALTERNATIVE remove_001 (unified delta from BEFORE):
// | --- before
// | +++ remove_001
// | @@ -11,7 +11,7 @@
// |                  "ngextra.icn",
// |                  PLAYER_RACE_CYCLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_CYCLE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_010 (unified delta from BEFORE):
// | --- before
// | +++ remove_010
// | @@ -3,9 +3,7 @@
// |                      firstColumnX + playerStep * playerCounter
// |                      + PLAYER_RACE_CYCLE_X_OFFSET
// |                  ),
// | -                static_cast<i16>(
// | -                    yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// | -                ),
// | +                yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y,
// |                  PLAYER_RACE_CYCLE_WIDTH,
// |                  PLAYER_RACE_CYCLE_HEIGHT,
// |                  "ngextra.icn",
// ALTERNATIVE remove_011 (unified delta from BEFORE):
// | --- before
// | +++ remove_011
// | @@ -3,15 +3,13 @@
// |                      firstColumnX + playerStep * playerCounter
// |                      + PLAYER_RACE_CYCLE_X_OFFSET
// |                  ),
// | -                static_cast<i16>(
// | -                    yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// | -                ),
// | +                yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y,
// |                  PLAYER_RACE_CYCLE_WIDTH,
// |                  PLAYER_RACE_CYCLE_HEIGHT,
// |                  "ngextra.icn",
// |                  PLAYER_RACE_CYCLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_CYCLE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_100 (unified delta from BEFORE):
// | --- before
// | +++ remove_100
// | @@ -1,8 +1,6 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter
// | -                    + PLAYER_RACE_CYCLE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter
// | +                    + PLAYER_RACE_CYCLE_X_OFFSET,
// |                  static_cast<i16>(
// |                      yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// |                  ),
// ALTERNATIVE remove_101 (unified delta from BEFORE):
// | --- before
// | +++ remove_101
// | @@ -1,8 +1,6 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter
// | -                    + PLAYER_RACE_CYCLE_X_OFFSET
// | -                ),
// | +                firstColumnX + playerStep * playerCounter
// | +                    + PLAYER_RACE_CYCLE_X_OFFSET,
// |                  static_cast<i16>(
// |                      yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// |                  ),
// | @@ -11,7 +9,7 @@
// |                  "ngextra.icn",
// |                  PLAYER_RACE_CYCLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_CYCLE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );
// ALTERNATIVE remove_110 (unified delta from BEFORE):
// | --- before
// | +++ remove_110
// | @@ -1,11 +1,7 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter
// | -                    + PLAYER_RACE_CYCLE_X_OFFSET
// | -                ),
// | -                static_cast<i16>(
// | -                    yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// | -                ),
// | +                firstColumnX + playerStep * playerCounter
// | +                    + PLAYER_RACE_CYCLE_X_OFFSET,
// | +                yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y,
// |                  PLAYER_RACE_CYCLE_WIDTH,
// |                  PLAYER_RACE_CYCLE_HEIGHT,
// |                  "ngextra.icn",
// ALTERNATIVE remove_111 (unified delta from BEFORE):
// | --- before
// | +++ remove_111
// | @@ -1,17 +1,13 @@
// |  new iconWidget(
// | -                static_cast<i16>(
// | -                    firstColumnX + playerStep * playerCounter
// | -                    + PLAYER_RACE_CYCLE_X_OFFSET
// | -                ),
// | -                static_cast<i16>(
// | -                    yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// | -                ),
// | +                firstColumnX + playerStep * playerCounter
// | +                    + PLAYER_RACE_CYCLE_X_OFFSET,
// | +                yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y,
// |                  PLAYER_RACE_CYCLE_WIDTH,
// |                  PLAYER_RACE_CYCLE_HEIGHT,
// |                  "ngextra.icn",
// |                  PLAYER_RACE_CYCLE_FRAME,
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
// | +                playerCounter + NEW_GAME_RACE_CYCLE_FIRST,
// |                  WIDGET_KIND_ICON_DIRECT,
// |                  PLAYER_WIDGET_FILL_COLOR
// |              );

// VC6 SP5 source family c76-newgame-scenario-widget6; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-scenario-widget6/results.json; RVA 0x7869f
// Complete 8 arms; elapsed 5.448810s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / bf67ff05a044140f / 85:85 / 85,0,0,0 / True
// 1 / {'family': 'remove_001', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / 64466459458b38c3 / 85:85 / 85,0,0,0 / True
// 2 / {'family': 'remove_010', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / 64466459458b38c3 / 85:85 / 85,0,0,0 / True
// 3 / {'family': 'remove_011', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / 6a8aaad00450e573 / 85:85 / 85,0,0,0 / True
// 4 / {'family': 'remove_100', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / 64466459458b38c3 / 85:85 / 85,0,0,0 / True
// 5 / {'family': 'remove_101', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / 6a8aaad00450e573 / 85:85 / 85,0,0,0 / True
// 6 / {'family': 'remove_110', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / 6a8aaad00450e573 / 85:85 / 85,0,0,0 / True
// 7 / {'family': 'remove_111', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / 1785b66eb2afa6b5 / 85:85 / 85,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// | new iconWidget(
// |             static_cast<i16>(
// |                 firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// |             ),
// |             static_cast<i16>(
// |                 yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// |             ),
// |             PLAYER_RACE_ICON_WIDTH,
// |             PLAYER_RACE_ICON_HEIGHT,
// |             "ngextra.icn",
// |             PLAYER_RACE_ICON_FRAME,
// |             ICON_DRAW_NORMAL,
// |             static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
// |             WIDGET_KIND_ICON_DIRECT,
// |             PLAYER_WIDGET_FILL_COLOR
// |         );
// ALTERNATIVE remove_001 (unified delta from BEFORE):
// | --- before
// | +++ remove_001
// | @@ -10,7 +10,7 @@
// |              "ngextra.icn",
// |              PLAYER_RACE_ICON_FRAME,
// |              ICON_DRAW_NORMAL,
// | -            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
// | +            playerCounter + NEW_GAME_RACE_ICON_FIRST,
// |              WIDGET_KIND_ICON_DIRECT,
// |              PLAYER_WIDGET_FILL_COLOR
// |          );
// ALTERNATIVE remove_010 (unified delta from BEFORE):
// | --- before
// | +++ remove_010
// | @@ -2,9 +2,7 @@
// |              static_cast<i16>(
// |                  firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// |              ),
// | -            static_cast<i16>(
// | -                yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// | -            ),
// | +            yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y,
// |              PLAYER_RACE_ICON_WIDTH,
// |              PLAYER_RACE_ICON_HEIGHT,
// |              "ngextra.icn",
// ALTERNATIVE remove_011 (unified delta from BEFORE):
// | --- before
// | +++ remove_011
// | @@ -2,15 +2,13 @@
// |              static_cast<i16>(
// |                  firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// |              ),
// | -            static_cast<i16>(
// | -                yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// | -            ),
// | +            yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y,
// |              PLAYER_RACE_ICON_WIDTH,
// |              PLAYER_RACE_ICON_HEIGHT,
// |              "ngextra.icn",
// |              PLAYER_RACE_ICON_FRAME,
// |              ICON_DRAW_NORMAL,
// | -            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
// | +            playerCounter + NEW_GAME_RACE_ICON_FIRST,
// |              WIDGET_KIND_ICON_DIRECT,
// |              PLAYER_WIDGET_FILL_COLOR
// |          );
// ALTERNATIVE remove_100 (unified delta from BEFORE):
// | --- before
// | +++ remove_100
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -            static_cast<i16>(
// | -                firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// | -            ),
// | +            firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET,
// |              static_cast<i16>(
// |                  yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// |              ),
// ALTERNATIVE remove_101 (unified delta from BEFORE):
// | --- before
// | +++ remove_101
// | @@ -1,7 +1,5 @@
// |  new iconWidget(
// | -            static_cast<i16>(
// | -                firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// | -            ),
// | +            firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET,
// |              static_cast<i16>(
// |                  yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// |              ),
// | @@ -10,7 +8,7 @@
// |              "ngextra.icn",
// |              PLAYER_RACE_ICON_FRAME,
// |              ICON_DRAW_NORMAL,
// | -            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
// | +            playerCounter + NEW_GAME_RACE_ICON_FIRST,
// |              WIDGET_KIND_ICON_DIRECT,
// |              PLAYER_WIDGET_FILL_COLOR
// |          );
// ALTERNATIVE remove_110 (unified delta from BEFORE):
// | --- before
// | +++ remove_110
// | @@ -1,10 +1,6 @@
// |  new iconWidget(
// | -            static_cast<i16>(
// | -                firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// | -            ),
// | -            static_cast<i16>(
// | -                yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// | -            ),
// | +            firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET,
// | +            yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y,
// |              PLAYER_RACE_ICON_WIDTH,
// |              PLAYER_RACE_ICON_HEIGHT,
// |              "ngextra.icn",
// ALTERNATIVE remove_111 (unified delta from BEFORE):
// | --- before
// | +++ remove_111
// | @@ -1,16 +1,12 @@
// |  new iconWidget(
// | -            static_cast<i16>(
// | -                firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// | -            ),
// | -            static_cast<i16>(
// | -                yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// | -            ),
// | +            firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET,
// | +            yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y,
// |              PLAYER_RACE_ICON_WIDTH,
// |              PLAYER_RACE_ICON_HEIGHT,
// |              "ngextra.icn",
// |              PLAYER_RACE_ICON_FRAME,
// |              ICON_DRAW_NORMAL,
// | -            static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
// | +            playerCounter + NEW_GAME_RACE_ICON_FIRST,
// |              WIDGET_KIND_ICON_DIRECT,
// |              PLAYER_WIDGET_FILL_COLOR
// |          );

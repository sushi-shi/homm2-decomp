// VC6 SP5 Castle source family c69-castle-raceBase; parent f8b6c956.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c69-castle-raceBase/results.json; RVA 0x24320
// Complete 8 arms; elapsed 4.756893s; input restored.
// Disposition: retain all cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / dcfacc22e7679c2b / 138:138 / 138,0,0,0 / True
// 1 / {'source_family': 'frame', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / 632320b5ff4d48cb / 138:138 / 138,0,0,0 / True
// 2 / {'source_family': 'y', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / 632320b5ff4d48cb / 138:138 / 138,0,0,0 / True
// 3 / {'source_family': 'y_frame', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / dd62f20ce8d08652 / 138:138 / 138,0,0,0 / True
// 4 / {'source_family': 'x', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / 632320b5ff4d48cb / 138:138 / 138,0,0,0 / True
// 5 / {'source_family': 'x_frame', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / dd62f20ce8d08652 / 138:138 / 138,0,0,0 / True
// 6 / {'source_family': 'x_y', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / dd62f20ce8d08652 / 138:138 / 138,0,0,0 / True
// 7 / {'source_family': 'x_y_frame', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / 5f0d49023cef0833 / 138:138 / 138,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |                 backgroundWidget = new iconWidget(
// |                     static_cast<i16>(
// |                         (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// |                     ),
// |                     static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// |                     BACKGROUND_TILE_SIZE,
// |                     BACKGROUND_TILE_SIZE,
// |                     "objntown.icn",
// |                     static_cast<i16>(raceBase + backFrame),
// |                     ICON_DRAW_NORMAL,
// |                     TOWN_WIDGET_ID_NONE,
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     BACKGROUND_FILL_COLOR
// |                 );
// ALTERNATIVE frame (unified source delta from BEFORE):
// | --- before
// | +++ frame
// | @@ -6,7 +6,7 @@
// |                      BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      "objntown.icn",
// | -                    static_cast<i16>(raceBase + backFrame),
// | +                    raceBase + backFrame,
// |                      ICON_DRAW_NORMAL,
// |                      TOWN_WIDGET_ID_NONE,
// |                      WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE y (unified source delta from BEFORE):
// | --- before
// | +++ y
// | @@ -2,7 +2,7 @@
// |                      static_cast<i16>(
// |                          (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// |                      ),
// | -                    static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// | +                    (rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      "objntown.icn",
// ALTERNATIVE y_frame (unified source delta from BEFORE):
// | --- before
// | +++ y_frame
// | @@ -2,11 +2,11 @@
// |                      static_cast<i16>(
// |                          (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// |                      ),
// | -                    static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// | +                    (rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      "objntown.icn",
// | -                    static_cast<i16>(raceBase + backFrame),
// | +                    raceBase + backFrame,
// |                      ICON_DRAW_NORMAL,
// |                      TOWN_WIDGET_ID_NONE,
// |                      WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE x (unified source delta from BEFORE):
// | --- before
// | +++ x
// | @@ -1,7 +1,5 @@
// |                  backgroundWidget = new iconWidget(
// | -                    static_cast<i16>(
// | -                        (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// | -                    ),
// | +                    (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT,
// |                      static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// |                      BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// ALTERNATIVE x_frame (unified source delta from BEFORE):
// | --- before
// | +++ x_frame
// | @@ -1,12 +1,10 @@
// |                  backgroundWidget = new iconWidget(
// | -                    static_cast<i16>(
// | -                        (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// | -                    ),
// | +                    (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT,
// |                      static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// |                      BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      "objntown.icn",
// | -                    static_cast<i16>(raceBase + backFrame),
// | +                    raceBase + backFrame,
// |                      ICON_DRAW_NORMAL,
// |                      TOWN_WIDGET_ID_NONE,
// |                      WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE x_y (unified source delta from BEFORE):
// | --- before
// | +++ x_y
// | @@ -1,8 +1,6 @@
// |                  backgroundWidget = new iconWidget(
// | -                    static_cast<i16>(
// | -                        (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// | -                    ),
// | -                    static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// | +                    (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT,
// | +                    (rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      "objntown.icn",
// ALTERNATIVE x_y_frame (unified source delta from BEFORE):
// | --- before
// | +++ x_y_frame
// | @@ -1,12 +1,10 @@
// |                  backgroundWidget = new iconWidget(
// | -                    static_cast<i16>(
// | -                        (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// | -                    ),
// | -                    static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// | +                    (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT,
// | +                    (rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      BACKGROUND_TILE_SIZE,
// |                      "objntown.icn",
// | -                    static_cast<i16>(raceBase + backFrame),
// | +                    raceBase + backFrame,
// |                      ICON_DRAW_NORMAL,
// |                      TOWN_WIDGET_ID_NONE,
// |                      WIDGET_KIND_ICON_DIRECT,

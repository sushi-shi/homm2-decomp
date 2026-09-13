// VC6 SP5 source family g91-drawing-grid-joins; parent aab81af5.
// Source record only: each alternative is relative to its own BEFORE, not cumulative.
// No cross-family or TU-state census is claimed.
// build/source-variant-batch/g91-drawing-grid-joins/results.json; RVA 0x3883f
// Complete 4 arms; elapsed 2.894005s; source restored.
// Disposition: retain original; every tested nonoriginal arm changes function text.
// Raw/normalized relocation hashes can reflect private label counter spelling;
// final proof compares actual ordered targets/sites and the complete claimed byte span.
// UpdateMouseGrid has an unchanged EH/FS site-model gap, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1240 / 518b216e386819b1 / 334427acbf1c97f0 / 81:81 / 81,0,0,0 / True
// 1 / {'source_family': 'copy_scope', 'candidate': 'baseline'} / 1247 / 441744f9fae93730 / d99e714d677ee6be / 81:81 / 79,0,0,2 / False
// 2 / {'source_family': 'shade_scope', 'candidate': 'baseline'} / 1235 / e9e0c450992ba48d / 7babf33493cf835b / 80:81 / 11,2,14,53 / False
// 3 / {'source_family': 'shade_scope_copy_scope', 'candidate': 'baseline'} / 1240 / 81b3de3bcb408857 / 202d670229559d37 / 80:81 / 11,2,13,54 / False

// AXIS source_family
// BEFORE (complete exact span):
// | i32 combatManager::UpdateGrid(i32 resetGridDisplay, i32 rebuildGrid) {
// |     b32 didRedraw;
// |     b32 oldShading;
// |     i32 maxY;
// |     b32 gridChanged;
// |     i32 minY;
// |     i32 minX;
// |     i32 maxX;
// |     i32 cell;
// |     b32 doShading;
// |
// |     if (gbNoShowCombat != 0)
// |         return 0;
// |
// |     if (rebuildGrid != 0) {
// |         if (m_playerId[IDX(m_currentSide)] == -1 || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
// |             || m_gridSelectionDisabled != 0) {
// |             memset(m_gridState, IDX(GRID_SHADE_NONE), sizeof(m_gridState));
// |         } else {
// |             SetupGridForArmy(&m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]);
// |         }
// |     }
// |     if (resetGridDisplay != 0)
// |         bGridWasShowing = false;
// |     if (gConfig.combatShadeLevel < 1 && gConfig.showCombatGrid == 0)
// |         return 0;
// |
// |     didRedraw = false;
// |     minX = COMBAT_MAX_EXTENT_X;
// |     minY = COMBAT_MAX_EXTENT_Y;
// |     maxX = 0;
// |     maxY = 0;
// |     doShading = false;
// |     oldShading = false;
// |     gridChanged = false;
// |
// |     if (gConfig.combatShadeLevel < 1)
// |         goto DrawCombatGrid;
// |
// |     for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |         if (m_previousGridState[cell] != m_gridState[cell])
// |             gridChanged = true;
// |         if (m_gridState[cell] != GRID_SHADE_NONE)
// |             doShading = true;
// |         if (m_previousGridState[cell] != GRID_SHADE_NONE)
// |             oldShading = true;
// |     }
// |
// |     if (resetGridDisplay != 0) {
// |         if (doShading == 0)
// |             goto DrawCombatGrid;
// |     } else {
// |         if (gridChanged == 0)
// |             return 0;
// |         if (oldShading != 0) {
// |             for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |                 if (m_previousGridState[cell] != m_gridState[cell]
// |                     || m_gridState[cell] != GRID_SHADE_NONE) {
// |                     if (m_hexCells[cell].m_gridLeft < minX)
// |                         minX = m_hexCells[cell].m_gridLeft;
// |                     if (m_hexCells[cell].m_gridTop < minY)
// |                         minY = m_hexCells[cell].m_gridTop;
// |                     if (m_hexCells[cell].m_gridRight > maxX)
// |                         maxX = m_hexCells[cell].m_gridRight;
// |                     if (m_hexCells[cell].m_gridBottom > maxY)
// |                         maxY = m_hexCells[cell].m_gridBottom;
// |                 }
// |             }
// |             if (minX < COMBAT_GRID_COPY_LEFT)
// |                 minX = COMBAT_GRID_COPY_LEFT;
// |             if (minY < COMBAT_GRID_COPY_TOP)
// |                 minY = COMBAT_GRID_COPY_TOP;
// |             if (maxX > COMBAT_GRID_COPY_RIGHT)
// |                 maxX = COMBAT_GRID_COPY_RIGHT;
// |             if (maxY > COMBAT_GRID_COPY_BOTTOM)
// |                 maxY = COMBAT_GRID_COPY_BOTTOM;
// |             m_combatBuffer->CopyToCareful(
// |                 m_backgroundBuffer,
// |                 minX,
// |                 minY,
// |                 minX - COMBAT_GRID_COPY_LEFT,
// |                 minY - COMBAT_GRID_COPY_TOP,
// |                 maxX - minX + 1,
// |                 maxY - minY + 1
// |             );
// |             didRedraw = true;
// |         }
// |     }
// |
// |     if (doShading == 0)
// |         goto DrawCombatGrid;
// |     for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |         if (m_gridState[cell] != GRID_SHADE_NONE) {
// |             DimIconToBitmap(
// |                 m_combatIcons[IDX(COMBAT_ICON_GRID)],
// |                 m_backgroundBuffer,
// |                 m_hexCells[cell].m_gridLeft,
// |                 m_hexCells[cell].m_gridTop,
// |                 1,
// |                 IDX(m_gridState[cell]) - 1,
// |                 ICON_DRAW_CLIP,
// |                 0,
// |                 0,
// |                 COMBAT_SCREEN_WIDTH,
// |                 COMBAT_AREA_HEIGHT
// |             );
// |             didRedraw = true;
// |         }
// |     }
// |
// | DrawCombatGrid:
// |     if (gConfig.showCombatGrid != 0) {
// |         if (bGridWasShowing != 0 && didRedraw == 0)
// |             goto CopyGridState;
// |         for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |             if (cell % COMBAT_GRID_ROW_LENGTH != 0
// |                 && cell % COMBAT_GRID_ROW_LENGTH != COMBAT_GRID_ROW_LENGTH - 1) {
// |                 MonoIconToBitmap(
// |                     m_combatIcons[IDX(COMBAT_ICON_GRID)],
// |                     m_backgroundBuffer,
// |                     m_hexCells[cell].m_gridLeft,
// |                     m_hexCells[cell].m_gridTop,
// |                     COMBAT_GRID_LINE_FRAME,
// |                     COMBAT_GRID_LINE_COLOR,
// |                     ICON_DRAW_CLIP,
// |                     0,
// |                     0,
// |                     COMBAT_SCREEN_WIDTH,
// |                     COMBAT_AREA_HEIGHT
// |                 );
// |             }
// |         }
// |         didRedraw = true;
// |         bGridWasShowing = true;
// |     }
// |
// | CopyGridState:
// |     memcpy(m_previousGridState, m_gridState, sizeof(m_previousGridState));
// |     return didRedraw;
// | }
// ALTERNATIVE copy_scope (unified delta from BEFORE):
// | --- before
// | +++ copy_scope
// | @@ -109,9 +109,10 @@
// |      }
// |
// |  DrawCombatGrid:
// | +    do {
// |      if (gConfig.showCombatGrid != 0) {
// |          if (bGridWasShowing != 0 && didRedraw == 0)
// | -            goto CopyGridState;
// | +            break;
// |          for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |              if (cell % COMBAT_GRID_ROW_LENGTH != 0
// |                  && cell % COMBAT_GRID_ROW_LENGTH != COMBAT_GRID_ROW_LENGTH - 1) {
// | @@ -134,7 +135,7 @@
// |          bGridWasShowing = true;
// |      }
// |
// | -CopyGridState:
// | +    } while (0);
// |      memcpy(m_previousGridState, m_gridState, sizeof(m_previousGridState));
// |      return didRedraw;
// |  }
// ALTERNATIVE shade_scope (unified delta from BEFORE):
// | --- before
// | +++ shade_scope
// | @@ -34,8 +34,9 @@
// |      oldShading = false;
// |      gridChanged = false;
// |
// | +    do {
// |      if (gConfig.combatShadeLevel < 1)
// | -        goto DrawCombatGrid;
// | +        break;
// |
// |      for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |          if (m_previousGridState[cell] != m_gridState[cell])
// | @@ -48,7 +49,7 @@
// |
// |      if (resetGridDisplay != 0) {
// |          if (doShading == 0)
// | -            goto DrawCombatGrid;
// | +            break;
// |      } else {
// |          if (gridChanged == 0)
// |              return 0;
// | @@ -88,7 +89,7 @@
// |      }
// |
// |      if (doShading == 0)
// | -        goto DrawCombatGrid;
// | +        break;
// |      for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |          if (m_gridState[cell] != GRID_SHADE_NONE) {
// |              DimIconToBitmap(
// | @@ -108,7 +109,7 @@
// |          }
// |      }
// |
// | -DrawCombatGrid:
// | +    } while (0);
// |      if (gConfig.showCombatGrid != 0) {
// |          if (bGridWasShowing != 0 && didRedraw == 0)
// |              goto CopyGridState;
// ALTERNATIVE shade_scope_copy_scope (unified delta from BEFORE):
// | --- before
// | +++ shade_scope_copy_scope
// | @@ -34,8 +34,9 @@
// |      oldShading = false;
// |      gridChanged = false;
// |
// | +    do {
// |      if (gConfig.combatShadeLevel < 1)
// | -        goto DrawCombatGrid;
// | +        break;
// |
// |      for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |          if (m_previousGridState[cell] != m_gridState[cell])
// | @@ -48,7 +49,7 @@
// |
// |      if (resetGridDisplay != 0) {
// |          if (doShading == 0)
// | -            goto DrawCombatGrid;
// | +            break;
// |      } else {
// |          if (gridChanged == 0)
// |              return 0;
// | @@ -88,7 +89,7 @@
// |      }
// |
// |      if (doShading == 0)
// | -        goto DrawCombatGrid;
// | +        break;
// |      for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |          if (m_gridState[cell] != GRID_SHADE_NONE) {
// |              DimIconToBitmap(
// | @@ -108,10 +109,11 @@
// |          }
// |      }
// |
// | -DrawCombatGrid:
// | +    } while (0);
// | +    do {
// |      if (gConfig.showCombatGrid != 0) {
// |          if (bGridWasShowing != 0 && didRedraw == 0)
// | -            goto CopyGridState;
// | +            break;
// |          for (cell = 0; cell < COMBAT_HEX_COUNT; cell++) {
// |              if (cell % COMBAT_GRID_ROW_LENGTH != 0
// |                  && cell % COMBAT_GRID_ROW_LENGTH != COMBAT_GRID_ROW_LENGTH - 1) {
// | @@ -134,7 +136,7 @@
// |          bGridWasShowing = true;
// |      }
// |
// | -CopyGridState:
// | +    } while (0);
// |      memcpy(m_previousGridState, m_gridState, sizeof(m_previousGridState));
// |      return didRedraw;
// |  }

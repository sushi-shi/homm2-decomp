// VC6 SP5 kbwin source family s67-kbwin-style-slots; parentf0502d36.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s67-kbwin-style-slots/results.json; RVA 0x71d60
// Complete 4 arms; elapsed 2.335362s; input restored.
// Disposition: retain original; every tested alternative changes text.
// No EH/numerical exceptions in this TU; all sixteen retail checks are complete.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 279 / 6c7566188abb0c0b / 1076792983fe069f / 8:8 / 8,0,0,0 / True
// 1 / {'source_family': 'replaced_style', 'candidate': 'baseline'} / 276 / 5835a91942042b84 / 36a5964f9e843214 / 8:8 / 7,1,0,0 / True
// 2 / {'source_family': 'window_style', 'candidate': 'baseline'} / 276 / e24c750a0751ac9f / f2cb209540d78ab6 / 8:8 / 7,1,0,0 / True
// 3 / {'source_family': 'window_style_replaced_style', 'candidate': 'baseline'} / 273 / bd55f74bb1cec8b3 / 6bed0a20e6896c93 / 8:8 / 6,2,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | void SetMenuStatus(i32 showMenu) {
// |     i32 winWidth;
// |     i32 height;
// |     i32l H2_UNUSED(windowStyle);
// |     i32l H2_UNUSED(replacedStyle);
// |
// |     if (gConfig.gfx[IDX(giCurExe)].fullScreen && showMenu)
// |         return;
// |     {
// |         winWidth = gConfig.gfx[IDX(giCurExe)].width;
// |         height = gConfig.gfx[IDX(giCurExe)].height;
// |         gConfig.gfx[IDX(giCurExe)].showMenu = showMenu;
// |         KBChangeMenu(NULL);
// |         gConfig.gfx[IDX(giCurExe)].width = winWidth;
// |         gConfig.gfx[IDX(giCurExe)].height = height;
// |         WritePrefs();
// |         windowStyle = GetWindowLongA(hwndApp, GWL_STYLE);
// |         if (gConfig.gfx[IDX(giCurExe)].showMenu)
// |             giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
// |         else
// |             giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
// |         replacedStyle = SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
// |         ShowWindow(hwndApp, SW_SHOWNA);
// |         ResizeWindow(-1, -1, gConfig.gfx[IDX(giCurExe)].width, gConfig.gfx[IDX(giCurExe)].height);
// |     }
// | }
// ALTERNATIVE replaced_style (unified source delta from BEFORE):
// | --- before
// | +++ replaced_style
// | @@ -2,7 +2,6 @@
// |      i32 winWidth;
// |      i32 height;
// |      i32l H2_UNUSED(windowStyle);
// | -    i32l H2_UNUSED(replacedStyle);
// |
// |      if (gConfig.gfx[IDX(giCurExe)].fullScreen && showMenu)
// |          return;
// | @@ -19,7 +18,7 @@
// |              giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
// |          else
// |              giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
// | -        replacedStyle = SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
// | +        SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
// |          ShowWindow(hwndApp, SW_SHOWNA);
// |          ResizeWindow(-1, -1, gConfig.gfx[IDX(giCurExe)].width, gConfig.gfx[IDX(giCurExe)].height);
// |      }
// ALTERNATIVE window_style (unified source delta from BEFORE):
// | --- before
// | +++ window_style
// | @@ -1,7 +1,6 @@
// |  void SetMenuStatus(i32 showMenu) {
// |      i32 winWidth;
// |      i32 height;
// | -    i32l H2_UNUSED(windowStyle);
// |      i32l H2_UNUSED(replacedStyle);
// |
// |      if (gConfig.gfx[IDX(giCurExe)].fullScreen && showMenu)
// | @@ -14,7 +13,7 @@
// |          gConfig.gfx[IDX(giCurExe)].width = winWidth;
// |          gConfig.gfx[IDX(giCurExe)].height = height;
// |          WritePrefs();
// | -        windowStyle = GetWindowLongA(hwndApp, GWL_STYLE);
// | +        GetWindowLongA(hwndApp, GWL_STYLE);
// |          if (gConfig.gfx[IDX(giCurExe)].showMenu)
// |              giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
// |          else
// ALTERNATIVE window_style_replaced_style (unified source delta from BEFORE):
// | --- before
// | +++ window_style_replaced_style
// | @@ -1,8 +1,6 @@
// |  void SetMenuStatus(i32 showMenu) {
// |      i32 winWidth;
// |      i32 height;
// | -    i32l H2_UNUSED(windowStyle);
// | -    i32l H2_UNUSED(replacedStyle);
// |
// |      if (gConfig.gfx[IDX(giCurExe)].fullScreen && showMenu)
// |          return;
// | @@ -14,12 +12,12 @@
// |          gConfig.gfx[IDX(giCurExe)].width = winWidth;
// |          gConfig.gfx[IDX(giCurExe)].height = height;
// |          WritePrefs();
// | -        windowStyle = GetWindowLongA(hwndApp, GWL_STYLE);
// | +        GetWindowLongA(hwndApp, GWL_STYLE);
// |          if (gConfig.gfx[IDX(giCurExe)].showMenu)
// |              giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
// |          else
// |              giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
// | -        replacedStyle = SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
// | +        SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
// |          ShowWindow(hwndApp, SW_SHOWNA);
// |          ResizeWindow(-1, -1, gConfig.gfx[IDX(giCurExe)].width, gConfig.gfx[IDX(giCurExe)].height);
// |      }

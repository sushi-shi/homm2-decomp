# Cursor rendering

Cursor meaning has one shared path: adventure/combat/spell code selects the
role and frame through `mouseManager::SetPointer`. Neither rendering adapter
decides which action the pointer represents. A role change invalidates the
selected image even when its frame number stays the same. Switching color mode
preserves that role and frame.

The artwork and presentation differ:

- Color mode keeps the existing software-rendered `ADVMCO.ICN`, `CMSECO.ICN`,
  and `SPELCO.ICN` images in game-pixel coordinates.
- Monochrome mode loads the original `ADVMBW%02d.BMP`, `CMSEBW%02d.BMP`, and
  `SPELBW%02d.BMP` resources and selects an SDL native cursor. Adventure and
  combat numbering starts at 1; spell numbering starts at 0. There are 96
  selectable frames (41 adventure, 16 combat, 39 spell).

These `.BMP` entries are not Windows BMP files: a six-byte header precedes
32-by-32 top-down byte pixels. The decoder validates the dimensions and length,
then preserves the reconstructed Windows mask semantics: 0 is transparent,
1 is white, and every other value is black. Some installed spell assets use
values above 2. Adventure/combat use the existing hotspot table; monochrome
spells use the original fixed `(15, 15)` hotspot. Color spell artwork has its
own dimensions and existing hotspots.

The platform interface owns no game roles, asset names, or archive logic. It
accepts decoded bits and a hotspot, caches native handles by their contents,
and changes the shape without changing visibility. Mouse-manager shutdown
restores the desktop cursor; video shutdown deselects and destroys the cached
handles before shutting down SDL video. Failed native creation/selection is
reported, leaves the prior cursor in place, and does not cache a failed handle.

## Regression provenance

The custom monochrome path exists in the Gold/Buka reconstruction. Native-port
commit `5a1e7537a` introduced a compatibility `SetCursor` stub that did nothing;
commit `fac319200` subsequently removed the Windows bitmap/mask/handle logic
and left only `ShowCursor(true)`. That shows the host desktop cursor but does
not select the game's context-dependent artwork. This fix supplies the missing
native adapter without duplicating game action selection.

## Tests

The ordinary asset-free CTest suite includes:

- `monochrome_cursor`: 262,144 pixel conversions across all byte values and
  positions, plus malformed dimensions, truncated input, and invalid hotspots.
- `mouse_cursor_selection`: the real mouse manager with synthetic resources,
  covering role/frame changes, same-number frames across roles, resource caching,
  hide/show, retained frames, and color/monochrome switching.
- `monochrome_cursor_backend` (Linux): the real SDL video implementation with
  only cursor calls wrapped, covering pixels, hotspots, handle reuse, visibility,
  creation/selection failures, reset, close, and reopen.

The optional `homm2_monochrome_cursor_assets_test` links the production game
sources and checks all 96 selections against installed assets using actual SDL
native cursor creation. It also verifies hidden reselection, mode switching,
and restoring the desktop cursor. It requires a cursor-capable video backend;
SDL's dummy backend does not implement hardware cursors. For example:

```sh
nix develop --command cmake --build build --target homm2_monochrome_cursor_assets_test
xvfb-run -a env SDL_VIDEODRIVER=x11 SDL_AUDIODRIVER=dummy \
  SDL_RENDER_DRIVER=software HOMM2_DATA=/path/to/game-data \
  XDG_DATA_HOME=/tmp/homm2-cursor-test-state \
  ./build/homm2_monochrome_cursor_assets_test
```

Set the usual `HOMM2_LOCALE_DATA` and `HOMM2_LANGUAGE` variables as needed for
localized archive overlays. No retail artwork is included in the tests.

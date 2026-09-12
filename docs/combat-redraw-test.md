# Combat background redraw regression

This optional integration test needs a legally installed copy of the game.
It is excluded from the default build and asset-free CTest suite. It links the
production game renderer and loads the original snow battlefield, peasant,
pikeman, quantity-bar, and font resources.

From a configured native build (inside `nix develop` when using Nix):

```sh
cmake --build build --target homm2_combat_redraw_test
env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy SDL_RENDER_DRIVER=software \
  HOMM2_DATA=/path/to/game/data \
  XDG_DATA_HOME=/tmp/homm2-combat-redraw-test-state \
  ./build/homm2_combat_redraw_test
```

Use a separate `XDG_DATA_HOME` as above so the fixture does not use or change
normal game preferences. Locale overlays can be selected with the same
`HOMM2_LANGUAGE`, `HOMM2_LOCALE_DATA`, and `HOMM2_LOCALE_PATH` settings as the game.

The fixture invalidates the background cache as walking does, then runs all
six melee attack/return sequences in both orientations beside eight neighbouring
hex positions. After each incremental redraw it compares the entire working
screen with a fresh full render, preserving the incremental pixels for the next
frame. This catches background holes outside the immediate update rectangle:
attack and cursor updates can subsequently expose those pixels.

Exit code 0 means every frame matched; 1 reports a rendering mismatch; 2 means
required setup is missing. No proprietary assets are included in the test.

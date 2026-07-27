// SetGameDefaults, RVA 0x004c49a0
//
// Retained source shape:
//
//     gConfig.musicVolume = CONFIG_VOLUME_MIN;
//     gConfig.soundVolume = CONFIG_VOLUME_MIN;
//     gConfig.autosave = 1;
//     gConfig.showRoute = 1;
//     i32* fullScreen =
//         &gConfig.gfx[IDX(CONFIG_EXECUTABLE_GAME)].fullScreen;
//
// The previous source declared fullScreen before the four scalar defaults.  Its
// clean object was 95.201750%, 435 bytes, with 42/42 relocations.  Moving the
// declaration after the defaults is semantic-preserving and makes the retail
// prologue exact through offset 0x33: push EBX/ESI/EDI precede materialization
// of the graphics pointer.  The clean result is 96.780700%, 435 bytes, with
// 42/42 relocations.
//
// Reviewed alternatives:
//
//     i32* fullScreen;
//     <four scalar defaults>
//     fullScreen = &gConfig.gfx[...].fullScreen;
//
// This emitted the same 96.780700% state.  Assigning the pointer between the
// scalar defaults returned to 95.201750%.
//
// Artifacts:
//   build/set-game-defaults-prefix-lifetime-clean/
//   build/set-game-defaults-prefix-lifetime-clean.json
//
// Disposition: retain defaults-then-pointer.  It is the smallest credible
// source recovery and improves both the first real divergence and clean MAX.

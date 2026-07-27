// SetGameDefaults, RVA 0x004c49a0
//
// Internal spellings were tested only after the loop-local structural family.
//
// Local declaration/type forms:
//
//     i32 / const i32 / i32l / register i32 / register i32l
//         mainVideoModeWidth = giMainVideoModeWidth;
//
// Color-mouse zero forms:
//
//     0
//     false
//     0L
//     static_cast<i32>(0)
//
// All combinations collapsed to the same clean islands.  Explicit locals for
// DEFAULT_WINDOW_WIDTH and DEFAULT_WINDOW_ORIGIN likewise did not change the
// allocation.
//
// Inline-read forms:
//
//     inline i32 GetMainVideoModeWidth() {
//         return giMainVideoModeWidth;
//     }
//
//     inline i32 ReadVideoModeWidth(const i32* width) {
//         return *width;
//     }
//
//     inline i32 ReadVideoModeWidth(const i32& width) {
//         return width;
//     }
//
// These folded to the direct memory comparison (96.780700% with the retained
// prefix).  A local width plus a one-use useSmallWindow predicate also folded
// to the ordinary local-width island.  Volatile-read forms were diagnostic
// only; they did not produce the retail load/register allocation and are not
// credible retained source.
//
// Artifacts:
//   build/set-game-defaults-local-spelling-clean/
//   build/set-game-defaults-register-pressure-clean/
//   build/set-game-defaults-inline-width-clean/
//   build/set-game-defaults-condition-shape-clean/
//   build/set-game-defaults-predicate-lifetime-clean/
//   build/set-game-defaults-volatile-diagnostic-clean/
//
// Disposition: none retained.

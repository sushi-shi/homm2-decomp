// advManager::ProcessMapChange @ PoL RVA 0x00010152
//
// PoL source before:
//   change.player != giCurPlayer
//   change.x/y != mapHero->m_x/y (MOVE_HERO and DEAD_HERO)
//   two raw cells + width*y + x pointer locals, each followed by DebugCheck(),
//   for the RECRUIT_HERO map-cell writes
//
// Buka exact / retained PoL source:
//   giCurPlayer != change.player
//   mapHero->m_x/y != change.x/y (both sites)
//   two direct worldMap.GetCell(change.x, change.y)->field writes
//
// Complete clean product:
//   build/source-variant-batch/process-map-change-donors/results.json
//   16/16 cells = player comparison * MOVE_HERO comparison * DEAD_HERO
//   comparison * recruit-cell ownership. Every arm emits the same 1871-byte
//   instruction text hash. The public batch score is 99.886580% because its
//   disposable objects retain two anonymous DATA_COMPGEN relocation spellings;
//   this is not a target-text difference.
//
// Normal retained-source audit after annotated-function refresh and full build:
//   100.000000%, 1871 bytes, 48/48 exact blocks, complete ordered 98/98
//   relocations. The source port changes neither bytes nor behavior.
//
// Disposition: invariant matching decomp; retain all four Buka donor choices
// in PoL. The direct GetCell writes recover semantic map ownership and remove
// two reconstruction-only pointer locals and empty DebugCheck hooks.

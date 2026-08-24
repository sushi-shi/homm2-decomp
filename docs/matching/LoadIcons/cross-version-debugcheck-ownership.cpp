/*
combatManager::LoadIcons cross-version audit, 2026-08-24.

Targets:
  PoL 2.0: RVA 0x00091dda, 999 bytes, 27 blocks.
  Buka:    RVA 0x00027a8b, 954 bytes, 26 blocks.

Classification: MATCHING-ONLY DEV CHANGE. Shipping DebugCheck() is an empty
inline hook, so moving its call site does not change release behavior.

PoL matrix:
  build/load-icons-debugcheck-ownership-manifest.json
  build/source-variant-batch/load-icons-debugcheck-ownership/results.json

All 9/9 clean structural arms completed. Function- and block-local forms with
DebugCheck() inside the non-neutral player-color arm both reached 100.000000%,
999/999 bytes, exact 27/27 CFG, and the complete ordered 58/58 relocations.
The block-local form is retained. The previous after-if call scored 99.977380%;
Buka's inline ternary scored 98.800900% under VC4.2. Scope alone was neutral.

Rejected clean arms:
  heroColor = condition ? neutral : playerColor;
  i32 heroColor = condition ? neutral : playerColor;
  direct Buka ternary in the sprintf argument;
  explicit goto ownership in either local scope.

Buka reverse matrix:
  build/load-icons-cross-compiler-scopes-manifest.json
  build/source-variant-batch/load-icons-cross-compiler-scopes/results.json

All 3/3 clean arms completed. Buka's inline ternary remains exact; both PoL
player-arm forms score 99.820274% under VC6. No tested shared spelling closes
both retail functions.
*/

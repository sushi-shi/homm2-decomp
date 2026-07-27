// CombatClipDrawToBuffer follow-up structures and bounded permutations.
//
// Repeating `frame * sizeof(IconEntry)` in every cast instead of retaining
// entryOffset produced 81.053276%, 702 bytes, and 37/37 relocations:
//
//   reinterpret_cast<IconEntry*>(
//       m_data + frame * sizeof(IconEntry)
//   )->x
//
// Reversing the mirror/entryOffset declaration order, or removing the mirror
// local and testing orientation directly, emitted the same 96.938520%,
// 691-byte state as the retained source.
//
// The complete depth-one AST census tested 85 variants. Seventy-six emitted
// the retained 96.938520% state; none improved it. A declaration-aware matrix
// tested 255 combinations of historically plausible spellings including
// flip, direction, frameOffset, iconOffset, and index; every successful top
// candidate also emitted the retained state. An additional compact-family
// state census compiled 43 typedef/enum/struct/class/packed/member/extern/
// static-data/prototype/function/include/mixed states without improvement.
//
// Artifacts:
//   build/match-variants/combatclip-repeated-offset-clean/results.json
//   build/match-variants/combatclip-direct-orientation-clean/results.json
//   build/match-variants/combatclip-reversed-local-order-clean/results.json
//   build/match-variants/combatclip-raw-ast-depth1/results.json
//   build/match-variants/combatclip-identifier-matrix/results.json
//   build/match-variants/combatclip-reversed-local-order-compact-states/results.json
//
// Disposition: all rejected. Lower scores were retained as artifact evidence,
// not pruned merely for being below MAX.

// G40: measured source alternatives for hero::CheckLevel, RVA 0x620c8.
// Exact-span matrix: build/g40-axes.json; complete results:
// build/g40-batch/results.json. Both arms compiled in 2.52s, no state probes.
// Both retain the same selection assignment and terminal tryCount decrement.
//
// Control (retained):
//     while (tryCount < HERO_SECONDARY_SKILL_SEARCH_LIMIT) {
//         ...
//         if (weight <= 0) {
//             choices[slot] = skill;
//             goto nextAttempt;
//         }
//         ...
//     }
// nextAttempt:
//     tryCount--;
//
// Candidate (rejected):
//     while (tryCount < HERO_SECONDARY_SKILL_SEARCH_LIMIT) {
//         ...
//         if (weight <= 0) {
//             choices[slot] = skill;
//             break;
//         }
//         ...
//     }
//     tryCount--;
//
// Goto: 1699 bytes, 67 blocks, 56 relocs; complete independent retail proof.
// Break: 1689 bytes, 66 blocks, 56 shifted relocs, 99.411766% diagnostic.
// Break removes the +0x69a trampoline and its tail skip and shortens the
// selection jump. See docs/reconstruction/G40.md for actual bytes/offsets.

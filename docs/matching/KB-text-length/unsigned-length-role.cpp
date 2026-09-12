// C30: complete independent local-type x comparison matrices for both functions.
// build/c30-{morale,luck}-{axes,manifest}.json and -batch/results.json.
// 4/4 each, 6.97/6.93 seconds, no truncation, source restored.
// Every arm exact target-local; retained unsigned/direct. Complete KB native
// comparison and independent whole-function retail bytes/sites/targets pass.
// See docs/reconstruction/C30-C31.md for the actual unchanged instructions.

// Declaration alternatives (same modifierStart name/scope):
i32 modifierStart;  // baseline
u32 modifierStart;  // retained size role

// Comparison alternatives; both independent of declaration choice:
if (modifierStart == static_cast<i32>(strlen(gText)))  // baseline
if (modifierStart == strlen(gText))                   // retained

// The morale body has braces around its strcat; the luck body does not.
// Those control-flow structures were preserved in every exact-span arm.

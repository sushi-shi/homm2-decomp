// FlipIconToBitmap setup out-reference family.
//
// Matrix:
//   build/iconf2b-setup-outrefs-axes-20260727.json
// Results:
//   build/match-variants/iconf2b-setup-outrefs-clean-20260727/results.json
//
// Seven clean structural arms crossed the direct setup with six inline helper
// spellings. The direct source remained 86.846150% (1251 bytes, 79/81 object
// relocations). Five helper spellings produced 73.758620% and one produced
// 73.718834%, all at 1275 bytes and 79/81 relocations. Every helper arm still
// emitted 75 blocks against retail's 76, with no exact aligned block.
//
// The family was therefore pruned before compiler-state trials: it adds an
// inline boundary but contradicts the retail setup topology and relocation
// ownership rather than exposing a plausible alternate compiler-state orbit.

static inline void FlipSetupFields(
    IconEntry* entry,
    i32 x,
    i32& x0,
    i32& width
) {
    // Matrix arms varied the order and grouping of these three operations.
    width = entry->w;
    x0 = x;
    x0 -= entry->x;
    x0 -= width;
}

void attempted_setup(IconEntry* entry, i32 x) {
    i32 x0;
    i32 width;
    FlipSetupFields(entry, x, x0, width);
}

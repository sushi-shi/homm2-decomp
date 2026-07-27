// DoBlast structural and compiler-state census, 2026-07-27.
//
// Target: SOURCE/SPELLS, RVA 0x00028d4f, retail size 0x33a.
//
// Fresh canonical evidence:
//   - 23 candidate blocks and 23 retail blocks, with identical topology.
//   - 0x7c-byte frames and the same saved registers.
//   - 50 candidate and 50 retail relocations.
//   - The clean listing's first visible difference is the operand order of the
//     for-loop comparison.  Other legitimate TU states independently change
//     the squared-distance term order and the two x87 coordinate additions.
//
// Matrices:
//   /tmp/doblast-loop-condition-manifest.json
//   build/match-variants/doblast-loop-condition
//     2 loop-condition spellings x (clean + 50 forest states), 102 complete.
//     Best: 99.947136%, size 0x33a, 50/50 relocations.  No exact closure.
//
//   /tmp/doblast-arithmetic-shape-clean.json
//   build/match-variants/doblast-arithmetic-shape-clean
//     16 clean combinations of distance-term order, loop comparison order,
//     and assignment/compound forms for both coordinate updates.
//     All 16 compiled to the identical 99.647575% clean object.
//
//   /tmp/doblast-inline-structure-clean.json
//   build/match-variants/doblast-inline-structure-clean
//     Direct arithmetic retained size 0x33a.  An inline squared-distance helper
//     grew the function to 0x33f; value- and reference-based coordinate helpers
//     grew it to 0x344.  These inline boundaries contradict retail size.
//
//   /tmp/doblast-loop-structure-clean.json
//   build/match-variants/doblast-loop-structure-clean
//     Both for-loop comparison spellings retained size 0x33a and the same
//     object.  Equivalent while-loop spellings emitted size 0x335 and were
//     rejected as a different CFG/code-size family.
//
// The retained current-hash MAX remains 99.9912%, which is higher than every
// result in this census.  Reconstructed source is unchanged and no generated
// TU-state declarations or old OD_STEER expressions were retained.

void DoBlast_for_family_excerpt(
    i32& segment_h,
    i32 segmentCount_f,
    float& currentX_i,
    float stepX_a,
    float& currentY_d,
    float stepY_e
) {
    for (segment_h = 0; segmentCount_f > segment_h; ++segment_h) {
        currentX_i = currentX_i + stepX_a;
        currentY_d = currentY_d + stepY_e;
    }
}

// Rejected size-0x335 structural arm.
void DoBlast_while_family_excerpt(
    i32& segment_h,
    i32 segmentCount_f,
    float& currentX_i,
    float stepX_a,
    float& currentY_d,
    float stepY_e
) {
    segment_h = 0;
    while (segment_h < segmentCount_f) {
        currentX_i += stepX_a;
        currentY_d += stepY_e;
        ++segment_h;
    }
}

// Rejected size-growing inline family.
inline float AdvanceBlastCoordinate_attempt(float current, float step) {
    return current + step;
}

inline i32 BlastDistanceSquared_attempt(i32 deltaX, i32 deltaY) {
    return deltaX * deltaX + deltaY * deltaY;
}

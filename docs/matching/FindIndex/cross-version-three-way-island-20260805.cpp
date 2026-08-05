/*
 * Cross-version reconstruction for FindIndex, BASE/Misc RVA 0x000c4540.
 * Donor: exact Gold/Buka mainline source at VA 0x004bd9f0.
 *
 * The prior PoL source cached entries[mid].key and expressed equality as a
 * second comparison inside the <= arm. Retail reuses the flags from one
 * three-way comparison. The mainline while(1) structure reconstructs that
 * ownership and keeps the low/high terminal checks together.
 *
 * A complete two relational-ownership x (clean + 50 forest/top states)
 * matrix compiled 102/102 cells. Both operand spellings were byte-identical
 * per state. Trial 3 closed exactly for the retained mainline structure:
 *
 *   build/find-index-relational-manifest.json
 *   build/source-variant-batch/find-index-relational/results.json
 *   build/tu-state-noise/find-index-cross-version-exact
 *
 * Focused replay: seed 1213156658, trial 3,
 * tag 484f4d32-0003-acb2ec2e, source hash c6baaffbe8c9.
 * Result: 100.0000%, 149/149 bytes, exact 12/12 CFG, complete ordered 4/4
 * relocations. Audited exact MAX banked; generated declarations discarded.
 */

i32 FindIndex(indexArray* entries, i32 low, i32 high, i32 key) {
    giFindMid = (low + high) >> 1;
    while (1) {
        if (high - low > 1) {
            if (key < entries[giFindMid].key)
                high = giFindMid;
            else if (key > entries[giFindMid].key)
                low = giFindMid;
            else
                return entries[giFindMid].value;
        } else {
            if (key == entries[low].key)
                return entries[low].value;
            if (key == entries[high].key)
                return entries[high].value;
            return INDEX_NOT_FOUND;
        }
        giFindMid = (low + high) >> 1;
    }
}

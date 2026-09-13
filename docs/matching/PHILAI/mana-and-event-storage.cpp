// C53/S53: complete source-shape products, VC6 SP5, SOURCE/PHILAI profile.
// Artifacts: build/c53-mana-{axes,manifest}.json and -batch/results.json;
//            build/s53-events-{axes,manifest}.json and -batch/results.json.
// Commands: homm2 permute src/SOURCE/PHILAI.cpp 0x890ad (or 0x8911d)
//   --axes-from build/<name>-axes.json --min-depth 0 --max-depth 0 --limit 16
//   -o build/<name>-manifest.json --run --batch-output build/<name>-batch
// Both products complete16/16, no truncation, source restored. No TU-state
// probes requested: do not interpret these families as all-state closure.

// C53: numerator and divisor each original/explicit/implicit; result each
// original/explicit. Reject the two both-implicit arms BEFORE compilation,
// because integer division changes semantics. The remaining16 arms cover
// all credible choices in this family. Replacement expressions:
// numerator: (float)deficit | static_cast<float>(deficit) | deficit
// divisor:   (float)sp      | static_cast<float>(sp)      | sp
// result:    (i32)(deficit * 5 * frac)
//          | static_cast<i32>(deficit * 5 * frac)
// Always retain outer parentheses around divisor; no reassociation axis.
// Twelve112-byte arms strict exact; four implicit-divisor arms110 bytes fail.
// Retained full body (one cast removed, two modernized):
i32 philAI::ManaRefreshValue(hero* h, i32 level) {
    i32 sp;
    i32 v;
    float frac;
    i32 deficit;

    v = 0;
    sp = h->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE * level;
    deficit = sp - h->m_spellPoints;
    if (deficit <= 0)
        return 0;
    /* Parenthesised divisor cast: see the same idiom in ValueOfBuyingCreature. */
    frac = deficit / (static_cast<float>(sp));
    if (deficit > 0)
        v = static_cast<i32>(deficit * 5 * frac);
    return v;
}

// S53: each exact declaration below is independently kept or removed in the
// unchanged full ValueOfEventAtPosition body, giving2^4=16 structural versions.
// The axes operate on these unique declaration lines, not brace-scanning or
// reconstructed excerpts of the long switch. No other source change per arm.
// No removal retained: every one reduces size from retail6578 to6457..6554.
// All343 relocation counts agree, but ordered sites/hashes change. Baseline
// generic exact=false includes39 unresolved private constants; embedded switch
// table decoding affects its late block diagnostic. Full native restoration
// passes; see C53-S53-B72.md for the complete16-row census and proof limits.
    i32 H2_UNUSED(artifactState)[3];
    i32 H2_UNUSED(cellState)[3];
    i32 H2_UNUSED(purchaseState)[3];
    i32 H2_UNUSED(townState)[9];

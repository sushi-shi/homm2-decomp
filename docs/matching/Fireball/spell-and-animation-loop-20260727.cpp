// Fireball spell ownership and animation-loop census, 2026-07-27.
//
// Target: SOURCE/SPELLS, RVA 0x00023762, retail size 0x623.
// Matrix: /tmp/fireball-spell-loop-manifest.json
// Artifacts:
//   build/match-variants/fireball-spell-loop
//   build/tu-state-noise/fireball-spell-trial3-record
//
// Fresh evidence found two divergences in an otherwise aligned 60-block CFG:
//
// 1. Retail loads the Fireball function's `spell` parameter and pushes it into
//    ModifyDamageForArtifacts.  The current reconstruction instead pushed the
//    immediate SPELL_MAGIC_ARROW.  The last exact reconstruction also passed
//    `spell`, so the constant was a semantic regression and is restored here.
//
// 2. Retail loads frameCount_i and compares frame_i against it.  The natural
//    `frame_i < frameCount_i` loop spelling records that operand ownership.
//
// The complete matrix covered:
//   2 loop spellings x 2 spell-argument owners x (clean + 50 forest states)
//   = 204 compiled variants.
//
// Restoring `spell` raises the clean result from 99.604260% at size 0x621 to
// 99.853080% at retail size 0x623.  Multiple combined-matrix states produced
// 100.000000% bytes and all 60 retail blocks.  The combined runner reported a
// relocation-gate mismatch, so its result was not accepted directly.
//
// After making `spell` canonical, standalone tu_state_noise replayed forest
// trial 3 (seed 0x4642524c) against the unchanged target source.  That focused
// replay passed every closure gate: 100.000000%, size 0x623, and the complete
// ordered 51/51 relocation stream.  The official --record-max path raised the
// current hash's MAX from 99.8531% to 100.0000%.  No generated state
// declarations are retained.

void Fireball_spell_and_loop_excerpt(
    SpellType spell,
    i32& frame_i,
    i32 frameCount_i,
    i32l& baseDamage_w,
    hero* caster,
    hero* defender
) {
    for (frame_i = 0; frame_i < frameCount_i; ++frame_i) {
        // Animation body omitted.
    }

    ModifyDamageForArtifacts(
        &baseDamage_w,
        spell,
        caster,
        defender
    );
}

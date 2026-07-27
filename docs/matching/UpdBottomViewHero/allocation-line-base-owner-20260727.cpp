/*
 * UpdBottomViewHero allocation line-base ownership, 2026-07-27.
 *
 * The live function already defined the retail-addressed local static:
 *
 *     DATA(0x004f6300) static i16 s_bottomHeroLineBase =
 *         BOTTOM_HERO_LINE_BASE;
 *
 * but the army-count allocation metadata bypassed it and folded the enum
 * constant directly.  Retail instead sign-extends the i16 static, adds the
 * allocation-line offset, and passes the result to BaseAlloc.
 *
 * A reviewed two-arm exact-span matrix crossed the folded enum constant and
 * the local-static owner with clean plus 50 forest states: 102/102 cells
 * completed.  The local-static arm is exact in the clean state and every
 * probed state: 100.000000%, retail size 1411, 48/48 blocks, and 36/36
 * ordered relocations.  The enum arm remains 99.438354%, size 1405, and
 * lacks the retail DIR32 relocation to s_bottomHeroLineBase.
 *
 * Artifacts:
 *   build/upd-bottom-view-hero-line-base-manifest-20260727.json
 *   build/match-variants/upd-bottom-view-hero-line-base-20260727
 *
 * Disposition: retain the proven local-static source owner.  No generated
 * declarations or compiler-state steering are retained.
 */

void UpdBottomViewHero_allocation_line_base_owner_record() {
}

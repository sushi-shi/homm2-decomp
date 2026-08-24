// hero::HeroScreenUpdate (PoL RVA 0x6cb33; Buka RVA 0x6148c)
//
// Artifact:
//   build/source-variant-batch/hero-screen-update/results.json
// Complete 4 source arms x (clean + 50 forest/top states), 204/204. Seed
// 1213156658; source restored; wall-time and generator untruncated.
//
// Buka's `giHeroScreenSrcIndex == index` arm has four exact islands; the PoL
// orientation has none. Adopt the Buka comparison. `index -> i` never closes:
// clean 97.045456%, best 99.750000%. Keep the PoL slot-sensitive name.
// `/Od` frame audit: 1/1 aligned.
//
// Forest/top trial 1 is an audited 168-byte closure with exact 9/9 CFG and 8/8
// ordered relocations. Clean live state remains 97.272730%; hash MAX is 100.

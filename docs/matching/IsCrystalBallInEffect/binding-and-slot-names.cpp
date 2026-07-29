// IsCrystalBallInEffect (SOURCE/ADVMGR, RVA 0x6c241) — 2026-07-29 matrix -> MAX 100
//
// Matrix: build/icbie-axes.json -> build/source-variant-batch/icbie
// (6 arms x {clean + 8 forest states}, complete).
//
// Axis hero_binding: gpGame->GetHero(...) inline (was in tree) leaves the /Ob1
// continuation stub, 5 bytes long. Direct &gpGame->m_heroRecs[...] splice
// (WINNER) matches retail.
//
// Axis distance_name (od_slots solve): retail walk order is crystalHero(-0x4),
// heroIndex(-0x8), distance-slot(-0xc), requiring bucket(crystalHero=3) <=
// bucket(heroIndex=5) < bucket(third local); `distance` (bucket 2) violates
// it, `range` (9) and `crystalDistance` (6) both satisfy it and both reached
// audited exact — `range` kept as the natural 1996 spelling.
//
// Banked MAX 98.8923 -> 100.0000 (build/tu-state-noise/advmgr-icbie).

// ValueOfBuyingCreature (SOURCE/PHILAI, RVA 0x3d852) — 2026-07-29 matrix -> EXACT
//
// Matrix: build/vobc-axes.json -> build/source-variant-batch/vobc
// (9 arms x {clean + 12 forest states}, complete).
//
// Axis hero_binding (the extra retail jmp after `cmp eax,-1; je`):
//   direct_splice (was in tree): &gpGame->m_heroRecs[townPtr->m_occupyingHeroId]
//                 -> no /Ob1 continuation stub, 4 bytes short. REJECTED.
//   gethero_member (WINNER): gpGame->GetHero(townPtr->m_occupyingHeroId)
//   getheroslot_local: file-local GetHeroSlot(...) — byte-identical to the
//                 member inline here; member form kept for consistency with
//                 EvaluateHeroEvent.
//
// Axis risk_chain (fst/fstp + fmulp placement):
//   risk_first (was in tree): riskFactor * riskFactor * riskFactor * creatureValue
//                 -> compiler hoisted creatureValue to the front (fild first,
//                 three fmul-by-temp). REJECTED.
//   risk_cubed_paren (WINNER): (riskFactor * riskFactor * riskFactor) * creatureValue
//                 -> keeps risk-cubed as the left subtree: fst keeps the
//                 assignment on the stack, two fmul-by-temp, fild, fmulp. The
//                 explicit parens defeat the chain canonicalization here.
//   value_first: creatureValue * risk^3 — REJECTED (matches our old wrong shape).
//
// Winner is exact at the CLEAN state: 99.97727 fuzzy in the matrix, with the
// only remaining row the delink attribution `iLeftRightSave+0x10` for retail's
// __adjust_fdiv /QIfdiv guard word; the paired normalization rewrites it once
// the surrounding bytes align, after which objdiff reports zero differing rows
// and homm2 build records 100. Reloc audit 31/31.

// DetermineHeroToMove (SOURCE/PHILAI, RVA 0x3b748) — 2026-07-29 matrices
//
// Matrices: build/dhtm-axes.json + build/dhtm2-axes.json ->
// build/source-variant-batch/dhtm{,2} (4 arms x {clean + 12 forest states} each;
// method note: these two should have been ONE matrix with all dialect options).
//
// Both GetPlayerHero call sites (loop mobility read, final return):
//   gpGame->GetPlayerHero(player, i)      -> inline stub + single-register *283
//     factorization, size 295 (10 long). REJECTED despite highest fuzzy (93.08):
//     wrong size can never close.
//   ...m_players[player].HeroId(i)...     -> byte-identical to GetPlayerHero
//     (stub returns). REJECTED.
//   gpGame->m_heroRecs[gpGame->m_players[player].m_heroIds[i]]  (WINNER, both
//     sites): no stub, retail size 285, block topology 11/11 exact with zero
//     edge/pred/flow/shift deltas, first-divergence=none.
//
// Residual: register-color churn in the *283 player-record factorization
// (retail: eax=i first, (i-p)+4*(71p) fold via ecx/edx; ours: p-chain in eax)
// plus one val>jb loop-guard mirror — at-state class, island search territory.
// Fuzzy dropped 92.3 -> 89.9 on integration; that is the size-fix shift
// penalty, not a regression (judge by topology, per AGENTS.md step 7).

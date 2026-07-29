// army::MoveAttack (SOURCE/ARMY, RVA 0x52ad9) — 2026-07-29 matrices
//
// Matrix 1: build/moveattack-axes.json — entry_loop.
// goto_again (WINNER, integrated): the entry retry is a backward `again:`
// goto, not while(1)/continue/break; kills the dead loop-back jmp and the
// break jmp (1044 -> 1034 bytes, 96.92 -> 97.69).
//
// Matrix 2: build/moveattack2-axes.json — move_guard split. REJECTED:
// splitting the occupant `||` into two `goto move` guards changes nothing
// (both arms byte-equal or worse at every probe state).
//
// OPEN: five extra jmps remain vs retail 1009 bytes — the `goto move` hop
// pair at the occupant condition, a dead join after the DoAttack else-chain,
// and two tail jmps near `finish:`. Needs a --blocks --dot comparison of the
// label/fallthrough plan; suspect the attack chain and the move/finish code
// ORDER differs from ours (body order vs case values, AGENTS.md step 4).

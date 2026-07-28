// GetBestHero (SOURCE/PHILAI, RVA 0x3e2a8) — 2026-07-29 matrix
//
// Matrix: build/getbesthero-axes.json -> build/source-variant-batch/getbesthero
// (6 source arms x {clean + 16 forest states}, complete, not truncated).
//
// Axis cell_access:
//   natural_index   (was in tree): m_worldMap.cells[width * y + x].m_triggerType
//                   -> size 421 (12 short), combined-index scaling, no stub. REJECTED.
//   getcell_inline  (WINNER): m_worldMap.GetCell(x, y)->m_triggerType
//                   -> size 433 = retail, per-term scaled addressing from the
//                   inline body (cells + width * y + x) plus the /Ob1
//                   continuation stub. Clean 98.97, forest best 99.9587 with
//                   16/16 blocks and zero topology deltas.
//   pointer_splice: (cells + width * y + x)->m_triggerType -> size 428: per-term
//                   scaling but no stub. REJECTED.
//
// Axis score_compare:
//   assign_then_if (kept): randomizedScore = ...; if (randomizedScore > bestScore)
//   assign_in_condition:   if ((randomizedScore = ...) > bestScore)
//   Byte-identical at every tested state -> canonicalized; kept the plain form.
//
// Residual after integration: one fst/fcomp vs fcom/fstp scheduling pair at the
// score compare; the matrix shows forest states flip it (99.9587 observed).
// Island search on the integrated source is the remaining lever.

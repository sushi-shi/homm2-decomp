// combatManager::RightClick (SOURCE/COMMAND, RVA 0x2ce19) — 2026-07-29, OPEN
//
// Matrix: build/rclick-axes.json -> build/source-variant-batch/rclick
// (8 arms x {clean + 8 forest states}, complete). No winner: best arm
// early_return+return0 scored 95.80 with 38/41 blocks and 17 edge deltas;
// the tree's wrapper-if shape stayed at 97.20/656 bytes (retail 678).
//
// Evidence so far: retail's `je` for the side==COMBAT_SIDE_NONE test lands on
// its own 7-byte xor/jmp return-0 stub (guard-clause with private return), and
// retail carries extra `jmp` blocks after the ballista and inner-switch tails
// (five DELETE rows, 22 missing bytes). The tested axes (side guard shape,
// ballista/inner break-vs-return) do not by themselves reproduce that layout.
//
// UNTRIED: enumerate the outer switch's case BODY ORDER vs case values
// (AGENTS.md step 4), the default-case modulo guard spelling
// (col == ROW_LENGTH-1 precomputed vs inline `hexIndex % ...`), and the giant
// blocked-hex || chain's term grouping. Rerun as one combined matrix.
//
// 2026-07-29 second pass (build/rclick2-axes.json): adding dead `break;`
// after the four case `return 0;` sites fixes the BLOCK COUNT exactly
// (41/41) but the flow still diverges at B6 with ~50 edge deltas and the
// shifted layout drops fuzzy to 95.03 — the early case-dispatch/guard
// structure is still wrong, so the breaks alone were not integrated.
// The dead-break hypothesis is half-right; solve B6 first, then re-add.

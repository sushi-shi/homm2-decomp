// DimensionDoorHandler (SOURCE/ADVMGR, RVA 0x65191) — 2026-07-29 matrix, OPEN
//
// Matrix: build/ddh-axes.json -> build/source-variant-batch/ddh
// (8 arms x {clean + 8 forest states}, complete). Best 96.66, size stuck at
// 789 vs retail 796 in every arm; topology first-divergence at B2
// (target_shift), 52/54 blocks with large edge deltas.
//
// Measured, byte-pinned observations that did NOT close alone:
//   coords screen_xy: retail reads payload.mouse.screenX/screenY (+0x10/+0x14)
//     where the tree reads mouse.x/y (+0x4/+0x8). Compiles to the retail
//     operands but moves fuzzy < 0.1% because the residual mass is elsewhere.
//   select_case break_ shape: adding the empty-then break jmp did not
//     reproduce retail's triple jmp cluster.
//   handled -> done rename (bucket 3 -> 7, fixes the mouseY/handled walk-order
//     tie): marginal fuzzy change only.
//
// Conclusion: the 7-byte deficit plus ~20 ARG rows need a full CFG/frame
// dossier (blocks --dot, slot bijection over ALL locals including the switch
// temps at -0x20). UNTRIED: complete od_slots solve; switch dispatch order
// comparison; a combined matrix rerun once the frame is solved (fold the three
// pinned pieces above into it).

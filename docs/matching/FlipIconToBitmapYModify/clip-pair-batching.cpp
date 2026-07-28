/*
 * FlipIconToBitmapYModify, RVA 0xd9ce0: clip-pair batching and store order.
 *
 * Hypothesis: retail's compute-compute-store-store clip sequence (clipR
 * stored before clipB) came from batched locals or a reversed statement
 * order. Four reviewed shapes (direct B;R, direct R;B, batched locals in both
 * compute orders with R-first stores) were crossed with clean plus 60
 * top-of-TU forest states, complete 244-arm product:
 *
 *   build/iconf2by-clip-pair-axes-20260728.json
 *   build/iconf2by-clip-pair-manifest-20260728.json
 *   build/source-variant-batch/iconf2by-clip-pair/
 *
 * Every alternative shape lands at 93.25% (1419 bytes, 144/144) in the
 * trial-60 orbit; only the existing direct clipB-then-clipR source retains
 * the 94.4102% island. Rejected.
 *
 * The audited-exact IconToBitmapYModify object settles the pair question by
 * symmetry: its source writes clipR;clipB and MSVC emits the stores B,R, so
 * the flipped decoder's retail store order R,B is what clipB;clipR source
 * (the current spelling) produces once the surrounding allocation matches.
 *
 * The real orbit anchor is the shear parameter's register treatment. In both
 * the exact forward object and the retail flipped bytes, shear passes
 * through a register only transiently in setup (ebp is immediately recycled
 * for clipW, enabling the 3-byte lea over two callee-saved registers) and the
 * command loop reloads shear from the stack at each row-visibility use. The
 * candidate instead pins shear in ebp for the whole function; 128 forest
 * states (seed 1213156658), 64 layered states atop trial 60, 100 seed-777
 * states, and 50 target-insertion states never flip the allocation.
 * Residual: MAX 94.4102, exact 59/59 CFG, candidate 1419 vs retail 1421.
 * UNTRIED: an evidence-backed structural parent that changes the loop's
 * row-visibility ownership without inventing steering.
 */

void FlipIconToBitmapYModify_clip_pair_batching_attempt() {
}

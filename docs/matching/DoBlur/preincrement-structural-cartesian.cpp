/*
 * DoBlur Cartesian descendants of the direct-preincrement exact-block island,
 * 2026-07-27.
 *
 * The parent publication structure is fixed in every arm:
 *
 *     output++;
 *     output[-1] = lookup[redSum & ~QUANTIZATION_MASK]
 *                        [(greenSum & ~QUANTIZATION_MASK)
 *                         + (blueSum >> QUANTIZATION_SHIFT)];
 *
 * It was crossed with three independent ownership mechanisms selected from
 * the prior high-scoring islands:
 *
 *   - acquisition-order / directional-flat spilled sample slots;
 *   - red/blue/green, red/green/blue, or blue/red/green component tables;
 *   - nested / staged byte-address implementation of BlurComponentAt.
 *
 * These mechanisms change output publication, sample storage, channel-table
 * ownership, and inlined address construction at disjoint source spans.  The
 * complete explicit product was:
 *
 *     1 * 2 * 3 * 2 * (clean + 50 top-of-TU forest states)
 *       = 612/612 compilations
 *
 * It completed in 593.24 seconds without truncation and restored source.
 *
 * Best result for every combined source structure:
 *
 *   sample slots       tables            accessor       best       size
 *   acquisition        red/blue/green    nested         93.702530  1705
 *   acquisition        red/blue/green    staged         93.358650  1707
 *   acquisition        red/green/blue    nested         93.358650  1707
 *   acquisition        red/green/blue    staged         93.483120  1707
 *   acquisition        blue/red/green    nested         93.504220  1707
 *   acquisition        blue/red/green    staged         93.282700  1707
 *   directional        red/blue/green    nested         93.710970  1705
 *   directional        red/blue/green    staged         93.367090  1707
 *   directional        red/green/blue    nested         93.371310  1707
 *   directional        red/green/blue    staged         93.491560  1707
 *   directional        blue/red/green    nested         93.516880  1707
 *   directional        blue/red/green    staged         93.295360  1707
 *
 * All successful objects retain 43/43 relocation counts.  More importantly,
 * every clean Cartesian arm has 29/29 retail instruction-count blocks and a
 * 1705-byte function.  This proves the exact-block publication mechanism can
 * coexist with directional sample storage, every selected table order, and
 * staged byte addressing.  The clean combinations are distinct target-text
 * islands except where MSVC canonicalizes two table orders.
 *
 * The strongest combined state uses directional slots, red/blue/green tables,
 * and nested byte addressing at forest trials 10 and 24.  It reaches
 * 93.710970%, size 1705, text SHA 183d9dabaf9369a3.  This is 0.008440 points
 * above the corresponding acquisition-order preincrement state, showing a
 * real but small beneficial interaction.
 *
 * The high-scoring state orbit and exact-block orbit still do not coexist:
 * each structure's best forest state has 28 instruction-count-exact blocks
 * with B10 alone size-only.  Staged byte addressing is compatible with the
 * clean exact skeleton but does not recover the stronger register schedule.
 * No arm reaches retail size 1700, exact ordered relocations, or the durable
 * live-source 95.729960% MAX.
 *
 * Artifacts:
 *   build/doblur-preincrement-structural-cartesian-axes-20260727.json
 *   build/doblur-preincrement-structural-cartesian-manifest-20260727.json
 *   build/match-variants/doblur-preincrement-structural-cartesian-20260727
 *
 * Disposition: preserve the complete Cartesian result as combined structural
 * evidence, retain the live source, and make no generated configuration or
 * MAX change.
 */

void DoBlur_preincrement_structural_cartesian_record() {
}

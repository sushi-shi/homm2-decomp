/*
 * DoBlur whole-channel accumulator ownership on the retained alternate
 * sample partition and red/blue/green component-table declaration parent.
 *
 * This pass targets the retained 95.729960% island's only block-size
 * divergence: candidate B10 has 217 instructions while retail has 216.
 * Earlier accumulator matrices were either based on the lower exact-frame
 * branch, wrapped red alone, or used a temporary effective source.  They did
 * not cover all three channel owners on the current retained source hash.
 *
 * Four reviewed structural versions each received the clean state plus the
 * same 50-state declaration forest, for 4 * 51 = 204/204 compilations:
 *
 * 1. Three table-owning accumulator objects:
 *
 *      class BlurComponentAccumulator {
 *      public:
 *          void Init(BlurComponentTable& table, i32 first, i32 second);
 *          void Add(i32 offset);
 *          u32 Value() const;
 *      private:
 *          BlurComponentTable* m_table;
 *          u32 m_value;
 *      };
 *
 *      BlurComponentAccumulator redSum;
 *      redSum.Init(redComponents, sample13, samples[SOUTH_ONE_SAMPLE]);
 *      redSum.Add(samples[SOUTH_TWO_SAMPLE]);
 *      // ...retained term order...
 *
 *    Best: 95.563290%, size 1703, trial 7.
 *
 * 2. One table-owning accumulator reused for red, green, and blue, copying
 *    each completed Value() to a scalar before Init() selects the next table.
 *
 *    Best: 95.563290%, size 1703, trial 7.
 *
 * 3. Three one-word value-wrapper accumulator objects with an inlined
 *    converting constructor, operator+=, and u32 conversion.
 *
 *    Best: 95.341774%, size 1703, trial 38.
 *
 * 4. One indexed `u32 componentSums[3]` owner with the existing red, green,
 *    and blue expression statements otherwise unchanged.
 *
 *    Best: 95.563290%, size 1703, trial 31.
 *
 * The separate table owners, reused table owner, and indexed array converge
 * on identical target text SHA 0a540af53da4975f despite their different
 * source and TU states.  This is a real lower structural island rather than
 * an accidental score tie.
 *
 * The retained paired object confirms the same 29-block CFG and branch
 * skeleton as retail: 28 blocks have equal instruction counts and B10 alone
 * remains size-only at 217 candidate instructions versus 216 retail.  Thus
 * whole-channel ownership reorganizes B10 and reaches a strong distinct
 * state, but it does not remove the final instruction.
 *
 * Artifacts:
 *   build/doblur-rbg-three-component-accumulators-manifest.json
 *   build/match-variants/doblur-rbg-three-component-accumulators
 *   build/doblur-rbg-reused-component-accumulator-manifest.json
 *   build/match-variants/doblur-rbg-reused-component-accumulator
 *   build/doblur-rbg-channel-value-accumulators-manifest.json
 *   build/match-variants/doblur-rbg-channel-value-accumulators
 *   build/doblur-rbg-indexed-channel-accumulators-manifest.json
 *   build/match-variants/doblur-rbg-indexed-channel-accumulators
 *   build/tu-state-noise/doblur-rbg-component-accumulator-trial7
 *
 * Disposition: preserve the 95.563290% object as a lower structural island.
 * No authored accumulator form closes B10, the durable 95.729960% MAX remains
 * unchanged, and no experimental source or generated TU-state input is
 * retained.
 */

void DoBlur_rbg_whole_channel_accumulator_ownership_record() {
}

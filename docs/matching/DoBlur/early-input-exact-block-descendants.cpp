/*
 * DoBlur descendants of the early-input, retail-frame sample-partition island.
 *
 * Base shape:
 *
 *     i32 samples[13];                    // north-four is spilled
 *     u8* input = ...;
 *     i32 sample15, sample14, sample13;   // west-four is sample13
 *
 * with blue/red/green table declarations.  Forest trial 50 reaches only
 * 92.909290% and size 1701, but its CFG has 29/29 blocks and every
 * index-aligned block has the retail instruction count and branch targets.
 * In particular B10 is 216 instructions on both sides.  The candidate still
 * differs substantially in register allocation and instruction order, and
 * the complete ordered relocation streams do not match, so this is clue
 * evidence rather than closure.
 *
 * The retained assembly comparison localizes the opcode-order differences to
 * B4 (palette fill), B5 (resource calls), B10 (channel accumulation), and B12
 * (palette allocation); every other block has the same opcode sequence.  B10
 * uses west4=EBP, north1=ECX, south4=ESI, whereas retail uses
 * west4=EBP, north1=ESI, south4=ECX.
 *
 * Three new ownership/lifetime mechanisms were tested as whole source shapes,
 * each against clean plus exactly 50 forest states:
 *
 *   component-table aggregate: 3 * 51 = 153/153
 *     direct RGB fields best:       92.978905%, size 1706
 *     RGB fields through references: 93.035866%, size 1707
 *
 *   component-sum aggregate: 3 * 51 = 153/153
 *     direct RGB fields best:       94.044304%, size 1706
 *     RGB fields through references: 76.310130%, size 1810
 *
 *   sample-scalar lifetime: 2 * 51 = 102/102
 *     row scope best:               92.909290%, size 1701
 *     pixel scope best:             94.833336%, size 1703
 *
 * The aggregate and pixel-scope arms all preserve the 29-block CFG but return
 * to 28 instruction-count-exact blocks plus one size-only block.  The direct
 * sum aggregate and pixel-scope samples are nevertheless genuine new byte
 * islands, demonstrating that their low clean scores are not grounds for
 * pruning them.
 *
 * Artifacts:
 *   build/tu-state-noise/doblur-early-input-brg-exact-trial50
 *   build/doblur-early-input-exact-table-aggregate-manifest.json
 *   build/match-variants/doblur-early-input-exact-table-aggregate
 *   build/doblur-early-input-exact-sum-aggregate-manifest.json
 *   build/match-variants/doblur-early-input-exact-sum-aggregate
 *   build/doblur-early-input-exact-inner-scalar-lifetime-manifest.json
 *   build/match-variants/doblur-early-input-exact-inner-scalar-lifetime
 *   docs/matching/DoBlur/early-input-exact-hybrid-cartesian.cpp
 *
 * Disposition: preserve every island as structural evidence.  None is exact
 * and none advances the durable 95.729960% MAX.
 */

void DoBlur_early_input_exact_block_descendants_record() {
}

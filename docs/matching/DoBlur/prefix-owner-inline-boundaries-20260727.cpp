/*
 * DoBlur prefix-owner and palette-fill inline-boundary experiments,
 * 2026-07-27.
 *
 * These three reviewed structures target the first real divergence, directly
 * after RGBLOOKP.BIN allocation and in the component-table fill loop.  Each
 * was tested as a complete clean-plus-50 declaration-forest census.  Source
 * was restored after every census and generated declarations were not kept.
 *
 * 1. Give the lookup pointer a semantic one-field workspace owner:
 *
 *      struct SBlurLookupWorkspace {
 *          BlurLookupRow* rows;
 *      } lookupWorkspace;
 *
 *    All allocation, ReadBlock, quantized lookup, and free sites directly
 *    named lookupWorkspace.rows.  The clean state was 92.987340%, size 1705;
 *    trial 35 reached 95.563290%, size 1703.  The 29-block CFG and 43
 *    relocation count remained intact, but the structure did not close the
 *    allocation/table-fill prefix or the size-only pixel block.
 *
 * 2. Extract the three per-color table stores into an inline function taking
 *    the red/blue/green tables, byte offset, and palette cursor by value.  It
 *    was semantically inlined in every retained object.  Clean reached
 *    93.274260%, size 1708; trial 49 reached 95.341774%, size 1703.  This is a
 *    lower compiler-state orbit, not the missing original inline boundary.
 *
 * 3. Materialize one reused u32 paletteComponent inside the fill loop before
 *    each red, green, and blue store.  The optimizer removes the source
 *    temporary, but it was still swept because declaration ownership can
 *    affect VC4.2 allocation.  Clean reached 92.632910%, size 1701; trial 5
 *    reached 94.719406%, size 1701.  It did not improve the prefix schedule.
 *
 * Artifacts:
 *   build/tu-state-noise/doblur-lookup-workspace-20260727
 *   build/doblur-lookup-workspace-states-20260727.json
 *   build/tu-state-noise/doblur-inline-palette-fill-20260727
 *   build/doblur-inline-palette-fill-states-20260727.json
 *   build/tu-state-noise/doblur-palette-component-owner-20260727
 *   build/doblur-palette-component-owner-states-20260727.json
 *
 * Disposition: preserve the paired clue artifacts but retain canonical source.
 * None exceeds the durable 95.738396% structural-island evidence or yields
 * exact target size, bytes, and ordered relocations.
 */

void DoBlur_prefix_owner_inline_boundaries_record() {
}

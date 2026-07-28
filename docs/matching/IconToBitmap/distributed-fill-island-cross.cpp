/*
 * Shared forward clipped-fill ownership cross, 2026-07-27.
 *
 * The shared-count form selected the left-clipped count in two branches, then
 * called one common memset. Retail instead duplicates the memset setup in both
 * branches and converges only at the final rep stosb:
 */
#if 0
if (s_clipR >= fillRight)
    memset(row + clipX, s_color, (count - clipX) + X);
else
    memset(row + clipX, s_color, clipW);
#endif

/*
 * Both structural arms were crossed against clean plus the same 50
 * declaration-forest states with independent fuzzy and canonical-CFG ranking:
 *
 *   build/match-variants/icon2b-fill-structure-dual-target-20260727/
 *   build/match-variants/icon2bc-fill-structure-dual-target-20260727/
 *
 * IconToBitmap:
 *
 *   shared-count clean: 74.239010%, 80/80 exact graph, 83/83 relocs;
 *   distributed clean:  70.156590%, 79/80 divergent graph, 84/83 relocs;
 *   distributed trial 2: 75.173080%, 80/80 exact graph, 83/83 relocs.
 *
 * IconToBitmapColorTable:
 *
 *   shared-count clean: 75.960090%, 87/86 divergent graph, 91/91 relocs;
 *   distributed clean:  79.957750%, 86/86 exact graph, 92/91 relocs;
 *   distributed trial 11: 80.469480%, 86/86 exact graph, 92/91 relocs;
 *   distributed trial 49: 80.328636%, 86/86 exact graph, 91/91 relocs.
 *
 * Canonical graph equality includes labeled successors and predecessors; all
 * exact-graph islands have zero labeled-edge and predecessor delta.
 *
 * Disposition: retain the distributed memset ownership as the one shared
 * developer-level macro structure. Its clean raw-copy object occupies a lower
 * structural orbit, but the complete state product proves an exact retail CFG
 * island for both consumers. Fuzzy score and clean block count do not justify
 * pruning it.
 */

void IconToBitmap_distributed_fill_island_cross_attempt() {
}

/*
 * DoBlur component-table workspace aggregate, 2026-07-27.
 *
 * The retained 95.729960% island has a 0xc60 frame and an unused four-byte
 * allocator hole before its three contiguous component tables.  Retail has a
 * 0xc5c frame and packs the tables four bytes earlier.  Earlier work tested a
 * component-table aggregate only on a lower retail-frame sample partition;
 * it had not been crossed onto the retained red/blue/green parent or the
 * direct-preincrement exact-block publication structure.
 *
 * This matrix compared separate tables with a single local workspace in three
 * physical field orders:
 */

#if 0
struct SBlurComponentWorkspace {
    BlurComponentTable red;
    BlurComponentTable blue;
    BlurComponentTable green;
} componentWorkspace;

// All component accesses in this arm directly name componentWorkspace.red,
// componentWorkspace.blue, or componentWorkspace.green.  Red/green/blue and
// blue/red/green physical field orders were tested as separate arms.
#endif

/*
 * Each of the four ownership arms was crossed with direct post-increment and
 * direct pre-increment output publication, then the clean state plus 50
 * top-of-TU declaration-forest states:
 *
 *   4 * 2 * 51 = 408/408 cells in 373.21 seconds, untruncated.
 *
 *   owner                       publication       clean       best
 *   separate red/blue/green     post-increment    92.854430   95.729960
 *   separate red/blue/green     pre-increment     91.957810   93.702530
 *   aggregate, any field order  post-increment    93.386080   95.236290
 *   aggregate, any field order  pre-increment     91.759490   93.407170
 *
 * Every result has 43/43 relocation counts and the retail 29-block flow.
 * Separate/pre-increment alone preserves all 29 clean instruction counts.
 * Every aggregate arm has 28 exact block counts plus B10 size-only, including
 * its clean states.  Aggregate field order changes the target text SHA while
 * remaining score-equivalent, so these are real compiler islands rather than
 * an inert spelling; nevertheless none repairs the frame/block residual or
 * exceeds the durable MAX.
 *
 * Artifacts:
 *   build/doblur-component-workspace-axes-20260727.json
 *   build/doblur-component-workspace-manifest-20260727.json
 *   build/match-variants/doblur-component-workspace-20260727
 *
 * Disposition: preserve the aggregate family as lower structural evidence.
 * Retain the separate component tables and direct post-increment source; no
 * generated declarations, configuration changes, or MAX changes survive.
 */

void DoBlur_component_workspace_aggregate_record() {
}

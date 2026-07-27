/*
 * DoBlur structural descendants targeting the audited input-register and
 * green-accumulation mismatch of the 95.213080% exact-frame hybrid.
 *
 * The starting disposable structure used the retail-frame sample partition,
 * early input lifetime, pixel-scope special samples, one RGB component-sum
 * record, and red/green/blue component tables.  Its trial-25 object had
 * input=ESI rather than retail EDX and B10 had 217 rather than 216
 * instructions, with the extra move at the start of green accumulation.
 *
 * Pixel-loop ownership matrix:
 *   - row-owned do loop;
 *   - for-owned count;
 *   - for-owned input with body increment;
 *   - for-owned input with update-expression increment;
 *   - for-owned input/output/count cursors;
 *   - an explicit nested pixel-value scope.
 *
 * Every shape received clean plus 50 forest states: 6 * 51 = 306/306.
 * The count-owned loop, all-cursor loop, and nested scope canonicalized to the
 * original 95.213080% trial-25 object.  The input-owned loops topped out at
 * 94.200424%.  No loop spelling repaired input ownership.
 *
 * The next Cartesian matrix crossed row/pixel lifetime for the 13-element
 * spilled sample array with a rolling input pointer, a per-iteration snapshot,
 * and a const snapshot: 2 * 3 * 51 = 306/306.
 *
 *   pixel array + rolling input: 95.219406%, size 1703, trial 47
 *   pixel array + snapshots:    95.208860%, size 1703, trial 47
 *   row array + rolling input:  95.213080%, size 1703
 *   row array + snapshots:      94.991560%, size 1703
 *
 * This is a genuine new target-byte island, but its concrete B10 ownership is
 * still input=ESI with west4/north1/south4 in EBP/ECX/EDX.  Retail uses
 * input=EDX and EBP/ESI/ECX.  B10 remains 217 versus retail 216.
 *
 * Grouping input, output, and remaining in a local cursor record was then
 * tested with assignment and aggregate initialization.  The complete
 * 3 * 51 = 153/153 matrix topped out at 94.689870% for either record spelling;
 * the separate locals retained 95.219406%.
 *
 * Finally, the newly proven pixel-array lifetime was crossed back onto the
 * historical retained red/blue/green alternate-partition parent, together
 * with retained versus early input lifetime: 2 * 2 * 51 = 204/204.
 *
 *   row array + retained input: 95.729960%, trial 10
 *   row array + early input:    95.729960%, trial 13
 *   pixel array + either input: 94.719406%, trial 32
 *
 * Both pixel-array arms compile to text SHA ec1611167d6340eb, the known
 * early-input-clean state.  Direct comparison shows input=ECX, special samples
 * EBP/ESI/EDX, B10 215 versus retail 216, and stack-table bases shifted by four
 * bytes.  Thus pixel-array lifetime is independent and helpful on the RGB
 * sum-record descendant, but conflicts with rather than augments the
 * 95.729960% RBG parent.
 *
 * A stack-use histogram clarified that shift.  Retail and candidate each have
 * 24 distinct non-table stack offsets, so there is no extra accessed runtime
 * value.  The candidate alone leaves 0x34 unused, then uses 0x6c and begins its
 * tables at 0x70; retail packs continuously through 0x68 and begins tables at
 * 0x6c.  The extra four bytes are an allocator hole/name-sensitive reserved
 * owner, not a missing semantic local.
 *
 * The first legitimate owner-removal test nested the dead-after-call
 * `lookupId` result directly into `PointToFile` while retaining a named
 * `resourceMgr`.  On the RBG pixel-array shape, both forms received clean plus
 * 50 states: 2 * 51 = 102/102.
 *
 *   nested call result: 95.552740%, size 1703, trial 27
 *   named lookupId:     94.719406%, size 1701, trial 32
 *
 * The merge opens a materially better orbit, but the retained trial-27 object
 * still has frame 0xC60, the unused 0x34 hole, tables at 0x70, input=ECX, and
 * B10 217 versus retail's frame 0xC5C, tables at 0x6c, input=EDX, and B10 216.
 * It changes scheduling without closing the slot-layout residual.
 *
 * Finally, `od_slots.py` classified the inner array name `samples` in bucket 7.
 * Four meaningful compiler-visible aliases spanning buckets 0, 4, 6, and 8
 * were tested with it as one permuter axis, each against clean plus 50 states:
 * 5 * 51 = 255/255.  Every name is byte-identical within its paired TU state;
 * optimized DoBlur does not expose this `/Od` name-bucket lever.
 *
 * Artifacts:
 *   build/doblur-9513-loop-ownership-manifest.json
 *   build/match-variants/doblur-9513-loop-ownership
 *   build/doblur-9513-sample-input-lifetime-manifest.json
 *   build/match-variants/doblur-9513-sample-input-lifetime
 *   build/tu-state-noise/doblur-9513-pixel-samples-trial47
 *   build/doblur-95219-cursor-owner-manifest.json
 *   build/match-variants/doblur-95219-cursor-owner
 *   build/doblur-rbg-parent-sample-input-lifetime-manifest.json
 *   build/match-variants/doblur-rbg-parent-sample-input-lifetime
 *   build/tu-state-noise/doblur-rbg-parent-pixel-samples-trial32
 *   build/doblur-rbg-pixel-samples-lookup-owner-manifest.json
 *   build/match-variants/doblur-rbg-pixel-samples-lookup-owner
 *   build/tu-state-noise/doblur-rbg-pixel-samples-nested-lookup-trial27
 *   build/doblur-rbg-pixel-nested-lookup-sample-name-buckets-manifest.json
 *   build/match-variants/doblur-rbg-pixel-nested-lookup-sample-name-buckets
 *
 * Disposition: preserve 95.219406% and the nested-lookup 95.552740% state as
 * lower structural islands.  No shape is exact, the durable 95.729960% MAX
 * remains unchanged, and no experimental source or generated TU-state input
 * is retained.
 */

void DoBlur_pixel_sample_and_loop_ownership_record() {
}

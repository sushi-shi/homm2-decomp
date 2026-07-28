/*
 * FlipIconToBitmap, RVA 0xd1ba0: clipped-literal local-join recovery.
 *
 * Matrix:
 *   build/iconf2b-literal-join-axes-20260727.json
 *   build/source-variant-batch/iconf2b-literal-join-clean-20260727/
 *
 * The original reconstruction used two small labels (`set_skip` and
 * `skip_set`) solely to publish s_srcSkip or join the copy loop.  Both a scoped
 * `do { ... } while (0)` with `break` and the corresponding `continue` form
 * compiled byte-identically to those labels: 81.090190%, 76/76 blocks, with
 * identical relocations.  The `break` form was retained because it matches the
 * proven FlipIconToBitmapYModify family spelling and is credible expansion from
 * a multiline decoder macro.  Larger semantic dispatch gotos were not changed.
 */

void FlipIconToBitmap_literal_join_do_loop_attempt() {
}

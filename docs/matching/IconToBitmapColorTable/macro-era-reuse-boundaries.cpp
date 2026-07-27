/*
 * IconToBitmapColorTable, RVA 0xd32a0:
 * plausible 1995-era inline and multiline-macro reuse boundaries.
 *
 * The retained source has two small inline helpers and a do/break literal-run
 * clipping diamond.  Since do { ... } while (0) is the conventional safe
 * wrapper for a multiline macro, each boundary was tested as direct text,
 * parameterized macro text, local-name macro text, and (for the helpers)
 * parameterized inline code.
 *
 * Helper Cartesian matrix:
 *
 *   4 ColorTableDimPixel shapes
 *     x 4 ColorTableOutsideSource shapes
 *     x (clean + 50 declaration-forest states)
 *     = 816/816 compiled, 246 normalized object states.
 *
 * Every best object retained size 1430, 89/91 ordered relocations, exact
 * 86-block flow, and 67 exact plus 19 size-only blocks.  Grouped maxima:
 *
 *   dim inline x outside inline                         83.983570%
 *   dim inline x outside direct/either macro            83.866196%
 *   dim direct x outside inline                         83.737090%
 *   either dim macro x outside inline                   83.431920%
 *   dim direct/either macro x outside direct/either     83.352110%
 *
 * Direct text and both macro spellings of the outside-source expression were
 * codegen/state-equivalent.  The retained outside-source inline alone selects
 * the highest observed orbit.  The retained dim inline is likewise stronger
 * than its direct and macro forms.  Preserve both inline helpers.
 *
 * Artifacts:
 *   build/icon2bc-helper-macro-era-axes.json
 *   build/icon2bc-helper-macro-era-clean.json
 *   build/match-variants/icon2bc-helper-macro-era-clean/
 *   build/icon2bc-helper-macro-era-states.json
 *   build/match-variants/icon2bc-helper-macro-era-states/
 *
 * The complete literal clipping/publication run was then replaced by the
 * representative macro invocation below.  Clean plus all 50 forest states
 * compiled 51/51.  For every corresponding state, the invocation and the
 * expanded retained do/break body had identical text hashes, normalized
 * relocation hashes, and scores.  Trial 8 retained the 83.983570% MAX.
 *
 * Artifacts:
 *   build/icon2bc-literal-run-macro-axis.json
 *   build/icon2bc-literal-run-macro-states.json
 *   build/match-variants/icon2bc-literal-run-macro-states/
 *
 * A plausible period spelling is:
 */
#if 0
#define H2_CLIP_LITERAL_RUN(                                                \
    COUNT, DST, COPY_SRC, SRC, ROW, CUR_X, RUN, CLIP_MODE,                 \
    CLIP_X, CLIP_Y, CLIP_W, GLOBAL_DST, GLOBAL_Y, CLIP_BOTTOM,             \
    CLIP_RIGHT, OUTSIDE_SOURCE)                                             \
    do {                                                                    \
        GLOBAL_DST = DST;                                                   \
        COPY_SRC = SRC;                                                     \
        if (CLIP_MODE == ICON_DRAW_NO_CLIP) {                               \
            DST = ROW + CUR_X;                                              \
            COUNT = RUN;                                                    \
        } else {                                                            \
            if (GLOBAL_Y < CLIP_Y || CLIP_BOTTOM < GLOBAL_Y) {              \
                COUNT = 0;                                                  \
                break;                                                      \
            }                                                               \
            i32 right = CUR_X + RUN;                                        \
            if (right <= CLIP_X                                             \
                || (GLOBAL_DST = DST, CLIP_RIGHT < CUR_X)) {                \
                COUNT = 0;                                                  \
                break;                                                      \
            }                                                               \
            i32 clipRight = CLIP_RIGHT;                                     \
            if (CLIP_X <= CUR_X) {                                         \
                DST = ROW + CUR_X;                                          \
                COUNT = clipRight >= right                                 \
                    ? RUN : (clipRight - CUR_X) + 1;                        \
            } else {                                                        \
                DST = ROW + CLIP_X;                                         \
                COUNT = clipRight >= right                                 \
                    ? (RUN - CLIP_X) + CUR_X : CLIP_W;                      \
                COPY_SRC = OUTSIDE_SOURCE(SRC, CUR_X, CLIP_X);             \
            }                                                               \
        }                                                                   \
    } while (0)

H2_CLIP_LITERAL_RUN(
    cnt, savedDst, gCTSrcCopy, gCTSrc, row, X, cmd, clip,
    clipX, clipY, clipW, gCTDst, gCTY, gCTClipB, gCTClipR,
    ColorTableOutsideSource
);
#endif

/*
 * Family classification narrows the reuse claim.  A generic ColorTable-style
 * do/break transplant into IconToBitmap was decisively worse and destroyed
 * that function's exact 80-block CFG.  IconToBitmap's literal path is a
 * memcpy-shaped raw copy, while FlipIconToBitmap and
 * FlipIconToBitmapColorTable are mirrored cursor/skip pipelines.  Therefore
 * this is not evidence for a generic raw-copy clipping macro shared by every
 * icon blitter.
 *
 * The paired `fill_run`/`do_fill` and `dim_run`/`do_dim` join points across
 * Icon2b, icon2bc, Iconf2b, iconf2bc, icon2by, and iconf2by do support a
 * broader common RLE-dispatcher template or include fragment.  They do not by
 * themselves prove one literal macro body: the exact IconToBitmapYModify
 * sibling puts its dim body before the shared fill join and has no do_dim
 * transfer, while this target orders fill before dim and needs both joins.
 * Retail normalized mnemonic similarity also selects IconToBitmap (0.8289),
 * not IconToBitmapYModify (0.5239), as this function's closest forward
 * sibling.
 *
 * That whole-template surface was already structurally covered and was not
 * replayed:
 *
 *   docs/matching-matrices/icon2bc-template-surface-c4c0562.tsv
 *   docs/matching-matrices/icon2bc-forward-sibling-99d936f.tsv
 *   docs/matching-matrices/iconf2b-template-8116876.tsv
 *
 * In particular, the forward-sibling pass transferred IconToBitmap's outer
 * literal-selection ownership, compared the command/fill/dim/publication
 * families and ordered relocation sequence, and found the gain to be a whole
 * decoder-lifetime interaction.  The Iconf2b proof independently confirms
 * shared command dispatch, fill/dim body order, literal overlap gate, and
 * right-skip publication with iconf2bc.  The newly tested macro/demacro
 * boundary above is distinct: it asks whether the already-retained literal
 * fragment can be an invocation without changing any preprocessed function
 * tokens, and proves that equivalence across all 51 states.
 *
 * The credible reusable unit is narrower: forward ColorTable destination and
 * source publication, clipping, and the per-pixel transform.  The family
 * ReadIconRleByte helper remains a supported lower island already covered by
 * saved-destination-reader-descendant.cpp and family-entry-reader-matrix.cpp:
 * it approaches retail size and relocation count but moves away from the
 * aligned block bodies, so it is not retained.
 *
 * Disposition: keep the expanded do/break body and both inline helpers.  A
 * one-use macro would discard readable effective source without proving reuse.
 * Retain the macro signature as evidence for future coordinated ColorTable
 * family work; no generated declaration or macro remains in reconstructed
 * source.
 */

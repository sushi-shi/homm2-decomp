/*
 * IconToBitmap literal-run expanded-macro family, 2026-07-27.
 *
 * Hypothesis: the literal-run clipping diamond may be an expansion of a
 * shared 1995 multiline macro or inline template used by the icon family.
 *
 * Sibling comparison
 *
 * IconToBitmapYModify uses the same vertical/horizontal clipping cases as
 * directly nested conditionals around memcpy calls. IconToBitmapColorTable
 * instead selects destination, source, and count inside:
 *
 *   do {
 *       if (outside vertically) {
 *           count = 0;
 *           break;
 *       }
 *       ...
 *   } while (0);
 *
 * It then consumes the selected values in a shared per-pixel color-table
 * loop. That do/break structure is credible expanded-macro evidence, but its
 * applicability to the raw-copy function had not been tested.
 *
 * Four exact-span structural versions were clean-screened:
 *
 *   1. retained direct/nested memcpy calls;
 *   2. ColorTable-shaped do/break selection with uninitialized outputs;
 *   3. do/break selection with default raw-copy outputs;
 *   4. shared output tail using nested selection without breaks.
 *
 * Clean results:
 *
 *   retained direct:          81.123630%, size 1219, 78/83 relocations;
 *   ColorTable do/break:      73.346150%, size 1226, 76/83 relocations;
 *   defaulted do/break:       73.978020%, size 1210, 77/83 relocations;
 *   shared tail, no break:    75.184070%, size 1215, 77/83 relocations.
 *
 * Artifacts:
 *
 *   build/icon2b-literal-macro-expansions-axes-20260727.json
 *   build/icon2b-literal-macro-expansions-clean-manifest-20260727.json
 *   build/match-variants/
 *     icon2b-literal-macro-expansions-clean-20260727/
 *
 * Complete state product
 *
 * Every structure received clean plus the same 50 top-inserted declaration-
 * forest states: 204/204 variants completed in 142.30 seconds, without wall
 * truncation, and the source was restored.
 *
 *   build/icon2b-literal-macro-expansions-state-manifest-20260727.json
 *   build/match-variants/
 *     icon2b-literal-macro-expansions-state-20260727/
 *
 * Best state per structure:
 *
 *   retained direct:
 *     81.673080%, size 1220, 79/83 relocations;
 *     80/80 blocks, exact flow, 63 exact plus 17 size-only bodies.
 *
 *   ColorTable do/break:
 *     73.719780%, size 1226, 76/83 relocations;
 *     82/80 blocks, 47 exact, 20 size-only, nine shifted targets,
 *     four flow-kind mismatches, and two extra blocks.
 *
 *   defaulted do/break:
 *     74.956050%, size 1195, 78/83 relocations;
 *     80/80 blocks, but only 53 exact plus 15 size-only bodies,
 *     seven shifted targets, and five flow-kind mismatches.
 *
 *   shared tail without breaks:
 *     75.546700%, size 1203, 78/83 relocations;
 *     81/80 blocks, 48 exact, 23 size-only, six shifted targets,
 *     three flow-kind mismatches, and one extra block.
 *
 * Disposition
 *
 * The exact ColorTable expansion creates the wrong block family and loses
 * three more ordered relocation occurrences. Its defaulted descendant
 * recovers block count but not retail flow, size, relocations, or score.
 * The no-break shared tail also introduces an extra block.
 *
 * The reusable do/break mechanism therefore appears tied to ColorTable's
 * explicit destination/source publications and per-pixel transformed copy,
 * rather than to the raw-copy clipping region itself. Preserve the current
 * IconToBitmap source and its generated 81.6731 MAX. No probe declaration,
 * expanded-macro source, or generated configuration was retained.
 */

#if 0
// Rejected ColorTable-shaped selection.
u32 count;
u8* copyDst;
u8* copySrc = gIcSrc;
do {
    if (clip == ICON_DRAW_NO_CLIP) {
        copyDst = row + X;
        count = command;
    } else {
        if (gIcY < clipY || gIcClipB < gIcY) {
            count = 0;
            break;
        }
        i32 right = X + command;
        if (right <= clipX || gIcClipR < X) {
            count = 0;
            break;
        }
        i32 clipRight = gIcClipR;
        if (clipX <= X) {
            copyDst = row + X;
            count = clipRight >= right ? command : (clipRight - X) + 1;
        } else {
            copyDst = row + clipX;
            count = clipRight >= right ? (command - clipX) + X : clipW;
            copySrc = gIcSrc + (clipX - X);
        }
    }
} while (0);
if (count != 0)
    memcpy(copyDst, copySrc, count);
#endif

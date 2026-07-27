/*
IconToBitmapYModify, retail RVA 0x000da270, size 0x588.

This is a rejected-source experiment ledger, not compiled game source.

Removing the small row-visibility helper and spelling its predicate directly at
all three call sites is a credible family structure:

    static inline i32 IconRowVisible(i8* shear, i32 clipTop) {
        return shear[gYMY] != ICON_SHEAR_SKIP_ROW
            && clipTop <= gYMY && gYMY <= gYMClipB;
    }

versus:

    shear[gYMY] != ICON_SHEAR_SKIP_ROW
        && clipY <= gYMY && gYMClipB >= gYMY

The helper/direct axis was compiled against the clean state and 50 forest
states inserted after the TU include block (102/102 candidates):

    build/icon2by-row-visibility-inline-axes.json
    build/match-variants/icon2by-row-visibility-inline-top/

Top insertion exposed a better compiler-state orbit.  The retained helper arm
at forest trial 39 reached 98.045685%, size 1419, relocations 131/130, with
64/64 matching flow and 63 exact block sizes plus one size-only block.  The
direct-predicate arm reached 97.512690% in the same state and did not improve
the structure, so the helper remains reconstructed source.

The new maximum was independently replayed and recorded for the unchanged
effective-source hash:

    build/tu-state-noise/icon2by-top-trial39-record-20260727/

The sole remaining non-exact block is the clipped literal-copy tail.  A final
top-insertion matrix crossed three ownership forms (153/153 candidates):

    memcpy(gYMRow + clipX,
           gYMSrc + (clipX - gYMX),
           (gYMRun - clipX) + gYMX);

    i32 copyOffset = clipX - gYMX;
    u8* copySrc = gYMSrc + copyOffset;
    memcpy(gYMRow + clipX, copySrc, (gYMRun - clipX) + gYMX);

and one source pointer shared across the two nested clipping arms:

    build/icon2by-copy-top-axes.json
    build/match-variants/icon2by-copy-top/

The branch-local offset was byte-neutral in every state and reproduced the
98.045685% island.  Sharing it changed orbit but regressed to 96.878174% with
61 exact and three size-only blocks.  No generated source was retained.
*/

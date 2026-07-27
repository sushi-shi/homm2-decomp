/*
IconToBitmapYModify, retail RVA 0x000da270, size 0x588.

This is a rejected-source experiment ledger, not compiled game source.

The clean function already has the retail 64-block CFG: all flow edges and
targets agree.  The initial block census had 60 exact block bodies and four
size-only blocks.  Two exact-span axes crossed the setup multiplication and the
upper row-clipping comparison:

    gYMRow = dest->m_pixels + gYMPitch * gYMY;
    gYMRow = dest->m_pixels + gYMY * gYMPitch;

    clipTop <= gYMY && gYMY <= gYMClipB
    clipTop <= gYMY && gYMClipB >= gYMY

All four source shapes were compiled against the clean state and 50 target-site
forest states (204/204 candidates):

    build/icon2by-setup-visibility-axes.json
    build/match-variants/icon2by-setup-visibility/

Both axes were byte-neutral within every corresponding state.  The best state
was 95.748730%, size 1419, relocations 131/130; no arm raised the retained MAX.

The next matrix targeted the first differing clipped fill/copy ownership:

    (gYMRun - clipX) + gYMX
    gYMX + (gYMRun - clipX)

and direct, branch-local staged, and shared source-offset forms of:

    gYMSrc + (clipX - gYMX)

The staged forms used ordinary semantic locals:

    i32 copyOffset = clipX - gYMX;
    u8* copySrc = gYMSrc + copyOffset;

Nine source combinations were compiled against the clean state and 50
target-site forest states (459/459 candidates):

    build/icon2by-fill-copy-axes.json
    build/match-variants/icon2by-fill-copy/

Every fill-count and branch-local copy spelling was byte-neutral within its
state.  The best remained 95.748730%, size 1419, relocations 131/130.  These
spelling axes should not be repeated at target-site insertion; the top-insertion
orbit is recorded separately.
*/

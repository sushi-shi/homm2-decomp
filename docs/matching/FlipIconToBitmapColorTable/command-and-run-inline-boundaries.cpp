// Experiment: extend the successful row-visibility inline family along two
// independent boundaries: the outer command-byte read and full run visibility.
// Artifact: build/match-variants/iconf2bc-inline-boundaries-clean-20260726/
// Matrix:
//   retained row helper only                 87.035260%, size 1341
//   command reader                           69.695210%, size 1396
//   shared run visibility                    83.073050%, size 1344
//   command reader + shared run visibility   66.566750%, size 1406
// All arms retained 84/83 relocation counts.
// Disposition: rejected; the smaller row-only boundary remains authoritative.

#if 0
static inline i32 FlipColorReadCommand(u8*& src)
{
    return *src++;
}

static inline i32 FlipColorRunVisible(
    i32 clipX, i32 clipY, i32 left, i32 right)
{
    return FlipColorRowVisible(clipY)
        && clipX <= left
        && right <= gFCClipR;
}

cmd = FlipColorReadCommand(src);

if (FlipColorRunVisible(clipX, clipY, left = (X - count) + 1, X)) {
}

if (FlipColorRunVisible(
        clipX, clipY, static_cast<i32>((X - count) + 1), X)) {
}
#endif

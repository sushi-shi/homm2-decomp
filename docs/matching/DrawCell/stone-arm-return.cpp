/*
DrawCell, RVA 0x0005bb7c: out-of-bounds stone-arm return (2026-07-27).

Before this change the candidate had 313 blocks versus 314 in retail.  The
first branch-kind divergence was B38, immediately after the out-of-bounds
TileToBitmap call.  Retail contains two consecutive jumps there:

  1. an explicit jump to the function epilogue;
  2. an unreachable jump over the syntactic `else` arm.

That is the /Od shape produced by an explicit return inside an if arm that
still has an else.  The prior source relied only on the if/else join, so it
emitted the first jump but not the unreachable second block.

The retained structural version is:
*/

#if 0
if (gbAllBlack == 0
    && (mapX < 0 || mapY < 0 || mapX >= MAP_WIDTH || mapY >= MAP_HEIGHT)) {
    // Existing stone-frame selection remains unchanged.
    TileToBitmap(
        m_stoneTiles,
        s_drawStoneTile,
        gpWindowManager->m_screen,
        s_drawPixelX,
        s_drawPixelY
    );
    return;
} else {
    // Existing visible/covered cell drawing remains unchanged.
}
#endif

/*
The exact-span authored shape was compiled against clean plus 50 declaration-
forest states:

  build/drawcell-stone-return-manifest-20260727.json
  build/match-variants/drawcell-stone-return-20260727/

The clean structural source improved 91.179100% -> 91.221240% and added the
missing five-byte block.  More importantly, its skeleton is now 314/314
blocks: 285 exact bodies, 25 size-only differences, four shifted targets,
zero flow-kind differences, and zero missing blocks.  The frame remains 0x1c
with ebx/esi/edi saved, and relocations remain exactly 551/551.

Forest trials 20, 27, 40, and 44 reached 99.067430%, size 9406 versus retail
9419.  This exceeds the previous effective-source MAX of 99.0253%.  Trial 20
was replayed after the legitimate source change established new hash
5019ed1e765d:

  build/tu-state-noise/drawcell-stone-return-trial20-record-max-20260727/
  build/drawcell-stone-return-trial20-states-20260727.json

The official recorder retained 99.0674% for the new hash.  No generated state
was retained, and DrawCell has no directly referenced generated string/data
literal whose topology could be reordered by this change.

The first remaining size-only family is route visibility indexing at B69-B72.
Retail separately scales `mapY * MAP_WIDTH` and `mapX` before adding the
visibility-map pointer; the retained array subscript folds the combined index
into the word load.  Two complete clean plus 50 follow-up matrices tested it:

  build/drawcell-visibility-index-manifest-20260727.json
  build/match-variants/drawcell-visibility-index-20260727/

Changing all four expressions from `mapY * MAP_WIDTH + mapX` to
`mapX + mapY * MAP_WIDTH` was byte-identical in every state and was rejected.

  build/drawcell-visibility-pointer-manifest-20260727.json
  build/match-variants/drawcell-visibility-pointer-20260727/

Spelling all four expressions as
`*(m_visibilityMap + mapY * MAP_WIDTH + mapX)` opened a different island.  It
advanced each affected block from 9/11 to 10/11 instructions and brought the
best trial size to 9414, five bytes short of retail.  However, it still did
not reproduce retail's first differing block and its best score was only
98.973030%, below the retained 99.067430% island.  This incomplete structural
descendant was preserved as evidence but not retained in reconstructed source.
*/

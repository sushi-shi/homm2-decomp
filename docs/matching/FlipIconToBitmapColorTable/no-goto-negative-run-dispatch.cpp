// Experiment: replace all do_fill/do_dim transfers with a coherent fillRun
// decision, retaining one shared fill body followed by the dim body.
// Artifact: build/match-variants/iconf2bc-no-goto-dispatch-clean-20260726/
// Result: 84.113350%, size 1361 versus retail 1357, relocations 84/83.
// Blocks: 78/76, only 15 exact; 18 target shifts, 38 flow-kind mismatches,
// and 2 missing blocks.
// Disposition: rejected as a different topology. Together with the shared
// fill/dim tails in Icon2b, icon2bc, and Iconf2b, this strongly favors the
// existing shared-continuation family over a source-level fill decision.

#if 0
i32 flags = 0;
i32 fillRun = 0;
if (count != 0) {
    if (cmd == ICON_RLE_LONG_SOLID_COMMAND)
        count = *src++;
    gFCColor = colorTable[*src++];
    fillRun = 1;
} else {
    flags = *src++;
    count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
    if (count == 0)
        count = *src++;
    gFCDimLen = count;
    if (color != 0 && (flags & ICON_RLE_DIM_RECOLOR_FLAG)) {
        gFCCnt2 = count;
        gFCColor = static_cast<u8>(color);
        fillRun = 1;
    }
}

if (fillRun) {
    FillBody();
    continue;
}
DimBody();
#endif

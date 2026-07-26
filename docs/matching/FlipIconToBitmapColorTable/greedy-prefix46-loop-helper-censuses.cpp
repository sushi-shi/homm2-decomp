// Helper-boundary and loop-ownership descendants of the 46-byte setup island.
//
// A source-advance inline helper retained the 76/76 CFG but moved to a much
// worse internal orbit.  Its full 50-state census found one 46-byte prefix at
// state 14, only 80.012596%, size 1344 and relocations 84/83:
//   build/match-variants/iconf2bc-prefix46-inline-advance-states-20260726/
//
// Moving command ownership inside the infinite decoder loop, with both for
// and while spellings, preserved topology.  The credible inner-command form
// was swept through 50 states and returned to the ordinary state-39 object:
//   build/match-variants/iconf2bc-prefix46-loop-command-owner-clean-20260726/
//   build/match-variants/iconf2bc-prefix46-inner-command-states-20260726/
//
// A shared inline byte-reader was tested at the command site and at all
// command/control sites.  Both helper boundaries contradicted the retail CFG:
// 77 candidate blocks versus 76 retail, with 12 exact, 13 size-only,
// 23 target-shift, 28 flow-kind, and 1 missing block.  Candidate sizes were
// 1399-1407.  The forms were therefore pruned before TU-state permutation:
//   build/match-variants/iconf2bc-prefix46-decoder-boundary-clean-20260726/
//
// Disposition: retain the direct decoder reads and current loop family as the
// structural seed.  No reconstructed source or MAX change.

#if 0
static inline u8* AdvanceFlipColorSource(u8* src, IconEntry* entry)
{
    return src + entry->srcOffset;
}

src = AdvanceFlipColorSource(src, entry);

for (;;) {
    i32 cmd = *src++;
    // Existing decoder body.
}

static inline u8 ReadIconRleByte(u8*& src)
{
    return *src++;
}

i32 cmd = ReadIconRleByte(src);
#endif

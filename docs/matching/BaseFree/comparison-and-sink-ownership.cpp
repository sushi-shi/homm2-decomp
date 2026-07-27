/*
BaseFree (RVA 0x000c3f80, retail size 902)
Comparison-only entry ownership and per-site log-sink ownership, 2026-07-27.

The live object has the retail 28-block CFG and retail size. Its six differing
blocks consist of three newline-append schedules plus the entry-table register
ownership in the search body. The historical hash-scoped MAX remains 94.0482%;
the clean object used by these matrices is 91.333336%, size 902, with 55/55
raw relocation sites and 23 exact plus five size-only blocks.

Retail's loop loads gpMemEntry as a table base, materializes both the selected
entry address and its ptr field for the comparison, then returns to direct
gpMemEntry[index] accesses for mutation. Earlier attempts had made ownership
uniform across the whole body, so this matrix tested the missing hybrid:

  - direct comparison,
  - snapshot only the ptr field,
  - snapshot only the entry address,
  - snapshot the entry address and then the ptr field.

All four shapes were crossed with the clean state and the same 50 expansive
top declaration-forest states. The complete 204/204 matrix emitted one unique
object: every source/state combination was exactly 91.333336%, size 902,
55/55 raw relocation sites, and the same 28-block classification. MSVC
therefore canonicalizes these comparison-only snapshots completely.

Artifacts:
  build/basefree-compare-owner-axes-20260727.json
  build/basefree-compare-owner-manifest-20260727.json
  build/match-variants/basefree-compare-owner-20260727/results.json

The first and third retail log sinks push the FILE owner before scanning the
first argument, suggesting that append evaluation might belong to fputs rather
than a preceding statement. A second matrix independently nested the existing
word append into the null-pointer sink, the matched-entry sink, the bad-delete
sink, or all three sinks. The current separate statements were the fifth arm.

All five shapes crossed with clean plus 50 states completed 255/255 and again
emitted the same single 91.333336% object. Per-site expression ownership is
also canonicalized and cannot explain the retail scheduling.

Artifacts:
  build/basefree-nested-fputs-axes-20260727.json
  build/basefree-nested-fputs-manifest-20260727.json
  build/match-variants/basefree-nested-fputs-20260727/results.json

Disposition: retain neither generated source shape. These matrices close two
previously untested mixed-ownership branches without changing reconstructed
source or the historical MAX.
*/

#if 0
// Comparison-only ownership arms; the existing body remains direct-indexed.
void* comparedPointer = gpMemEntry[entryIndex].ptr;
if (comparedPointer == ptr) {
}

MemEntry* comparedEntry = &gpMemEntry[entryIndex];
if (comparedEntry->ptr == ptr) {
}

MemEntry* comparedEntry = &gpMemEntry[entryIndex];
void* comparedPointer = comparedEntry->ptr;
if (comparedPointer == ptr) {
}

// Applied independently at each of the three log sinks.
fputs(
    (*reinterpret_cast<u16*>(logText + strlen(logText)) =
         *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text),
     logText),
    logFile
);
#endif

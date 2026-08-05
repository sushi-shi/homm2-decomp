/*
LogTruncate, retail RVA 0x000c6070, size 0xa6.
Newline append publication and inline-boundary ownership, 2026-07-27.

The function has exact four-block CFG (three exact blocks and one size-only
block) and exact 7/7 ordered relocations. Its only disassembly divergence is
inside B2. The candidate loads the newline word before the first strlen scan,
then reconstructs the end pointer from ECX. Retail keeps the scan continuation
in EDI, loads the word afterward, and stores it at [EDI - 1]. Retail is three
bytes shorter: 166 versus 169.

Two independent structure matrices were crossed with the clean state plus 50
top-of-TU declaration-forest states:

  build/logtruncate-append-ownership-axes-20260727.json
  build/logtruncate-append-ownership-manifest-20260727.json
  build/match-variants/logtruncate-append-ownership-20260727/results.json

  build/logtruncate-split-inline-axes-20260727.json
  build/logtruncate-split-inline-manifest-20260727.json
  build/match-variants/logtruncate-split-inline-20260727/results.json

The first matrix completed 7 * 51 = 357/357 builds in 371.03 seconds:
direct word assignment, an explicit end pointer, end-then-word locals, an
indexed length, a single inline append owner, an inline append-and-measure
owner, and constant-size memcpy.

The second matrix completed 5 * 51 = 255/255 builds in 267.87 seconds:
direct assignment, a separate inline end finder, a reference-returning end
finder, caller-owned end discovery plus an inline store, and distinct inline
find/store boundaries.

All 612 variants produced the identical 92.500000%, size-169, 7/7-relocation
target state. The helpers, locals, references, and memcpy spelling are fully
eliminated; none preserves a retail-like inline continuation. This is stronger
than a flat score: the normalized text and complete instruction stream are
identical for every arm and every tested compiler state.

Disposition: retain the direct source and durable MAX 92.5. Do not retain
helpers or compiler probes. A future attempt must change the actual string-end
mechanism or expose a different TU-state orbit; further local spellings of the
same strlen-plus-word assignment are exhausted.

Cross-version follow-up, 2026-08-05:

The full exact Gold/Buka guard-clause plus strcat structure was tested as a
separate structural parent across clean plus 50 states.  The complete 102/102
matrix confirmed that it belongs to a different compiler/version orbit: every
Gold arm was size 196 and 70.785710%, while every retained direct-word arm was
size 169 and 92.500000%.  PoL retail is size 166.  No Gold source is retained.

Artifacts:
  build/logtruncate-cross-version-axes.json
  build/logtruncate-cross-version-manifest.json
  build/source-variant-batch/logtruncate-cross-version/results.json
*/

#if 0
static inline u16* FindLogWordEnd(char* text) {
    return reinterpret_cast<u16*>(text + strlen(text));
}

static inline void StoreLogWord(u16* end, const char* suffix) {
    *end = *reinterpret_cast<const u16*>(suffix);
}

u16* logEnd = FindLogWordEnd(logText);
StoreLogWord(logEnd, gMiscText.log.truncateNewline.text);
#endif

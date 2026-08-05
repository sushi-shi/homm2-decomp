/*
PrintMemoryLeaks (RVA 0x000c4310, retail size 308)
Gold/Buka helper-boundary and PoL inline-helper matrices, 2026-08-05.

PoL retail contains the logging append body inline.  The retained open body is
96.043950%, size 311, with the exact 10-block graph and the complete 19-record
raw relocation stream; its only structural frontier is the one-instruction
size difference in the newline-append block.

The exact Gold/Buka source uses two early guards, a counted for loop, and a
direct LogStr(gText) call.  A complete 2 x 51 matrix showed that VC4.2 does not
inline that later-defined public function: every donor arm was 39.978024%,
size 140, with only 11 relocations.  The direct donor boundary contradicts PoL
retail.

Retail's inline body still justified testing an explicit source-level inline
helper.  A second complete matrix crossed the open body, an inline helper with
the retained CFG, and the inline helper with the 2.1 guard/for CFG against all
51 clean/state shapes.  All 153 cells finished.  Both inline-helper parents
were byte-identical to the open body in every state: 96.043950%, size 311,
exact graph, and 19/19 raw relocations.  No island altered the append schedule.

Artifacts:
  build/printmemoryleaks-cross-version-axes.json
  build/printmemoryleaks-cross-version-manifest.json
  build/source-variant-batch/printmemoryleaks-cross-version/results.json
  build/printmemoryleaks-inline-helper-axes.json
  build/printmemoryleaks-inline-helper-manifest.json
  build/source-variant-batch/printmemoryleaks-inline-helper/results.json

Disposition: retain the open PoL body and existing MAX.  Preserve the direct
donor rejection and byte-neutral inline-helper result as orbit evidence; no
generated helper or state declaration is retained.
*/

#if 0
static inline void WriteLogText(char* text) {
    // Same semantic body as LogStr.
}
#endif

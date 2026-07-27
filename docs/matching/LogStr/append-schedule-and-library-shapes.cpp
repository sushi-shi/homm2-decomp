/*
LogStr (RVA 0x000c6120, retail size 158)
Append scheduling and library-shape matrices, 2026-07-27.

The candidate and retail functions have the same five-block CFG, four exact
blocks, eight ordered semantic relocation sites, and the same behavior. The
only differing block is the newline append:

  candidate: load newline; scan; complement strlen count; indexed word store
  retail:    push FILE; scan; load newline; store through scan end pointer

The live and historical-MAX object is 88.500000%, size 165. Raw comparison
objects contain 9/9 relocation records.

The first complete matrix tested whether MSVC could be made to preserve the
scan-end pointer by changing expression ownership:

  - open assignment,
  - explicit end pointer,
  - reference bound to the end word,
  - end pointer followed by a newline-word snapshot,
  - newline-word snapshot followed by the end pointer,
  - append nested in fputs' first argument.

All six source shapes were crossed with clean plus 50 expansive top
declaration-forest states. All 306/306 builds emitted the same unique
88.500000%, size-165, 9/9-relocation object. Pointer/reference lifetimes and
right-to-left fputs evaluation are completely canonicalized in this TU.

Artifacts:
  build/logstr-append-schedule-axes-20260727.json
  build/logstr-append-schedule-manifest-20260727.json
  build/match-variants/logstr-append-schedule-20260727/results.json

Raw retail relocation review proved that the newline word comes from the
gMiscText aggregate, not an anonymous newline literal. A second complete
matrix nevertheless tested the plausible library-call ownerships:

  word assignment:   88.500000%, size 165, 9/9 raw relocations
  strcat:            64.685190%, size 192, 9/9 raw relocations
  strcpy(pointer):   61.740740%, size 196, 9/9 raw relocations
  strcpy(index):     61.740740%, size 196, 9/9 raw relocations

All four arms crossed with clean plus 50 states completed 204/204 without
truncation. The library forms add copy machinery and contradict retail size;
none opens the scan-end-pointer lowering.

Artifacts:
  build/logstr-library-append-axes-20260727.json
  build/logstr-library-append-manifest-20260727.json
  build/match-variants/logstr-library-append-20260727/results.json

Disposition: retain the direct word assignment and the existing MAX. No
generated source or probe declaration is retained.
*/

#if 0
u16* logEnd = reinterpret_cast<u16*>(logText + strlen(logText));
*logEnd = *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);

u16& logEnd = *reinterpret_cast<u16*>(logText + strlen(logText));
logEnd = *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);

u16* logEnd = reinterpret_cast<u16*>(logText + strlen(logText));
u16 newlineWord = *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
*logEnd = newlineWord;

fputs(
    (*reinterpret_cast<u16*>(logText + strlen(logText)) =
         *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text),
     logText),
    logFile
);

strcat(logText, gMiscText.log.appendNewline.text);
strcpy(logText + strlen(logText), gMiscText.log.appendNewline.text);
strcpy(&logText[strlen(logText)], gMiscText.log.appendNewline.text);
#endif

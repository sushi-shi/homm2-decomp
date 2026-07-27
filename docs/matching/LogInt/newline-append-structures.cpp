/*
LogInt, RVA 0x000c61c0: newline append structure census (2026-07-27).

Retail scans logText, pushes the FILE* before that scan completes, loads the
two-byte newline afterward, and stores it through the resulting EDI pointer.
The current equivalent word assignment loads the newline before the scan and
recomputes the destination from the complemented length.  Four complete
clean-plus-50 matrices tested source structures suggested by that divergence.

Append spelling matrix:
*/

#if 0
strcat(logText, gMiscText.log.appendNewline.text);
strcpy(logText + strlen(logText), gMiscText.log.appendNewline.text);

char* logEnd = logText + strlen(logText);
*reinterpret_cast<u16*>(logEnd) =
    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);

i32 logLength = strlen(logText);
*reinterpret_cast<u16*>(&logText[logLength]) =
    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
#endif

/*
  build/logint-append-shape-manifest.json
  build/match-variants/logint-append-shape/

The explicit pointer/index arms normalized to the current 96.600000%,
size-554 object in all states.  strcat emitted a larger size-578 object at
88.806060%; strcpy-at-end emitted size 585 at 87.842420%.

Fixed two-byte copy matrix:
*/

#if 0
memcpy(
    logText + strlen(logText),
    gMiscText.log.appendNewline.text,
    sizeof(gMiscText.log.appendNewline.text)
);
memcpy(logText + strlen(logText), gMiscText.log.appendNewline.text, 2);
memcpy(
    &logText[strlen(logText)],
    gMiscText.log.appendNewline.text,
    sizeof(gMiscText.log.appendNewline.text)
);
#endif

/*
  build/logint-fixed-copy-manifest.json
  build/match-variants/logint-fixed-copy/

All 204 builds normalized to the current 96.600000%, size-554,
22-ordered-relocation object.

The retail push order also suggested that append evaluation might be nested
inside fputs' first argument, after right-to-left evaluation pushes file.
The final matrix tested:
*/

#if 0
fputs(strcat(logText, gMiscText.log.appendNewline.text), file);

fputs(
    (*reinterpret_cast<u16*>(logText + strlen(logText)) =
         *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text),
     logText),
    file
);

fputs(
    (memcpy(
         logText + strlen(logText),
         gMiscText.log.appendNewline.text,
         sizeof(gMiscText.log.appendNewline.text)
     ),
     logText),
    file
);
#endif

/*
  build/logint-nested-fputs-manifest.json
  build/match-variants/logint-nested-fputs/

Nested strcat remained 88.806060%/578 bytes in every state.  Both comma
expressions normalized to the current 96.600000%/554-byte object.  Across
these three append matrices, 663 complete builds found no new island.

Together with the 357 inline-boundary builds recorded in
inline-ownership-boundaries.cpp, this campaign classified 1,020 complete
source/state combinations.  No generated probe source was retained.  The
current source remains the best live structure; the six-byte residual is now
best treated as a wider MSVC TU-state orbit rather than another synonymous
local append spelling.
*/

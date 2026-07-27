/*
LogInt, RVA 0x000c61c0: inline ownership boundary census (2026-07-27).

The live candidate and retail function have the same 18-block CFG, 17 exact
block bodies, the same 0x2bc frame, and the same 22 ordered relocations.  The
only differing body is the newline append/fputs tail.  These matrices tested
whether that tail came from a different inlined ownership boundary.

The first complete matrix tested the current direct body and three static
inline boundaries:
*/

#if 0
AppendLogNewline(logText);
LogTextEnd(logText);
WriteLogLine(file, logText, text);
#endif

/*
Each helper contained the same effective strcpy/newline/fputs operations as
the direct body.  All four source shapes were compiled in the clean state and
the same 50 declaration-forest states:

  build/logint-inline-boundary-manifest.json
  build/match-variants/logint-inline-boundary/

All 204 builds emitted the identical 96.600000% object: size 554 versus retail
548, with 22/22 ordered relocations.  The helper boundaries flattened completely.

The second complete matrix moved the entire sink into an inline helper, then
tested direct reuse of the preceding LogStr implementation:
*/

#if 0
static inline void WriteIntegerLogText(char* text) {
    char logText[TEXT_BUFFER_SIZE];
    if (giDebugLevel >= FILE_DEBUG_LEVEL) {
        FILE* file =
            fopen(gMiscText.log.appendFilename.text, gMiscText.log.appendMode.text);
        if (file != NULL) {
            strcpy(logText, text);
            *reinterpret_cast<u16*>(logText + strlen(logText)) =
                *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
            fputs(logText, file);
            fclose(file);
            if (giDebugLevel == DEBUGGER_OUTPUT_LEVEL)
                OutputDebugStringA(logText);
        }
    }
}

WriteIntegerLogText(text);

// Alternate arm:
inline void LogStr(char* text);
LogStr(text);
#endif

/*
Artifacts:

  build/logint-log-sink-manifest.json
  build/match-variants/logint-log-sink/

All 153 builds again emitted the same 96.600000%, size-554, 22-relocation
object.  Neither local-buffer ownership nor reuse of LogStr exposes another
compiler-state island, so none of these helpers was retained.
*/

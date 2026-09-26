// Structural hypothesis: reunite the current Misc/MiscRuntime owners while
// preserving the named writable track array recovered by owner-split.cpp.
// Measured with VC6 SP5 and native LIB/LINK; audited 2026-09-27.
// This tests whether the additional compiler producer needed by the separate
// DIMMER ownership experiment can be offset by a credible adjacent-owner merge.
// Producer arithmetic is diagnostic, never permission to discard data evidence.
//
// Artifacts (relative to matcher-1):
//   build/misc-named-owner-merge/run.py
//   build/misc-named-owner-merge/{prefix,suffix}.cpp
//   build/misc-named-owner-merge/include/       (root header snapshot)
//   build/misc-named-owner-merge/{lexical,includes-first}/Misc.cpp
//   build/misc-named-owner-merge/{lexical,includes-first}/Misc.obj
//   build/misc-named-owner-merge/results.json
//   build/misc-named-owner-merge/{audit.py,raw-audit.json}
//   build/misc-named-owner-merge/{link.py,image-audit.json}
//   build/misc-named-owner-merge/linked-operand-audit.json
// Each arm also retains native library, response files, MAP, EXE, PDB, and logs.
// No production sources, raw object bytes, or linked bytes were rewritten.
//
// COMPLETE two-arm product; both compiles returned zero without timeout and
// both native links completed. No state census or other source arms are claimed.
// The snapshot inputs are the current root Misc.cpp and MiscRuntime.cpp, not
// the older pre-split spelling with an anonymous track-name string literal.
// Common source structure remains:
//
//   #include <BASE/MiscState.h> // declares the shared storage
//   DATA(0x0051e5dc) H2_CONST char* gcCDTrackName = gcCDTrackNameText;
//   // Original Misc globals and functions, ending with IsCDDrive.
//   DATA(0x0051f120) char gcCDTrackNameText[] =
//       "\\Tracks2\\02-AudioTrack 02.ogg";
//   // Original MiscRuntime functions from DriveSupportsFreeSpaceQuery onward.
//
// Exact source generation is recorded by run.py:
//
//   suffix_body = suffix[suffix.index(
//       "#include <BASE/MiscGraphicsConstants.h>\n"):];
//
//   lexical = prefix + "\n#include <SOURCE/KB.h>\n" + suffix_body;
//
//   includes_first = prefix
//       .replace("#include <SOURCE/KBDeclarations.h>",
//                "#include <SOURCE/KB.h>")
//       .replace("#include <SOURCE/X_GLOBAL.h>",
//                "#include <SOURCE/X_GLOBAL.h>\n"
//                "#include <BASE/MiscGraphicsConstants.h>")
//       + "\n" + suffix_body.removeprefix(
//           "#include <BASE/MiscGraphicsConstants.h>\n");
//
// Both retain one copy of the shared enum/declaration preamble and every real
// function body. The lexical arm introduces the full KB audio dependency only
// at the old TU boundary; includes-first exposes it in the initial preamble.
// Flags are the identical current Misc/MiscRuntime profile:
//   /Od /MT /Gr /G5 /Ob1 /Gi- /GX /DNO_STRICT /Gy
// with /nologo /c and the copied include directory. Each merged object goes
// through native LIB into one archive, replacing Misc.lib in the root ordinary
// resource-link response file; MiscRuntime.lib is omitted. Other native inputs
// retain their positions. This is not a four-pass historical-header replay.
//
// Results are identical on all reported criteria for the two arms:
//
//   Raw function byte spans equal to the split baseline          47 / 47
//   Strict recorded ordered-relocation descriptions equal        42 / 47
//   Linked function VAs equal to their retail markers            47 / 47
//   Raw linked function spans equal to retail                    35 / 47
//   Differing bytes in the other 12 linked function spans             131
//   Differences outside recorded four-byte relocation operands          0
//   Misc data differences in [0x51e5dc, 0x51f534)               2742 / 3928
//   gcCDTrackNameText and pointer value                       0x0051e5f8
//   Required track-array address and pointer value            0x0051f120
//   Native CL producer 0x000b2306 count                              95
//   Retail/current unmerged root count for that producer             96
//
// The raw comparison contains 884 ordered relocation records per arm. Do not
// summarize the 47/47 raw byte result as complete relocation or image closure:
// audit.py deliberately compares some compiler-private identities literally.
// Its five nonidentical rows have the following frontiers:
//
//   FadeIn, FadeOut: +0x6 DIR32 EH-label names change ($L counters).
//   ReadPrefsFromFile: +0xa4/+0xb6 empty-string cells have unchanged offsets
//     48/52 but their containing .bss contribution grows from 61 to 73 bytes.
//   ReadPrefsFromRegistry: +0x746/+0x780 similarly retain offsets 56/60.
//   GetDataEntry: +0x6 EH-label name changes; empty strings at +0x92/+0x1fb/
//     +0x43b move from suffix .bss offsets 0/4/8 into merged offsets 64/68/72.
// These are limitations of whole-contribution/name identity in this particular
// comparator, not evidence that the ordered relocations were fully proved.
// Native linked spans independently establish exact bytes for FadeIn, FadeOut,
// and GetDataEntry in these images. The two preference-reading bodies are among
// the linked operand failures and must not be declared exact from raw code.
//
// linked-operand-audit.json checks native MAP addresses against all 47 source
// markers, compares full retail spans without masking, and then classifies each
// differing byte against the candidate relocation sites retained in raw-audit.
// The 12 affected functions (differing linked bytes in parentheses) are:
//   BaseFree (5), PrintMemoryLeaks (2), ShowMemoryStatus (1), ProcessAssert (2),
//   SetInstallDefaults (2), SetGameDefaults (2), ReadPrefsFromFile (3),
//   ReadPrefsFromRegistry (54), ReadPrefs (6), WritePrefsToFile (2),
//   WritePrefsToRegistry (51), IsCDDrive (1).
// Thus their failure is linked operand placement, not a changed instruction
// body outside relocation slots. This classification alone does not prove the
// target identities/addends correct; the retail byte mismatches remain real.
//
// Both named storage definitions are emitted in the same 58-byte .data section:
// pointer at offset 0, text at offset 28 (0x1c). Late lexical declaration does
// not split the section. The actual text therefore lands immediately after the
// prefix's initialized globals, despite its retained DATA marker at 0x51f120.
// Each object has one four-byte .CRT$XCU cell and the two ordinary compiler
// initialization helpers _$E19 / _$E18. PE section RVAs, virtual sizes, raw
// sizes, and raw offsets all agree with retail; this does not cure internal
// storage order. The native CL producer count falls by one as expected, but
// no DIMMER combination was linked by this matrix, and full historical-header
// identity was neither measured nor claimed.
//
// Disposition: reject both measured merges. They preserve ordinary method
// codegen and remove one producer, but restore the known wrong track-array
// placement and break initialized-data operands. Keep the split ownership in
// production. This rejects only the measured named-array ownership family and
// include-timing alternatives; it does not prove every possible real owner or
// declaration structure impossible. No new unmeasured source descendant is
// proposed as a result, and no correction machinery is justified by the result.

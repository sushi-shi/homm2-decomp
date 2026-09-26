// VC6 SP5 measured ownership recovery, 2026-09-26.
// Worktree matcher-2, branch matcher/bss-audiere. Initial probes left
// production untouched; the accepted includes-first source is now retained.
//
// Independent source/retail evidence for one Midi owner:
// - MusicFlags.h describes the same startup/shutdown state published by Midi.
// - GetMusicFlagA, MusicFlagsActive, GetMusicFlagB occupy 0x4c5770..0x4c57ca;
//   MIDIStartup immediately follows at 0x4c57d0 (normal /Gy alignment).
// - gMusicFeatureEnabled is at 0x51f550, immediately before CurrentMidiFile
//   at 0x51f554; a one-byte value plus normal four-byte alignment.
// - Both claimed units use the identical production base /Gy flag profile.
// - MusicFlags has no CRT initializer; Midi owns one ctype initializer pair.
// This is a semantic module-boundary hypothesis, not an arbitrary merge to
// manipulate compiler counts.
//
// Reproducer/artifacts:
//   build/music-owner-merge/probe.py
//   build/music-owner-merge/{lexical_prefix,includes_first}.{cpp,obj,log}
//   build/music-owner-merge/results.json
//   build/music-owner-merge/link-probe.py
//   build/music-owner-merge/link-results.json
//   build/music-owner-merge/debug-difference-proof.json
//   build/music-owner-merge/{ordinary,misc_combined}/
// Compile uses run_compile and unit_flags(BASE/Midi) from the verified Nix
// worktree shell. All links/libraries use untouched stock LINK.EXE/LIB.EXE.
//
// Complete structural matrix: two ordinary source arrangements.
// A: full current MusicFlags.cpp followed by full current Midi.cpp. Includes
//    retain their original lexical positions, exercising the exact former
//    boundary without pretending that it remains a translation-unit boundary.
// B: all Midi includes first, then this music-state prefix, then the unchanged
//    Midi enums/data/functions. This is the preferred ordinary owner layout.
//
// #include <va.h>
// #include <BASE/soundManager.h>
// #include <BASE/Midi.h>
// #include <BASE/MusicFlags.h>
// #include <mss.h>
// #include <SOURCE/KB.h>
// #include <SOURCE/X_GLOBAL.h>
// #include <BASE/resourceManager.h>
// #include <BASE/MIDIWrap.h>
// #include <BASE/Misc.h>
// #include <stdio.h>
//
// DATA(0x0051f550) u8 gMusicFeatureEnabled = 1;
//
// VA(0x004c5770, 0xa)
// u8 GetMusicFlagA(void) { return gMusicFlagA; }
//
// VA(0x004c5780, 0x32)
// u8 MusicFlagsActive(void) {
//     b32 active;
//     if (gMusicFeatureEnabled && gMusicFlagB)
//         active = true;
//     else
//         active = false;
//     return active;
// }
//
// VA(0x004c57c0, 0xa)
// u8 GetMusicFlagB(void) { return gMusicFlagB; }
//
// // Existing Midi enums, globals and functions follow verbatim.
//
// Both arms compile successfully and retain all nine public functions plus
// three emitted ctype helpers EXACT: function size, raw bytes after masking
// relocation operands, and complete ordered relocation sites/types/names/raw
// addends. This comparison needs no counter-name or symbol-identity exception.
// results.json retains the complete section bytes, symbol placements, and
// ordered relocation arrays for both arms and both original raw objects.
//
// Data topology is the required ordinary concatenation:
// - gMusicFeatureEnabled .data+0; CurrentMidiFile+4; bGotMidi+8;
//   gMidiFilenameFormat+68. Merged .data size81 = byte1 + alignment3 + old77.
// - The entire 488-byte Midi .bss, its symbol offsets, and its .CRT$XCU remain
//   unchanged. Only the original $E19/$E18 pair is emitted.
//
// Focused current-target checks before compiling: all four prefix/startup
// claims are live exact; MusicFlagsActive has 5/5 exact blocks and2/2 matching
// relocations; MIDIStartup has7/7 matching relocations. No new functions or
// initialized storage are introduced by the merge.
//
// Stock final-link controls, both using arm B:
// 1. Ordinary root --rsrc inputs: remove MusicFlags from BASE-prefix.lib and
//    replace Midi.lib's single object with the merged object. Entire .text,
//    .data, .rsrc and all section RVAs/virtual sizes remain byte-identical to
//    the ordinary baseline. The only three .rdata byte differences are in
//    IMAGE_DEBUG_DIRECTORY (timestamp and PDB record SizeOfData).
//    VC6 C++ Rich producer count changes96->95, as expected for one fewer TU.
// 2. Combine with matcher-1's independently recovered Misc split inputs:
//      build/misc-owner-split/staged-noaudio/link.rsp
//      prefix-noaudio.obj, suffix.obj, stage0..3.lib
//    Rebuild only stage3 without MusicFlags and replace Midi.lib. Entire
//    .text/.data/.rsrc remain byte-identical to that split baseline; the only
//    two .rdata differences are the debug timestamp. VC6 C++ producer count
//    changes97->96, recovering retail's count through real source ownership.
//
// Combined raw section mismatches against retail (not a completion claim):
//   .text501 bytes; .rdata8; .data2; .rsrc0.
// All section RVAs and virtual sizes equal retail. The other historical Rich
// record differences remain; no claim of full image closure is made here.
// debug-difference-proof.json verifies all control-link .rdata changes lie
// in the 28-byte debug directory at RVA0xea3f0; program data is untouched.
//
// Disposition: ACCEPT as a credible Midi module ownership recovery. Prefer
// includes-first source layout. Integrate together with the independently
// evidenced Misc ownership split so the actual C++ input count remains96.
// Root integration still must regenerate source-owned manifests,
// delinked targets and link archive scheduling, run homm2 build, and audit
// final relocation/address placement. No object transformation or synthetic
// compiler/producer input is part of this solution.

// Retained-source validation:
// homm2 redelink completed after rebuilding97 units and regenerating all
// source-owned manifests/targets. homm2 build reports1727/1727 exact
// functions and291998/291998 exact data bytes. The three-byte data-count
// increase is the existing inter-owner alignment becoming internal Midi
// .data padding; the linked .data bytes were proven unchanged above.
// Fresh production Midi.obj independently matches all12 audited merged
// function/helper fingerprints. Focused relocation checks at0xc5770,
// 0xc5780,0xc57c0,0xc57d0 have1,2,1,7 paired relocations, respectively;
// MusicFlagsActive retains5/5 exact blocks. od-frames audits11 paired Midi
// functions/helpers, all aligned, with zero frame/slot mismatches.
// Root should regenerate README/status ledger itself; lane-generated
// README.md and config/match_baseline.tsv are not integration payloads.

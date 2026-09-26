// Unit hypothesis: the inferred BASE/Misc owner spans two native compiler TUs.
// Measured: 2026-09-26, matcher/bss-misc, pinned VC6 SP5 and native LIB/LINK.
// This is a source/build hypothesis, not a claim that the original files have
// been recovered. No COFF or executable bytes were rewritten in these probes.
//
// Reproduction scripts and complete source snapshots:
//   build/misc-owner-split.py
//   build/misc-owner-staged-link.py
//   build/misc-owner-noaudio.py
//   build/misc-owner-analysis.py
//   build/misc-owner-split/{prefix.cpp,suffix.cpp,prefix-noaudio.cpp,KB-narrow.h}
// Exact source/header diff from the checkout:
//   build/misc-owner-split/source-product.patch
// Raw COFF section and ordered relocation census:
//   build/misc-owner-split/results.json
//   build/misc-owner-split/{local-analysis,local-analysis-noaudio}.json
// Native full-image comparisons:
//   build/misc-owner-split/{image-comparison,image-comparison-noaudio}.json
// Best byte and topology selections coincide:
//   build/misc-owner-split/staged-noaudio/{HMM2PL.exe,HMM2PL.map,link.rsp}
//   build/misc-owner-split/retail-function-byte-review.json
// The worse parent remains at staged-archives/; both single-archive arms
// remain at prefix-suffix/ and suffix-prefix/. All requested links completed.
//
// Structural source product:
//
//  1. Prefix owns the original globals and functions through IsCDDrive.
//     The shared pointer now refers to the suffix's actual named text array:
//
//       extern char gcCDTrackNameText[];
//       DATA(0x0051e5dc) H2_CONST char* gcCDTrackName = gcCDTrackNameText;
//
//     gBlitBottom/gBlitRight also gain external linkage because suffix code
//     uses them. Their declarations and definitions preserve the original
//     i32 type, zero initialization and storage. No new storage is added.
//
//  2. Suffix starts at DriveSupportsFreeSpaceQuery, keeps the remaining real
//     functions and owns the named writable character array:
//
//       extern H2_CONST char* gcCDTrackName;
//       extern i32 gBlitBottom;
//       extern i32 gBlitRight;
//       extern u8 giChangeThreshold[FADE_CHANGE_THRESHOLD_COUNT];
//       char gcCDTrackNameText[] = "\\Tracks2\\02-AudioTrack 02.ogg";
//
//     The original shared declarations, enums and required includes are kept;
//     both TUs use the unmodified unit_flags(BASE/Misc) profile.
//
//  3. The credible child narrows the prefix's KB dependency. A copied KB.h
//     replaces #include <BASE/soundManager.h> with class soundManager;.
//     KB.h needs that class only for extern soundManager* gpSoundManager;
//     the prefix uses no soundManager fields or methods. PollSound is a free
//     function already declared in KB.h. The full transitive audio header is
//     still included by the suffix. This removes the prefix's unneeded ctype
//     initialization; no pragma, fake use, guard manipulation, or codegen
//     steering is used.
//
// Link product:
//
// Replacing Misc with the two native objects in BASE-prefix.lib, in BOTH
// member orders, extracts the suffix first and is rejected. The track array
// lands at 0x51e5dc and DriveSupportsFreeSpaceQuery takes InitMemEntry's RVA.
// Archive member order does not control this dependency-driven extraction.
//
// The staged diagnostic replaces BASE-prefix.lib with four native libraries:
// its members before Misc, the Misc prefix, the Misc suffix, and its remaining
// members. All other inputs come from the ordinary resource-mode link.
// Native LIB receives members in the same reversal convention as the normal
// build. No ordering correction or rewritten object participates.
//
// Source/build arm            .text diff  .rdata diff  .data diff  track VA
// ordinary unsplit                 632            8        2744  0x51e5f8
// split, either member order    179218          798        3981  0x51e5dc
// staged split, full headers    170001          508         245  0x51f120
// staged, narrow prefix header    501            8           2  0x51f120
// All arms have exact .rsrc. Counts compare full raw section payloads to retail.
//
// The full-header staged parent has an identifiable first code frontier:
// prefix ctype initialization emits 39- and 18-byte helpers, aligned as BASE
// COMDATs. DriveSupportsFreeSpaceQuery moves by +0x50 to 0x4bf340. Both TUs
// also emit a four-byte CRT initializer cell instead of the original one.
// The narrow-header child removes the prefix's entire CRT contribution and
// helper tail; all suffix functions return to their retail addresses.
//
// Child verification:
//   * All 47 annotated Misc functions have EXACT RAW LINKED retail bytes at
//     their retail VAs, including absolute pointer operands and calls.
//   * The complete 3928-byte initialized-data interval
//     [0x51e5dc,0x51f534) matches retail byte for byte. The pointer cell at
//     0x51e5dc contains 0x51f120, the actual single-copy suffix array address.
//   * All PE section RVAs, virtual sizes, raw sizes and raw offsets equal
//     retail. Thus the data interval result does not hide section-size drift.
//   * The two remaining .data byte differences are outside Misc:
//       0x4ef140: Audiere initializer pointer 0x4cd030 versus 0x4cd000;
//       0x516ae0: REQUEST cFRDummy pointer 0x533d84 versus 0x533d98.
//   * The native CL producer count becomes 97; retail and the ordinary
//     unsplit baseline both have 96. This unresolved provenance contradiction
//     prevents calling the complete build explanation recovered.
//
// Disposition: retain the narrow-header split as a verified local branch,
// not as integrated reconstructed source. The earlier statement that no
// source/build ownership can put this text at the retail position assumed
// Misc's inferred TU boundary; this experiment disproves that broad claim.
// The actual archive staging and the extra native compiler producer still
// need independent explanation. Do not merge an unrelated TU or rewrite
// metadata merely to hide the producer mismatch.
//
// UNTRIED adjacent semantic-owner hypothesis, outside this lane:
// MusicFlags' three audio-state accessors immediately precede Midi's bodies
// (GetMusicFlagB at 0x4c57c0; MIDIStartup at 0x4c57d0). Its feature byte at
// 0x51f550 immediately precedes Midi's CurrentMidiFile at 0x51f554. The owner
// header already says Midi publishes this state directly. MusicFlags has no
// CRT initialization, Midi has a tail initializer pair, and both use the same
// base /Gy profile. That is independent semantic/address evidence for testing
// whether MusicFlags is actually Midi's prefix. No such merge is measured or
// accepted here, and a matching count alone could not justify accepting one.
//
// PRODUCTION CANDIDATE (same date, not yet integrated on master):
//
// The verified source product is now represented without a copied probe
// header: Misc.cpp owns the prefix, MiscRuntime.cpp owns the suffix, and
// MiscState.h declares their shared storage. The real track array carries
// DATA(0x0051f120). KBForward.h holds KB's existing prerequisites and forward
// declarations; KBDeclarations.h holds its declarations. KB.h remains the
// compatibility umbrella and includes the full audio owner in the original
// declaration order. Only Misc.cpp chooses the declaration-only interface.
// No function body changed. Native archive scans are BASE-prefix, Misc,
// MiscRuntime, BASE-middle, Midi, BASE-suffix. Configure uses current source
// function-owner markers to bootstrap before regenerated symbol metadata.
//
// Validation artifacts:
//   build/misc-production-{redelink,build,link}.log
//   build/misc-production-{relocs,blocks,selftest-shell}.log
//   build/misc-production-proof.json
//   build/link/rsrc/HMM2PL.exe
//
// homm2 redelink and homm2 build pass: 1727/1727 exact functions and
// 291992/291992 tracked data bytes. The three-byte coverage-denominator drop
// is the .bss contribution boundary: the original 73-byte contribution becomes
// 61 and 9 bytes, with the same three alignment bytes supplied by native LINK
// between contributions. Raw final PE section layout is unchanged.
// homm2 link --rsrc reproduces the branch's 47/47 raw-exact Misc functions,
// all 3928 initialized Misc bytes, and section mismatch counts 501/8/2/0 for
// .text/.rdata/.data/.rsrc. SetupCDDrive has 47/47 relocation owners and all
// 40 blocks exact. The complete tool suite passes 953 tests (six skips).
// Its extra compiler producer remains pending the independently tested Midi
// ownership recovery; this candidate alone does not claim whole-file closure.

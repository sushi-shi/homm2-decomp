// Unit: BASE/Misc
// Object: build/objdiff/base/BASE/Misc.obj
// Target storage:
//   pointer 0x0051e5dc
//   text    0x0051f120 (between IsCDDrive and DriveSupportsFreeSpaceQuery data)
//
// ATTEMPTED: replace the direct literal initializer with a pointer to a named
// array and reopen the default data segment at the source boundary between
// IsCDDrive and DriveSupportsFreeSpaceQuery:
//
//   extern char gcCDTrackNameText[];
//   static char* gcCDTrackName = gcCDTrackNameText;
//   ...
//   #pragma data_seg(".data")
//   char gcCDTrackNameText[] = "\\Tracks2\\02-AudioTrack 02.ogg";
//   #pragma data_seg()
//
// VC6 emitted two ordinary .data sections, but it placed both before .bss and
// every function COMDAT: section 3 held the pointer/named globals (0x1c bytes)
// and section 4 held the 0x1e-byte text.  Source placement did not put the
// second contribution at retail's position between candidate sections 43 and
// 45.  The named backing array and pragma also have no independent semantic
// evidence.
//
// Disposition: REJECTED and reverted.  Do not treat reopening the segment as a
// source-authentic replacement for the current diagnostic adapter.

// Further measured source forms:
//
//   * moving the direct pointer definition between IsCDDrive and
//     DriveSupportsFreeSpaceQuery;
//   * file-static and external inline accessors with a function-local static
//     pointer, defined both at the original global position and between those
//     helpers;
//   * a function-local static pointer in SetupCDDrive;
//   * const-pointee, external-pointer, and volatile-pointer declarations; and
//   * 50 unchanged-source declaration-forest trials.
//
// The direct-definition and file-static-inline arms retain the 0x3a-byte main
// .data section containing both pointer and literal.  The external-inline arm
// splits a 0x1e-byte literal contribution, but emits it with SetupCDDrive's
// late data contributions rather than between IsCDDrive's data and
// DriveSupportsFreeSpaceQuery.  The Setup-local arm merges the literal into
// SetupCDDrive's existing associated data.  Moving the accessor definition
// does not change either result.  All 50 state trials retain the baseline
// pointer/literal topology, and VC6 SP5 /YX does too.
//
// Artifacts:
//   build/link/misc-track-source-position/results.json
//   build/link/misc-track-state/results.json
//   build/link/project-flag-probes/misc-yx/Misc.obj
//   build/link/compiler-version-probes/Misc-{gi,gi-default,gf,nogy,rtm,sp3}.obj
//   build/link/misc-gf-link/results.json
//
// /Gi, omitted /Gi-, VC6 RTM, and VC6 SP3 preserve the baseline merged
// pointer/literal contribution.  Omitting /Gy merges the TU's data and loses
// the retail per-function contribution topology.  /Gf naturally gives the
// track text its own 0x1e-byte COMDAT, but also splits every string in the TU;
// linking that untouched object produces 37,463 .text and 11,922 .data byte
// mismatches and places the track text at file offset 0x11e5f8 instead of
// retail 0x11f120.
//
// Static/external __forceinline accessors and a return-reference accessor were
// also measured.  Static forms keep the literal in the main .data section;
// external forms emit it with SetupCDDrive's late contribution (section 47).
// A source-positioned __declspec(selectany) backing array is a standalone data
// COMDAT, but VC6 schedules global data COMDATs at section 4 regardless of its
// definition after IsCDDrive.  None produces the ordinary section-44 retail
// contribution, and all were reverted.

// VC6 language-selection spellings are also invariant.  The complete 8-arm
// matrix at build/link/source-language-spelling/results.json covers .cpp default,
// explicit /TP, .c/.C/.cc/.cxx with /TP, and .c/.C through /Tp.  Every object
// keeps the pointer and literal together in section 3 at offset 0x1c.

// Historical path-state probes add 29 unchanged-source objects:
//
//   build/link/source-path-spelling/results.json
//   build/link/include-path-spelling/results.json
//   build/link/output-path-spelling/results.json
//
// They cover repository-relative, basename, historical E: source/include/output,
// installed VC98 include, /Fd, /Fp, case, and short-name spellings.  Every object
// keeps gcCDTrackName in section 3 and its 0x1e-byte text at section-3 offset
// 0x1c.  The retail split is independent of those compiler path identities.

// Ordinary cross-object ownership probe:
//
//   build/link/misc-cross-object/results.json
//
// Misc was compiled with its section-3 pointer referring to an external writable
// array, while a second VC6 TU supplied the exact 0x1e-byte array in an ordinary
// .data section.  The backing object was placed before Misc, after Misc, first,
// and last in BASE-prefix.lib.  All four archive layouts produce the same image:
// LINK extracts the backing member after processing Misc, so the text lands at
// VA 0x0051f534 rather than retail 0x0051f120.  Each has 1,261 whole-file byte
// mismatches (56 .text and 941 .data section bytes); .rdata and .rsrc remain
// exact.  A separate archive before/after BASE-prefix lands at 0x00523db4 or
// 0x0051f534, while a direct object lands at 0x0051df64 regardless of its response
// position.  None can interleave a second object contribution between Misc's
// IsCDDrive data (candidate section 43) and DriveSupportsFreeSpaceQuery (section
// 44).  The compiler record census remains unchanged, so this rejection is based
// on actual contribution placement rather than incidental Rich metadata.
//
// Disposition: REJECT a separate backing TU or library member.  An ordinary
// source replacement must make VC6 emit the contribution inside Misc at that
// exact section boundary; archive scheduling cannot substitute for that missing
// object-internal topology.

// IsCDDrive lexical-owner diagnostic:
//
//   build/link/misc-track-placement-probe.py
//   build/link/misc-track-source-position/results.json
//
// Retail places the 0x1e-byte track text immediately after IsCDDrive's four-byte
// "A:\\" literal contribution, so four disposable variants added an unused
// function-local static pointer/const pointer/array/const array containing the
// same text.  The pointer variants do make VC6 append a second copy to
// IsCDDrive's data, but they expand that contribution from 0x4 to 0x24 bytes and
// leave the original 0x1e-byte global-initializer copy in section 3.  The array
// form instead adds the duplicate to main data; the unused const array is erased.
// All retain 79 sections, and none yields the retail single-copy 0x4 + 0x1e
// sequence.  The experiment supports the lexical placement reading but rejects
// an ordinary function-local static as its source.  No unused storage is retained.

// Additional pointer/storage ownership matrix:
//
//   build/link/misc-track-source-position/results.json
//
// Making the top-level pointer const or const volatile moves the pointer into a
// new early .rdata section while leaving the string in main .data.  A class
// static backing array merges the pointer and array in main .data.  A template
// static backing array does split an exact 0x1e standalone contribution, but VC6
// emits that COMDAT as early section 4 even when its definition appears between
// IsCDDrive and DriveSupportsFreeSpaceQuery.  An explicit specialization at that
// boundary is rejected because the pointer initializer has already instantiated
// the primary.  These ordinary C++ ownership forms cannot produce section 44.

// Real incremental-source-history probe:
//
//   build/link/incremental-source-history/results.json
//
// A stable VC6 SP5 /Zi /Gm /Gi database was seeded with a null track pointer,
// then the direct literal initializer was restored and compiled twice.  The
// transition folds the pointer and text back into the main .data contribution;
// the repeated compile retains it.  The missing split is not stale incremental
// state from adding the initializer late.

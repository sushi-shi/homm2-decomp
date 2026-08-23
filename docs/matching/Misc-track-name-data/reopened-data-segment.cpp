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

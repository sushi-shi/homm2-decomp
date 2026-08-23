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

// Unit: BASE/Misc
// Measured: 2026-09-26, matcher/bss-misc, VC6 SP5.
// Artifacts (disposable source snapshots, raw objects, complete COFF section
// and ordered relocation records, compile logs):
//   build/misc-accessor-build-matrix.py
//   build/misc-accessor-build-matrix/results.json
//
// Question: can debug-information emission or automatic inlining make a real
// accessor defined between IsCDDrive and DriveSupportsFreeSpaceQuery own the
// track literal at that boundary, without retaining an extra function?
//
// The source variants replace the original file-scope definition below with
// GetCDTrackName at that boundary and replace both SetupCDDrive reads of
// gcCDTrackName with GetCDTrackName(). No unused storage or trigger call is
// added. All six explicitly listed source/build arms compiled successfully;
// none timed out. This is a structural/build matrix, not a TU-state census.
//
// Original:
//   DATA(0x0051e5dc) static H2_CONST char* gcCDTrackName =
//       "\\Tracks2\\02-AudioTrack 02.ogg";
//
// Accessor source family (QUALIFIER is one of the four rows below):
//
//   QUALIFIER H2_CONST char* GetCDTrackName(void) {
//       static H2_CONST char* gcCDTrackName =
//           "\\Tracks2\\02-AudioTrack 02.ogg";
//       return gcCDTrackName;
//   }
//
// Flags start with unit_flags(BASE/Misc), including its /Gy and base_nogf
// profile. Appended flags are shown explicitly:
//
// source                        flags  sections  pointer             text
// original                      none      79     section 3 + 0       s3 + 0x1c
// original                      /Z7      131     section 4 + 0       s4 + 0x1c
// static accessor               /Ob2      61     section 3 + 0x18    s3 + 0xb44
// static inline accessor        /Ob2      60     section 3 + 0x18    s3 + 0xb44
// inline accessor               /Z7      133     section 74 + 0      s72 + 0
// __forceinline accessor        /Z7      133     section 74 + 0      s72 + 0
//
// Original /Z7 adds debug contributions but leaves pointer and text together
// in the same 0x3a-byte ordinary .data contribution. Both /Ob2 static-accessor
// arms merge function strings into a 0xd10-byte leading .data contribution,
// losing the retail per-function data topology. Both /Z7 external-accessor
// arms produce a 0x1e-byte text COMDAT, but immediately after SetupCDDrive's
// text section 71, not after IsCDDrive. Its pointer is another late COMDAT;
// the original leading data contribution shrinks to 0x18 bytes.
//
// Ordered relocation evidence: the original and both external-accessor arms
// retain SetupCDDrive DIR32 reads at offsets 0x13c and 0x1f4; the external
// arms target ?gcCDTrackName@?1??GetCDTrackName@@YIPADXZ@4PADA rather than the
// original _gcCDTrackName. They therefore preserve the two actual pointer
// reads while changing storage ownership; placement still rejects them.
// /Z7's extra SECTION/SECREL debug relocations are not executable references.
//
// Current semantic evidence: IsCDDrive (0xbf2b0) and
// DriveSupportsFreeSpaceQuery (0xbf2f0) both have identical instruction diffs
// against retail. IsCDDrive's block view has one exact block. The retail
// track text at 0x11f120 has one data reference, from the cell at 0x11e5dc.
// No existing access in either earlier function requires this track owner.
// Adding unused storage or a discarded accessor call there has no supporting
// semantic evidence and was not used as a candidate reconstruction.
//
// Disposition: REJECT these six build/source arms as a way to recover the
// retail layout. No game source or compiler flags are changed. This evidence
// rejects the measured ownership/build family; it does not establish a
// universal impossibility theorem for source or justify an object correction.

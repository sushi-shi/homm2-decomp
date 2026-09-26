// Target: native DIMMER tail-wrapper order with retail 16-byte method starts.
// Measured 2026-09-26, matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
// No production flags, compiler binaries, source, or objects were changed.
//
// Hypothesis: no-/Gy already gives the desired ??_G -> ??1 tail under the
// inline-destructor declaration. Could a real backend alignment control retain
// those semantics and insert the compiler's normal inter-function alignment?
// The earlier 32-arm CPU/optimization matrix was NOT repeated.
//
// Binary inspection before compilation:
// - Saved CL /? output: no public function-alignment-size option is listed.
// - C2.DLL contains one alignment-named option, "-bzalign", at raw 0x9d9a8
//   (VA 0x107a39a8). Its 12-byte option record is at raw 0x9d73c:
//      { name=0x107a39a8, target=0x1079900c, kind=0x00000101 }
//   This is a boolean control, not a numeric boundary parameter.
// - CL.EXE also contains the spelling "bzalign" at raw 0x8440, supporting a
//   direct driver spelling as well as the established /d2 backend pass-through.
// - The contiguous C2 option table raw 0x9d5c8..0x9d8b0 has 62 records.
//   Its nested -O group has a,w,s,t,g,p,y; -G has w,h,i,f,3,4,5,6,y,A,D,T,Z,X.
//   No explicit numeric function-alignment option appears in these groups.
// - The superficially possible -A# stores a string at 0x10799008; its only
//   direct reader (0x10757702) calls 0x107579cb. The nonzero branch at
//   0x1076a856 only walks the string until NUL and returns, without storing
//   alignment state. It is not evidence for a usable alignment option.
//
// Complete measured matrix:
//   source ownership: ordinary destructor / inline-declared destructor
//   backend option:   control / /d2bzalign / /bzalign
// All six arms use unit_flags(BASE/DIMMER) with only /Gy removed, plus the
// selected new option. The inline source form is the previously measured
// declaration "inline virtual ~dimmerWidget(void) OVERRIDE;" with its unchanged
// out-of-line empty body. Thus there are two controls and four new probes.
//
// All 6/6 compiled, with no unknown-option warnings, timeout, or truncation.
// Both spellings preserve the full control function records exactly: section
// index, offset, size, raw bytes, and every ordered relocation site/type/target/
// raw addend. In particular, the first five functions remain at section offsets
//   0x0, 0x2b, 0x6a, 0xe1, 0xfa
// instead of retail-relative
//   0x0, 0x30, 0x70, 0xf0, 0x110.
// The ordinary ownership form has ??1 before ??_G; the inline form has the
// desired tail ??_G then ??1 but still the incorrect first-five alignment.
// All .text sections keep native 16-byte section alignment (0x60500020 for
// the ordinary section, 0x60501020 for COMDATs); these options do not introduce
// missing inter-function alignment inside the shared ordinary contribution.
//
// Disposition: the only alignment-named option discovered is accepted but does
// not solve DIMMER. No supported force-function-align knob was found in this
// bounded help/table inspection. This is not a universal impossibility claim
// about all compiler internals, untested debug controls, or source ownership.
// No code, synthetic padding, directives, fake globals, or COFF/PE corrections
// are retained. No final link is needed to reject unchanged object placement.
//
// Artifacts under build/link/dimmer-backend-alignment/:
//   decode-options.py, c2-options.json, c2-option-groups.json
//   cl-help.log, a-option-reader.asm, a-handler*.asm, bzalign-reader.asm
//   alignment-flags.asm, asm-alignment-{emitter,pseudo}.asm
//   run.py, run.log, results.json, audit.json
//   ordinary/ and inline/: full disposable source/header and three raw objects.

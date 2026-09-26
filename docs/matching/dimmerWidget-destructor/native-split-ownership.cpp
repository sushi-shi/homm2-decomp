// Target: retail dimmerWidget scalar deleting wrapper at VA 0x004d3440.
// Measured 2026-09-26 in matcher-3, branch matcher/bss-dimmer, pinned VC6 SP5.
//
// Hypothesis: a real translation-unit split might place the deleting wrapper
// with an out-of-line destructor in a suffix object, while preserving the
// constructors and other methods in the prefix object. This is different from
// moving a definition inside the same TU (historical matrices 2 and 15).
//
// Artifacts, relative to matcher-3:
//   build/link/dimmer-native-split/{run.py,results.json,run.log}
//   build/link/dimmer-native-split/{link.py,link-results.json,link-run.log}
//   build/link/dimmer-native-split/{audit.py,audit-results.json}
//   Per-arm source/header/objects, complete linker/librarian response files,
//   linker logs, four successful EXEs, and their native LINK maps.
//
// Each source arm is compiled as two real /Gy TUs using unit_flags(BASE/DIMMER).
// The prefix defines exactly the existing two constructors, Read, Main, Draw.
// The suffix defines only the existing destructor (except the in-class arm).
// It includes only va.h and its dimmerWidget owner header. Prefix includes are
// unchanged. The private copied owner header qualifies widget.h's include as
// <BASE/widget.h>; no layout, fields, base class, or other methods change.
//
// Four complete ownership forms:
// A. Ordinary virtual destructor declaration; out-of-line body in suffix.
// B. Inline virtual destructor declaration; out-of-line body in suffix.
// C. B plus explicit inline on the suffix definition.
// D. Empty destructor defined in the class; no suffix out-of-line definition.
//
// Both archive member orders (prefix/suffix and suffix/prefix) were linked for
// each form, using ordinary VC6 LIB.EXE and LINK.EXE with all game inputs.
// The DIMMER member alone was replaced inside a rebuilt BASE-suffix.lib.
// Other library/resource prerequisites were copied from the root's fresh raw
// --rsrc build; these links are isolated diagnostics, not a fresh full build
// or root integration result. No /INCLUDE, /ORDER, synthetic uses, pragmas,
// padding, symbol edits, object transforms, or executable patches were used.
//
// Completeness: all eight objects compiled. All eight native links ran to a
// terminal result: four succeeded; four reported the expected unresolved
// destructor. No matrix timeout/truncation occurred.
//
// Raw objects:
// A prefix: ctor(), vtable, ??_G, ctor(args), Read, Main, Draw.
// A suffix: ??1, vtable, ??_G. Thus a competing wrapper exists, but its local
//           order is ordinary destructor before deleting wrapper.
// B/C prefix: same early ??_G as A. B/C suffix: no DIMMER method is emitted.
// D prefix: ctor(), vtable, ??_G, ??1, ctor(args), Read, Main, Draw.
// D suffix: no DIMMER method is emitted.
//
// Native links, identical for the two member orders:
// A: ctor() 0x004d3310; ??_G 0x004d3340 (PREFIX wins); ctor(args) 0x004d3370;
//    Read 0x004d33b0; Main 0x004d3430; Draw 0x004d3450;
//    ??1 0x004d68b0 (suffix extracted later than the other BASE members).
// B/C: LNK2001 ??1dimmerWidget@@UAE@XZ, then LNK1120. An inline body hidden
//      exclusively in the suffix cannot supply the prefix's required body.
// D: ctor() 0x004d3310; ??_G 0x004d3340; ??1 0x004d3370;
//    ctor(args) 0x004d3390; Read 0x004d33d0; Main 0x004d3450; Draw 0x004d3470.
//
// Complete byte/relocation audit:
// - Every one of the 27 emitted DIMMER method instances in the eight raw
//   objects equals the production baseline in raw bytes and ordered relocation
//   site/type/identity/addend. The preceding native-alignment dossier separately
//   verified that baseline against the current retail delink target.
// - All seven functions in each of the four successful native links were read
//   from the PE. Their expected complete bytes were reconstructed from the
//   audited raw methods and every DIR32/REL32 map destination/addend. All 28
//   agree, so competing-copy selection is not concealing a body difference.
// - This proves method content under the measured layouts, not retail layout:
//   all successful links put the wrapper at 0x004d3340, not 0x004d3440.
//
// Disposition: all four structural forms and both member orders are rejected.
// This specific ordinary prefix/destructor-suffix ownership split does not
// replace the correction. No production source/header/build change retained.
// Do not generalize the result to every conceivable original object ownership;
// further work needs a concrete use/ownership fact beyond these declarations.

// A: owner header
//     virtual ~dimmerWidget(void) OVERRIDE;
//    suffix source
//     dimmerWidget::~dimmerWidget() {}
//
// B: owner header
//     inline virtual ~dimmerWidget(void) OVERRIDE;
//    suffix source
//     dimmerWidget::~dimmerWidget() {}
//
// C: owner header
//     inline virtual ~dimmerWidget(void) OVERRIDE;
//    suffix source
//     inline dimmerWidget::~dimmerWidget() {}
//
// D: owner header
//     virtual ~dimmerWidget(void) OVERRIDE {}
//    suffix source contains no out-of-line definition.

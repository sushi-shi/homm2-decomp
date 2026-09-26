// Target: all seven exact DIMMER methods AND retail contribution placement.
// Measured 2026-09-26 in matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
//
// Hypothesis: an optimizing TU-level backend setting might retain native
// function-entry alignment while a legitimate per-function optimization pragma
// restores the retail /Od bodies. This differs from changing global flags alone.
//
// Final complete matrix (12/12 compiled; no warnings/timeouts/truncation):
//   destructor ownership (2): ordinary / inline-declared out-of-line empty body
//   global build family  (2): baseline /Od plus /Og; expanded speed flags below
//   method pragma        (3): absent / optimize("", off) / optimize("g", off)
// The selected pragma appears ONCE, immediately before the first method's VA
// marker. Every existing method definition/body remains otherwise unchanged.
// No explicit /Gy appears in either final build family, and raw objects confirm
// that the first five methods share the main .text contribution.
//
// Expanded speed family: baseline flags with /Od and /Gy removed, followed by
//   /Og /Oi /Ot /Oy- /Ob1 /Gs
// This separates speed/intrinsic controls from implicit function packaging and
// keeps the retail frame/inlining policy. It is not labelled literal /O2.
//
// Diagnostic setup caveat, preserved separately:
// Literal /O2 implicitly enabled /Gy. Attempting to append /Gy- produces
//   Command line warning D4002 : ignoring unknown option '/G-'
// and leaves packaging enabled. Those six-arm O2 families do NOT count as
// no-/Gy coverage. Their objects/logs/results are retained as diagnostics;
// the final complete matrix uses the expanded speed family above instead.
//
// Outcome:
// - All eight pragma-off arms restore every one of the seven method bodies,
//   with exact raw bytes and complete ordered relocation offsets/types/targets/
//   addends relative to the audited production baseline. They also restore the
//   unaligned first-five offsets 0x0,0x2b,0x6a,0xe1,0xfa.
// - The four no-pragma optimizing controls use 16-byte starts, but their method
//   bodies differ and first-five starts are 0x0,0x20,0x60,0xc0,0xd0.
// - Required retail-relative starts are 0x0,0x30,0x70,0xf0,0x110.
//   No arm satisfies both exact bodies/relocations and those starts.
// - Ordinary ownership still places ??1 in main .text before the later ??_G
//   COMDAT. Inline ownership orders ??_G then ??1 as separate COMDATs, but the
//   main section also owns _$E19 and _$E18 before both wrappers. In the exact-
//   body inline arms those helpers start at main-section offsets 0x10d/0x134;
//   this is another contribution-order difference, not proof of retail closure.
//
// Disposition: rejected. Per-function optimization state and native alignment
// remain coupled in these measured families. No source pragma or flag retained.
// In particular, restored bytes alone are not sufficient and no alignment-only
// improvement is accepted. No final link is needed to reject these object layouts.
//
// Artifacts under build/link/dimmer-pragma-alignment/:
//   run.py, run.log, results.json
//   <ownership>-<profile>-<pragma>/{DIMMER.cpp,dimmerWidget.h,DIMMER.obj}
//   initial-O2-implies-Gy-results.json
//   O2-Gy-minus-rejected-results.json
// Results retain every code symbol, all seven raw method spans and ordered
// relocations, and independent exact-body/retail-offset predicates.

// Source alternatives (one selected before first method):
//   // control: no pragma
//   #pragma optimize("", off)
//   #pragma optimize("g", off)
//
// Destructor declaration alternatives:
//   virtual ~dimmerWidget(void) OVERRIDE;
//   inline virtual ~dimmerWidget(void) OVERRIDE;
// Definition in both cases remains:
//   dimmerWidget::~dimmerWidget() {}

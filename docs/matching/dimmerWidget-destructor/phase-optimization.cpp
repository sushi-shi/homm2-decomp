// Target: recover DIMMER's exact seven method bodies and retail placement.
// Measured 2026-09-26 in decomp-gold-2.1-buka with the pinned VC6 toolchain.
//
// Hypothesis: C2-only optimization could provide entry alignment independently
// of C1XX's /Od lowering. Earlier global /Og and pragma matrices changed both
// bodies and alignment; they did not isolate the driver phase arguments.
//
// Complete matrix: two ownership forms (ordinary/inline-declared destructor)
// x four flag sets (production without /Gy, then that baseline plus /Og,
// /d2Og, or /d2Ot). All eight compiled, without warnings or timeouts.
// The source definitions and method bodies are unchanged in every arm.
//
// Driver validation: /Bd traces for control, /d2Og, and /Og establish:
// - /d2Og adds -Og to C2's arguments, leaving C1XX's arguments unchanged.
// - ordinary /Og adds -Og to C1XX, not C2's command line.
// These are actual phase arguments, not an assumption based on flag spelling.
// The per-function intermediate representation remains relevant: a C2 argument
// is not proof that it overrides the function's front-end optimization state.
//
// Outcomes:
// - Control, /d2Og, and /d2Ot preserve all seven raw bodies and complete ordered
//   relocation offsets/types/identities/addends from the production /Gy object.
// - All six such arms also retain unaligned first-five offsets:
//     0, 0x2b, 0x6a, 0xe1, 0xfa.
// - Both global-/Og controls alter method bodies; their first-five offsets are
//     0, 0x20, 0x60, 0xc0, 0xd0.
// - Required retail offsets remain 0, 0x30, 0x70, 0xf0, 0x110.
// - Ordinary ownership puts the ordinary destructor in main .text before the
//   scalar COMDAT. Inline ownership has scalar then ordinary COMDATs, but the
//   main .text also contains the ctype initialization helpers before them.
// No arm recovers the full retail layout. No final link is needed to reject
// these already contradictory raw-object layouts. No flags or source retained.
//
// Artifacts under build/link/dimmer-phase-optimization/:
//   run.py, run.log, baseline.json, results.json
//   driver_trace.py, driver_trace.log, *-trace.log
//   <ownership>-<profile>-control/{DIMMER.cpp,dimmerWidget.h,DIMMER.obj}
// Baseline is freshly extracted from root's production raw DIMMER.obj.
// Results retain every method span, all ordered relocations, and all code-symbol
// section/offset records. Exact-prefix comparison excludes only interfunction
// padding; relocation comparison is unmasked and complete for each span.

// Destructor declaration alternatives:
//   virtual ~dimmerWidget(void) OVERRIDE;
//   inline virtual ~dimmerWidget(void) OVERRIDE;
// Definition in both arms:
//   dimmerWidget::~dimmerWidget() {}

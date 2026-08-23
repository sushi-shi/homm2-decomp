// Final-link source-shape experiment; not compiled source.
//
// Target: BASE/DIMMER deleting-destructor placement.
// Candidate before/after: ??_GdimmerWidget@@UAEPAXI@Z at RVA 0xd3340.
// Retail: RVA 0xd3440, after Draw and before the explicit destructor.
// Artifact: build/link/HMM2PL.link.json (2026-08-22 local rebuild).
// Matrix completeness: one evidence-backed dialect arm plus the clean baseline.
// Object result: all 10 DIMMER functions and data remained exact under
// functionRelocDiffs=all.
// Final-link result: byte-identical to the clean candidate; 92.529748% whole
// image and 1700/1727 project RVAs exact. The scalar deleting destructor stayed
// at 0xd3340, so this arm is rejected and the clean declaration was restored.

// Attempted declaration:
virtual inline ~dimmerWidget(void) OVERRIDE;

// Broader destructor-dialect matrix:
//
//   build/link/dimmer-dtor-shapes/results.json
//
// Eleven complete VC6 SP5 /Gy arms cover redundant `virtual`/`OVERRIDE`
// omission, `()` versus `(void)`, inline and non-inline declarations, and empty
// bodies defined inside or outside the class.  All seven out-of-class forms are
// byte- and topology-identical to baseline: the default constructor is section
// 3, the vtable is section 4, and the scalar deleting destructor is section 5.
// The four in-class bodies keep all seven function bodies exact, but move the
// ordinary destructor to section 6; the deleting destructor remains section 5
// and the second constructor moves to section 7.  No arm has retail order, so
// none qualifies for an end-to-end link trial.

// The independent unchanged-source census at
// build/link/dimmer-comdat-state-census/results.json compiled 50 declaration-
// forest states.  Every state retained sections 3/5/6/7/8/9/10 for default
// constructor, deleting destructor, second constructor, Read, Main, Draw, and
// ordinary destructor, with exact section payload/relocation fingerprints.
// Moving the destructor declaration to the end of the class and removing the
// explicit destructor were also measured in
// build/link/comdat-source-order/results.json.  The former is identical; the
// latter moves the ordinary destructor early and still leaves the deleting
// destructor immediately after the vtable.

// These results identify the mechanism: VC6 emits the class vtable and its
// compiler-generated deleting destructor when the first constructor is emitted.
// Destructor declaration spelling cannot delay that contribution until after
// Draw.  Reordering the constructor definitions would also move the exact
// constructor contributions away from their retail order and is structurally
// contradicted.

// Historical language-selection matrix:
//
//   build/link/source-language-spelling/results.json
//
// Eight .cpp/.c/.C/.cc/.cxx inputs using default C++, /TP, and per-file /Tp all
// emit the scalar deleting destructor as section 5, before the second constructor
// and the ordinary methods.  Source suffix and language-option spelling do not
// expose the retail placement after Draw.

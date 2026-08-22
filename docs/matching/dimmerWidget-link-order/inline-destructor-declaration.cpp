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

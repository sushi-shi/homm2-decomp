// Target: scalar deleting destructor at retail VA 0x004d3440.
// Question: can ordinary no-/Gy code generation recover the tail order AND
// the independent 16-byte boundaries, without a COFF section-order correction?
//
// Measured 2026-09-26 in matcher-3, branch matcher/bss-dimmer, with the pinned
// VC6 SP5 build environment. No generated source or probe declarations retained.
// Artifact directory (relative to this worktree):
//   build/link/dimmer-native-alignment/
//   run.py, baseline.obj, baseline.json, results.json, run.log, and 32 objects.
//   audit.py and retail-audit.json compare all seven raw method payloads and
//   complete ordered relocation sites/types/identities/addends with the current
//   delink target: all seven are equal, before masking relocation bytes.
//
// The matrix used unit_flags(BASE/DIMMER), removed /Gy, replaced /G5 with each
// of /G3 /G4 /G5 /G6, and selected one optimization suffix:
//   A. /Od                     (unchanged baseline option)
//   B. /Od /Og                 (enable global optimization)
//   C. /Od /Os                 (favor size)
//   D. /Od /Ot                 (favor speed)
//
// These 16 build states were crossed with BOTH source ownership forms:
//   1. Existing virtual ~dimmerWidget(void) declaration and out-of-line body.
//   2. inline virtual ~dimmerWidget(void) declaration; same out-of-line body.
//
// Thus all 32 requested combinations compiled; no timeout/truncation occurred.
// The generated header only rewrites its relative widget.h include to the
// equivalent owner-qualified <BASE/widget.h> so disposable compilation works.
// All seven DIMMER method payloads and each ordered relocation site/type/name/
// raw addend were recorded. Function bodies were compared against a freshly
// compiled production-/Gy baseline; prefix comparison permits real compiler
// padding after a body when functions share one section.
//
// Result:
// - /G3, /G4, /G5 with A or D preserve all seven bodies and ordered relocations.
//   But the main section's first five functions start at
//       0x0, 0x2b, 0x6a, 0xe1, 0xfa
//   rather than retail-relative 0x0, 0x30, 0x70, 0xf0, 0x110.
//   Ordinary destructor ownership also leaves ??1 before ??_G. Inline ownership
//   gives the attractive tail ??_G then ??1, but retains the bad first five starts.
// - B (/Og) aligns starts to 16 bytes, but changes all seven method bodies.
//   First five starts are 0x0, 0x20, 0x60, 0xc0, 0xd0; neither bytes nor layout
//   are retail-compatible.
// - C (/Os) changes bodies, without obtaining the independent retail boundaries.
//   First five starts are 0x0, 0x29, 0x5a, 0xcf, 0xe5.
// - /G6 with A or D changes the argument constructor and its relocation sites;
//   first five starts become 0x0, 0x2b, 0x76, 0xed, 0x106.
//
// Fresh baseline evidence: homm2 sema disasm 0xd3440 --diff --lite reports
// identical 20-instruction assembly; block diff has 3 exact blocks; the raw
// wrapper is 0x2e bytes and its two REL32 relocations are at +0xb -> ??1 and
// +0x1e -> operator delete. Both candidate and delink target agree. Their
// physical order, rather than wrapper semantics, remains the problem.
//
// Disposition: rejected all 32 states. CPU tuning and these standard native
// optimization/alignment modes do not replace /Gy while preserving retail
// methods. This extends matrix 7 of comdat-order-matrix.cpp; it does not prove
// that every possible build/ownership state is exhausted. No source or build
// flag change is retained. The remaining credible avenue is evidence about
// original link contribution ownership/selection, not additional equivalent
// method spellings or synthetic alignment instructions.

// Source form 1 (baseline):
// class dimmerWidget : public widget {
// public:
//     virtual ~dimmerWidget(void) OVERRIDE;
//     // Other declarations unchanged.
// };
// dimmerWidget::~dimmerWidget() {}

// Source form 2 (probe):
// class dimmerWidget : public widget {
// public:
//     inline virtual ~dimmerWidget(void) OVERRIDE;
//     // Other declarations unchanged.
// };
// dimmerWidget::~dimmerWidget() {}

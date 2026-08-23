// VC6-measured source-shape ledger for AudiereSampleNode::~AudiereSampleNode.
// Target RVA: 0x004cd050, size 0x2c.  The clean inline declaration plus
// end-of-TU definition is byte-exact and follows the last public function, but
// its /Gy COMDAT still precedes the TU's RefPtr helpers and $E21/$E20 static
// helpers.  Retail emits it after all five helpers.
//
// Compiler artifact used for both arms:
//   build/objdiff/base/BASE/AudiereEffects.obj
// The ordinary build path was compiled and inspected with llvm-objdump/llvm-readobj;
// the source and artifact were restored to the exact clean state after measurement.

// Arm 1: explicit inline class-body destructor.
//
// struct AudiereSampleNode {
//     ...
//     ~AudiereSampleNode() {}
// };
//
// Result: byte- and section-identical to the clean implicit destructor. The order
// remains Purge -> AudiereSampleNode dtor -> Find -> Play -> ... . Rejected because
// it cannot affect the first linked divergence.

// Arm 2: declaration in soundBackends.h, definition after the last public function.
//
// struct AudiereSampleNode {
//     ...
//     ~AudiereSampleNode();
// };
// AudiereSampleNode::~AudiereSampleNode() {}
//
// Result: the destructor moves after AudiereSampleIterationActive, but still precedes
// RefPtr<OutputStream>::~RefPtr, RefPtr<OutputStream>::operator=,
// RefPtr<AudioDevice>::~RefPtr, $E21, and $E20. Retail needs it after all five.
// Worse, PlayAudiereSample shrinks from the exact retail 0x39c bytes to 0x385 because
// VC6 exposes/inlines the explicit definition. Rejected as a semantic-codegen
// contradiction, not merely a lower fuzzy score.

// Arm 3: Gruntz-style next-TU first-definer ownership.
//
// Declaring the destructor in soundBackends.h but defining it at the start of
// AudiereMusic.cpp puts its exact 0x2c contribution at retail RVA 0x004cd050 and
// makes AudiereMusic object-exact.  However, PlayAudiereSample changes to 95.10432%
// and gains a REL32 call to the now non-inline destructor (41 candidate relocation
// references versus 39 retail).  Retail therefore requires the implicit/inlinable
// ownership visible to AudiereEffects; the physically attractive next-TU owner is
// rejected by code and relocation evidence.

// Unchanged-source compiler-state census:
//
//   build/link/audiere-comdat-state-census/results.json
//   50 declaration-forest trials, insertion=top, seed=0x41554449
//
// All 50 compiled in the same relative orbit.  The node destructor was section 7;
// the three retail-predecessor RefPtr helpers were sections 8, 9, and 10.  No trial
// moved the destructor after them.

// Project-flag probe:
//
//   build/link/project-flag-probes/audiere-yx/AudiereEffects.obj
//
// Compiling the unchanged TU with VC6 SP5 /YX preserves the same destructor,
// RefPtr-helper, and static-exit-helper section order.  Automatic PCH state does
// not expose the retail orbit.

// Compiler/header/process probes:
//
//   build/link/compiler-version-probes/AudiereEffects-{rtm,sp3}.obj
//   build/link/batch-compile-probes/
//   build/link/compiler-version-probes/AudiereEffects-explicit-pch.obj
//   build/link/compiler-version-probes/AudiereEffects-sp5-vc5headers.obj
//   build/link/audiere-project-flag-matrix/results.json
//
// VC6 RTM, SP3, and SP5 front ends retain the same order.  Compiling
// AudiereEffects/AudiereMusic together in either CL source order does too.  An
// explicit /Yc + /Yu PCH boundary through soundBackends.h is identical to the
// clean object.  SP5 paired with Microsoft's authentic VC5 header set preserves
// the six-symbol order as well (and changes the .drectve defaults, so it is not
// the retail header surface).
//
// A bounded project-state matrix covered /Zi, /Z7, /Gf omission, /Gi and /Gi
// omission, /GX omission, /Ob0, /Ob2, /G6, /GR, /Gm, /FD, /Oi, /Oa, and /Ow.
// Every arm which still emitted all six target helpers kept their relative
// order as node destructor, three RefPtr helpers, $E21, $E20.  /GX omission
// suppresses required cleanup functions and is structurally contradicted.

// Historical path-state matrix:
//
//   build/link/source-path-spelling/results.json
//   build/link/include-path-spelling/results.json
//   build/link/output-path-spelling/results.json
//
// Eleven source spellings, eight header-search spellings, and ten output/PDB/PCH
// spellings were compiled with the production /Gy flags.  These cover repository-
// relative and basename forms, the evidenced E:\Users\igorl\VSS\HMM\HMM2 tree,
// installed-VC98 and short-name system include paths, and the historical
// temp\release\game output directory.  All 29 objects retain sections 18..23 as
// node destructor, three RefPtr helpers, $E21, $E20.  Path hashing and output-name
// state therefore do not account for the retail order.

// Historical language-selection matrix:
//
//   build/link/source-language-spelling/results.json
//
// Eight .cpp/.c/.C/.cc/.cxx forms using default C++, /TP, and per-file /Tp all
// retain that same six-section order.  The reconstructed suffix and the spelling
// used to force C++ mode are not the missing compiler state.

// Template-owner diagnostics (measured and reverted): explicit specializations
// of only the three live RefPtr members move those exact bodies before the node
// destructor, but VC6 still emits $E21/$E20 after it.  Explicit ctype<unsigned
// short>::id specialization changes BSS/initializer topology, while explicit
// instantiation retains the baseline order and adds storage.  A header-inline
// node destructor plus an unused AudiereMusic destroy helper emits no duplicate
// destructor in AudiereMusic.  These arms reject both a standard-header queue
// explanation and the simplest Gruntz-style adjacent-TU first-definer shape.

// Gruntz linker-model and /ORDER probe:
//
//   build/link/order-comdat-probe.py
//   build/link/order-comdat-probe/retail-public.order
//   build/link/order-comdat-probe/HMM2PL.map
//
// Gruntz's LINK 5.10 disassembly and direct probes establish that a .text
// contribution is appended whole in object-arrival order; the only apparent
// cross-object interleaving is first-definer selection among duplicate COMDATs.
// AudiereEffects is the sole candidate object defining this destructor, so that
// mechanism has no second body to select.  A real LINK 6.00 /ORDER probe rebuilt
// BASE-suffix from untouched AudiereEffects and DIMMER objects and supplied all
// 2,641 public MAP symbols in retail order.  LINK rejects ordinary NoDuplicates
// project functions with LNK4065 and cannot name the local $E21/$E20 helpers
// (LNK4037); accepted Any-selection COMDATs instead form a global front band.
// The node destructor lands at 0x00417fd0 rather than 0x004cd050.  /ORDER cannot
// express the required position after two object-local counter helpers and is
// not a transformation-free substitute for the reviewed section move.

// Disposition: retain the authentic inline destructor and classify its final-link
// COMDAT position as an original object/compiler ownership wall. No explicit dtor,
// source padding, /ORDER directive, or synthetic linker root is retained.

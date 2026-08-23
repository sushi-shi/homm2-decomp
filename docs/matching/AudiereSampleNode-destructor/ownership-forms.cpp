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

// Force-inline spelling probe:
//
//   build/link/comdat-source-order/results.json
//
// Replacing inline with __forceinline on both the declaration and end-of-TU
// definition preserves the baseline six-symbol order.  Defining the
// __forceinline destructor in the class emits it earlier still (section 5 in
// that probe).  Neither spelling exposes the retail tail orbit.

// Minimal-rebuild flag-transition probe:
//
//   build/link/minimal-rebuild-comdat/results.json
//
// A real VC6 SP5 /Zi /Gm database was seeded without /Gy, then the unchanged TU
// was rebuilt with /Gy and compiled once more.  The transition emitted fresh
// per-function COMDATs in the same relative order as a separate fresh /Gy /Gm
// build: node destructor, three RefPtr helpers, $E21, $E20.  The third compile
// reported "Skipping... (no relevant changes detected)" and retained that
// order.  Minimal rebuild does not carry the non-/Gy layout into the /Gy object.

// Real incremental-source-history probe:
//
//   build/link/incremental-source-history/results.json
//
// A stable VC6 SP5 /Zi /Gm /Gi database was first compiled without the node
// destructor definition.  Restoring it and compiling twice emits the new node
// destructor before the three RefPtr helpers and $E21/$E20 on both passes.  The
// compiler does not append a newly introduced definition after existing helper
// COMDATs, so ordinary incremental compilation cannot produce the retail tail.

// A complementary late-use history kept the inline definition but seeded the
// same real /Zi /Gm /Gi database with all three AudiereSampleNode delete
// expressions absent.  Restoring those delete sites newly materializes the node
// destructor in section 31, before the three RefPtr helpers and $E21/$E20 in
// sections 33..41.  The repeated compile keeps that order.  Delaying first
// semantic use therefore does not append the COMDAT at the retail tail either
// (`AudiereEffects_deletes_late` in the same results file).

// Mixed compiler-phase and residual build-mode probes:
//
//   build/link/mixed-vc6-phases/{results.json,topology.json}
//   build/link/residual-flag-probes/results.json
//
// The complete local Cartesian matrix crossed both distinct VC6 drivers, all
// RTM/SP3/SP5 C++ front ends, and all RTM/SP3/SP5 back ends.  Four combinations
// with the SP5 front end and an older back end are interface-incompatible; all
// fourteen compiling combinations retain node destructor -> three RefPtr
// helpers -> $E21 -> $E20.  /Zi, /ZI (which is incompatible with the retail
// /Ob1 state), /ZI with /Gm, /FD, or /FR, browse information alone, /O2, and
// explicit /Gf-/GF- likewise expose no tail-destructor orbit.  Compiler service-
// pack phase mixing and the remaining ordinary IDE build modes are rejected.

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

// Private-type and constructor-boundary matrix:
//
//   build/link/audiere-node-ownership/results.json
//
// No other TU uses AudiereSampleNode, so six further arms moved the complete
// struct from soundBackends.h into AudiereEffects.cpp and varied its constructor
// and destructor between class-body, pre-function out-of-class, and end-of-TU
// inline definitions. Keeping the class-body constructor preserves the exact
// Purge, Play, and node-destructor fingerprints, but also preserves the baseline
// node dtor -> three RefPtr helpers -> $E21 -> $E20 order. Every out-of-class
// constructor arm retains that order and changes PlayAudiereSample; implicit or
// class-body destructor arms move the destructor earlier and also change Play.
// Private versus shared-header ownership is therefore not the missing queue
// state, and the exact in-class constructor shape remains evidence-backed.

// Include-order matrix:
//
//   build/link/audiere-include-order/results.json
//
// Seven dependency-valid include orders (baseline plus six permutations of
// audiere.h, soundBackends.h, soundManager.h, and KB.h entry points) all
// compile. Include order does steer template instantiation: four arms flip the
// RefPtr<AudioDevice> destructor ahead of the RefPtr<OutputStream> pair, and
// the kb_first arm additionally changes the PlayAudiereSample body. But every
// arm keeps ??1AudiereSampleNode in the leading COMDAT slot (section 18,
// dtor_after_refptrs false): the end-of-TU inline destructor is emitted at its
// first use inside Purge, which precedes every RefPtr instantiation regardless
// of which header enters the TU first. Header entry order therefore cannot
// supply the retail three-RefPtr -> $E21 -> $E20 -> node-dtor tail, and the
// object adapter's reviewed section move remains the bounded residual.

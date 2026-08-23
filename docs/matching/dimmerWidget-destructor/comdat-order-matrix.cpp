// Target: dimmerWidget deleting-destructor placement at retail VA 0x004d3440.
// Candidate baseline places ??_GdimmerWidget at 0x004d3340, immediately after
// the first constructor.  Retail order is:
//
//   ctor() -> ctor(args) -> Read -> Main -> Draw -> ??_G -> ??1
//
// Measured with the pinned VC6 SP5 compiler and the BASE/DIMMER flags.  Probe
// objects were disposable `/tmp/DIMMER-*.obj` files; the retained result below
// records the complete bounded matrices and their disposition.

// Matrix 1: all six declaration orders of these three overrides.
//
//   virtual ~dimmerWidget(void);
//   virtual void Draw(void);
//   virtual MessageDispatchResult Main(tag_message&);
//
// Result: all six emitted identical section order.  Declaration order does not
// move the deleting-destructor COMDAT.

// Matrix 2: destructor ownership forms.
//
//   A. out-of-line declaration and definition (baseline)
//   B. inline declaration plus inline out-of-line definition
//   C. inline qualifier on the definition only
//   D. empty definition in the class declaration
//   E. no derived destructor declaration or definition
//
// A-C preserve the retail-sized 0x1c ordinary destructor but keep ??_G early.
// D emits both ??_G and the 0x1c ??1 early.  E emits both early and changes ??1
// to 0x13 bytes.  D-E are structurally contradicted by retail placement; E is
// also contradicted by size.

// Matrix 3: project-state flags.
//
//   /Gi-, omitted /Gi, /Gi, /Zi, /Z7, /Zi /Gi, and /GX omitted
//
// Every valid arm kept ??_G immediately after the first constructor.  /Gi plus
// /Z7 is rejected by VC6 as an incompatible option pair.

// Matrix 4: constructor inline ownership.
//
//   A. default constructor inline
//   B. argument constructor inline
//   C. both constructors inline, under /Ob0, /Ob1, and /Ob2
//
// A or B suppresses that constructor and leaves ??_G after the remaining
// constructor.  C suppresses both constructors and delays the vtable/??_G block
// until after Read, Main, Draw, and ??1.  This is the only different emission
// orbit observed, but it cannot represent retail: both constructors are live,
// retail-ordered DIMMER bodies, and neither /Ob0 nor /Ob2 restores them.

// Cross-TU duplicate check: BASE/WINDOW is the only other TU that includes and
// uses dimmerWidget.  Compiling it with an inline empty destructor emits no
// dimmerWidget destructor or vtable COMDAT, so it cannot provide a later copy
// for LINK to select.

// Family check: the retail compgen inventory contains fourteen other BASE
// scalar deleting destructors.  Their ordinary first-constructor-adjacent
// placement is reproduced by the candidate link; DIMMER is the exceptional
// tail-placed wrapper, so this is not a general VC6/LIBCMT address shift.

// Matrix 5: unchanged-source compiler-state census.
//
//   build/link/dimmer-comdat-state-census/results.json
//   50 declaration-forest trials, insertion=top, seed=0x44494d4d, /Gy retained
//
// All 50 compiled.  Every one emitted the same relative section order:
// default constructor (3), deleting destructor (5), argument constructor (6),
// Read (7), Main (8), Draw (9), ordinary destructor (10).  All seven section
// payload/ordered-relocation fingerprints remained exact.  No state put ??_G
// after Draw, so the candidate orbit is invariant across the bounded census.

// Matrix 6: automatic PCH state.
//
//   build/link/project-flag-probes/dimmer-yx/DIMMER.obj
//
// VC6 SP5 /YX preserves the baseline section and symbol order, including the
// deleting destructor immediately after the first constructor.

// Matrix 7: compiler generation, batch process, explicit PCH, and /Gy removal.
//
//   build/link/compiler-version-probes/DIMMER-{rtm,sp3}.obj
//   build/link/compiler-version-probes/DIMMER-nogy-inline-decl.obj
//   build/link/compiler-version-probes/DIMMER-explicit-pch.obj
//   build/link/batch-compile-probes/
//
// VC6 RTM, SP3, and SP5 all retain the baseline /Gy order.  Compiling WINDOW
// and DIMMER in either order in one CL process does too, as does an explicit
// /Yc + /Yu boundary through dimmerWidget.h.  Without /Gy, the ordinary methods
// do occur in logical source order; an inline destructor declaration also puts
// deleting then ordinary destructor at the tail.  But the merged .text starts
// are 0x0, 0x2b, 0x6a, 0xe1, 0xfa, while retail requires independent 16-byte
// boundaries at 0x0, 0x30, 0x70, 0xf0, 0x110.  The apparently attractive order
// therefore destroys exact function placement and is rejected.

// Matrix 8: minimal-rebuild transition from no /Gy to /Gy.
//
//   build/link/minimal-rebuild-comdat/results.json
//
// A real VC6 SP5 /Zi /Gm database was seeded with the unchanged TU and no /Gy,
// then rebuilt with /Gy and compiled once more.  The transition emitted fresh
// COMDATs in the ordinary candidate order: default constructor, deleting
// destructor, argument constructor, Read, Main, Draw, ordinary destructor.
// A separate fresh /Gy /Gm object has the same relative order; the third shared-
// database compile reported "Skipping... (no relevant changes detected)" and
// retained it.  /Gm does not preserve the attractive merged-text tail order
// across the flag change.

// Matrix 9: historical source, include, and compiler-output paths.
//
//   build/link/source-path-spelling/results.json
//   build/link/include-path-spelling/results.json
//   build/link/output-path-spelling/results.json
//
// Eleven source spellings, eight header-search spellings, and ten /Fo-/Fd-/Fp
// spellings cover the evidenced E:\Users\igorl\VSS\HMM\HMM2 source and
// temp\release\game trees, an installed VC98 include path, relative paths, case,
// and short names.  All 29 production-/Gy objects retain ctor section 3,
// deleting-destructor section 5, and the remaining methods in sections 6..10.
// Compiler path identity is not the missing emission-order state.

// Matrix 10: real /Gi + /Gm source-history transition.
//
//   build/link/incremental-source-history/results.json
//
// A stable VC6 SP5 /Zi /Gm /Gi database was seeded with the ordinary destructor
// definition absent, then the definition was restored and the TU was compiled
// twice.  Both restored objects retain default constructor, deleting destructor,
// argument constructor, Read, Main, Draw, ordinary destructor.  Adding the real
// destructor late does not make the already-generated deleting wrapper follow it.

// Matrix 11: older compiler generation.
//
//   build/link/compiler-version-probes/DIMMER-vc42.obj
//   build/link/vc5-DIMMER.cpp
//
// An isolated DIMMER TU was compiled with the locally pinned VC5 compiler and
// with the sibling Price-of-Loyalty project's pinned VC4.2 compiler
// (CL 10.20.6166).  VC5 emits constructor, vtable, deleting destructor, then the
// argument constructor and remaining methods: the same early-wrapper orbit.
// VC4.2 /Gy /GX also emits the wrapper before the argument constructor.  It is
// additionally code-incompatible with Buka: its exception-aware default
// constructor is 0x7b bytes instead of the exact VC6/retail 0x2b bytes and has
// an .xdata contribution and CxxFrameHandler relocations.  Thus the matching
// tail order in Price of Loyalty is not a reusable older-compiler object orbit;
// its different optimized source/flags cannot supply Buka's exact /Od bodies.

// Matrix 12: late constructor history.
//
//   build/link/incremental-source-history/results.json
//   key: DIMMER_constructors_late
//
// The seed omitted both constructor definitions, which delays Read, Main, Draw,
// the ordinary destructor, and the deleting wrapper into a different partial-
// object orbit.  Restoring both constructors under the same real /Zi /Gm /Gi
// database regenerates the complete object in the ordinary order: default
// constructor, deleting destructor, argument constructor, Read, Main, Draw,
// ordinary destructor.  A repeat compile is identical.  Late constructor
// introduction does not preserve the attractive seed order.

// Matrix 13: mixed compiler phases and remaining IDE modes.
//
//   build/link/mixed-vc6-phases/{results.json,topology.json}
//   build/link/residual-flag-probes/results.json
//
// Both distinct VC6 drivers were crossed with RTM/SP3/SP5 C++ front ends and
// RTM/SP3/SP5 back ends.  Four SP5-front-end/older-back-end combinations reject
// their intermediate interface; all fourteen compiling combinations retain the
// early deleting destructor.  /Zi, /ZI with and without /Gm, /FD, or /FR,
// browse information, /O2, and explicit /Gf-/GF- also keep the wrapper between
// the two constructors.  The Price of Loyalty target's similar tail placement
// is therefore not explained by optimization level, service-pack phase mixing,
// or an ordinary IDE build switch.

// Cross-version ownership evidence:
//
//   investigation/reports/pol-project-tree.txt
//   investigation/reports/pol-compilands.txt
//
// The earlier Price of Loyalty executable has the same exceptional order--two
// constructors, Read, Main, Draw, then the scalar deleting destructor--and its
// CodeView inventory attributes the whole 0x004dd330..0x004dd440 range and the
// vtable to .\Win32_RE\DIMMER.OBJ.  The tail wrapper is therefore not a Buka-
// only adjacent-object ownership accident.

// Native /ORDER probe:
//
//   build/link/order-comdat-probe.py
//   build/link/order-comdat-probe/HMM2PL.map
//
// BASE-suffix was rebuilt from the untouched raw DIMMER and AudiereEffects
// objects and LINK 6.00 received a 2,641-symbol retail-order file.  Ordinary
// project functions use NoDuplicates COMDAT selection and are rejected from the
// order file with LNK4065, while accepted Any-selection compiler COMDATs move to
// a global front band.  ??_GdimmerWidget lands at 0x0041dfb0 rather than retail
// 0x004d3440.  Because /ORDER cannot keep the surrounding ctor/Read/Main/Draw
// contributions in place, it cannot replace the object-local section move.

// Matrix 14: class-name census.
//
//   build/link/comdat-name-census/results.json
//
// The stripped image pins no class names, so renaming dimmerWidget is a legal
// reconstruction move. Eight names (same-length substitutions, shorter, longer)
// all emit the identical production-flag order: default constructor (3),
// deleting destructor (5), argument constructor (6), Read (7), Main (8),
// Draw (9), ordinary destructor (10). The compiler-generated COMDAT scheduler
// is not keyed by the decorated name; renaming cannot move ??_G to the tail.

// Matrix 15: mechanism closure (docs/compiler-re-allocation-order.md).
//
// C1XX RE plus probes m1/m2/d1/d2: the ??_7/??_G pair is generated eagerly
// and atomically at the first vtable requirement, which a constructor always
// is; moving the destructor definition (EOF or first) does not move the pair,
// no option in the complete 103-record front-end table parameterises the
// emission path, and no IDE-era (/Yu-derived) state reorders code emission.
// Retail's pair after Draw means the constructors were not the first vtable
// requirement in that compile - a state unreachable under the pinned front
// end for this source. The wall now carries a complete mechanism proof.

// Disposition: no source or ordinary project-state arm naturally emits the
// complete retail order.  Do not retain a source perturbation, force a symbol,
// patch the vtable, or add an /ORDER file.  The residual points to an unresolved
// original object/link ownership difference for the compiler-generated COMDAT.

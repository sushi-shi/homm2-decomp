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

// Matrix 8: historical source, include, and compiler-output paths.
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

// Disposition: no source or ordinary project-state arm naturally emits the
// complete retail order.  Do not retain a source perturbation, force a symbol,
// patch the vtable, or add an /ORDER file.  The residual points to an unresolved
// original object/link ownership difference for the compiler-generated COMDAT.

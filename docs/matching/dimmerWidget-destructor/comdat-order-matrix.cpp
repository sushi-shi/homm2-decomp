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

// Disposition: no source or ordinary project-state arm naturally emits the
// complete retail order.  Do not retain a source perturbation, force a symbol,
// patch the vtable, or add an /ORDER file.  The residual points to an unresolved
// original object/link ownership difference for the compiler-generated COMDAT.

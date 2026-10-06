// Target: ??_GdimmerWidget after Draw (retail 0x004d3440); the candidate
// emits it immediately after the default constructor.
//
// Measured 2026-10-06 on branch work/exact-hash-from-homm1 with the pinned VC6
// SP5 compiler and the BASE /Gy profile. These are reduced disposable TUs (a
// base with a virtual destructor, Main and Draw; a derived class with two
// constructors, Read, Main, Draw and a destructor), compiled outside the
// build; section order was read from the COFF section table.
//
// Retail constructors store the derived vftable (DIR32 ??_7dimmerWidget@@6B@
// at ctor+0x28), so the vftable is referenced from the first constructor.
//
// Destructor forms (with Main/Draw ordinary out-of-line virtuals):
//   virtual ~dw();            + dw::~dw() {} at end
//   inline virtual ~dw();     + inline dw::~dw() {} at end
//   virtual inline ~dw();     + dw::~dw() {} at end
//   virtual ~dw() {}          in the class body
// Virtual function forms (destructor variants as above):
//   inline virtual Main, Draw and ~dw, all defined inline after Read
//   only Draw inline; only Main inline
//
// All eight emit ??_G immediately after the first constructor; the in-class
// destructor additionally emits ??1 there. Declaring the vftable's other
// virtuals inline, or defining them after the constructors, does not defer
// the deleting-destructor COMDAT. This extends matrices 2 and 4 of
// comdat-order-matrix.cpp to the inline-virtual forms. No source change.
//
// Closure (2026-10-06): under /YX the in-class destructor above emits
// c0 cA R M D G X then the ctype pair, the retail order, with all bodies
// unchanged. See docs/matching/emission-order/pch/results.md.

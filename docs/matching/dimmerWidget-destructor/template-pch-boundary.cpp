// Measured 2026-09-27 in matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
// Structural parent: nested-template-split.cpp's narrow prefix and exact suffix.
// Hypothesis: PCH creation might finish pending class instantiation before parsing
// the destructor definition, preserving the successful two-TU emission order in
// one compiler TU and one producer record. Earlier PCH tests predated this owner.
//
// Complete 4-arm product:
//   boundary = named prefix.h end / source-level #pragma hdrstop
//   compile  = ordinary control / real /Yc PCH creation.
// All four use unchanged baseline flags, including /Od /Ob1 /Gy. The /Yc arms
// add /Ycprefix.h or /Yc, respectively, and /Fpboundary.pch.
//
// prefix.h contains the meaningful nested BaseWidget owner and all five primary
// method definitions, then explicit class instantiation, with no dtor definition.
// After its boundary the SAME DIMMER.cpp includes the original SOURCE/KB.h and
// supplies the primary-template destructor definition. No second compiler TU,
// object merger, padding, fake code, or COFF transformation is involved.
//
// Results: 4/4 compile, no timeouts or truncated product.
// - Named-header /Yc creates an 835960-byte PCH.
// - hdrstop /Yc creates an 835968-byte PCH.
// - Every object has exactly one @comp.id absolute symbol, value 729862.
// - Every arm emits the same code section order:
//     ctor0(3), ctor6(5), Read(6), Main(7), Draw(8), dtor(9), scalar(10),
//     $E19(11), $E18(12), ctype-id-helper(13).
//   The vtable is section 4. Required order has scalar BEFORE dtor.
// - All seven method bodies match exact bytes and complete ordered relocation
//   sites/types/semantic targets/addends. Derived template names map to roles;
//   external/base symbols remain literal. Initializer helpers $E19/$E18 and the
//   ctype id helper also match the existing raw baseline bodies/relocations.
// - The first control run logs Wine graphics diagnostics; there are no compiler
//   warnings/errors affecting any of the four results.
//
// Disposition: rejected. These real PCH creation boundaries do not finish the
// pending class instantiation before the later destructor definition. Exact
// bodies and single-producer identity alone do not meet native layout equality.
// No production source/build change retained; no final link needed to reject
// the demonstrably reversed destructor pair.
//
// Artifacts under build/link/dimmer-template-pch/:
//   run.py, run.log, results.json
//   {header,hdrstop}-{control,Yc}/
//     DIMMER.cpp, prefix.h, heroWindow.h, resourceGlobals.h, DIMMER.obj
//     boundary.pch (only /Yc arms)
// Results retain all code spans/relocations and data-symbol/producer inventory.

// DIMMER.cpp, named-header boundary:
// #include "prefix.h"
// #include <SOURCE/KB.h>
// template<class BaseWidget>
// heroWindow::DimmerWidget<BaseWidget>::~DimmerWidget() {}
//
// hdrstop family inserts only this directive after the prefix.h include:
// #pragma hdrstop
//
// prefix.h ends with the ordinary explicit class instantiation:
// template class heroWindow::DimmerWidget<widget>;

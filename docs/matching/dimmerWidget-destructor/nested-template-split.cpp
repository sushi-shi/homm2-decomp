// Measured 2026-09-26 in matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
// Target: all seven DIMMER bodies, ordered relocations, native contribution order.
// Production source remains unchanged. This is a local structural clue, not a
// claim that the complete retail executable has been reproduced.
//
// Evidence motivating this family:
// - WINDOW is the actual owner: heroWindow's resource constructor is the only
//   source client constructing and reading a dimmerWidget; ownership then passes
//   through widget*. Nesting under heroWindow has a real ownership interpretation.
// - The prior primary-template matrix separated first-constructor vtable emission
//   from the deleting wrapper. That refutes the old dossier's universal claim
//   that these are inseparable; its observations apply to its tested families.
// - The compiler queue audit describes insertion-ordered work nodes, without a
//   layout priority field. Declaration/definition ownership can change enqueue
//   timing, unlike harmless renaming or manually inserted alignment code.
//
// A: complete 8-arm matrix:
//    owner = concrete heroWindow::dimmerWidget / nested template
//            heroWindow::DimmerWidget<BaseWidget>
//    destructor = ordinary / inline-declared / in-class empty / implicit.
// The actual heroWindow header is copied into each disposable probe and receives
// the nested declaration; the outer object's fields and layout are unchanged.
// BaseWidget is semantically used for inheritance, construction, Main forwarding,
// and Dim dispatch. There are no dummy template parameters or emitted probes.
//
// All 8 compile. Nested ownership alone mirrors the prior unnested result:
// - Concrete ordinary/inline: ctor0,scalar,ctor6,Read,Main,Draw,dtor.
// - Concrete in-class/implicit: ctor0,scalar,dtor,ctor6,Read,Main,Draw.
// - Template ordinary/inline: ctor0,ctor6,Read,Main,Draw,dtor,scalar.
// - Template in-class: dtor,scalar,ctor0,ctor6,Read,Main,Draw.
// - Template implicit: ctor0,ctor6,Read,Main,Draw,scalar,dtor.
// Every explicit form has all seven exact bodies and ordered semantic relocation
// identities. Implicit forms omit the derived-vtable store: dtor 0x13 vs 0x1c.
// None of these eight alone closes both body and order.
//
// B: evidence-backed definition-ownership descendant, 2 pairs / 4 compilations:
// - Prefix defines the first five primary-template methods but has NO destructor
//   definition; explicit class instantiation is at EOF.
// - Suffix defines only the primary-template destructor, followed by explicit
//   class instantiation. Both TUs share the same complete class declaration.
// - Include control: prefix keeps SOURCE/KB.h, or replaces that broad include
//   with resourceGlobals.h declaring only the existing gpResourceManager pointer.
//   All other source includes remain. The pointer is the only declaration from
//   KB used by these five methods; resourceManager's real owner header supplies
//   its type/methods. This narrows declarations; it defines no new storage.
// - The suffix retains the original includes, including SOURCE/KB.h. Whether an
//   authentic two-TU historical build used that ownership remains unresolved.
//
// All four objects compile, without timeout/truncation. Both prefix arms emit:
//   ctor0,ctor6,Read,Main,Draw,scalar
// and leave the ordinary destructor undefined. Both suffix arms emit:
//   dtor,scalar,$E19,$E18,ctype-id-helper
// with duplicate vtable/scalar COMDATs. Ordinary native LINK selects the earlier
// prefix scalar/vtable. Every emitted DIMMER method matches raw bytes and complete
// ordered relocation sites/types/semantic targets/addends. The suffix's $E19,
// $E18, and ctype id helper also match the existing raw baseline exactly.
//
// Native diagnostic links use only the two raw objects, no libraries, no entry,
// and /FORCE:UNRESOLVED. This deliberately isolates contribution layout. These
// DLLs are NOT executable completeness evidence; external destinations remain
// unresolved. The .rsp/.log files record all flags and unresolved diagnostics.
//
// Native narrow-prefix relative .text positions (all match retail local starts):
//   000 ctor0      030 ctor6      070 Read       0f0 Main
//   110 Draw       130 scalar     160 dtor       180 $E19
//   1b0 $E18       1d0 ctype-id-helper
// Retail begins this span at VA 0x004d3310. Its next BUTTON ctor is 0x004d34e0
// (relative 0x1d0); the full game normally deduplicates the ctype id helper against
// its earlier owner. That final deduplication must be verified in the full link.
// The full-prefix control instead puts its own $E19/$E18/helper before dtor,
// moving dtor to relative 0x1c0 and retaining two initializer pairs; rejected.
//
// Storage audit:
// - Narrow prefix owns only the DIMMER vtable contribution; no .CRT$XCU or COMMON.
// - Suffix owns duplicate vtable, one 4-byte .CRT$XCU _$S20 cell, and the 1-byte
//   COMMON ctype initialization guard ??_B?1???id@?$ctype@G@std@@$D@@9@51.
// - Native narrow link selects one vtable, one .CRT$XCU cell, and one guard.
// - Full-prefix control owns two .CRT$XCU cells and initializer pairs.
// All raw sections, characteristics, bytes, defined/undefined/common symbols,
// and relocations are retained in layout.json and split-audit.json.
//
// Remaining full-image constraints:
// - WINDOW must use the recovered nested specialization consistently. Mangled
//   derived identities change; semantic role mapping is an audit convention,
//   never a linked-object mutation. Base/external identities remain literal.
// - A new compilation unit can increment the C++ producer count. Exact local
//   code order alone is insufficient to accept that image metadata difference.
// - Previous owner Textntry has its own initializers at d32c0/d32f0, immediately
//   before DIMMER d3310. Appending prefix source there is not automatically valid:
//   primary template instantiation normally precedes those initializer helpers.
// - Next owner BUTTON starts d34e0. Moving suffix into BUTTON normally moves its
//   initializer pair after BUTTON bodies. Neither adjacency is an established
//   source/build explanation; no such merger was retained or claimed measured.
//
// Artifacts under build/link/dimmer-nested-owner/:
//   run.py, run.log, results.json                    (A: 8 arms)
//   split.py, split.log, split-results.json          (B: 2 pairs)
//   split-narrow-prefix/{DIMMER.cpp,heroWindow.h,resourceGlobals.h,DIMMER.obj}
//   split-narrow-suffix/{DIMMER.cpp,heroWindow.h,DIMMER.obj}
//   corresponding split-full-prefix / split-full-suffix controls
//   layout.py, layout.json, {narrow,full}-layout.{dll,map,rsp,log}
//   audit.py, audit.log, split-audit.json
// No production change, object patch, manual padding, or fake code retained.

// Structural source skeleton (complete generated source is in artifacts):
// class heroWindow {
// public:
//     template<class BaseWidget>
//     class DimmerWidget : public BaseWidget {
//     public:
//         DimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id,
//             H2_ENUM_PARAM(WidgetKind, i16) kind);
//         DimmerWidget(void);
//         void Read(void);
//         virtual MessageDispatchResult Main(tag_message&) OVERRIDE;
//         virtual void Draw(void) OVERRIDE;
//         virtual ~DimmerWidget(void) OVERRIDE;
//     };
//     // Existing heroWindow fields/methods remain unchanged.
// };
//
// Prefix: the five existing method bodies, with template<class BaseWidget>,
// heroWindow::DimmerWidget<BaseWidget>:: qualification, and BaseWidget replacing
// widget only at the actual base construction/forwarding calls; then:
// template class heroWindow::DimmerWidget<widget>;
//
// Suffix:
// template<class BaseWidget>
// heroWindow::DimmerWidget<BaseWidget>::~DimmerWidget() {}
// template class heroWindow::DimmerWidget<widget>;

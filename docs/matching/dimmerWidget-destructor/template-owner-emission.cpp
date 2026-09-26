// Target: the seven DIMMER method bodies and their retail contribution order.
// Measured 2026-09-26, matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
//
// Structural hypothesis: a dimming decorator with a meaningful BaseWidget
// template parameter could defer helper emission to explicit instantiation.
// This tests a different owner from the previously exhausted concrete class.
// Constructor base initialization, Main forwarding, and Dim dispatch use the
// parameter. No probe declaration, padding, assembly, or COFF mutation is used.
// The hypothesis is not a recovered retail identity and is not retained.
//
// Complete bounded products, all attempted without timeout/truncation:
// A: 8 arms = primary definitions / explicit member specializations
//             x extern-template class declaration absent / present
//             x explicit class instantiation absent / at EOF.
// B: 6 arms = original / retail-oriented declaration order
//             x ordinary / inline-declared / implicit destructor.
//    All B arms use primary definitions and explicit class instantiation.
// C: 4 arms = destructor primary definition / explicit specialization
//             x explicit destructor instantiation absent / present.
//    Destructor definition follows the class instantiation; other declarations
//    use B's retail-oriented order. This tests definition availability rather
//    than merely moving the destructor among ordinary source definitions.
// D: 2 arms = ordinary / inline explicit destructor specialization, declared
//    before class instantiation and defined at EOF. Other members remain primary
//    templates. This tests whether declaration availability plus inline ownership
//    lets the class walk defer a specialization until after its wrapper.
//
// Total: 20 arms; 14 emit all seven methods, 2 intentionally emit none without
// instantiation, 4 are compiler rejections. Rejected syntax remains recorded.
//
// A outcomes:
// - Explicit member specializations preserve eager order:
//     ctor0, scalar, ctor6, Read, Main, Draw, dtor.
//   An extern-template declaration does not suppress the eager wrapper.
// - Primary definitions with explicit instantiation instead follow declaration
//   order, with the constructor overload set reversed:
//     ctor6, ctor0, dtor, Draw, Main, Read, scalar.
//   All seven bodies and semantic relocation identities are exact.
// - Combining extern class declaration and class instantiation produces C2949:
//   cannot use 'auto' and 'extern' on the same template-class specialization.
//
// B outcomes:
// - Declaring ctor6, ctor0, Read, Main, Draw, dtor produces:
//     ctor0, ctor6, Read, Main, Draw, dtor, scalar.
//   Ordinary and inline-declared destructors give identical bodies and order.
// - Omitting the explicit destructor produces the desired retail order:
//     ctor0, ctor6, Read, Main, Draw, scalar, dtor.
//   However, the implicit destructor is 0x13 rather than retail 0x1c: it omits
//   the derived-vtable store/relocation. The other six methods remain exact.
//   This is a useful emission clue, not matching closure.
//
// C outcomes:
// - A primary destructor definition after class instantiation still emits in
//   declaration order: ctor0,ctor6,Read,Main,Draw,dtor,scalar. Moving availability
//   textually does not defer this instantiation beyond the wrapper.
// - A destructor specialization after class instantiation emits eagerly before
//   the other instantiated members: dtor,scalar,ctor0,ctor6,Read,Main,Draw.
// - VC6 rejects explicit destructor instantiation syntax with C2909 and C2631
//   (requires a return type / destructors not allowed a return type).
// D outcomes:
// - Both specialization declarations compile, with all seven exact bodies and
//   semantic relocation identities. Both still emit dtor,scalar first, followed
//   by ctor0,ctor6,Read,Main,Draw. Inline does not defer the specialization.
//
// Raw evidence: results retain every emitted method's bytes and all ordered
// relocation sites/types/targets/addends, plus code/vtable section order.
// Comparing template owners maps only the derived ctor/dtor/vtable/method
// identities to explicit semantic roles; base-widget and external identities
// remain literal. This is NOT a claim of identical COFF symbol spelling or a
// full linked-image relocation audit. Baseline bodies/relocations were separately
// audited against retail in dimmer-native-alignment/retail-audit.json.
// Every emitting arm has either wrong contribution order or a wrong destructor;
// these raw-object contradictions suffice to reject without a complete link.
//
// Artifacts: build/link/dimmer-template-ownership/
//   run.py, run.log, results.json                      (A)
//   descendants.py, descendants.log, descendants.json  (B)
//   late-dtor.py, late-dtor.log, late-dtor.json         (C)
//   declared-specialization.py/.log/.json             (D)
//   per-arm {dimmerWidget.h,DIMMER.cpp,DIMMER.obj}
// Disposition: no production source/build change retained; exactness remains
// open. No universal impossibility claim follows from these finite products.

// Reviewed generic owner skeleton (method declarations vary as above):
// template<class BaseWidget>
// class DimmerWidget : public BaseWidget {
// public:
//     DimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id,
//         H2_ENUM_PARAM(WidgetKind, i16) kind);
//     DimmerWidget(void);
//     void Read(void);
//     virtual MessageDispatchResult Main(tag_message& message) OVERRIDE;
//     virtual void Draw(void) OVERRIDE;
//     virtual ~DimmerWidget(void) OVERRIDE;
// };
// typedef DimmerWidget<widget> dimmerWidget;
//
// Primary definitions preserve existing method bodies, replacing concrete base
// constructor/Main/Dim qualification with BaseWidget. Representative definitions:
// template<class BaseWidget>
// DimmerWidget<BaseWidget>::DimmerWidget(void)
//     : BaseWidget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {}
// template<class BaseWidget>
// void DimmerWidget<BaseWidget>::Draw(void) { BaseWidget::Dim(); }
// template<class BaseWidget>
// DimmerWidget<BaseWidget>::~DimmerWidget() {}
// template class DimmerWidget<widget>;
//
// Specialization family instead uses template<> and DimmerWidget<widget>, with
// original widget base qualification. Implicit ownership omits only the dtor
// declaration and definition. C moves the explicit definition after the class
// instantiation and optionally appends:
// template DimmerWidget<widget>::~DimmerWidget();  // measured VC6 rejection

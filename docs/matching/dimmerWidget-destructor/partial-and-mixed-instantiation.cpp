// Measured 2026-09-27, matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
// Verified cwd, branch, and HOMM2_DIR before compiling in one persistent shell.
// Canonical source snapshots and reference body records were read from root's
// decomp-gold-2.1-buka build/link/dimmer-nested-owner and dimmer-native-alignment.
// No production source or build configuration was edited.
//
// Parent: meaningful heroWindow::DimmerWidget<BaseWidget>, where BaseWidget
// supplies actual inheritance, base construction, Main forwarding, and Dim.
// The two-file primary-template owner can recover DIMMER's complete native
// layout but adds a compiler producer. The one-file owner emits seven exact
// bodies, with ordinary destruction before scalar destruction at its tail.
// Constructor explicit-instantiation syntax rejected in the previous matrix
// did NOT establish what valid partial ordinary-member requests would do.
//
// A. Complete 32-arm partial request product:
//    - all eight subsets of Read/Main/Draw explicit instantiation requests;
//    - ordinary / inline destructor declaration;
//    - empty primary destructor definition before / after the requests.
//    Each arm ends with (or requests immediately before the destructor definition)
//    an explicit whole-class instantiation to keep both constructors owned.
//    No constructor/destructor explicit-instantiation syntax is attempted.
//
// All 32 compile, all seven raw bodies and complete ordered semantic relocation
// sites/types/identities/addends are exact. Every arm emits the same complete
// function/vtable topology:
//    ctor0[3], vtable[4], ctor6[5], Read[6], Main[7], Draw[8],
//    ordinary-dtor[9], scalar[10], then ctype initialization helpers.
// Partial requests do not force a completed sub-batch ahead of the class walk.
//
// B. Complete 32-arm mixed-owner structural product:
//    Independently choose each of the five public method definitions as either
//    a primary-template definition or an explicit widget specialization.
//    Bits 0..4 mean ctor0, ctor6, Read, Main, Draw respectively.
//    The destructor stays a primary-template definition, and the real class
//    specialization is explicitly instantiated at EOF. Widget-specialized
//    bodies replace BaseWidget qualification with widget; all actual operations
//    and the recovered six-argument ABI remain unchanged.
//
// All 32 compile, with seven exact bodies and ordered semantic relocations.
// They produce 28 distinct method orders, retained in summary.json. These are
// real order levers: explicit method specializations are emitted eagerly before
// the remaining generic class-instantiation batch. Any specialized constructor
// emits the deleting wrapper immediately after itself. With neither constructor
// specialized, both constructors remain in the generic batch and the destructor
// precedes the wrapper at its tail. Examples (not sorted by a fuzzy score):
//    00000: ctor0,ctor6,Read,Main,Draw,dtor,scalar
//    00100: Read,ctor0,ctor6,Main,Draw,dtor,scalar
//    01010: ctor6,scalar,Main,ctor0,Read,Draw,dtor
//    11100: Read,Main,Draw,ctor0,ctor6,dtor,scalar
//    11111: ctor0,scalar,ctor6,Read,Main,Draw,dtor
// The 01010 and 11100 branches deliberately retain worse order: their body and
// relocation evidence remains credible, and the nonlocal ownership distinction
// is measured rather than rejected by score.
//
// C. Evidence-backed descendant of every B arm, complete 32-arm product:
//    Add inline to every selected explicit specialization definition, testing
//    whether inline specialization ownership defers the eager batch.
//    All 32 compile, all seven exact bodies/relocations persist. Each C arm has
//    identical complete section records to its corresponding B arm, including
//    characteristics, bytes, ordered relocations, and defined section symbols.
//    Inline alone does not recover a new orbit in these mixed-definition parents.
//
// Total 96/96 successful compiles, no timeout, rejection, or truncation. All
// compile with the production DIMMER flags, including /Od /Ob1 /Gy. Every arm
// retains all raw object sections, code/helper/vtable symbols, and ordered
// relocation records. Template-owned DIMMER names map to semantic roles for
// comparison; base-widget and external symbols remain literal. This mapping
// is diagnostic only and is never applied to linked objects. Baseline method
// evidence was separately audited against retail in retail-audit.json.
//
// No arm has retail order ctor0,ctor6,Read,Main,Draw,scalar,dtor. No native final
// link or executable byte reduction is claimed. These finite products establish
// how explicit instantiation and specialization interact in this source family;
// they do not prove that broader ownership or native linker selection cannot
// recover the order. No source change is retained.
//
// Artifacts relative to matcher-3 (all source/header/OBJ snapshots retained):
// build/link/dimmer-partial-instantiation/
//   run.py, run.log, results.json, summary.json                 (A)
//   mixed.py, mixed.log                                       (B driver/log)
//   inline_mixed.py, inline_mixed.log                          (C driver/log)
// build/link/dimmer-mixed-specializations/results.json         (B)
// build/link/dimmer-inline-specializations/results.json        (C)
//
// Actual valid partial request pool (any subset followed by whole-class request):
// template void heroWindow::DimmerWidget<widget>::Read(void);
// template MessageDispatchResult
//     heroWindow::DimmerWidget<widget>::Main(tag_message&);
// template void heroWindow::DimmerWidget<widget>::Draw(void);
// template class heroWindow::DimmerWidget<widget>;
//
// Primary definition example:
// template<class BaseWidget>
// void heroWindow::DimmerWidget<BaseWidget>::Draw(void) {
//     BaseWidget::Dim();
// }
//
// Explicit widget specialization (the descendant inserts inline after <>):
// template<>
// void heroWindow::DimmerWidget<widget>::Draw(void) {
//     widget::Dim();
// }
//
// The unchanged primary destructor is:
// template<class BaseWidget>
// heroWindow::DimmerWidget<BaseWidget>::~DimmerWidget() {}

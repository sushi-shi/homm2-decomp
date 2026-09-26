// Measured 2026-09-27, matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
// Worktree/branch/HOMM2_DIR verified before compiling. No root/matcher-4 edits.
//
// Hypothesis: explicitly declaring the concrete destructor specialization before
// a whole-class instantiation, but defining it after that request, might defer
// its ordinary body until after scalar destruction without a second compiler
// producer. Preserve the explicit empty destructor's required derived-vptr store.
//
// Prior evidence acknowledged: template-owner-emission.cpp family D already
// tested the unnested all-primary EOF request with ordinary/inline specialized
// destructor declaration/definition. Its dtor/scalar prefix is a control here,
// not a novel result. This product adds the actual nested, narrow-header owner,
// a closest mixed constructor-specialized parent, and distinct request/body
// boundaries. The five-method specialization matrix previously kept dtor primary.
//
// Complete Cartesian product, 24 compiled cells / 20 distinct source products:
//   public-method parent: all primary / ctor0 explicitly widget-specialized;
//   class instantiation request: before methods / after both ctors / after Draw;
//   destructor definition: immediately after that request / at EOF;
//   destructor specialization declaration and definition: ordinary / inline.
// At the after-Draw boundary, immediate and EOF spellings coincide (four duplicate
// cells retained for transparent complete matrix accounting).
// Every arm declares the specialization before all methods and all requests.
// Narrow header shape is the measured forward declaration in heroWindow.h plus
// the complete meaningful BaseWidget template in dimmerWidget.h. All five public
// method bodies preserve actual constructor/Read/Main/Dim behavior. No fake root,
// data, padding, alias, linker correction, or implicit destructor is introduced.
//
// 24/24 compile; zero rejections/timeouts. All seven methods in every arm have
// exact reference raw bytes and complete ordered semantic relocation identities,
// sites, types and addends. The ordinary destructor remains the required 0x1c
// body with derived-vtable store. No arm recovers retail contribution order.
// Two method orders occur:
// 1. dtor,scalar,ctor0,ctor6,Read,Main,Draw (14 cells): all-primary parents,
//    plus ctor0-specialized with the dtor definition before that constructor.
// 2. ctor0,scalar,dtor,ctor6,Read,Main,Draw (10 cells): the specialized constructor
//    precedes the specialized destructor body; remaining primary members defer.
//
// Thus request placement does not flush the primary member batch ahead of a
// later specialized destructor. Specialized definitions follow their eager
// definition order relative to one another, including wrapper generation;
// deferred primary members still follow them. Inline adds no third order here.
// The worse second orbit is retained as source/object evidence, not pruned by a
// fuzzy score. This rejects this finite one-TU ownership family, not all possible
// source/build explanations. The successful two-TU source split is unchanged.
//
// Artifacts: build/link/dimmer-specialized-dtor-boundary/
//   run.py, run.log, results.json, summary.json
//   each arm: DIMMER.cpp, include/BASE/{heroWindow,dimmerWidget}.h, DIMMER.obj
// All raw section bytes/characteristics, defined symbols and ordered relocations
// are recorded, including initialization helpers and vtable topology. Semantic
// role comparison maps only the derived DIMMER identities; external/base names
// remain literal. Reference bodies were independently audited against retail.
// No final native link or executable byte reduction is claimed for these arms.
//
// Reviewed specialization source skeleton (inline is the independent axis):
// template<> heroWindow::DimmerWidget<widget>::~DimmerWidget();
// ... first N public primary/specialized method definitions ...
// template class heroWindow::DimmerWidget<widget>;
// template<> heroWindow::DimmerWidget<widget>::~DimmerWidget() {}
// ... remaining public method definitions ...
//
// EOF variant moves only that destructor definition after the remaining methods.

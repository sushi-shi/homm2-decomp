// Measured 2026-09-27 in root decomp-gold-2.1-buka, pinned VC6 SP5.
// Parent: the meaningful heroWindow::DimmerWidget<BaseWidget> template owner.
// Its split source definitions recover native layout but add a compiler input.
// Hypothesis: requesting only the five public method instantiations could leave
// the destructor to the deleting wrapper's dependency, avoiding the whole-class
// declaration-order walk that emits ordinary destruction before the wrapper.
//
// Complete eight-arm product:
// - destructor declaration: ordinary / inline
// - empty primary-template destructor definition: before / after instantiation
// - instantiation scope: whole class / five individual public methods
// All source bodies and actual BaseWidget inheritance/forwarding are unchanged.
// The source includes KB.h once and owns the existing ctype initialization pair.
//
// Results: four whole-class controls compile, four member arms are rejected by
// VC6 with C2909 and C2533 on the two explicit constructor instantiations.
// No timeout, truncation, or partial product. Rejected syntax cannot establish
// an emission order; no claim is made that other compiler generations reject it.
// All four compiling controls preserve exact seven bodies and ordered relocation
// sites/types/semantic targets/addends, but emit:
//   ctor0, ctor6, Read, Main, Draw, ordinary destructor, scalar destructor.
// Required retail order reverses only the final pair. Moving definition text
// across the instantiation request does not force an earlier completed class walk.
//
// Disposition: no retained source or build change. This particular selective
// constructor-instantiation route is unavailable in the pinned front end.
// No full link is justified by compiler rejection or the controls' wrong order.
// Artifacts: build/link/dimmer-selective-instantiation/{run.py,run.log,results.json}
// Each arm retains DIMMER.cpp, heroWindow.h, resourceGlobals.h and, if successful,
// raw DIMMER.obj. Baseline bodies come from dimmer-native-alignment/baseline.json.
// Reproducer uses the reviewed role/compile setup in dimmer-nested-owner/run.py.
// Worktree, branch and HOMM2_DIR were verified before compiling in one Nix shell.

// Whole-class control:
// template class heroWindow::DimmerWidget<widget>;
//
// Selective member product (first two lines rejected by VC6):
// template heroWindow::DimmerWidget<widget>::DimmerWidget(void);
// template heroWindow::DimmerWidget<widget>::DimmerWidget(i16,i16,i16,i16,i16,i16);
// template void heroWindow::DimmerWidget<widget>::Read(void);
// template MessageDispatchResult heroWindow::DimmerWidget<widget>::Main(tag_message&);
// template void heroWindow::DimmerWidget<widget>::Draw(void);

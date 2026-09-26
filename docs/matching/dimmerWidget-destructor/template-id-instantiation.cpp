// VC6 SP5, measured 2026-09-27 on decomp-gold-2.1-buka.
// Follow-up to selective-instantiation.cpp: its rejected constructor requests
// used the injected class name, not a full template-id. The ownership hypothesis
// remains requesting the five real methods without the whole-class walk, so the
// scalar wrapper can request the ordinary destructor afterward. BaseWidget has
// its existing semantic role; method bodies and storage are unchanged.
//
// Complete 2 x 5 product, ten compiler invocations, no timeout/truncation:
// - destructor: ordinary / inline-declared;
// - constructor spelling: whole-class control / injected-name request control /
//   template-id requests / template-id requests plus out-of-class definitions /
//   template-id requests, definitions, and declarations.
// Source generation and all full source/header snapshots are preserved in:
//   build/link/dimmer-template-id-instantiation/run.py
//   build/link/dimmer-template-id-instantiation/{run.log,results.json}
// Every arm retains compile.log and, when successful, DIMMER.obj.
// Compiler flags are unchanged unit_flags(BASE/DIMMER), including /Od /Ob1 /Gy.
// Worktree/branch/HOMM2_DIR were verified in the persistent Nix shell.
//
// Only the two whole-class controls compile. Both retain exact seven bodies
// and complete ordered relocation sites/types/semantic identities/addends, but
// emit ctor0,ctor6,Read,Main,Draw,dtor,scalar: the last two are reversed.
// Injected-name requests reproduce C2909/C2533. Full-id requests fail C2909
// and C2143 at the '<'; adding full-id primary definitions/declarations also
// causes syntax diagnostics. Those rejected arms establish no emission order.
// Disposition: reject this bounded dialect family. No source/build change or
// final link; no broader impossibility claim about untested compiler syntax.

// Request control:
// template heroWindow::DimmerWidget<widget>::DimmerWidget(void);
// template heroWindow::DimmerWidget<widget>::DimmerWidget(i16,i16,i16,i16,i16,i16);
//
// Full template-id request arm:
// template heroWindow::DimmerWidget<widget>::DimmerWidget<widget>(void);
// template heroWindow::DimmerWidget<widget>::DimmerWidget<widget>(i16,i16,i16,i16,i16,i16);
//
// Corresponding optional primary definition spelling:
// template<class BaseWidget>
// heroWindow::DimmerWidget<BaseWidget>::DimmerWidget<BaseWidget>(void)
//     : BaseWidget(0,0,0,0,0,WIDGET_KIND_NONE) {}
// Optional class declaration spelling:
// DimmerWidget<BaseWidget>(void);
// The three ordinary member instantiation requests remain exactly those in
// selective-instantiation.cpp in every member-request arm.

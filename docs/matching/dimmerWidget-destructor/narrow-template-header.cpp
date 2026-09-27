// Measured 2026-09-27, matcher-3 / matcher/bss-dimmer, pinned VC6 SP5.
// Parent: the native-exact local DIMMER template split. The complete nested
// template initially lived in heroWindow.h, which is included by many TUs.
// Hypothesis: its actual widget definition belongs in dimmerWidget.h, while
// heroWindow only declares its nested owner. This narrows shared-header input
// without changing the class, inheritance, members, definitions, or TU split.
//
// Complete matrix: two header ownership forms x three current-root source
// snapshots (DIMMER, DIMMERDestructor, WINDOW), six successful compiles.
// Both forms use identical production flags and a per-arm header overlay.
// No source body was edited for this matrix. Cwd, branch, and HOMM2_DIR were
// verified in the persistent matcher-3 build shell before compiling.
//
// Control: heroWindow.h includes widget.h and defines the complete nested
// DimmerWidget<BaseWidget> template. dimmerWidget.h contains its widget typedef.
// Candidate: heroWindow.h has only the nested template forward declaration and
// no widget.h include; dimmerWidget.h includes widget.h and defines the complete
// nested template out of class under the existing packed layout, then aliases it.
//
// All six outputs match their previously successful native-link object
// references in every code/data section's index, name, size, characteristics,
// raw bytes, public definitions, and complete ordered relocations. DIMMER prefix
// and suffix references are build/link/dimmer-nested-owner/split-narrow-*/DIMMER.obj;
// WINDOW reference is build/dimmer-split-native/WINDOW.obj. Anonymous $L/$SG/$T
// targets are compared by actual COFF section/value, with named targets literal.
// No object mutation or relinking occurred. Full vs candidate pairs are equal
// under that same complete code/data comparison.
//
// Result: the complete template can be confined to the widget-specific header
// without losing any of the measured prefix/suffix/client object topology.
// This narrows compiler input for other heroWindow consumers; it is not proof
// that every other TU is unaffected by the remaining nested forward declaration.
// A canonical full build/native replay remains the integration check.
//
// Artifacts: build/dimmer-header-ownership/{run.py,run.log,results.json}
//   full/ and forward/: immutable header overlays, source snapshots, raw objects
//   narrow-header.patch: incremental two-header patch against the full split
// No root files were edited; the parent owns integration in matcher-4.
//
// heroWindow.h:
// class heroWindow {
// public:
//     template<class BaseWidget> class DimmerWidget;
//     // Existing fields and methods unchanged.
// };
//
// dimmerWidget.h:
// #include <BASE/heroWindow.h>
// #include <BASE/widget.h>
// #pragma pack(push, 1)
// template<class BaseWidget>
// class heroWindow::DimmerWidget : public BaseWidget {
// public:
//     DimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id,
//         H2_ENUM_PARAM(WidgetKind, i16) kind);
//     DimmerWidget(void);
//     void Read(void);
//     virtual MessageDispatchResult Main(tag_message& message) OVERRIDE;
//     virtual void Draw(void) OVERRIDE;
//     virtual ~DimmerWidget(void) OVERRIDE;
// };
// #pragma pack(pop)
// typedef heroWindow::DimmerWidget<widget> dimmerWidget;
// SIZE(dimmerWidget, 0x20);

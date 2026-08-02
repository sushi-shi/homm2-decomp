# user-dtor-out-of-line

**Symptom.** Retail `delete p;` sites call a standalone `??1X@@QAE@XZ` and the
COMDAT exists at the TU tail; our build inlines the compiler-generated dtor at
the delete site (only the member dtor call is visible) and emits no `??1X`.

Probe A/B (VC6 SP5, `/Od /MT /Gr /G5 /Ob1`), class `S { int b; P p; int c; }`
where `P` is an audiere-style RefPtr:

- implicit dtor (`pa.cpp`): delete site inlines; obj contains only `??1P`.
- `/Ob0` or `#pragma inline_depth(0)`: emits `??1S` (0x16) but it CALLS `??1P`
  instead of inlining the member dtor — wrong shape:
  `55 8bec 51 894dfc 8b4dfc 83c104 e8<??1P> 8be5 5d c3`
- **user-declared dtor with out-of-line empty definition** (`pf.cpp`,
  `~S();` + `S::~S() {}`): emits `??1S` (0x36) whose body inlines the member
  dtor — retail's exact shape:
  `55 8bec 83ec08 894df8 8b45f8 83c004 8945fc 8b4dfc 833900 7418 ... ff5004
   8b4dfc c70100000000 8be55dc3`
  vs retail `??1soundManager` (0x37, the extra byte is audiere's stdcall
  `push` before `call [ecx+4]`).

**Close.** Declare the dtor in the class, define it empty out of line in the
TU whose region holds the retail body (`game::~game`,
`soundManager::~soundManager` at the tail of SOURCE/KB;
`dimmerWidget::~dimmerWidget` in BASE/DIMMER). Commit 9182c077.

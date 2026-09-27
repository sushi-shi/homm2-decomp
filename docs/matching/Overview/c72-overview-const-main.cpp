// VC6 SP5 Overview source family c72-overview-const-main; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-const-main/results.json; RVA 0x7bda1
// Complete 2 arms; elapsed 1.880979s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 9cebe67a573ac9ef / 42:42 / 42,0,0,0 / True
// 1 / {'source_family': 'literal_names', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 317ddcd0b4e5c09f / 42:42 / 42,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |     gpMouseManager->SetPointer(
// |         const_cast<char*>("advmice.mse"),
// |         OVERVIEW_POINTER_FRAME,
// |         MOUSE_AUTO_CURSOR_TYPE
// |     );
// |     gpResourceManager->GetBackdrop(const_cast<char*>("overback.icn"), gpWindowManager->m_screen, 1);
// |     overWin = new heroWindow(0, 0, const_cast<char*>("overwind.bin"));
// |     if (overWin == NULL) {
// |         MemError();
// |     }
// |     SetWinText(overWin, OVERVIEW_WINDOW_TEXT_ID);
// |     OVScrollKnob = new iconWidget(
// |         SCROLL_KNOB_X,
// |         SCROLL_KNOB_Y,
// |         SCROLL_KNOB_WIDTH,
// |         SCROLL_KNOB_HEIGHT,
// |         const_cast<char*>("scrollcn.icn"),
// |         SCROLL_KNOB_FRAME,
// ALTERNATIVE literal_names (unified source delta from BEFORE):
// | --- before
// | +++ literal_names
// | @@ -1,10 +1,10 @@
// |      gpMouseManager->SetPointer(
// | -        const_cast<char*>("advmice.mse"),
// | +        "advmice.mse",
// |          OVERVIEW_POINTER_FRAME,
// |          MOUSE_AUTO_CURSOR_TYPE
// |      );
// | -    gpResourceManager->GetBackdrop(const_cast<char*>("overback.icn"), gpWindowManager->m_screen, 1);
// | -    overWin = new heroWindow(0, 0, const_cast<char*>("overwind.bin"));
// | +    gpResourceManager->GetBackdrop("overback.icn", gpWindowManager->m_screen, 1);
// | +    overWin = new heroWindow(0, 0, "overwind.bin");
// |      if (overWin == NULL) {
// |          MemError();
// |      }
// | @@ -14,5 +14,5 @@
// |          SCROLL_KNOB_Y,
// |          SCROLL_KNOB_WIDTH,
// |          SCROLL_KNOB_HEIGHT,
// | -        const_cast<char*>("scrollcn.icn"),
// | +        "scrollcn.icn",
// |          SCROLL_KNOB_FRAME,

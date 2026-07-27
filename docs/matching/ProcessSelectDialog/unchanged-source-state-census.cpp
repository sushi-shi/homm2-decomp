/*
dropListWidget::ProcessSelectDialog, RVA 0x000dcb10: unchanged-source census
(2026-07-27).

The starting reconstruction had the original ternary saved-background-width
assignment shown below.  Its clean object was 2075 bytes, had all 36
relocations, and scored 97.127520%.  The CFG comparison found 100/100 blocks,
98 exact bodies, two size-only bodies, and no flow-kind, target, or missing
block differences.
*/

#if 0
m_savedBackgroundWidth =
    m_scrollRange > 0 ? scrollWidth[0] + m_listWidth : m_listWidth;
#endif

/*
The unchanged effective source was compiled in the clean state and all 50
declaration-forest states:

  build/droplist-process-select-baseline-states.json
  build/match-variants/droplist-process-select-baseline-states-20260727/

Trial 9 reached 98.402690%, size 2075, with 36/36 relocations.  Trials 10 and
50 reproduced the earlier 98.288590% island.  The official trial-9 replay
temporarily advanced the old source hash d1072388f554 to 98.4027%.  This MAX
was superseded when the saved-background-width structure changed legitimately.
No generated probe source was retained.
*/

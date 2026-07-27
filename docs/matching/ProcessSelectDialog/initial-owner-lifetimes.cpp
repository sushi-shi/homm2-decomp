/*
dropListWidget::ProcessSelectDialog, RVA 0x000dcb10: initial owner lifetimes
(2026-07-27).

Retail and the clean object first diverged while assigning m_scrollRange and
m_topIndex: retail kept m_maxVisibleItems in dx and m_selectedIndex in cx,
whereas the clean candidate used the opposite ownership.  The following
developer-plausible snapshots were tested independently and together.
*/

#if 0
i16 maxVisibleItems = m_maxVisibleItems;
i16 selectedIndex = m_selectedIndex;

if (numItems > maxVisibleItems) {
    m_scrollRange = numItems - maxVisibleItems;
    m_topIndex = selectedIndex;
    if (selectedIndex < 0)
        m_topIndex = 0;
    if (m_scrollRange < m_topIndex)
        m_topIndex = m_scrollRange;
}
#endif

/*
The four-arm matrix (baseline, max-visible snapshot, selected-index snapshot,
and both snapshots) was crossed completely with clean plus 50 forest states:

  build/droplist-process-select-initial-owner-axes.json
  build/droplist-process-select-initial-owner-states-20260727/

Clean scores were 97.127520%, 95.897650%, 95.889260%, and 94.684560%.
Their best forest results were respectively 98.402690%, 98.261740%,
98.115770%, and below those islands.  None closed and none exceeded the
unchanged-source maximum, so the snapshots were rejected and the source was
restored.
*/

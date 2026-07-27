/*
dropListWidget::ProcessSelectDialog, RVA 0x000dcb10: saved-background-width
branch structure (2026-07-27).

Retail has a branch-shaped assignment and retains an otherwise unexplained
`lea ebp, [this + 0x7e]` at the end of its nonpositive arm.  Five credible
source families were compared before internal spellings:
*/

#if 0
// Starting ternary.
m_savedBackgroundWidth =
    m_scrollRange > 0 ? scrollWidth[0] + m_listWidth : m_listWidth;

// Retained positive branch.
if (m_scrollRange > 0) {
    m_savedBackgroundWidth = scrollWidth[0] + m_listWidth;
} else {
    m_savedBackgroundWidth = m_listWidth;
}

// Reversed branch.
if (m_scrollRange <= 0) {
    m_savedBackgroundWidth = m_listWidth;
} else {
    m_savedBackgroundWidth = scrollWidth[0] + m_listWidth;
}

// Rejected initialize-then-add family.
m_savedBackgroundWidth = m_listWidth;
if (m_scrollRange > 0)
    m_savedBackgroundWidth += scrollWidth[0];

// Rejected owner-alias family; pointer and reference forms were tested.
i16 *savedBackgroundWidth = &m_savedBackgroundWidth;
*savedBackgroundWidth = m_listWidth;
if (m_scrollRange > 0)
    *savedBackgroundWidth += scrollWidth[0];
#endif

/*
Initial clean structural comparison:

  build/droplist-process-select-saved-width-axes.json

The starting ternary scored 97.127520% with 98/100 exact blocks.  The retained
positive if/else scored slightly lower at 97.045300%, but improved structural
alignment to 99/100 exact blocks, one size-only block, no topology mismatch,
retail size 2079, and 36/36 relocations.  Initialize/add scored 92.038590% and
the pointer/reference aliases 91.417786%; all three lost a block and had
contradictory flow, so they were pruned.  The reversed if/else remained
structurally coherent at 97/100 exact blocks.

The ternary, positive if/else, and reversed if/else were then crossed
completely with clean plus 50 forest states:

  build/droplist-process-select-saved-width-if-axes.json
  build/match-variants/droplist-process-select-saved-width-if-states-20260727/

The positive branch produced the clear new island.  Trial 15 had retail size,
all 36 relocations, 99/100 exact blocks, and no flow-kind, target, or missing
block differences.  Replaying that state through the official recorder on
retained source hash 88198e92cdf7 measured 99.622480% and only 11 differing
bytes:

  build/tu-state-noise/droplist-process-select-explicit-if-trial15-20260727/

Because the residual was exceptionally close, the retained structure received
an expanded 200-forest census:

  build/tu-state-noise/droplist-process-select-explicit-if-expanded-20260727/

That complete sweep found 115 distinct object/relocation states.  Trial 15
remained best, so the official MAX is 99.6225%.  The positive if/else is kept
because it explains the retail CFG and code size; generated probes were
restored and are not part of reconstructed source.
*/

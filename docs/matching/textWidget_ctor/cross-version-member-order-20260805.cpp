/*
 * Cross-version exact closure for textWidget::textWidget(),
 * BASE/TEXTWDGT RVA 0x000d1060.
 *
 * The exact donor initializes m_font and m_text before color/alignment. The
 * former PoL reconstruction reversed those semantic phases. A complete
 * two-order x (clean + 50 forest/top states) matrix compiled 102/102 cells:
 *
 *   build/text-widget-ctor-order-manifest.json
 *   build/source-variant-batch/text-widget-ctor-order/results.json
 *
 * The mainline pointer-first order is clean-state exact: 62/62 bytes, one
 * exact block, and complete ordered 2/2 relocations. It selects retail EDI
 * ownership where the prior order selected ESI. Live 100.0000% retained.
 */

textWidget::textWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_font = NULL;
    m_text = NULL;
    m_color = FONT_DRAW_DEFAULT;
    m_alignment = FONT_ALIGN_CENTER;
    m_kind = WIDGET_KIND_TEXT;
}

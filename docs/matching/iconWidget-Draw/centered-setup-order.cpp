/*
 * iconWidget::Draw, RVA 0xd0f70: centered-icon setup statement order.
 *
 * The only live divergence after the span-boundary normalization was one
 * scheduling swap in the WIDGET_KIND_ICON_CENTERED setup: retail interleaves
 * `mov dx,[entry+w]; sub bx,[entry+y]; mov cx,[m_width]; sub di,[entry+x]`
 * while the previous statement order compiled to loads-then-subtractions.
 * 200 unchanged-source TU states (two seeds, both insertions) never moved the
 * schedule, proving a source shape rather than a compiler-state residual.
 *
 * Eight reviewed orderings/declaration shapes of the four-statement group were
 * crossed with clean plus 50 top-of-TU forest states:
 *
 *   build/iconwdgt-draw-centered-setup-axes-20260728.json
 *   build/iconwdgt-draw-centered-setup-manifest-20260728.json
 *   build/source-variant-batch/iconwdgt-draw-centered-setup/
 *
 * Result: `w_y_x_width` closes exactly on the CLEAN TU state
 * (100.000000%, size 229, 4/4 relocations, exact 11/11 blocks):
 *
 *   iconWidth = entry->w;
 *   y -= entry->y;
 *   x -= entry->x;
 *   widgetWidth = m_width;
 *
 * The widget-width load is written last and the compiler schedules it between
 * the two entry-relative subtractions. The `y_w_width_x` order also reached
 * 100 but only under generated forest states, so the clean-closing order was
 * retained. Split declarations remain; merging them into initializers was not
 * required for closure.
 */

void iconWidget_Draw_centered_setup_order_attempt() {
}

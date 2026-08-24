// textWidget::textWidget(void)       PoL RVA 0xd1060, Buka RVA 0xc2f50
// dropListWidget::dropListWidget()   PoL RVA 0xdbf00, Buka RVA 0xcfdb0
//
// Artifacts:
//   build/source-variant-batch/text-widget-default-order/results.json
//   build/source-variant-batch/drop-list-widget-default-order/results.json
// Coverage: two complete 2/2 clean-source products; source restored.
//
// textWidget: Buka/current PoL order exact at 62 bytes, 1/1 CFG, ordered
// 2/2 relocations; historical PoL color-first order 97.368420%.
// No edit required.
// dropListWidget: PoL and Buka orders both exact at 59 bytes, 1/1 CFG,
// ordered 2/2 relocations. PoL adopts authoritative Buka order.
// No gameplay or shipped-byte change.

/*
listBoxWidget::listBoxWidget(void) store order
================================================

PoL 2.0 RVA 0xdb060; Buka 2.1 RVA 0xce6a0.

Complete clean VC4.2 product: 2/2
---------------------------------
  build/listbox-ctor-buka-order-manifest.json
  build/source-variant-batch/listbox-ctor-buka-order/results.json

  PoL order:  items, scrollbar, selected, count, last selected
  Buka order: count, items, selected, last selected, scrollbar  RETAINED

Both arms emit identical exact text: 66 bytes, one block, ordered 2/2
relocations.

Disposition: invariant matching-decomp port of Buka's store order.
No gameplay or shipped-byte change.
*/

/*
button::button(void) store order
================================

PoL 2.0 RVA 0xdd440; Buka 2.1 RVA 0xd34e0.

Complete clean VC4.2 product: 2/2
---------------------------------
  build/button-ctor-buka-order-manifest.json
  build/source-variant-batch/button-ctor-buka-order/results.json

  PoL:  normal frame, pressed frame, icon ID, select mode, hotkey, icon
  Buka: icon ID, icon, normal frame, pressed frame, select mode, hotkey

Both arms emit identical exact text: 52 bytes, one block, ordered 2/2
relocations.

Disposition: invariant matching-decomp port of Buka's store order.
No gameplay or shipped-byte change.
*/

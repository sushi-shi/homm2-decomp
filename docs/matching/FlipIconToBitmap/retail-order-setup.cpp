/*
 * FlipIconToBitmap, RVA 0xd1ba0: retail-order setup (steered from the
 * retained 87.82 island's bytes).
 *
 * The island diff shows retail reading w through flat [entries + 13*frame +
 * 4] FIRST and spilling it, computing s_src by adding srcOffset into the
 * m_data register, lowering the left edge incrementally (sub/sub/inc), and
 * publishing entry -> left -> right -> y with the y-add interleaved. Three
 * setup shapes x three clip-term spellings crossed with clean plus 40
 * top-of-TU forest states (369-arm product):
 *
 *   build/iconf2b-retail-order-axes-20260729.json
 *   build/iconf2b-retail-order-manifest-20260729.json
 *   build/source-variant-batch/iconf2b-retail-order/
 *
 * retail_order_flat_w with the existing clip terms wins both objectives:
 * 88.6048% at forest trial 39 with exact 76/76 topology, recorded by replay
 * (previous epoch 87.8223). The x0/mirror clip spellings tie or lose in
 * every state; the current terms are retained.
 *
 *   build/tu-state-noise/iconf2b-retailorder-replay-20260729/
 */

void FlipIconToBitmap_retail_order_setup_attempt() {
}

/*
 * heroWindowManager::BroadcastMessage, RVA 0x000cac40, 2026-07-27.
 *
 * Retail and the retained source both emit one 17-instruction block, the
 * exact 53-byte function span, and zero relocations.  Their only difference
 * is the final construction/dispatch schedule:
 *
 *   retained:
 *     store msg.payload.widget.data.value
 *     push &msg.payload
 *     load this->vftable
 *
 *   retail:
 *     load this->vftable
 *     store msg.payload.widget.data.value
 *     push &msg.payload
 *
 * A 16-arm structural matrix crossed four message construction owners with
 * four virtual dispatch owners:
 *
 *   construction:
 *     - direct tag_message fields;
 *     - tag_messageWidgetPayload reference;
 *     - tag_messageWidgetPayload pointer;
 *     - payload fields before msg.type.
 *
 *   dispatch:
 *     - direct Main(msg);
 *     - baseManager pointer;
 *     - baseManager reference;
 *     - MessageDispatchResult result local.
 *
 * The clean structural census completed 16/16.  The complete Cartesian
 * state census then compiled all 16 structures against clean plus 50 forest
 * states: 816/816 in 780.51 seconds, with no truncation.
 *
 * Best by construction family:
 *   direct fields:       81.176470%, size 53, relocs 0/0;
 *   payload before type: 72.000000%, size 53, relocs 0/0;
 *   payload reference:   69.411766%, size 53, relocs 0/0;
 *   payload pointer:     69.411766%, size 53, relocs 0/0.
 *
 * Every dispatch owner was byte-identical within its construction family,
 * and all 51 states were byte-identical for direct-field construction.
 *
 * One final real inline boundary was tested separately:
 *
 *     static inline MessageDispatchResult DispatchWindowMessage(
 *         heroWindowManager* manager, tag_message& message
 *     ) {
 *         return manager->Main(message);
 *     }
 *
 *     return DispatchWindowMessage(this, msg);
 *
 * Its clean plus 50-state sweep completed 51/51.  It was byte-identical to
 * direct Main(msg) in every state, so the helper did not move the vftable
 * load across the final payload store.
 *
 * Artifacts:
 *   build/broadcast-message-structure-axes-20260727.json
 *   build/broadcast-message-structure-clean-manifest-20260727.json
 *   build/match-variants/broadcast-message-structure-clean-20260727/results.json
 *   build/broadcast-message-structure-states-manifest-20260727.json
 *   build/match-variants/broadcast-message-structure-states-20260727/results.json
 *   build/broadcast-message-inline-dispatch-manifest-20260727.json
 *   build/match-variants/broadcast-message-inline-dispatch-20260727/results.json
 *
 * Disposition: retain the direct-field/direct-virtual source.  All tested
 * structures preserve semantics and block topology, but none reaches exact
 * bytes or improves the durable 81.1765% MAX.  No generated source, probe
 * declaration, or manual configuration edit is retained.
 */

void heroWindowManager_BroadcastMessage_construction_and_dispatch_record() {
}

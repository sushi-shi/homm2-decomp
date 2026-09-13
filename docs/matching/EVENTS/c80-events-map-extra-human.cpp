// VC6 SP5 source product c80-events-map-extra-human; RVA 0x3b640.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.159884s; no compile failure or timeout; source restored.
// build/source-variant-batch/c80-events-map-extra-human/results.json and build/c80-events-map-extra-human-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'void-owner': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False
// 1 / {'void-owner': 'recover', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False

// AXIS void-owner
// BEFORE (complete exact span):
// |             EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |             eventExtra_o = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |             if (!eventExtra_o->active) {
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,3 +1,3 @@
// |              EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// | -            eventExtra_o = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// | +            eventExtra_o = static_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |              if (!eventExtra_o->active) {
// EXTRA BEFORE:
// |         case MAP_OBJECT_BOTTLE:
// |             if (cell->m_objectMetadata) {
// |                 signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |                 if (strlen(signExtra_k->text) > SIGN_MINIMUM_TEXT_LENGTH)
// |                     EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);
// EXTRA REPLACEMENT:
// |         case MAP_OBJECT_BOTTLE:
// |             if (cell->m_objectMetadata) {
// |                 signExtra_k = static_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |                 if (strlen(signExtra_k->text) > SIGN_MINIMUM_TEXT_LENGTH)
// |                     EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);
// EXTRA BEFORE:
// |         case MAP_OBJECT_SIGN:
// |             if (cell->m_objectMetadata) {
// |                 signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |                 if (strlen(signExtra_k->text) > SIGN_MINIMUM_TEXT_LENGTH)
// |                     EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);
// EXTRA REPLACEMENT:
// |         case MAP_OBJECT_SIGN:
// |             if (cell->m_objectMetadata) {
// |                 signExtra_k = static_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |                 if (strlen(signExtra_k->text) > SIGN_MINIMUM_TEXT_LENGTH)
// |                     EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);

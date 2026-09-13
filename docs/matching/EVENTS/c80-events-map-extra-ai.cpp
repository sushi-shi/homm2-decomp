// VC6 SP5 source product c80-events-map-extra-ai; RVA 0x44d73.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.089710s; no compile failure or timeout; source restored.
// build/source-variant-batch/c80-events-map-extra-ai/results.json and build/c80-events-map-extra-ai-axes.json
// Disposition: apply trial1. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'void-owner': 'keep', 'candidate': 'baseline'} / 11116 / fe51168989c69b33 / c73a53b52e3d49b1 / 367:374 / 366,0,0,1,7 / False
// 1 / {'void-owner': 'recover', 'candidate': 'baseline'} / 11116 / fe51168989c69b33 / c73a53b52e3d49b1 / 367:374 / 366,0,0,1,7 / False

// AXIS void-owner
// BEFORE (complete exact span):
// |         case MAP_OBJECT_SPHINX:
// |             eventExtra_o = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |             if (eventExtra_o->active == 0)
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1,3 +1,3 @@
// |          case MAP_OBJECT_SPHINX:
// | -            eventExtra_o = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// | +            eventExtra_o = static_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
// |              if (eventExtra_o->active == 0)

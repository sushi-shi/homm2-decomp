// VC6 SP5 source product r29-events-text-roles; RVA 0x3b640.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.084259s; no compile failure or timeout; source restored.
// build/source-variant-batch/r29-events-text-roles/results.json and build/r29-events-text-roles-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'input-vs-resource': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / c2595203b0e45670 / 557:561 / 556,0,0,1,4 / False
// 1 / {'input-vs-resource': 'recover', 'candidate': 'baseline'} / 24620 / db4cc6c2f27266e2 / 8774a133b3eda3cd / 557:561 / 556,0,0,1,4 / False

// AXIS input-vs-resource
// BEFORE (complete exact span):
// |     char sphinxAnswer_a[SPHINX_ANSWER_BUFFER_SIZE];
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +1,2 @@
// |      char sphinxAnswer_a[SPHINX_ANSWER_BUFFER_SIZE];
// | +    char resourceName[SPHINX_ANSWER_BUFFER_SIZE];
// EXTRA BEFORE:
// |             strcpy(sphinxAnswer_a, gResourceNames[IDX(resourceType_a)]);
// |             sphinxAnswer_a[0] = ToLowerCp1251(sphinxAnswer_a[0]);
// |             sprintf(gText, gEventText[EVENT_TEXT_RESOURCE_PICKUP], sphinxAnswer_a);
// EXTRA REPLACEMENT:
// |             strcpy(resourceName, gResourceNames[IDX(resourceType_a)]);
// |             resourceName[0] = ToLowerCp1251(resourceName[0]);
// |             sprintf(gText, gEventText[EVENT_TEXT_RESOURCE_PICKUP], resourceName);
// EXTRA BEFORE:
// |                     case ARTIFACT_EVENT_MODE_RESOURCE_3:
// |                         EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |                         sprintf(sphinxAnswer_a, gResourceNames[IDX(artifactResourceType_k)]);
// |                         sphinxAnswer_a[0] = ToLowerCp1251(sphinxAnswer_a[0]);
// |                         sprintf(
// |                             gText,
// |                             "{\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2}\n\n\xcb\xe5\xef\xf0\xe5\xea\xee\xed \xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2 "
// EXTRA REPLACEMENT:
// |                     case ARTIFACT_EVENT_MODE_RESOURCE_3:
// |                         EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |                         sprintf(resourceName, gResourceNames[IDX(artifactResourceType_k)]);
// |                         resourceName[0] = ToLowerCp1251(resourceName[0]);
// |                         sprintf(
// |                             gText,
// |                             "{\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2}\n\n\xcb\xe5\xef\xf0\xe5\xea\xee\xed \xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2 "
// EXTRA BEFORE:
// |                                 "\xe8 \xef\xf0\xee\xf1\xe8\xf2 %s - 3 \xe5\xe4\xe8\xed\xe8\xf6\xfb. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 "
// |                                 "\xea\xf3\xef\xe8\xf2\xfc \xef\xf0\xe5\xe4\xec\xe5\xf2?",
// |                             gArtifactNames[IDX(artifact_g)],
// |                             sphinxAnswer_a
// |                         );
// |                         NormalDialog(
// |                             gText,
// EXTRA REPLACEMENT:
// |                                 "\xe8 \xef\xf0\xee\xf1\xe8\xf2 %s - 3 \xe5\xe4\xe8\xed\xe8\xf6\xfb. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 "
// |                                 "\xea\xf3\xef\xe8\xf2\xfc \xef\xf0\xe5\xe4\xec\xe5\xf2?",
// |                             gArtifactNames[IDX(artifact_g)],
// |                             resourceName
// |                         );
// |                         NormalDialog(
// |                             gText,
// EXTRA BEFORE:
// |                     case ARTIFACT_EVENT_MODE_RESOURCE_5:
// |                         EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |                         sprintf(sphinxAnswer_a, gResourceNames[IDX(artifactResourceType_k)]);
// |                         sphinxAnswer_a[0] = ToLowerCp1251(sphinxAnswer_a[0]);
// |                         sprintf(
// |                             gText,
// |                             "{\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2}\n\n\xcb\xe5\xef\xf0\xe5\xea\xee\xed \xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2 "
// EXTRA REPLACEMENT:
// |                     case ARTIFACT_EVENT_MODE_RESOURCE_5:
// |                         EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |                         sprintf(resourceName, gResourceNames[IDX(artifactResourceType_k)]);
// |                         resourceName[0] = ToLowerCp1251(resourceName[0]);
// |                         sprintf(
// |                             gText,
// |                             "{\xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2}\n\n\xcb\xe5\xef\xf0\xe5\xea\xee\xed \xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2 "
// EXTRA BEFORE:
// |                                 "\xe8 \xef\xf0\xee\xf1\xe8\xf2 %s - 5 \xe5\xe4\xe8\xed\xe8\xf6. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 "
// |                                 "\xea\xf3\xef\xe8\xf2\xfc \xef\xf0\xe5\xe4\xec\xe5\xf2?",
// |                             gArtifactNames[IDX(artifact_g)],
// |                             sphinxAnswer_a
// |                         );
// |                         NormalDialog(
// |                             gText,
// EXTRA REPLACEMENT:
// |                                 "\xe8 \xef\xf0\xee\xf1\xe8\xf2 %s - 5 \xe5\xe4\xe8\xed\xe8\xf6. \xc6\xe5\xeb\xe0\xe5\xf2\xe5 "
// |                                 "\xea\xf3\xef\xe8\xf2\xfc \xef\xf0\xe5\xe4\xec\xe5\xf2?",
// |                             gArtifactNames[IDX(artifact_g)],
// |                             resourceName
// |                         );
// |                         NormalDialog(
// |                             gText,

// Parent 7d4ffd56; 4 complete arms in 3.5855419570580125 seconds.
// Selected trial 1: {'source_shape': 'omit0_cast1', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (397, '3cd9026bdc029c21', '39354268ed528056').
// Same size and masked payload as baseline: ['omit0_cast0', 'omit0_cast1'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/c46-s48-object-manifest.json; build/source-variant-batch/c46-s48-object/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// townObject::townObject(
//     FactionType townType,
//     H2_ENUM_PARAM(BuildingSlotType, i32) buildingId,
//     char* iconBaseName
// ) {
//     char name[TOWN_OBJECT_FILENAME_SIZE];
//     i32 x;
//     i32 y;
//     i32 H2_UNUSED(tempY);
//     i32 w;
//     i32 h;
//     H2_ENUM_STORAGE(BuildingSlotType, i32) id_h;
//
//     m_animationFrame = 0;
//     m_icon = NULL;
//     m_border = NULL;
//     m_visible = 1;
//     m_animationFrameCount = sBuildingInfo[IDX(townType)][IDX(buildingId)].animationFrameCount;
//     x = sBuildingInfo[IDX(townType)][IDX(buildingId)].x;
//     y = sBuildingInfo[IDX(townType)][IDX(buildingId)].y;
//     w = sBuildingInfo[IDX(townType)][IDX(buildingId)].width;
//     h = sBuildingInfo[IDX(townType)][IDX(buildingId)].height;
//     id_h = buildingId;
//     m_buildingId = id_h;
//     sprintf(name, "%s.icn", iconBaseName);
//     m_icon = gpResourceManager->GetIcon(name);
//     if (id_h != TOWN_OBJECT_NONE) {
//         m_border = new border(
//             x,
//             y,
//             w,
//             h,
//             static_cast<i16>(IDX(id_h)),
//             WIDGET_KIND_TRANSPARENT,
//             0,
//             NULL
//         );
//         if (m_border == NULL)
//             MemError();
//     }
// }
//
// Option omit0_cast0:
//
//
// Option omit0_cast1:
// --- baseline
// +++ omit0_cast1
// @@ -30,7 +30,7 @@
//              y,
//              w,
//              h,
// -            static_cast<i16>(IDX(id_h)),
// +            IDX(id_h),
//              WIDGET_KIND_TRANSPARENT,
//              0,
//              NULL
//
// Option omit1_cast0:
// --- baseline
// +++ omit1_cast0
// @@ -6,7 +6,6 @@
//      char name[TOWN_OBJECT_FILENAME_SIZE];
//      i32 x;
//      i32 y;
// -    i32 H2_UNUSED(tempY);
//      i32 w;
//      i32 h;
//      H2_ENUM_STORAGE(BuildingSlotType, i32) id_h;
//
// Option omit1_cast1:
// --- baseline
// +++ omit1_cast1
// @@ -6,7 +6,6 @@
//      char name[TOWN_OBJECT_FILENAME_SIZE];
//      i32 x;
//      i32 y;
// -    i32 H2_UNUSED(tempY);
//      i32 w;
//      i32 h;
//      H2_ENUM_STORAGE(BuildingSlotType, i32) id_h;
// @@ -30,7 +29,7 @@
//              y,
//              w,
//              h,
// -            static_cast<i16>(IDX(id_h)),
// +            IDX(id_h),
//              WIDGET_KIND_TRANSPARENT,
//              0,
//              NULL
//

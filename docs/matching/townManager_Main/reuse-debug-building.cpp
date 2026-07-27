// Structural attempt: reuse debugBuilding_e instead of copying it to an enum local.
//
// The retail B13-B15 sequence reads debugBuilding_e directly. Removing the
// redundant local made the candidate exactly retail-sized (6192 bytes) and
// raised clean state to 99.217415%.
// Matrix: build/match-variants/townmain-debug-building-local-20260727

if ((gTownEligibleBuildMask[IDX(m_town->m_type)]
     & BIT(static_cast<BuildingSlotType>(debugBuilding_e)))
    || debugBuilding_e == IDX(BUILDING_SLOT_CASTLE))
    BuildObj(static_cast<BuildingSlotType>(debugBuilding_e));

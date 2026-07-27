// Structural attempt: remove the explicit BuildingSlotType switch local.
//
// Retail has one compiler switch temporary. The explicit selectedBuilding
// local produced a second copy and one excess frame slot. Switching directly
// on the widget ID raised clean state to 99.108460%.
// Matrix: build/match-variants/townmain-direct-widget-switch-20260727

switch (message.payload.widget.id) {
    // At typed call sites only:
    // GetBuildingInfo(
    //     m_town->m_type,
    //     static_cast<BuildingSlotType>(message.payload.widget.id),
    //     1
    // );
}

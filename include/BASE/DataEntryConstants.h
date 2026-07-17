#ifndef HOMM2_BASE_DATA_ENTRY_CONSTANTS_H
#define HOMM2_BASE_DATA_ENTRY_CONSTANTS_H

typedef enum DataEntryLayout {
    DATA_ENTRY_WINDOW_X = 0xb1,
    DATA_ENTRY_WINDOW_Y = 0x14,
    DATA_ENTRY_INPUT_BOX_Y_OFFSET = 0x17
} DataEntryLayout;

typedef enum DataEntryPhase {
    DATA_ENTRY_PHASE_IMMEDIATE = 0,
    DATA_ENTRY_PHASE_POINTER_SENT = 1,
    DATA_ENTRY_PHASE_READY = 2
} DataEntryPhase;

// Widget identifiers serialized in the evntwin*.bin data-entry windows.
typedef enum DataEntryWidgetId {
    DATA_ENTRY_PROMPT_WIDGET = 1,
    DATA_ENTRY_TEXT_WIDGET = 10,
    DATA_ENTRY_BUTTON_ONE = 0x7801,
    DATA_ENTRY_CANCEL_BUTTON = 0x7802,
    DATA_ENTRY_BUTTON_FIVE = 0x7805,
    DATA_ENTRY_BUTTON_SIX = 0x7806,
    DATA_ENTRY_BUTTON_SEVEN = 0x7807,
    DATA_ENTRY_BUTTON_EIGHT = 0x7808
} DataEntryWidgetId;

#endif // HOMM2_BASE_DATA_ENTRY_CONSTANTS_H

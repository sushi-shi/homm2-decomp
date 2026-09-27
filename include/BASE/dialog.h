#ifndef HOMM2_BASE_DIALOG_H
#define HOMM2_BASE_DIALOG_H

#include <va.h>

// Reserved window-record button slots. Each dialog assigns its own meaning
// (close, cancel, accept, etc.); these IDs pass unchanged through widget messages.
H2_ENUM_BEGIN(DialogButtonId)
    DIALOG_BUTTON_0 = 0x7800,
    DIALOG_BUTTON_1 = 0x7801,
    DIALOG_BUTTON_2 = 0x7802,
    DIALOG_BUTTON_3 = 0x7803,
    DIALOG_BUTTON_4 = 0x7804,
    DIALOG_BUTTON_5 = 0x7805,
    DIALOG_BUTTON_6 = 0x7806,
    DIALOG_BUTTON_7 = 0x7807,
    DIALOG_BUTTON_8 = 0x7808,
H2_ENUM_END(DialogButtonId)

#endif

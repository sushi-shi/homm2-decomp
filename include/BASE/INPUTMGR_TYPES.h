#ifndef HOMM2_BASE_INPUTMGR_TYPES_H
#define HOMM2_BASE_INPUTMGR_TYPES_H

#include <va.h>

H2_ENUM_BEGIN(InputManagerTextSlotSize)
    INPUT_MANAGER_RELEASE_CAPTURE_SLOT_SIZE = 0x18,
    INPUT_MANAGER_NAME_SLOT_SIZE            = 0x10
H2_ENUM_END(InputManagerTextSlotSize)

typedef struct SInputManagerText {
    char leftReleaseCaptureFailure[INPUT_MANAGER_RELEASE_CAPTURE_SLOT_SIZE];
    char rightReleaseCaptureFailure[INPUT_MANAGER_RELEASE_CAPTURE_SLOT_SIZE];
    char managerName[INPUT_MANAGER_NAME_SLOT_SIZE];
} SInputManagerText;

SIZE(SInputManagerText, 0x40);

#endif

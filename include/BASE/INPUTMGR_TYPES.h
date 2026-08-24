#ifndef HOMM2_BASE_INPUTMGR_TYPES_H
#define HOMM2_BASE_INPUTMGR_TYPES_H

#include <Ints.h>

typedef enum InputManagerTextSlotSize {
    INPUT_MANAGER_RELEASE_CAPTURE_SLOT_SIZE = 0x18,
    INPUT_MANAGER_NAME_SLOT_SIZE            = 0x10
} InputManagerTextSlotSize;

typedef struct SInputManagerText {
    char leftReleaseCaptureFailure[INPUT_MANAGER_RELEASE_CAPTURE_SLOT_SIZE];
    char rightReleaseCaptureFailure[INPUT_MANAGER_RELEASE_CAPTURE_SLOT_SIZE];
    char managerName[INPUT_MANAGER_NAME_SLOT_SIZE];
} SInputManagerText;


#endif

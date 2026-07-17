#ifndef HOMM2_BASE_TEXTWDGT_TYPES_H
#define HOMM2_BASE_TEXTWDGT_TYPES_H

#include <va.h>

HOMM2_ENUM_VALUES_BEGIN(TextWidgetSourceFileSize)
    TEXT_WIDGET_SOURCE_FILE_SLOT_SIZE = 0x2c
HOMM2_ENUM_VALUES_END(TextWidgetSourceFileSize)

typedef struct STextWidgetSourceFiles {
    char read[TEXT_WIDGET_SOURCE_FILE_SLOT_SIZE];
    char destruction[TEXT_WIDGET_SOURCE_FILE_SLOT_SIZE];
    char resizeFree[TEXT_WIDGET_SOURCE_FILE_SLOT_SIZE];
    char resizeAlloc[TEXT_WIDGET_SOURCE_FILE_SLOT_SIZE];
} STextWidgetSourceFiles;

SIZE(STextWidgetSourceFiles, 0xb0);

#endif

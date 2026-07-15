#ifndef HOMM2_BASE_ICONMONORLE_H
#define HOMM2_BASE_ICONMONORLE_H

// Serialized run-length domain used by monochrome icon streams.
typedef enum IconMonoRleCommandConstant {
    ICON_RLE_MONO_NEWLINE_COMMAND = 0x00,
    ICON_RLE_MONO_END_COUNT = 0x00,
    ICON_RLE_MONO_SKIP_FLAG = 0x80,
    ICON_RLE_MONO_RUN_MASK = 0x7f
} IconMonoRleCommandConstant;

#endif // HOMM2_BASE_ICONMONORLE_H

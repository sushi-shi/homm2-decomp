#ifndef HOMM2_BASE_ICONMONORLE_H
#define HOMM2_BASE_ICONMONORLE_H

// Serialized run-length domain used by monochrome icon streams.
HOMM2_ENUM_VALUES_BEGIN(IconMonoRleCommandConstant)
    ICON_RLE_MONO_NEWLINE_COMMAND = 0x00,
    ICON_RLE_MONO_END_COUNT = 0x00,
    ICON_RLE_MONO_SKIP_FLAG = 0x80,
    ICON_RLE_MONO_RUN_MASK = 0x7f
HOMM2_ENUM_VALUES_END(IconMonoRleCommandConstant)

#endif // HOMM2_BASE_ICONMONORLE_H

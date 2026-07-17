#ifndef HOMM2_BASE_MIDI_TYPES_H
#define HOMM2_BASE_MIDI_TYPES_H

#include <va.h>

H2_ENUM_BEGIN(MidiTextSlotSize)
    MIDI_TEXT_SHORT_SLOT_SIZE = 0x4,
    MIDI_TEXT_LOG_SLOT_SIZE = 0x8,
    MIDI_TEXT_FILENAME_FORMAT_SLOT_SIZE = 0x14
H2_ENUM_END(MidiTextSlotSize)

typedef struct SMidiText {
    char startupBegin[MIDI_TEXT_SHORT_SLOT_SIZE];
    char startupDriver[MIDI_TEXT_SHORT_SLOT_SIZE];
    char startupOpen[MIDI_TEXT_LOG_SLOT_SIZE];
    char startupOpenResult[MIDI_TEXT_LOG_SLOT_SIZE];
    char shutdownBegin[MIDI_TEXT_SHORT_SLOT_SIZE];
    char shutdownDriver[MIDI_TEXT_SHORT_SLOT_SIZE];
    char shutdownComplete[MIDI_TEXT_SHORT_SLOT_SIZE];
    char playBegin[MIDI_TEXT_LOG_SLOT_SIZE];
    char filenameFormat[MIDI_TEXT_FILENAME_FORMAT_SLOT_SIZE];
} SMidiText;

SIZE(SMidiText, 0x40);

#endif

#ifndef HOMM2_INPUTMGR_H
#define HOMM2_INPUTMGR_H

#include <Ints.h>

i32 KeyboardMessageHandler(void*, u32 message, u32 virtualKey, i32l messageData);
i32 MouseMessageHandler(void*, u32 message, u32, i32l messageData);
void CheckChangeCursor(i32 x, i32 y, i32 force);
// Maps a key event's code through Buka's CP1251 character table.
void TranslateInputCharacterCp1251(struct tag_message& event);

#endif

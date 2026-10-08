#ifndef HOMM2_EDITOR_RUMREDIT_H
#define HOMM2_EDITOR_RUMREDIT_H

// The rumour editor (src/EDITOR/rumredit.cpp, rumredit.bin):
// eventsManager::EditRumour, its fill-in and its dialog handler.

#include <H2/Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>

// The record header and the text the open rumour dialog edits.
extern rumourEventExtra gRumour;
#define gRumourText gRumorTextCache // spelling fixes .bss order
extern char* gRumourText;

MessageDispatchResult EditRumourHandler(struct tag_message& message);

#endif

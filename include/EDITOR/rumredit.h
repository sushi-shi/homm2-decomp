#ifndef HOMM2_EDITOR_RUMREDIT_H
#define HOMM2_EDITOR_RUMREDIT_H

// The rumour editor (src/EDITOR/rumredit.cpp, rumredit.bin):
// eventsManager::EditRumor, its text update and its dialog handler.

#include <va.h>
#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>

// The record header and the text the open rumour dialog edits.
extern rumourEventExtra gRumor;
#define gRumorText gRumorTextCache // spelling fixes .bss order
extern char* gRumorText;

MessageDispatchResult EditRumorHandler(struct tag_message& message);

#endif

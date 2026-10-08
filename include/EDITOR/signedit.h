#ifndef HOMM2_EDITOR_SIGNEDIT_H
#define HOMM2_EDITOR_SIGNEDIT_H

// The sign and bottle editor (src/EDITOR/signedit.cpp): eventsManager::EditSign,
// its fill-in and its dialog handler. It reuses the rumour dialog
// (rumredit.bin).

#include <H2/Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>

// The record header and the text the open sign dialog edits.
extern signEventExtra gSign;
extern char* gSignText;

MessageDispatchResult EditSignHandler(struct tag_message& message);

#endif

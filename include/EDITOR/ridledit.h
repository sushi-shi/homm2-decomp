#ifndef HOMM2_EDITOR_RIDLEDIT_H
#define HOMM2_EDITOR_RIDLEDIT_H

// The sphinx editor (src/EDITOR/ridledit.cpp, ridledit.bin):
// eventsManager::EditSphinx, its dialog update and its handler.

#include <va.h>
#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>

// The record header and the riddle the open sphinx dialog edits.
extern mapEventExtra gSphinx;
extern char* gSphinxText;

MessageDispatchResult EditSphinxHandler(struct tag_message& message);

#endif

#ifndef HOMM2_EDITOR_X_SPEDIT_H
#define HOMM2_EDITOR_X_SPEDIT_H

// The spell scroll editor (src/EDITOR/x_spedit.cpp, x_spedit.bin):
// eventsManager::EditSpellScroll and its dialog handler.

#include <H2/Ints.h>
#include <BASE/message.h>

// The spell the open dialog's list has selected.
extern i32 gSpellScrollChoice;

MessageDispatchResult EditSpellScrollHandler(struct tag_message& message);

#endif // HOMM2_EDITOR_X_SPEDIT_H

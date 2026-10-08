#ifndef HOMM2_EDITOR_X_SPEDIT_H
#define HOMM2_EDITOR_X_SPEDIT_H


#include <H2/Ints.h>
#include <BASE/message.h>


extern i32 gSpellScrollChoice;

MessageDispatchResult EditSpellScrollHandler(struct tag_message& message);

#endif

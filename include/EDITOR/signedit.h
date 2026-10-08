#ifndef HOMM2_EDITOR_SIGNEDIT_H
#define HOMM2_EDITOR_SIGNEDIT_H


#include <H2/Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>


extern signEventExtra gSign;
extern char* gSignText;

MessageDispatchResult EditSignHandler(struct tag_message& message);

#endif

#ifndef HOMM2_EDITOR_RUMREDIT_H
#define HOMM2_EDITOR_RUMREDIT_H


#include <H2/Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>


extern rumourEventExtra gRumour;
extern char* gRumourText;

MessageDispatchResult EditRumourHandler(struct tag_message& message);

#endif

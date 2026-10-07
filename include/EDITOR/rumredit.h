#ifndef HOMM2_EDITOR_RUMREDIT_H
#define HOMM2_EDITOR_RUMREDIT_H


#include <Ints.h>
#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>


extern rumourEventExtra gRumour;
extern char* gRumourText;

MessageDispatchResult EditRumourHandler(struct tag_message& message);

#endif

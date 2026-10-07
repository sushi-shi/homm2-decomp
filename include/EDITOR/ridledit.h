#ifndef HOMM2_EDITOR_RIDLEDIT_H
#define HOMM2_EDITOR_RIDLEDIT_H


#include <Ints.h>
#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/EVENTS.h>


extern mapEventExtra gSphinx;
extern char* gSphinxText;

MessageDispatchResult EditSphinxHandler(struct tag_message& message);

#endif

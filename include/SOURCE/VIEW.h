#ifndef HOMM2_VIEW_H
#define HOMM2_VIEW_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/combatTypes.h>

struct tag_message;

MessageDispatchResult HandleViewGeneral(struct tag_message&);
extern H2EnumStorage<CombatSide, i32> iViewGeneralWhichSide;

#endif

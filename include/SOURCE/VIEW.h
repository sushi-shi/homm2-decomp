#ifndef HOMM2_VIEW_H
#define HOMM2_VIEW_H

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/combatTypes.h>

struct tag_message;

MessageDispatchResult HandleViewGeneral(struct tag_message& message);
extern H2_ENUM_STORAGE(CombatSide, i32) iViewGeneralWhichSide;

#endif

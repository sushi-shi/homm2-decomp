#ifndef HOMM2_SOURCE_VIEW_H
#define HOMM2_SOURCE_VIEW_H

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/combatTypes.h>

struct tag_message;

MessageDispatchResult HandleViewGeneral(struct tag_message& message);
extern H2EnumStorage<CombatSide, i32> iViewGeneralWhichSide;

#endif

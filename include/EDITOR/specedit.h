#ifndef HOMM2_EDITOR_SPECEDIT_H
#define HOMM2_EDITOR_SPECEDIT_H


#include <Ints.h>
#include <Ints.h>
#include <BASE/message.h>

class heroWindow;


extern heroWindow* gSpecWindow;

extern i32 gLandPercent;


b32 EditMapSpecifications(b32 randomMap);
void FillVictoryConditionList(void);
void SetVictoryConditionChoice(i32 choice);
void FillLossConditionList(void);
void SetLossConditionChoice(i32 choice);
void UpdateSpecificationsWindow(void);
void AddMapEvent(void);
void EditMapEvent(void);
void DeleteMapEvent(void);
void AddMapRumour(void);
void EditMapRumour(void);
void DeleteMapRumour(void);
MessageDispatchResult SpecificationsHandler(struct tag_message& message);

#endif

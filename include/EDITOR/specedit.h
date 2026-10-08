#ifndef HOMM2_EDITOR_SPECEDIT_H
#define HOMM2_EDITOR_SPECEDIT_H

// The map specification dialog (src/EDITOR/specedit.cpp, specedit.bin): the
// map's name and description, difficulty, players, victory and loss
// conditions, time events and rumours.

#include <H2/Ints.h>
#include <BASE/message.h>

class heroWindow;

// The open specification dialog.
extern heroWindow* gSpecWindow;
// The share of the map's cells that are not water, in percent.
extern i32 gLandPercent;

// Edits gEditMapHeader; cancelling restores it. The editor passes whether
// the map came from the random map generator. Returns false when cancelled.
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

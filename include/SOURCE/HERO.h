#ifndef HOMM2_HERO_H
#define HOMM2_HERO_H

#include <Ints.h>
#include <BASE/message.h>

struct tag_message;

void HeroMessageUpdate(const char* text);
void UpdateHeroScreenStatusBar(struct tag_message& message);
MessageDispatchResult HeroHandler(struct tag_message& message);
void RedrawHeroScreen(void);
i32 HeroView(i32 heroId, b32 noDismiss, b32 fadeAlreadyOut);
void SetupHeroView(void);
void DoHeroSplit(i32 destinationSlot, i32 sourceSlot);

#endif

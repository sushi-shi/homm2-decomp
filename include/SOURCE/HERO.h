#ifndef HOMM2_HERO_H
#define HOMM2_HERO_H

#include <Ints.h>
#include <BASE/message.h>

struct tag_message;

void HeroMessageUpdate(const char*);
void UpdateHeroScreenStatusBar(struct tag_message&);
MessageDispatchResult HeroHandler(struct tag_message&);
void RedrawHeroScreen(void);
i32 HeroView(i32, b32, b32);
void SetupHeroView(void);
void DoHeroSplit(i32, i32);

#endif

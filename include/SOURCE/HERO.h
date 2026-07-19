#ifndef HOMM2_HERO_H
#define HOMM2_HERO_H

#include <Ints.h>

struct tag_message;

void HeroMessageUpdate(char*);
void UpdateHeroScreenStatusBar(struct tag_message&);
i32 HeroHandler(struct tag_message&);
void RedrawHeroScreen(void);
i32 HeroView(i32, i32, i32);
void SetupHeroView(void);
void DoHeroSplit(i32, i32);

#endif

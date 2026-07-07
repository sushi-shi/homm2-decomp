#ifndef HOMM2_HERO_H
#define HOMM2_HERO_H
// Declarations of the free functions DEFINED in HERO.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

void HeroMessageUpdate(char *);
void UpdateHeroScreenStatusBar(struct tag_message &);
int HeroHandler(struct tag_message &);
void RedrawHeroScreen(void);
int HeroView(int, int, int);
void SetupHeroView(void);
void DoHeroSplit(int, int);

#endif // HOMM2_HERO_H

#ifndef HOMM2_BASE_MUSICFLAGS_H
#define HOMM2_BASE_MUSICFLAGS_H

#include <H2/Ints.h>


extern u8 gMusicFlagA;
extern u8 gMusicFlagB;
extern u8 gMusicFeatureEnabled;

u8 GetMusicFlagA(void);
u8 GetMusicFlagB(void);
u8 MusicFlagsActive(void);

#endif

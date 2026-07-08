#ifndef HOMM2_BASE_MSS_H
#define HOMM2_BASE_MSS_H
// Miles Sound System (AIL) — external library imports used by soundmgr.cpp.
// These are NOT defined in our code (they come from the Miles .lib); declared
// here like the Win32 imports so the sound-manager TU can link. Extend as more
// AIL_* calls are recovered from the soundManager methods.
extern "C" void _AIL_serve_0(void);
#endif // HOMM2_BASE_MSS_H

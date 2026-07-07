#ifndef HOMM2_HELPERS_H
#define HOMM2_HELPERS_H
// Consolidated extern declarations of cross-TU helper / runtime free functions. Single
// source of truth so signatures cannot drift between TUs (retail already had e.g. two
// incompatible _open decls). Staging location — move each to its owning module header
// once known.
#include <_all.h>

// --- project helpers (defined in various game/base TUs) ---
extern int Random(int, int);
extern void LogStr(char *);
extern void LogInt(char *, int, int, int, int, int, int, int);
extern void *BaseAlloc(unsigned int, char *, int);
extern void BaseFree(void *, char *, int);
extern void FileError(char *);
extern long FileSize(char *);
extern void AiPrint(char *);
extern void IconToBitmap(class icon *, class bitmap *, int, int, int, int, int, int, int, int, int);
extern int EventWindowHandler(struct tag_message &);
extern void SendMapChange(int, signed char, unsigned char, unsigned char, int, unsigned char, unsigned char);
extern void SetFullScreenStatus(int);
extern void ShutDown(char *);
extern int calc_crc_long(unsigned char *, int);
extern int CalcTerrainCost(int, int, int, int, int, int);

// --- extern "C" project functions ---
extern "C" int __cdecl BitTest(void *, int);
extern "C" void PollSound(void);
// CRT (_open/_close/_read/_write, memset, ...) come from the real CRT headers
// (<io.h>, <string.h>), NOT from here.

#endif // HOMM2_HELPERS_H

#ifndef HOMM2_TYPES_H
#define HOMM2_TYPES_H
// Hand-maintained shared home for record/model structs that GLOBAL declarations reference,
// so their layout is declared once and cannot drift between TUs. TU-PRIVATE modelling
// structs do NOT belong here — they live in per-TU headers (they often need TU-local
// typedefs). Move each type below to its owning module header as ownership becomes clear.
// Included by _globals.h.
#include <Ints.h>
// forward declarations (was <_all.h>):
class sample;

typedef unsigned int UInt32;   // crc32Table[]
struct MemEntry;               // gpMemEntry — full def not yet reconstructed; used via pointer
struct _SAMPLE;                // SAMPLE2::pMem — opaque

struct configStruct { char pad[0x1a0]; };                                   // gConfig
struct SCreatureInfo { unsigned short value; char pad[24]; };               // gCreatureInfo[]
struct tag_tilePoint { signed char x; signed char _1; signed char y; signed char _3; };  // normalDirTable[]
struct tag_monsterInfo { short m_0; char m_pad[24]; };                      // gMonsterDatabase[] (sizeof 26)
struct SSpellInfo { char m_pad0[14]; unsigned char m_e; char m_pad1[7]; };  // gsSpellInfo[] (sizeof 22)
struct SNetPlayerInfo { char m_pad[0xcc]; };                                // gsNetPlayerInfo[]
struct SAMPLE2 { class sample *pSample; struct _SAMPLE *pMem; };            // NULL_SAMPLE2

#pragma pack(push, 1)
struct monsterRV { int rv; char pad[22]; };                                 // gMonsterInfo[] (26B, pack 1)
struct SWinSetup { unsigned char m_0; unsigned short m_1; char *m_3; };     // gWinSetup[] (7B, pack 1)
#pragma pack(pop)

#endif // HOMM2_TYPES_H

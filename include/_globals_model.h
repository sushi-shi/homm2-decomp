#ifndef HOMM2_GLOBALS_MODEL_H
#define HOMM2_GLOBALS_MODEL_H
#include <va.h>
// Synthetic / MODELLING globals — readable aliases the matcher introduced for data that has
// NO CodeView symbol of its own: overlay views of the monster-database region, and .rdata
// float literals (MSVC-hashed ??_C@ names) referenced as named consts for reloc control.
// They DO have retail addresses; those marked DATA-PENDING are pinned from the reloc when
// their user function is byte-matched (they alias object-local literals / computed bases).
// Kept OUT of _globals.h (which is CodeView-backed) — assert_globals_data exempts these.
#include <_types.h>
#include <windows.h>
#include <nb30.h>
#include <SOURCE/comwin.h>

// VAs pinned from the retail disassembly of the referencing fns (no CodeView symbol of their own):
DATA(0x004faeb8) extern SCreatureInfo gCreatureInfo[];   // monster-DB view (game::ExperienceValueOfStack)
DATA(0x004faeb2) extern monsterRV gMonsterInfo[100];     // monster-DB view, 26B recs (philAI::ComputeUpgradeValue)
// (gRVWeightOre/Merc/Crystal removed — they were just gafAITurnCostResource[2|1|4]; the code
//  now indexes the array directly via the RES_* enum, which matches the retail byte-for-byte.)
DATA(0x00533b40) extern POINT gMouseScreenPt;    // GetCursorPos scratch (mouseManager::MouseCoords)
DATA(0x00533b48) extern POINT gMouseCheckPt;     // GetCursorPos scratch (mouseManager::CheckUpdateMousePos/ShowColorPointer)


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
DATA(0x00528d38) extern int const_00128d38;            // 13-byte BSS flag buffer (??_C@_0N@PMOM@...)

// TILE.cpp inline-asm scratch (no CodeView symbol): the blitter parks the mode word and a
// mirror-path row counter in static storage between its __asm sections.
DATA(0x0051fec0) extern unsigned int gTileMode;
DATA(0x0051fec4) extern int gTileRowCtr;

// netwin.cpp netbios per-session status byte array (no CodeView symbol), indexed by session id.
DATA(0x005173b4) extern char gNetStatus[];

// netwin.cpp netbios probe state (no CodeView symbol): the LANA number netbios was found on,
// and the "netbios present" flag, set by is_netbios_avail and read by nb_init.
DATA(0x005173a0) extern unsigned char gNetbiosAvail;
DATA(0x005173a4) extern unsigned char gNetbiosLana;

// netwin.cpp netbios subsystem state (no CodeView symbols — file-scope statics in netwin.c).
// Sizes are confirmed by the address tiling (NCB=0x40, CRITICAL_SECTION=0x18, buffers adjacent).
DATA(0x005173a8) extern unsigned char gNbShutdown;         // shutdown flag, cleared by nb_init
DATA(0x005173ac) extern unsigned char gNbMaxSess;          // max session count (nb_init param2)
DATA(0x0051739c) extern int           gNbCallRetries;      // FUN_004a7fe9 retry counter (cap 0x14)
DATA(0x005173b0) extern unsigned char gNbLocalNum;         // local netbios name number
DATA(0x005173c0) extern char         *gNbGroupName;        // netbios group name ("Empire Too")
DATA(0x005173c4) extern char         *gNbListenName;       // netbios listen/any name (nb_sess case 3)
DATA(0x0052ae68) extern tag_Anchor    gNbFreeQueue;        // queue anchor (init_anchor)
DATA(0x0052ae70) extern unsigned char gNbSessLsn[7];       // per-session LSN, 0xff = free slot
DATA(0x0052ae78) extern unsigned char gNbRcvData[0x7000];  // receive data pool, init 0xff
DATA(0x00531e78) extern unsigned char gNbNameBuf[0x70];    // name scratch, init 0
DATA(0x00531ee8) extern unsigned char gNbSessBuf[0x1000];  // session scratch, init 0xff
DATA(0x00532ee8) extern NCB           gNbSessNcb[7];        // per-session NCB
DATA(0x005330a8) extern NCB           gNbCtlNcb;            // control NCB
DATA(0x005330e8) extern tag_Anchor    gNbRcvQueue;         // receive queue (nb_rcv pops it)
DATA(0x005330f0) extern tag_Anchor    gNbSndQueue;         // send queue
DATA(0x005330f8) extern CRITICAL_SECTION gNbRcvLock;       // guards gNbRcvQueue
DATA(0x00533110) extern HANDLE        gNbEvents[9];         // manual-reset events
DATA(0x00533138) extern CRITICAL_SECTION gNbSndLock;       // guards gNbSndQueue

#endif // HOMM2_GLOBALS_MODEL_H

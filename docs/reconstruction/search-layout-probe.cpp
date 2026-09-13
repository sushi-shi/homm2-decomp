// U02/U03/U04: compile-only VC6 layout evidence, never linked into game code.
#include <SOURCE/searchArray.h>
#include <stddef.h>

typedef char SearchGridViewSize[sizeof(searchCell)==9 && sizeof(searchNode)==9 ? 1 : -1];
typedef char SearchStorageSize[sizeof(searchStorage)==260 ? 1 : -1];
typedef char SearchGridPointer[offsetof(searchStorage,cells)==0 && offsetof(searchStorage,nodes)==0 ? 1 : -1];
typedef char SearchDirections[offsetof(searchStorage,directions)==4 ? 1 : -1];
typedef char SearchDirectionSize[sizeof(static_cast<searchStorage*>(0)->directions)==256 ? 1 : -1];
typedef char SearchArraySize[sizeof(searchArray)==0x2518 ? 1 : -1];
typedef char SearchQueueCount[offsetof(searchArray,m_queueCount)==0 ? 1 : -1];
typedef char SearchMaxQueue[offsetof(searchArray,m_maxQueueCount)==4 ? 1 : -1];
typedef char SearchPathLength[offsetof(searchArray,m_pathLength)==8 ? 1 : -1];
typedef char SearchTargetX[offsetof(searchArray,m_specialTargetX)==12 ? 1 : -1];
typedef char SearchTargetY[offsetof(searchArray,m_specialTargetY)==16 ? 1 : -1];
typedef char SearchQueue[offsetof(searchArray,m_queue)==20 ? 1 : -1];
typedef char SearchStorage[offsetof(searchArray,m_storage)==0x2414 ? 1 : -1];

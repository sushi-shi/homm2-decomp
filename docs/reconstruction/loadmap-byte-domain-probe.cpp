// Compile-only: numeric bytes used by LoadMap and RandomizeEvents.
#include <Ints.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/town.h>

typedef char SignedCharInThisABI[(static_cast<char>(255) == -1) ? 1 : -1];
typedef char SignedByte[(static_cast<i8>(255) == -1) ? 1 : -1];
typedef char UnsignedSentinel[(static_cast<u8>(-1) == 255) ? 1 : -1];
typedef char CoordinateByteSize[(sizeof(u8) == 1) ? 1 : -1];
typedef char TypeByteSize[(sizeof(i8) == 1) ? 1 : -1];
typedef char TrailerWordSize[(sizeof(u16) == 2) ? 1 : -1];
typedef char EventXSize[(sizeof(static_cast<EventExtra*>(0)->x) == 2) ? 1 : -1];
typedef char EventYSize[(sizeof(static_cast<EventExtra*>(0)->y) == 2) ? 1 : -1];

void RequireUnsignedByte(u8*);
void RequireUnsignedWord(u16*);
void CheckMapDestinationTypes(EventExtra* event, mineRecord* mine, town* castle) {
    RequireUnsignedWord(&event->x);
    RequireUnsignedWord(&event->y);
    RequireUnsignedByte(&mine->x);
    RequireUnsignedByte(&mine->y);
    RequireUnsignedByte(&mine->guardianCount);
    RequireUnsignedByte(&castle->m_x);
    RequireUnsignedByte(&castle->m_y);
    RequireUnsignedByte(&castle->m_boatX);
    RequireUnsignedByte(&castle->m_boatY);
}

// Compile-only evidence, never linked into the game.
#include <SOURCE/KB.h>
typedef char SetupSize[sizeof(OldMainNetSetup)==212 ? 1 : -1];
typedef char BufferSize[sizeof(OldMainNetBuffer)==256 ? 1 : -1];
typedef char PlayerCount[OLD_MAIN_PLAYER_COUNT==6 ? 1 : -1];

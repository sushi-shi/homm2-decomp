// U05: compile-only VC6 layout assertions; not linked into reconstructed code.
#include <SOURCE/KB_TYPES.h>
#include <SOURCE/army.h>
#include <stddef.h>

typedef char MonsterSize[sizeof(tag_monsterInfo) == 26 ? 1 : -1];
typedef char MonsterCost[offsetof(tag_monsterInfo, cost) == 0 ? 1 : -1];
typedef char MonsterValue[offsetof(tag_monsterInfo, fightValue) == 2 ? 1 : -1];
typedef char MonsterIcon[offsetof(tag_monsterInfo, iconIndex) == 6 ? 1 : -1];
typedef char MonsterSprite[offsetof(tag_monsterInfo, spriteName) == 17 ? 1 : -1];
typedef char MonsterFlagsOffset[offsetof(tag_monsterInfo, attributes) == 22 ? 1 : -1];
typedef char MonsterFlagsSize[sizeof(MonsterFlags) == 4 ? 1 : -1];
typedef char ArmyRecordSize[sizeof(army) == 0x482 ? 1 : -1];
typedef char ArmyMonsterOffset[offsetof(army, m_monster) == 0xba ? 1 : -1];

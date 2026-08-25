#ifndef HOMM2_IRONFIST_HOOKS_H
#define HOMM2_IRONFIST_HOOKS_H

#include <Ints.h>

class army;
class hero;
class mapCell;
class town;

enum class SpellType : i8;

/*
 * Game-side entry points into the Ironfist layer. The recovered sources call
 * these as single lines at the same places Ironfist patched its callbacks
 * into the retail executable, so merges from the upstream port stay small.
 */
void Ironfist_Startup(void);
void Ironfist_ResetGameState(void);
void Ironfist_NewMap(char* filename);
void Ironfist_AdvManagerReady(void);
void Ironfist_NewDay(void);
void Ironfist_CheckEndGame(void);
void Ironfist_HeroMoved(void);
b32 Ironfist_LocationVisit(mapCell* cell, i32 x, i32 y);
void Ironfist_CastleConquered(i32 castleIdx, i32 playerIdx);
void Ironfist_ArtifactStat(hero* h, i32 artifact, b32 take);
void Ironfist_TownOpened(town* t);
void Ironfist_UnitRecruit(i32 creatureType);
i32 Ironfist_CalcMobility(hero* h, i32 points);
i32 Ironfist_CalcLuck(hero* h, army* stack, town* castle, i32 luck);
i32 Ironfist_CalcMorale(hero* h, town* twn, i32 morale);
i32 Ironfist_CalcManaCost(hero* h, i32 spell, i32 cost);
float Ironfist_CalcSpellChance(army* stack, SpellType spell, float chance);
void Ironfist_MeleeAttack(army* attacker, i32 retaliation);
void Ironfist_MeleeAttackComplete(army* attacker, army* target);
void Ironfist_MonsterInteract(mapCell* cell);
void Ironfist_TooltipText(mapCell* cell, i32 x, i32 y);
void Ironfist_AppendLuckInfo(hero* h);
void Ironfist_AppendMoraleInfo(hero* h);
void Ironfist_BattleStart(void);
void Ironfist_Shutdown(void);

void Ironfist_ShareVision(i32 sourcePlayer, i32 destPlayer);
void Ironfist_CancelShareVision(i32 sourcePlayer, i32 destPlayer);
void Ironfist_SetVisibilityShared(i32 x, i32 y, i32 player, i32 radius);
void Ironfist_AllWaterVisibleShared(i32 player);
void Ironfist_DisallowBuilding(i32 townIdx, i32 building);
b32 Ironfist_BuildingDisallowed(town* t, i32 building);
void Ironfist_SetAIArmySharing(b32 allow);
b32 Ironfist_AIArmySharingAllowed(void);
void Ironfist_ForceChase(i32 sourceHeroId, i32 destHeroId, b32 force);
b32 Ironfist_ForcedChaseValue(i32 heroId, i32* value);
void Ironfist_UndoWellGrowth(void);
void Ironfist_HeroPoolRegainMobility(void);

#endif

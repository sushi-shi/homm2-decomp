#ifndef HOMM2_IRONFIST_HOOKS_H
#define HOMM2_IRONFIST_HOOKS_H

#include <string>

#include <Ints.h>

class army;
class hero;
class mapCell;
class town;

enum class SpellType : i8;

namespace ironfist::hooks {

/*
 * Game-side entry points into the Ironfist layer. The recovered sources call
 * these as single lines at the same places Ironfist patched its callbacks
 * into the retail executable, so merges from the upstream port stay small.
 */
void NewDay();
void CheckEndGame();
void HeroMoved();
b32 LocationVisit(mapCell* cell, i32 x, i32 y);
void CastleConquered(i32 castleIdx, i32 playerIdx);
void ArtifactChanged(hero* h, i32 artifact, b32 take);
void TownOpened(town* t);
void UnitRecruited(i32 creatureType);
i32 ModifyMobility(hero* h, i32 points);
i32 ModifyLuck(hero* h, army* stack, town* castle, i32 luck);
i32 ModifyMorale(hero* h, town* twn, i32 morale);
i32 ModifyManaCost(hero* h, i32 spell, i32 cost);
float ModifySpellChance(army* stack, SpellType spell, float chance);
void MeleeAttackStarted(army* attacker, i32 retaliation);
void MeleeAttackCompleted(army* attacker, army* target);
void MonsterInteraction(mapCell* cell);
b32 OverrideTooltip(mapCell* cell, i32 x, i32 y, std::string& text);
void AppendLuckInfo(hero* h);
void AppendMoraleInfo(hero* h);
void BattleStarted();

} // namespace ironfist::hooks

#endif

#ifndef HOMM2_IRONFIST_COMBAT_STATE_H
#define HOMM2_IRONFIST_COMBAT_STATE_H

#include <array>
#include <bitset>
#include <vector>
#include <IRONFIST/creatures.h>
#include <SOURCE/combatManager.h>

namespace ironfist::state {

// Identifies an incarnation of a battle slot, including after a summon reuses it.
struct StackIdentity {
    i32 side = -1;
    i32 slot = -1;
    u64 generation = 0;
};

// Mechanical consequences of the chosen movement plan, independent of frames.
struct AttackApproach {
    bool jump = false;
    bool charge = false;
    bool distantTeleport = false;
};

class CombatState {
public:
    struct FireWall {
        i32 hexIdx;
        i32 turnsLeft;
        i32 currentFrame;
    };

    void BeginBattle(combatManager& owner);
    void EndBattle();
    void ResetStack(army& stack);
    void RemoveStack(army& stack);
    StackIdentity Identity(const army& stack) const;
    army* Resolve(StackIdentity identity) const;
    bool IsActiveFor(const combatManager* owner) const { return owner && owner_ == owner; }
    u64 BattleGeneration() const { return owner_ ? battleGeneration_ : 0; }
    hero* Captain(u64 generation) const;
    AttackApproach Approach(const army& stack) const;
    void SetApproach(army& stack, AttackApproach approach);

    void GrantAbility(army& stack, CreatureAttribute ability);
    bool HasAbility(const army& stack, CreatureAttribute ability) const;
    bool HasAbilityCharge(const army& stack, CreatureAttribute ability) const;
    bool ConsumeAbility(army& stack, CreatureAttribute ability);
    bool IsAnimating(const army& stack, CreatureAttribute ability) const;
    void StartAnimation(army& stack, CreatureAttribute ability);
    void FinishAnimation(army& stack, CreatureAttribute ability);

    i32 ShieldHP(const army& stack) const;
    bool RefillShield(army& stack, i32 capacity);
    void ClearShield(army& stack);
    i32 AbsorbDamage(army& stack, i32 damage);

    const std::vector<FireWall>& FireWalls() const { return walls_; }
    void AddOrRefreshFireWall(i32 hex, i32 turns, i32 frame = 0);
    void AdvanceRound();
    void AdvanceWallAnimation(i32 frameCount);

private:
    struct StackRecord {
        u64 generation = 0;
        bool active = false;
        std::bitset<H2EnumIndex(CreatureAttribute::Count)> abilities;
        std::bitset<H2EnumIndex(CreatureAttribute::Count)> charges;
        std::bitset<H2EnumIndex(CreatureAttribute::Count)> animating;
        i32 shieldHP = 0;
        AttackApproach approach;
    };

    StackRecord* Find(army& stack);
    const StackRecord* Find(const army& stack) const;
    StackIdentity Locate(const army& stack) const;
    combatManager* owner_ = nullptr;
    u64 nextGeneration_ = 0;
    u64 battleGeneration_ = 0;
    std::array<std::array<StackRecord, COMBAT_ARMY_STORAGE_SLOT_COUNT>, COMBAT_SIDE_COUNT> stacks_{};
    std::vector<FireWall> walls_;
};

} // namespace ironfist::state
#endif

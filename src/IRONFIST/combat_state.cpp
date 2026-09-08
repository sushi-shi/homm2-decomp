#include <IRONFIST/combat_state.h>
#include <algorithm>

namespace ironfist::state {

AttackApproach CombatState::Approach(const army& stack) const {
    const auto* record = Find(stack);
    return record && record->active ? record->approach : AttackApproach{};
}

void CombatState::SetApproach(army& stack, AttackApproach approach) {
    if (auto* record = Find(stack); record && record->active)
        record->approach = approach;
}

void CombatState::BeginBattle(combatManager& owner) {
    EndBattle();
    owner_ = &owner;
    battleGeneration_ = ++nextGeneration_;
}

hero* CombatState::Captain(u64 generation) const {
    return owner_ && generation == battleGeneration_
        && owner_->m_heroes[H2EnumIndex(COMBAT_DEFENDER_SIDE)] == &owner_->m_captain
        ? &owner_->m_captain : nullptr;
}

void CombatState::EndBattle() {
    stacks_ = {};
    walls_.clear();
    owner_ = nullptr;
    // Keep the sequence across battles: an old handle must never revive.
}

StackIdentity CombatState::Locate(const army& stack) const {
    if (owner_) {
        for (i32 side = 0; side < COMBAT_SIDE_COUNT; ++side)
            for (i32 slot = 0; slot < COMBAT_ARMY_STORAGE_SLOT_COUNT; ++slot)
                if (&owner_->m_armies[side][slot] == &stack)
                    return {side, slot, stacks_[side][slot].generation};
    }
    return {};
}

const CombatState::StackRecord* CombatState::Find(const army& stack) const {
    const auto id = Locate(stack);
    return id.side < 0 ? nullptr : &stacks_[id.side][id.slot];
}

CombatState::StackRecord* CombatState::Find(army& stack) {
    return const_cast<StackRecord*>(static_cast<const CombatState&>(*this).Find(stack));
}

void CombatState::RemoveStack(army& stack) {
    if (auto* record = Find(stack))
        *record = {};
}

void CombatState::ResetStack(army& stack) {
    if (auto* record = Find(stack)) {
        *record = {};
        record->generation = ++nextGeneration_;
        record->active = true;
    }
}

StackIdentity CombatState::Identity(const army& stack) const {
    const auto* record = Find(stack);
    return record && record->active ? Locate(stack) : StackIdentity{};
}

army* CombatState::Resolve(StackIdentity id) const {
    if (!owner_ || id.side < 0 || id.side >= COMBAT_SIDE_COUNT || id.slot < 0
        || id.slot >= COMBAT_ARMY_STORAGE_SLOT_COUNT)
        return nullptr;
    const auto& record = stacks_[id.side][id.slot];
    return record.active && record.generation == id.generation
        ? &owner_->m_armies[id.side][id.slot] : nullptr;
}

void CombatState::GrantAbility(army& stack, CreatureAttribute ability) {
    if (auto* record = Find(stack); record && record->active) {
        record->abilities.set(H2EnumIndex(ability));
        record->charges.set(H2EnumIndex(ability));
    }
}

bool CombatState::HasAbility(const army& stack, CreatureAttribute ability) const {
    const auto* record = Find(stack);
    return record && record->active && record->abilities.test(H2EnumIndex(ability));
}

bool CombatState::HasAbilityCharge(const army& stack, CreatureAttribute ability) const {
    const auto* record = Find(stack);
    return record && record->active && record->charges.test(H2EnumIndex(ability));
}

bool CombatState::ConsumeAbility(army& stack, CreatureAttribute ability) {
    if (!HasAbilityCharge(stack, ability))
        return false;
    Find(stack)->charges.reset(H2EnumIndex(ability));
    return true;
}

bool CombatState::IsAnimating(const army& stack, CreatureAttribute ability) const {
    const auto* record = Find(stack);
    return record && record->active && record->animating.test(H2EnumIndex(ability));
}

void CombatState::StartAnimation(army& stack, CreatureAttribute ability) {
    if (auto* record = Find(stack); record && record->active)
        record->animating.set(H2EnumIndex(ability));
}

void CombatState::FinishAnimation(army& stack, CreatureAttribute ability) {
    if (auto* record = Find(stack))
        record->animating.reset(H2EnumIndex(ability));
}

i32 CombatState::ShieldHP(const army& stack) const {
    const auto* record = Find(stack);
    return record && record->active ? record->shieldHP : 0;
}

bool CombatState::RefillShield(army& stack, i32 capacity) {
    if (auto* record = Find(stack); record && record->active && record->shieldHP < capacity) {
        record->shieldHP = capacity;
        return true;
    }
    return false;
}

void CombatState::ClearShield(army& stack) {
    if (auto* record = Find(stack))
        record->shieldHP = 0;
}

i32 CombatState::AbsorbDamage(army& stack, i32 damage) {
    if (damage <= 0)
        return damage;
    if (auto* record = Find(stack); record && record->active) {
        const i32 absorbed = std::min(damage, record->shieldHP);
        record->shieldHP -= absorbed;
        return damage - absorbed;
    }
    return damage;
}

void CombatState::AddOrRefreshFireWall(i32 hex, i32 turns, i32 frame) {
    if (!owner_)
        return;
    for (auto& wall : walls_) {
        if (wall.hexIdx == hex) {
            wall = {hex, turns, frame};
            return;
        }
    }
    walls_.push_back({hex, turns, frame});
}

void CombatState::AdvanceRound() {
    const auto dodge = H2EnumIndex(CreatureAttribute::AstralDodge);
    for (auto& side : stacks_)
        for (auto& record : side)
            if (record.active && record.abilities.test(dodge))
                record.charges.set(dodge);
    for (auto& wall : walls_)
        --wall.turnsLeft;
    walls_.erase(std::remove_if(walls_.begin(), walls_.end(),
        [](const FireWall& wall) { return wall.turnsLeft < 0; }), walls_.end());
}

void CombatState::AdvanceWallAnimation(i32 frameCount) {
    for (auto& wall : walls_)
        wall.currentFrame = frameCount > 0 ? (wall.currentFrame + 1) % frameCount : 0;
}

} // namespace ironfist::state

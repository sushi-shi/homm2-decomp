#include <IRONFIST/combat_movement.h>
#include <IRONFIST/state.h>

#include <array>
#include <SOURCE/ARMY.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist::movement {

bool Execute(army& actor, const Plan& plan) {
    if (!plan || actor.m_hex != plan.source)
        return false;
    auto& extensions = state::Get().combat;
    const auto identity = extensions.Identity(actor);
    for (size_t i = 0; i < plan.steps.size(); ++i) {
        const auto& step = plan.steps[i];
        if (extensions.Resolve(identity) != &actor || actor.m_quantity <= 0 || actor.m_hex != step.from)
            return false;
        const Traversal traversal(*gpCombatManager, actor, extensions);
        if (!traversal.CanLand(step.to, step.kind == StepKind::Fly))
            return false;
        switch (step.kind) {
            case StepKind::Walk:
                if (!traversal.CanStep(actor.m_hex, step.direction))
                    return false;
                actor.Walk(step.direction, i + 1 == plan.steps.size(), i != 0);
                break;
            case StepKind::Jump:
                for (i32 crossed : step.crossedHexes)
                    if (!traversal.CanTraverse(crossed))
                        return false;
                if (step.attackJump)
                    extensions.StartAnimation(actor, CreatureAttribute::Jumper);
                actor.ArcJump(step.from, step.to);
                break;
            case StepKind::Charge: {
                if (!traversal.ClearLine(step.from, step.to))
                    return false;
                std::array<std::array<bool, COMBAT_ARMY_STORAGE_SLOT_COUNT>, COMBAT_SIDE_COUNT> seen{};
                std::vector<i32> affected;
                for (i32 hex : step.crossedHexes) {
                    const auto& cell = gpCombatManager->m_hexCells[hex];
                    const i32 side = H2EnumIndex(cell.m_occupantSide);
                    const i32 slot = cell.m_occupantIndex;
                    if (side < 0 || side >= COMBAT_SIDE_COUNT || slot < 0 || slot >= COMBAT_ARMY_STORAGE_SLOT_COUNT
                        || cell.m_occupantSide == actor.m_side
                        || (cell.m_occupantSide == actor.m_targetSide && slot == actor.m_targetIndex)
                        || seen[side][slot])
                        continue;
                    seen[side][slot] = true;
                    affected.push_back(hex);
                }
                gCharging = true;
                // Resolve the planned crossed stacks regardless of rendering.
                actor.ChargingDamage(affected);
                actor.FlyTo(step.to);
                break;
            }
            case StepKind::Fly:
                actor.FlyTo(step.to);
                break;
        }
    }
    return extensions.Resolve(identity) == &actor && actor.m_quantity > 0;
}

} // namespace ironfist::movement

#include <IRONFIST/combat_movement.h>

#include <algorithm>
#include <array>
#include <limits>
#include <queue>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist::movement {
namespace {

bool OnBoard(i32 hex) {
    return hex >= 0 && hex < COMBAT_HEX_COUNT && hex % ARMY_HEX_COLUMNS != 0
        && hex % ARMY_HEX_COLUMNS != ARMY_HEX_COLUMNS - 1;
}

bool CastleWall(i32 hex) {
    switch (hex) {
        case 9: case 22: case 34: case 47: case 59:
        case 73: case 86: case 100: case 113: case 92: return true;
        default: return false;
    }
}

// Axial coordinates for the same offset grid as SetupAdjacencyArray.
i32 HexDistance(i32 from, i32 to) {
    const i32 r1 = from / ARMY_HEX_COLUMNS;
    const i32 r2 = to / ARMY_HEX_COLUMNS;
    const i32 q1 = from % ARMY_HEX_COLUMNS - (r1 + (r1 & 1)) / 2;
    const i32 q2 = to % ARMY_HEX_COLUMNS - (r2 + (r2 & 1)) / 2;
    const i32 dq = q2 - q1;
    const i32 dr = r2 - r1;
    return (std::abs(dq) + std::abs(dr) + std::abs(dq + dr)) / 2;
}

} // namespace

i32 Plan::Length() const {
    i32 length = attackDirection ? 1 : 0;
    for (const auto& step : steps)
        length += step.cost;
    return length;
}

Plan Plan::WithinBudget(i32 budget) const {
    Plan result;
    result.reachable = reachable;
    result.source = result.destination = source;
    for (const auto& step : steps) {
        if (step.cost > budget)
            break;
        result.steps.push_back(step);
        result.destination = step.to;
        budget -= step.cost;
        if (step.endsTurn)
            break;
    }
    if (result.steps.size() == steps.size())
        result.attackDirection = attackDirection;
    return result;
}

Traversal::Traversal(const combatManager& board, const army& actor, const state::CombatState& extensions)
    : m_board(board), m_actor(actor),
      m_wide(static_cast<bool>(actor.m_monster.attributes & MONSTER_ATTRIBUTE_WIDE)),
      m_jumping(extensions.HasAbilityCharge(actor, CreatureAttribute::CREATURE_ATTRIBUTE_JUMPER)),
      m_flying(static_cast<bool>(actor.m_monster.attributes & MONSTER_ATTRIBUTE_FLYING)),
      m_charging(extensions.HasAbility(actor, CreatureAttribute::CREATURE_ATTRIBUTE_CHARGER)) {}

i32 Traversal::Adjacent(i32 source, CombatHexDirection direction) const {
    if (!OnBoard(source) || direction < COMBAT_DIRECTION_NORTHEAST || direction > COMBAT_DIRECTION_NORTHWEST)
        return ARMY_HEX_INVALID;
    const i32 next = m_board.m_adjacency[source][H2EnumIndex(direction)];
    return OnBoard(next) ? next : ARMY_HEX_INVALID;
}

bool Traversal::EmptyOrSelf(i32 hex) const {
    if (!OnBoard(hex))
        return false;
    const auto& cell = m_board.m_hexCells[hex];
    return cell.m_occupantSide == COMBAT_SIDE_NONE
        || (cell.m_occupantSide == m_actor.m_side && cell.m_occupantIndex == m_actor.m_index);
}

bool Traversal::GroundOpen(i32 hex) const {
    if (!OnBoard(hex))
        return false;
    if (!m_board.m_hexCells[hex].m_blocked)
        return true;
    if (m_board.m_inCastleCombat && (hex == COMBAT_CASTLE_GATE_APPROACH_HEX
                                  || hex == H2EnumIndex(COMBAT_CASTLE_HEX_GATE))) {
        const auto& approach = m_board.m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX];
        return m_board.m_drawbridgeState != COMBAT_DRAWBRIDGE_RAISED
            || (m_actor.m_side == COMBAT_DEFENDER_SIDE && approach.m_occupantSide == COMBAT_SIDE_NONE
                && approach.m_deadOccupantCount == 0);
    }
    return false;
}

bool Traversal::PassableCell(i32 hex, bool jump) const {
    return EmptyOrSelf(hex) && (GroundOpen(hex)
        || (jump && m_jumping && !(m_board.m_inCastleCombat && CastleWall(hex))));
}

bool Traversal::CanLand(i32 hex, bool flying) const {
    const auto legal = [&](i32 cell) {
        return EmptyOrSelf(cell) && (flying ? !m_board.m_hexCells[cell].m_blocked : GroundOpen(cell));
    };
    if (!legal(hex))
        return false;
    return !m_wide || legal(Adjacent(hex, m_actor.m_facing == ARMY_FACING_RIGHT
        ? COMBAT_DIRECTION_EAST : COMBAT_DIRECTION_WEST));
}

bool Traversal::CanStandOnCell(i32 hex) const {
    return EmptyOrSelf(hex) && GroundOpen(hex);
}

bool Traversal::CanTraverse(i32 hex) const {
    if (!PassableCell(hex, true))
        return false;
    return !m_wide || PassableCell(Adjacent(hex, m_actor.m_facing == ARMY_FACING_RIGHT
        ? COMBAT_DIRECTION_EAST : COMBAT_DIRECTION_WEST), true);
}

bool Traversal::CanStep(i32 source, CombatHexDirection direction) const {
    return CanLand(Adjacent(source, direction));
}

bool Traversal::StopsMovement(i32 hex) const {
    if (!m_board.m_drawbridgeBackgroundVisible)
        return false;
    const i32 startRear = m_wide ? m_actor.m_hex + ArmyFacingRearHexOffset(m_actor.m_facing) : -1;
    const i32 rear = m_wide ? hex + ArmyFacingRearHexOffset(m_actor.m_facing) : -1;
    for (i32 index = 0; index < KB_MOAT_CELL_COUNT; ++index) {
        if (index == 4 && m_board.m_drawbridgeState != COMBAT_DRAWBRIDGE_RAISED)
            continue;
        const i32 moat = moatCell[index];
        if (moat != m_actor.m_hex && moat != startRear && (moat == hex || moat == rear))
            return true;
    }
    return false;
}

std::vector<i32> Traversal::StraightLine(i32 source, i32 target) const {
    if (!OnBoard(source) || !OnBoard(target) || source == target)
        return {};
    for (i32 d = 0; d < COMBAT_DIRECTION_ADJACENT_COUNT; ++d) {
        std::vector<i32> line;
        i32 hex = source;
        for (i32 count = 0; count < COMBAT_HEX_COUNT; ++count) {
            hex = Adjacent(hex, CombatHexDirectionFromCode(d));
            if (hex == ARMY_HEX_INVALID)
                break;
            line.push_back(hex);
            if (hex == target)
                return line;
        }
    }
    return {};
}

CombatHexDirection Traversal::StraightDirection(i32 source, i32 target) const {
    const auto line = StraightLine(source, target);
    if (!line.empty())
        for (i32 d = 0; d < COMBAT_DIRECTION_ADJACENT_COUNT; ++d)
            if (Adjacent(source, CombatHexDirectionFromCode(d)) == line.front())
                return CombatHexDirectionFromCode(d);
    return COMBAT_DIRECTION_INVALID;
}

bool Traversal::ClearLine(i32 source, i32 target) const {
    const auto line = StraightLine(source, target);
    if (line.empty())
        return source == target && OnBoard(source);
    for (i32 hex : line) {
        if (!GroundOpen(hex))
            return false;
        if (m_wide && !GroundOpen(Adjacent(hex, m_actor.m_facing == ARMY_FACING_RIGHT
            ? COMBAT_DIRECTION_EAST : COMBAT_DIRECTION_WEST)))
            return false;
    }
    return true;
}

Target Traversal::Destination(i32 hex, ArmyPathTarget mode) const {
    Target target;
    target.hex = hex;
    target.exactHex = mode != ARMY_PATH_ANY_TARGET_HEX;
    if (OnBoard(hex)) {
        const auto& cell = m_board.m_hexCells[hex];
        if (cell.m_occupantSide != COMBAT_SIDE_NONE
            && !(cell.m_occupantSide == m_actor.m_side && cell.m_occupantIndex == m_actor.m_index)) {
            target.side = cell.m_occupantSide;
            target.slot = cell.m_occupantIndex;
        }
    }
    return target;
}

std::optional<CombatHexDirection> Traversal::AttackDirection(i32 source, const Target& target) const {
    if (!OnBoard(source) || !target.IsAttack())
        return {};
    for (i32 d = 0; d < (m_wide ? COMBAT_DIRECTION_COUNT : COMBAT_DIRECTION_ADJACENT_COUNT); ++d) {
        auto direction = CombatHexDirectionFromCode(d);
        i32 base = source;
        i32 hit;
        if (m_wide && direction >= COMBAT_DIRECTION_WIDE_WEST) {
            const bool upper = direction == COMBAT_DIRECTION_WIDE_WEST;
            const auto diagonal = m_actor.m_facing == ARMY_FACING_LEFT
                ? (upper ? COMBAT_DIRECTION_NORTHWEST : COMBAT_DIRECTION_SOUTHWEST)
                : (upper ? COMBAT_DIRECTION_NORTHEAST : COMBAT_DIRECTION_SOUTHEAST);
            hit = Adjacent(source, diagonal);
        } else {
            if (m_wide && ((m_actor.m_facing == ARMY_FACING_LEFT && direction >= COMBAT_DIRECTION_SOUTHWEST)
                      || (m_actor.m_facing == ARMY_FACING_RIGHT && direction <= COMBAT_DIRECTION_SOUTHEAST)))
                base = Adjacent(source, m_actor.m_facing == ARMY_FACING_LEFT ? COMBAT_DIRECTION_WEST : COMBAT_DIRECTION_EAST);
            hit = Adjacent(base, direction);
        }
        if (!OnBoard(hit) || (target.exactHex && hit != target.hex))
            continue;
        const auto& cell = m_board.m_hexCells[hit];
        if (cell.m_occupantSide == target.side && cell.m_occupantIndex == target.slot)
            return direction;
    }
    return {};
}

bool Traversal::CanFinish(i32 hex, const Target& target, bool flying) const {
    if (!CanLand(hex, flying) || (target.approach != ARMY_HEX_INVALID && hex != target.approach))
        return false;
    return target.IsAttack() ? AttackDirection(hex, target).has_value() : hex == target.hex;
}

Plan Traversal::Ground(i32 source, Target target, i32 budget) const {
    Plan result;
    result.source = source;
    constexpr i32 unreachable = std::numeric_limits<i32>::max();
    std::array<i32, COMBAT_HEX_COUNT> costs;
    std::array<i32, COMBAT_HEX_COUNT> previous;
    std::array<CombatHexDirection, COMBAT_HEX_COUNT> directions;
    costs.fill(unreachable);
    previous.fill(ARMY_HEX_INVALID);
    directions.fill(COMBAT_DIRECTION_INVALID);
    using Candidate = std::pair<i32, i32>;
    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> queue;
    costs[source] = 0;
    queue.push({0, source});
    i32 finish = ARMY_HEX_INVALID;
    while (!queue.empty()) {
        const auto [cost, hex] = queue.top();
        queue.pop();
        if (cost != costs[hex])
            continue;
        if (CanFinish(hex, target)) {
            finish = hex;
            break;
        }
        // A moat is a legal destination, but ends this turn's movement.
        if (budget >= 0 && hex != source && StopsMovement(hex))
            continue;
        for (i32 d = 0; d < COMBAT_DIRECTION_ADJACENT_COUNT; ++d) {
            const auto direction = CombatHexDirectionFromCode(d);
            const i32 next = Adjacent(hex, direction);
            if (!CanTraverse(next))
                continue;
            const i32 nextCost = cost + 1 + (hex != source && StopsMovement(hex) ? m_actor.m_speed + 2 : 0);
            if ((budget >= 0 && nextCost > budget) || nextCost >= costs[next])
                continue;
            costs[next] = nextCost;
            previous[next] = hex;
            directions[next] = direction;
            queue.push({nextCost, next});
        }
    }
    if (finish == ARMY_HEX_INVALID)
        return result;
    result.reachable = true;
    result.destination = finish;
    if (target.IsAttack())
        result.attackDirection = AttackDirection(finish, target);
    std::vector<i32> route;
    for (i32 hex = finish; hex != source; hex = previous[hex])
        route.push_back(hex);
    std::reverse(route.begin(), route.end());
    i32 from = source;
    Step pending;
    pending.from = from;
    for (i32 hex : route) {
        if (pending.crossedHexes.empty())
            pending.direction = directions[hex];
        pending.crossedHexes.push_back(hex);
        ++pending.cost;
        if (!CanLand(hex)) {
            pending.kind = StepKind::MOVEMENT_JUMP;
            continue;
        }
        pending.to = hex;
        pending.endsTurn = StopsMovement(hex);
        result.steps.push_back(std::move(pending));
        from = hex;
        pending = {};
        pending.from = from;
    }
    // A charged jumper can leap the final four hexes of an attack. This
    // presentation choice is part of the plan used by every controller.
    if (m_jumping && target.IsAttack() && !result.steps.empty()) {
        size_t first = result.steps.size();
        i32 length = 0;
        while (first > 0 && length + result.steps[first - 1].cost <= 4) {
            if (first < result.steps.size() && result.steps[first - 1].endsTurn)
                break;
            length += result.steps[--first].cost;
        }
        if (first < result.steps.size()) {
            Step jump;
            jump.kind = StepKind::MOVEMENT_JUMP;
            jump.from = result.steps[first].from;
            jump.to = result.destination;
            jump.direction = result.steps[first].direction;
            jump.cost = length;
            jump.endsTurn = result.steps.back().endsTurn;
            jump.attackJump = true;
            for (size_t i = first; i < result.steps.size(); ++i)
                jump.crossedHexes.insert(jump.crossedHexes.end(),
                    result.steps[i].crossedHexes.begin(), result.steps[i].crossedHexes.end());
            result.steps.resize(first);
            result.steps.push_back(std::move(jump));
        }
    }
    return result;
}

Plan Traversal::Flight(i32 source, Target target) const {
    Plan result;
    result.source = source;
    if (!OnBoard(source) || !OnBoard(target.hex))
        return result;
    i32 closest = std::numeric_limits<i32>::max();
    for (i32 hex = 0; hex < COMBAT_HEX_COUNT; ++hex) {
        if (!CanFinish(hex, target, true))
            continue;
        const i32 distance = HexDistance(source, hex);
        if (distance < closest) {
            closest = distance;
            result.destination = hex;
        }
    }
    if (result.destination == ARMY_HEX_INVALID)
        return result;
    result.reachable = true;
    if (target.IsAttack())
        result.attackDirection = AttackDirection(result.destination, target);
    if (source != result.destination) {
        Step step;
        step.kind = StepKind::MOVEMENT_FLY;
        step.from = source;
        step.to = result.destination;
        // Retail flight is not limited by walking speed.
        result.steps.push_back(std::move(step));
    }
    return result;
}

Plan Traversal::Charge(i32 source, Target target, i32 budget) const {
    Plan result;
    result.source = source;
    if (!m_charging || !target.IsAttack())
        return result;
    const auto line = StraightLine(source, target.hex);
    const i32 chargeRange = budget < 0 ? m_actor.m_monster.speed : budget;
    if (line.empty() || static_cast<i32>(line.size()) > chargeRange
        || !ClearLine(source, target.hex))
        return result;
    // Crossed stacks do not block a charge, but the landing footprint must be
    // empty and adjacent to the selected target.
    for (auto it = line.rbegin(); it != line.rend(); ++it) {
        if (!CanFinish(*it, target))
            continue;
        Step step;
        step.kind = StepKind::MOVEMENT_CHARGE;
        step.from = source;
        step.to = *it;
        step.direction = StraightDirection(source, target.hex);
        const auto end = std::find(line.begin(), line.end(), *it) + 1;
        step.crossedHexes.assign(line.begin(), end);
        step.cost = static_cast<i32>(line.size());
        result.steps.push_back(std::move(step));
        result.reachable = true;
        result.destination = *it;
        result.attackDirection = AttackDirection(*it, target);
        return result;
    }
    return result;
}

Plan Traversal::Find(i32 source, Target target, i32 budget) const {
    if (!OnBoard(source) || !OnBoard(target.hex) || m_actor.m_quantity <= 0)
        return {};
    if (target.IsAttack() && (target.side < COMBAT_ATTACKER_SIDE || target.side > COMBAT_DEFENDER_SIDE
        || target.slot < 0 || target.slot >= COMBAT_ARMY_SLOT_COUNT
        || m_board.m_armies[H2EnumIndex(target.side)][target.slot].m_quantity <= 0))
        return {};
    if (m_flying)
        return Flight(source, target);
    if (CanFinish(source, target)) {
        Plan result;
        result.reachable = true;
        result.source = result.destination = source;
        if (target.IsAttack())
            result.attackDirection = AttackDirection(source, target);
        return result;
    }
    if (auto charge = Charge(source, target, budget))
        return charge;
    auto plan = Ground(source, target, budget);
    if (!plan && !target.IsAttack() && !target.exactHex && m_wide) {
        target.hex = Adjacent(target.hex, m_actor.m_facing == ARMY_FACING_LEFT
            ? COMBAT_DIRECTION_EAST : COMBAT_DIRECTION_WEST);
        if (OnBoard(target.hex))
            plan = Ground(source, target, budget);
    }
    return plan;
}

} // namespace ironfist::movement

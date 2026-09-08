#ifndef HOMM2_IRONFIST_COMBAT_MOVEMENT_H
#define HOMM2_IRONFIST_COMBAT_MOVEMENT_H

#include <optional>
#include <vector>
#include <IRONFIST/combat_state.h>

namespace ironfist::movement {

enum class StepKind { Walk, Jump, Fly, Charge };

struct Target {
    i32 hex = ARMY_HEX_INVALID;
    CombatSide side = COMBAT_SIDE_NONE;
    i32 slot = -1;
    bool exactHex = false;
    i32 approach = ARMY_HEX_INVALID;
    bool IsAttack() const { return side != COMBAT_SIDE_NONE; }
};

struct Step {
    StepKind kind = StepKind::Walk;
    i32 from = ARMY_HEX_INVALID;
    i32 to = ARMY_HEX_INVALID;
    CombatHexDirection direction = COMBAT_DIRECTION_INVALID;
    i32 cost = 0;
    bool endsTurn = false;
    bool attackJump = false;
    std::vector<i32> crossedHexes;
};

// A query owns its route. Drawing another preview cannot overwrite this plan.
struct Plan {
    bool reachable = false;
    i32 source = ARMY_HEX_INVALID;
    i32 destination = ARMY_HEX_INVALID;
    std::vector<Step> steps;
    std::optional<CombatHexDirection> attackDirection;
    explicit operator bool() const { return reachable; }
    i32 Length() const;
    Plan WithinBudget(i32 budget) const;
};

// All queries use const battlefield/stack inputs. No player/controller checks,
// live terrain edits, creature speed edits, or shared search scratch storage.
class Traversal {
public:
    Traversal(const combatManager& board, const army& actor, const state::CombatState& extensions);
    bool CanStandOnCell(i32 hex) const;
    bool CanLand(i32 hex, bool flying = false) const;
    bool CanTraverse(i32 hex) const;
    bool CanStep(i32 source, CombatHexDirection direction) const;
    bool StopsMovement(i32 hex) const;
    i32 Adjacent(i32 source, CombatHexDirection direction) const;
    std::vector<i32> StraightLine(i32 source, i32 target) const;
    CombatHexDirection StraightDirection(i32 source, i32 target) const;
    bool ClearLine(i32 source, i32 target) const;
    Target Destination(i32 hex, ArmyPathTarget mode = ARMY_PATH_ANY_TARGET_HEX) const;
    std::optional<CombatHexDirection> AttackDirection(i32 source, const Target& target) const;
    Plan Find(i32 source, Target target, i32 budget) const;
    Plan Flight(i32 source, Target target) const;

private:
    bool EmptyOrSelf(i32 hex) const;
    bool GroundOpen(i32 hex) const;
    bool PassableCell(i32 hex, bool jump) const;
    bool CanFinish(i32 hex, const Target& target, bool flying = false) const;
    Plan Ground(i32 source, Target target, i32 budget) const;
    Plan Charge(i32 source, Target target, i32 budget) const;
    const combatManager& board_;
    const army& actor_;
    bool wide_;
    bool jumping_;
    bool flying_;
    bool charging_;
};

// The command boundary consumes a plan; each step ends on a legal footprint.
// Returns false if an effect kills or removes the actor during movement.
bool Execute(army& actor, const Plan& plan);

} // namespace ironfist::movement
#endif

# Ironfist integration architecture

`ironfist-master` is the maintained integration of Project Ironfist's feature
set with the recovered Gold 2.1 engine. It succeeds the direct `ironfist` port.
The public compatibility target remains the pinned Project Ironfist source at
`314932011ed5308efb9f35cecc62e8ca638a7375`; the internal C++ architecture is
native to this repository.

The branch has two simultaneous obligations:

1. preserve supported content's named Lua APIs, callback order, resource paths,
   numeric IDs, and game rules; and
2. represent mechanics through the recovered HoMM2 model instead of maintaining
   a second shadow engine behind a collection of global patch functions.

## Integration boundary

All extension code is under `namespace ironfist`. Recovered engine sources use
fully qualified calls at the boundary; they do not import the namespace.

| Boundary | Responsibility | Must not own |
|---|---|---|
| `ironfist::runtime` | process startup/shutdown and map lifecycle | game rules |
| `ironfist::hooks` | optional Lua callback dispatch and result adaptation | unconditional mechanics |
| `ironfist::script` | Lua states, registration, bindings, map variables | host object policy |
| `ironfist::save` | XML encoding/decoding of detached `SessionData` | live game mutation or lifecycle |
| `ironfist::state` | extension state which cannot enter a recovered retail layout | duplicate retail object state |

Hooks are reserved for optional extension points. A rule that applies even
without a script is not a hook and belongs to the engine object responsible for
that rule.

## Mechanics and owners

| Mechanic | Owner in `ironfist-master` |
|---|---|
| shared visibility and propagation | `game` |
| AI army-sharing policy | `game` |
| forced AI hero chase policy | `game` |
| prohibited town buildings | `town` |
| Pandora Box combat setup | `combatManager` |
| disabled-Well weekly growth | `game::PerWeek` |
| shared hero-pool mobility refresh | `game::NextPlayer` |

Lua handlers call these owner methods. The runtime restores complete policy
snapshots after script defaults; the persistence codec only reads and writes
their values.

## Session lifecycle

`SessionData` owns the persistent record values, world cell/extra vectors,
visibility, object payloads, script source and variables, and extension policy.
It contains no manager/window pointers or owning engine allocations. The shared
record field list defines which recovered values cross this boundary and leaves
UI state out of snapshot transfers.

The XML codec decodes into a temporary snapshot. It checks dimensions and record
indices without changing the active game, and publishes the snapshot only when
decoding succeeds. Parsing failures return to the caller without terminating the
process or partially replacing the world.

`runtime::RestoreSession` prepares tracked engine storage, disables callbacks,
replaces records, establishes the current player, initializes scripts, and
restores saved variables and policy in that order. AI army sharing and building
bans follow the same policy phase as vision and forced chases. File-format
dispatch, save naming, post-load UI setup, and binary-loader completion belong
to the runtime/engine boundary.

The runtime tracks idle, new-map preparation, restoration, and ready phases.
`AdventureManagerReady` dispatches initial map/day callbacks only for a newly
prepared map and marks it ready before invoking user code. Loading a session
does not infer this lifecycle from a special save filename.

The disabled-Well calculation deliberately remains after neutral growth and AI
difficulty scaling. That is where the original Ironfist post-processing took
effect, so moving the rule into `game::PerWeek` does not silently alter AI town
growth.

## Types and state

Combat damage and added effects resolve through `ironfist::effects`. Resolution
applies health, spell, shield, and ability changes and produces an owned
`DamageResult` with slot identities, damage/absorption, casualties, and effect
outcomes. Burn also commits death occupancy before presentation. The presenter
adapts those results to the existing wince/death pipeline and formats messages;
its render-extent guard owns temporary drawing bounds. Headless burn omits the
presenter without omitting damage or death.

Movement records jump, charge, and distant-teleport approach in the owning
stack's battle record. Damage consumes that mechanical context, not animation
flags or shared movement globals. Jump and dodge animations are consequences
of resolved events; skipping them cannot preserve an already spent ability.
Shadow mark applies its influence during resolution instead of asking the
later `PowEffect` animation to cast it. Retail attack choreography and deferred
retail creature spells still use the existing `PowEffect` adapter.

Ironfist creatures, artifacts, spells, and map objects extend the existing
typed HoMM2 domains. Retail `*_COUNT` sentinels retain their original values;
larger storage capacities are explicit and code that opts into the extended
domain does so deliberately.

Creature abilities are represented by `CreatureAttribute`, not repeated string
lookups in simulation and rendering paths. XML names are translated once while
loading the creature database.

The recovered `game`, `town`, `hero`, and combat layouts remain unchanged.
Ironfist-only policy and simulation data such as vision relationships, forced
chases, prohibited buildings, and added combat state lives in the
`ironfist::state::State` sidecar. It is accessed through `state::Get()`; there is
no public extension-state global.

Lua states and map variables are owned by `ironfist::script`, not by the
simulation sidecar. Map variables use value ownership (`std::string` and nested
map values), removing the old shallow-copy pointer lifetime and leak hazards
without changing the serialized XML representation.

## Lua object lifetime

Heroes, players, towns, and battle stacks cross into Lua as native userdata
with a type and an opaque identity. They contain no native addresses. Every
object-taking handler uses `CheckObject<T>` with its actual argument position;
forged tables, wrong userdata types, nil, and expired identities produce Lua
errors before native access. Missing optional objects are represented by nil.

Hero-pool, player, and town records belong to the session generation. Those
identities survive ownership changes within the map, but expire when a session
is replaced or scripts shut down. Battle stack identities also check the slot's
generation, so a summon cannot revive a handle to a previous occupant. Captains
check the battle generation. New maps and session restores end the previous
battle, and shutdown closes scripts before deleting their native owners.

Temporary hero copies used by AI calculations can be borrowed by a callback.
`BindingScope` keeps that borrow valid only through the particular callback,
including nested calls, and retires it on success or error. Ordinary Lua getters
can return only known owned objects and cannot accidentally create such borrows.

The existing property names dispatch through native getter/setter tables.
`scripts/ironfist/binding.lua` is the maintained payload module and exports
`isValid`, backed by `IsObjectValid`, for retained references. A stale object's
properties raise an error; writing unknown or read-only properties also raises
an error. Campaign choices remain detached value snapshots. Scripts that used
the old `ptr` table member or constructed native-object tables must migrate to
the public getters and returned handles.

## Compatibility invariants

The integration retains:

- existing Lua-visible function, constant, and callback names;
- existing Lua registration-to-handler bindings and handler result counts;
- callback placement relative to the recovered host behavior;
- `.GIC`/`.GCC` save extensions;
- resource paths and the pinned aggregate bytes; and
- the numeric IDs published to scripts and data files.

`tools/ironfist_interface_audit.py` compares the public scripting surface with
the exact pinned upstream Git object for the 129 legacy functions in
`funcs.cpp`; `IsObjectValid` is an additional native binding helper.
`tools/test_ironfist_hook_contract.py`
checks integration order, keeps true mechanics out of the hook layer, verifies
their host owners, and protects the extension-state ownership rules. Both run
through CTest.

The broader evidence and known Gold-versus-PoL differences remain in
[Ironfist equivalence audit](ironfist-equivalence.md).

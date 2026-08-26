# Ironfist integration architecture

`ironfist-master` is the maintained integration of Project Ironfist's feature
set with the recovered Gold 2.1 engine. It succeeds the direct `ironfist` port.
The public compatibility target remains the pinned Project Ironfist source at
`314932011ed5308efb9f35cecc62e8ca638a7375`; the internal C++ architecture is
native to this repository.

The branch has two simultaneous obligations:

1. preserve the Lua names, callback order, save/XML schema, resource layout,
   numeric IDs, and valid-input game behavior expected by Ironfist content; and
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
| `ironfist::save` | the compatible Ironfist XML format | game simulation |
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

Lua handlers call these owner methods. Save loading also restores policy through
the owner methods rather than duplicating mechanics in persistence code.

The disabled-Well calculation deliberately remains after neutral growth and AI
difficulty scaling. That is where the original Ironfist post-processing took
effect, so moving the rule into `game::PerWeek` does not silently alter AI town
growth.

## Types and state

Ironfist creatures, artifacts, spells, and map objects extend the existing
typed HoMM2 domains. Retail `*_COUNT` sentinels retain their original values;
larger storage capacities are explicit and code that opts into the extended
domain does so deliberately.

Creature abilities are represented by `CreatureAttribute`, not repeated string
lookups in simulation and rendering paths. XML names are translated once while
loading the creature database.

The recovered `game`, `town`, `hero`, and combat layouts remain unchanged.
Ironfist-only persistence such as vision relationships, forced chases,
prohibited buildings, script state, and added combat state lives in the
`ironfist::state::State` sidecar. It is accessed through `state::Get()`; there is
no public extension-state global.

Map variables use value ownership (`std::string` and nested map values). This
removes the old shallow-copy pointer lifetime and leak hazards without changing
the serialized XML representation.

## Compatibility invariants

Internal renaming and ownership changes must not change:

- any Lua-visible function, constant, or callback name;
- Lua registration-to-handler bindings or handler result counts;
- callback placement relative to the recovered host behavior;
- `.GIC`/`.GCC` save extensions or XML element names;
- resource paths or the pinned resource-payload bytes; or
- the numeric IDs published to scripts and data files.

`tools/ironfist_interface_audit.py` compares the public scripting surface with
the exact pinned upstream Git object. `tools/test_ironfist_hook_contract.py`
checks integration order, keeps true mechanics out of the hook layer, verifies
their host owners, and protects the extension-state ownership rules. Both run
through CTest.

The broader evidence and known Gold-versus-PoL differences remain in
[Ironfist equivalence audit](ironfist-equivalence.md).

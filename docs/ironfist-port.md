# Ironfist Port

This branch (`ironfist`) carries Project Ironfist's features on top of the
Gold 2.1/Buka-based cross-platform `master`. The original PoL-based port is
retained in the backup history as migration evidence, but it is not a base of
this branch.

Ironfist source is a behavioral reference, not a copy source: its code was
written against its own reverse-engineered structures, while this tree has the
recovered types. Everything ported here is reimplemented against the recovered
names and layouts. Reference checkout:
`jkoppel/project-ironfist` at `31493201` (same tree as the feasibility
assessment).

## Ported so far

| Ironfist | Here | Notes |
|---|---|---|
| `shared/lib/lua` | `vendor/lua-5.3.3/` | pristine upstream Lua 5.3.3, no interpreter mains |
| `shared/lib/tinyxml2` | `vendor/tinyxml2-7.1.0/` | XML save, campaign, artifact, and creature data |
| `scripting/scripting.{h,cpp}` | `{include,src}/IRONFIST/scripting.*` | temp-file loader replaced with `luaL_dostring`; paths resolve case-insensitively under the platform data root |
| `scripting/lua_utils.*` | `IRONFIST/lua_utils.*` | |
| `scripting/callback.*` | `IRONFIST/callback.*` | `nonstd::optional` → `std::optional` |
| `scripting/deepbinding.*` | `IRONFIST/deepbinding.*` | |
| `scripting/consts.cpp` | `src/IRONFIST/consts.cpp` | full constant surface, see the mapping notes below |
| `scripting/funcs.cpp` | `src/IRONFIST/funcs.cpp` | all 129 registered functions |
| `gui/dialog_helpers.*` | `IRONFIST/dialog.*` | `MessageBoxA` → `platform::Host().ShowMessage` |
| callback patch sites | `IRONFIST/hooks.*` + one-line calls in `src/SOURCE/` | see below |
| `maps/MODULES`, `maps/GENERIC` | resource pack `SCRIPTS/` | runtime Lua; must be present under `HOMM2_DATA/SCRIPTS` |

Wired callback hooks (same placement as Ironfist's executable patches):

- `game::NewMap` → `ScriptingInit`; adventure-manager startup on a new game →
  `OnMapStart` + first `OnNewDay` (`KB.cpp`); `game::PerDay` → `OnNewDay`;
  `CheckEndGame` tail → `OnMapVictory` / `OnMapLoss`; game shutdown →
  `ScriptingShutdown`.
- `advManager::MoveHero` → `OnHeroMove`; `advManager::DoEvent` →
  `OnLocationVisit` (skip result honored); `game::ClaimTown` →
  `OnCastleConquered`; `GiveTakeArtifactStat` → `OnArtifactGive`/`OnArtifactTake`.
- `townManager::Open` → `OnTownOpen`; `recruitUnit::Open` → `OnUnitRecruit`.
- `hero::CalcMobility` → `OnCalcMobility`; `game::GetLuck` → `OnCalcLuck`;
  `armyGroup::GetMorale` → `OnCalcMorale`;
  `combatManager::InitNonVisualVars` → `OnBattleStart`.

The Lua-visible names and values are exactly Ironfist's scripting interface, so
existing Ironfist map scripts load unchanged.

## Name-mapping notes

Both trees recovered the same retail values, sometimes under different names.
The Lua constants keep Ironfist's names and values; the C++ spells them with
this tree's enums. Notable divergences:

- Ironfist `LOCATION_ROAD`(18), `LOCATION_ARMY_CAMP`(24), `LOCATION_TOWN`(35),
  `LOCATION_WELL`(38), `LOCATION_HERO`(42), `LOCATION_DWARF_CABIN`(65),
  `LOCATION_STREAM`(98), `LOCATION_HOLE`(114), `LOCATION_OUTCROPPING`(115),
  `LOCATION_ALCHEMIST_TOWER`(122) are this tree's `MAP_OBJECT_`
  `ALCHEMIST_TOWER`, `MONSTER`, `CASTLE`, `HUT_OF_MAGI`, `MERMAID`, `SIRENS`,
  `REEFS`, `ARENA`, `BARROW_MOUNDS`, `EXPANSION_OBJECT`.
- Ironfist building codes are bit indexes (0x0–0x1e); this tree's
  `TOWN_BUILDING_*` are the masks. The Lua constants stay indexes.
- Ironfist hero fields map to: `factionID` → `m_cursorType`
  (`HeroCursorType` is a `FactionType` typedef), `tempMoraleBonuses`/
  `tempLuckBonuses` → `m_morale`/`m_luck`, `primarySkills` → `m_primaryStats`,
  `mageGuildSpells`/`numSpellsOfLevel` → `m_spells`/`m_spellCounts[level+1]`
  (spell counts are 1-based in the retail layout), town `visitingHeroIdx` →
  `m_occupyingHeroId`, `numCreaturesInDwelling` → `m_garrison`.
- Ironfist mapCell fields map bit-for-bit: `objType` → `m_triggerType`,
  `extraInfo` → `m_objectMetadata`, `hasObject` → `m_animatedObject`,
  `field_4_1`/`isShadow` → `m_objectLayerBit0/1`, `groundIndex` →
  `m_terrainImageIndex`.
- Ironfist domain extensions: Cyborg faction 12 (all 13-wide faction tables),
  creatures 72–83 (attribute engine in `IRONFIST/creatures.*`), spells 65–72
  (`SpellType` rows 65–72; the spellbook grew in place to
  `KB_SPELL_TABLE_CAPACITY` with a split binary `hero::Read`/`Write` for
  retail-save compatibility).
- Per Ironfist behavior, `GetLuck`/`GetMorale` results are clamped to
  [-3, 3] after the callback even when no script is loaded (vanilla could
  exceed +3 luck via the Rainbow).

## Ported since the first milestone

- All 22 script callbacks are wired (the seven late ones: tooltip override,
  monster interact, the melee pair, spell chance, mana cost, luck/morale info
  text), and all 129 script functions are real — the last five stubs became
  the vision-sharing, building-ban, AI-army-sharing and forced-chase features,
  with state in `gIronfistExtra.adventure`.
- Preferences (file-backed, replacing the registry), the GUI widget-message
  helpers, `IsWellDisabled`, and the expansion state object.
- The data-driven artifact and creature layers: `DATA/artifacts.xml` and
  `DATA/creatures.xml` (installed from the pinned upstream source) are the
  authority; the retail tables grew to Ironfist's 256-entry capacities for
  the added artifacts (Pandora Box, Iron Fist) and creatures (72–83); secondary
  resource costs and random-spawn bounds are table-driven;
  `Ironfist_Startup()` loads everything before the retail main.

## The ledger is empty: everything is ported

The remaining-work ledger (`todo/ironfist/`, a copy of Ironfist's first-party
sources from which every ported function was deleted in the commit that ported
it) reached empty and was removed. `git log --diff-filter=D -- todo/` is the
full porting history. The late milestones, briefly:

- XML saves (`IRONFIST/save_xml.*`): the whole object graph, script text, map
  variables and the Ironfist-only state; `.GIC`/`.GCC` extensions via
  `GetSaveFileExtension`.
- Combat: the eight tech spells (Awareness through Implosion Grenade), the
  creature attribute abilities (strike-and-return, charger/jumper/teleporter
  movement, astral dodge, shadow mark, plasma blast, force shield, fire-bomb
  walls, burn), and their targeting/rendering.
- Town, hero, GUI: the full Cyborg faction — town building tree, Cybernetics
  skill surface, hero tables, level-up spell grants, map presence.
- Adventure: shipyard map object, monster free-join flags, barrier/tent
  auto-visit, artifact quick view, draw-order fixes, the movement-reminder
  pref, no-CD movie tolerance, Win32 menu cheats.
- Campaigns became data (`IRONFIST/campaigns.*`): retail tables seed map-keyed
  stores at startup and `CAMPAIGNS/*.cmp` files (upstream's campaign-metadata
  XML) register further campaigns from ID 4 up; the bundled Cyborg campaign is
  `cyborg.cmp` + `CAMP5_*.HXC`. Custom-campaign saves embed their metadata.
- The map editor tree stayed out of scope (this port builds one game binary
  and has no editManager/dialog-resource base); recorded in its deleting
  commit.

## Assets

The source installer fetches the pinned original Ironfist repository and
supplies `DATA/ironfist.agg` (built from upstream `assets/agg` with its
checked-in GrayFace packers; loads before the retail aggregates, optional),
`MAPS/` (Ironfist maps + the CAMP5 campaign maps), `CAMPAIGNS/cyborg.cmp`,
`MUSIC/Track44.ogg` (the Cyborg town theme), and `SCRIPTS/` (map Lua siblings
and modules). The portable audio path plays the Ogg directly and does not need a
retail XMI copied into the generated aggregate. Build and install these files
into `HOMM2_DATA`; see [Building the Ironfist resources](ironfist-resources.md).

## Build

Same as the port: `nix develop -c cmake -S . -B build -G Ninja
-DHOMM2_PLATFORM=SDL3 && nix develop -c cmake --build build`, or the flake
packages. The root `build.ninja` (classic Windows target) was already stale
before this branch (it references TUs removed upstream) and does not build the
Ironfist layer.

Status: all milestones landed on the Gold/Buka base. Clean Linux, MinGW/Windows,
and Emscripten/Web packages build, the catalog/encoding tests pass, and a
headless startup smoke passes with GOG data and the Ironfist resource pack
merged. Interactive Cyborg campaign and combat play-testing remains useful
runtime coverage.

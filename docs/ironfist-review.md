# Ironfist regression review — 2026-09-08

The maintained `ironfist-master` branch contains reproducible save/load, Lua,
and combat defects despite a successful native build and all 23 existing CTest
tests passing. The most urgent fixes are the script-text buffer overflow and
save-load script ownership. The existing tests largely cover platform behavior,
source policy, and interface shape; they do not establish game-state equivalence.

Reviewed revision: `dd48d405564b59021b4e3a605d7a29afdcb8a7dd`.
Comparison base: portable reconstruction `master` at
`16df440a7cdcab9809b60467b49bde8d5922ec9b`. This isolates Ironfist additions from
platform changes already shared with the reconstruction. Upstream comparisons
used Git objects at the declared Ironfist pin
`314932011ed5308efb9f35cecc62e8ca638a7375`, not its modified working tree.

This is a general review of the added mechanics and their integration, not a
claim that every possible gameplay path is correct. No interactive campaign,
multiplayer session, or AI balance evaluation was performed. Production-source
probes and traced callers substantiate the findings below; their individual
limits are stated. No engine fixes are included in this review branch.

| Priority | Finding | Evidence |
|---|---|---|
| P1 | Scripted luck/morale text can overwrite the global text buffer | AddressSanitizer reproduction |
| P1 | Loading a scriptless save retains the previous map's callbacks | Production load-body probe |
| P1 | Re-saving a loaded game can discard its embedded script | Production save-body probe |
| P2 | Shared-vision relationships disappear on load | Reset/read probe and missing serialization |
| P2 | Forced chases are omitted when there are no ordinary map variables | Production serialization probe |
| P2 | Valid nil callbacks and map-variable saves grow the Lua stack | Production scripting probe |
| P2 | Lua numbers do not round-trip through saves | Production scripting probe |
| P2 | Player names become empty on load | Production XML helper probe and reader trace |
| P2 | Recasting Force Shield on a depleted shield has no effect | Production combat method probe |
| P2 | Fire-wall rendering leaks references into a 16-bit counter | Acquisition/release and ownership trace |
| P2 | Remote XML saves are read from a different directory | Production path-resolution probe and caller trace |
| P2 | XML map extras bypass the engine's tracked allocator | Production map load/close probe |

**1. Bound script text appended to `gText`.**
[AppendLuckInfo and AppendMoraleInfo](../src/IRONFIST/hooks.cpp#L144) append a
newline and arbitrary Lua text using `strcat`. `GLOBAL_TEXT_BUFFER_SIZE` is only
768 bytes. Even with an initially empty buffer, an `OnShowLuckInfo` callback
returning `string.rep('x', 800)` produces an AddressSanitizer
`global-buffer-overflow` at `hooks.cpp:148`, writing 801 bytes. Existing text
reduces the available capacity further. Use the existing bounded UTF-8 utilities
for both appends and account for the newline and terminator. The probe uses
production hooks and scripting with a 768-byte replacement for the game global.

**2. Reset script ownership for every load, including scriptless/retail saves.**
[LoadGame](../src/IRONFIST/save_xml.cpp#L1250) resets adventure state, but does
not shut down the previous Lua states. [ReadRoot](../src/IRONFIST/save_xml.cpp#L1134)
only initializes scripts if a nonempty `<script>` is present. Loading a save
without one therefore leaves the previous map's callbacks active. A production
`ResetAdventureState`/`ReadRoot` probe retains `OnNewDay -> 'old-map'` after loading
a scriptless root. The format fallback to a retail save has the same missing
script reset. Clear map-specific Lua state at the load boundary and deliberately
initialize the generic artifact script for the newly loaded game. The probe
calls the load body directly; it does not drive the UI or load a whole map.

**3. Preserve the active script source when re-saving.**
[XmlFile::Save](../src/IRONFIST/save_xml.cpp#L548) reopens the installed script
using `gMapName` rather than serializing the source loaded from the save.
[ScriptContents](../src/IRONFIST/scripting.cpp#L151) has no retained source, and
the XML reader restores `m_mapFilename` but never updates `gMapName`. A game can
load successfully from an embedded script, then save successfully without any
`<script>` if the original file is missing; a stale `gMapName` can select another
map's script. The production probe reports `resave error=0 embedded script
present=0`. Store the active source alongside its Lua state, set it on new-map
and save initialization, and clear it when that state is replaced.

**4. Serialize shared-vision relationships.**
[ShareVision](../src/SOURCE/GAME.cpp#L5311) stores ongoing relationships in
`AdventureState::sharePlayerVision`. Resetting adventure state clears them, and
neither the XML writer nor reader serializes that matrix. The already revealed
tiles are saved, which hides the problem until a player explores new territory
after reloading. Relationships established inside `OnMapStart` are not restored
by normal save loading. The reset/read probe changes an active relationship
from true to false. Add optional relationship elements to the save schema and
restore saved runtime policy after script initialization. Test newly revealed
tiles, not merely the saved visibility bitmap.

**5. Persist forced chases independently of user map variables.**
[WriteMapVariables](../src/IRONFIST/save_xml.cpp#L609) returns if
`LoadMapVariablesFromLua()` is empty, before adding `_AICHASE_*` records. Thus a
script using `ForceComputerPlayerChase` without ordinary saved variables loses
the chase after loading. The production probe sets chase `0 -> 1` and observes
zero serialized entries. Remove that dependency: append engine-owned chase
records before deciding whether there is anything to write. Also test the
ordering of script initialization versus restored chase policy.

**6. Restore the Lua stack height on every callback/save exit.**
[PopLuaResult<string>](../src/IRONFIST/callback.cpp#L50) intentionally tolerates
`nil` but leaves it on the stack. Other rejected result types also remain, and
the multiple-result path removes only one value through the error helper.
[LoadMapVariablesFromLua](../src/IRONFIST/scripting.cpp#L275) leaves the
`mapVariables` table, or a nil lookup, on the stack too. Four valid nil-returning
tooltip callbacks grow the stack from 0 to 4; four saves with no `mapVariables`
do the same. Use a scoped saved stack height around each bridge operation and
balance unsupported-value paths. Test repeated nil callbacks, empty/nonempty
saves, malformed returns, and nested callbacks. Long-session crash timing was
not measured; stack growth itself is reproduced.

**7. Preserve Lua's numeric range and precision in saves.**
[GetMapVariableValue](../src/IRONFIST/scripting.cpp#L225) formats floating-point
values with `std::to_string`; [PushScalarToLua](../src/IRONFIST/scripting.cpp#L307)
parses integers through `atoi`, narrowing Lua's 64-bit integer range to C `int`.
The production probe round-trips `4294967297` as `2147483647` on this build,
and `0.00000001` as zero. The precise overflow result is implementation-dependent;
the range loss is not. Use a full-width integer parser and locale-independent
floating-point formatting with sufficient round-trip precision. Cover scalar
and nested-table values.

**8. Read player names from the attribute that the writer produces.**
`xml::WriteArray` emits `<playerNames index="0" value="Alice"/>`, while
[ReadRoot](../src/IRONFIST/save_xml.cpp#L1084) calls `QueryText`, which reads node
text. The helper probe reproduces `attribute=Alice restored name=''`. This
affects every saved player name, including turn announcements. Read the `value`
attribute with a destination capacity; optionally accept text-form legacy data.

**9. Make Force Shield recasts restore shield HP.**
[ValidSpellTarget](../src/SOURCE/SPELLS.cpp#L666) explicitly permits Force Shield
when its HP is below the creature's HP. However,
[SetSpellInfluence](../src/SOURCE/ARMY.cpp#L3262) returns immediately for an
existing influence, before reaching the HP initialization. A 100-HP shield
reduced to 25 remains at 25 after a recast, with a zero application result.
Both single and mass casts use this method. Refresh shield HP without adding a
second active-effect count, and verify resource expenditure against the restored
shield. The probe exercises the production setter; the accepted targeting and
cast paths were traced in source.

**10. Balance fire-wall icon acquisitions.**
[DrawFrame](../src/SOURCE/DRAWING.cpp#L999) acquires an icon for extents and again
for each wall drawn; [CycleCombatScreen](../src/SOURCE/COMMAND.cpp#L3101) also
acquires it on every cycle. None of these paths disposes those references.
`resourceManager::GetIcon` increments `resource::m_refCount`, an `i16`, and
`Dispose` deletes when its decremented count is nonpositive. Besides preventing
normal release, 32,768 outstanding references cross the signed range; later
disposal can delete an icon while references remain. Cache one owned reference
for the battle, or use balanced temporary acquisitions. This is confirmed by
source ownership tracing; a long battle causing a dangling access was not run.

**11. Use the same remote-save path for format probing and writing.**
[SaveGame](../src/IRONFIST/save_xml.cpp#L1222) writes `RMT*.BIN` under `DATA`, but
[LoadGame](../src/IRONFIST/save_xml.cpp#L1250) always probes `GAMES`. The portable
resolver preserves the differing subdirectories; the engine probe prints
distinct `.../DATA/RMTTEST.BIN` and `.../GAMES/RMTTEST.BIN` paths. A missing probe
file returns control to the retail loader, which does select `DATA`, but expects
the binary format rather than the XML just written. Callers include
`game::TransmitSaveGame` and `advManager::LoadRemote`. Centralize path
selection before format detection and test the actual remote-save exchange.
The path mismatch is reproduced; multiplayer transport was not exercised.

**12. Allocate XML map extras through the same owner as their destruction.**
[ReadMap](../src/IRONFIST/save_xml.cpp#L753) uses `new mapCellExtra[count]`, while
`fullMap::Close` calls `H2_FREE`. That is the engine's tracked `BaseFree`, which
logs `Bad Delete` and refuses to free an untracked allocation. A production
load/close probe with one extra reports allocation counts `0 -> 1 -> -1` and
`Bad Delete, File 'mapcell.cpp', Line 58`. Repeated XML loads therefore leak
extras and corrupt allocation accounting. Use `H2_ALLOC` consistently for
`fullMap` storage, preserving element initialization where required. This is an
integration boundary defect; upstream also uses a separate C++ allocation here.

Most of these patterns are present in pinned upstream Ironfist as well. They
are regressions relative to the reconstruction when importing the feature set,
not evidence that the recent namespace/ownership refactor created every bug.
The earlier AI scoring work in PR #10 is separate: the affected retail scorer
mostly predates Ironfist and is not counted among the findings above.

The broad pass also examined town/faction tables and recruitment, hero record
widths, random objects/artifacts, adventure event hooks, combat movement and
area-spell handling, campaign metadata/carryover, resource installation, and
rendering integration. I did not establish additional defects in those paths
beyond the findings listed. The existing documented differences for Gold versus
PoL, spell resistance, Cyborg spell restrictions, and Well suppression are
intentional or explicit policy and were not reported as new bugs. The
suspicious nested-table self-insertion survived a production XML round-trip
probe and is not counted. The short influence-to-spell table also did not yield
a reachable overflow through the reviewed AI callers: their switches exclude
the added influences.

Recommended fix order: address findings 1–3 first, restore save round-trip
invariants in 4–8 and 12 next, then fix shield recasts and rendering ownership.
The remote-save path merits its own integration test rather than being folded
into an unrelated gameplay change. Add behavioral tests with each fix; the
green interface suite alone would not catch these regressions.

Reproduce from this worktree:

```sh
nix develop --command bash
cmake -S . -B build -G Ninja
cmake --build build -j 4
ctest --test-dir build --output-on-failure
bash tools/review/ironfist/run.sh
```

The [probe sources](../tools/review/ironfist/) deliberately demonstrate the
unfixed behavior; they are not passing regression tests. The overflow mode is
expected to fail under AddressSanitizer. The engine probe links the actual
native game objects with a replacement `main`, calls XML internals using
`-fno-access-control`, and uses isolated `/tmp` data/user directories. The Lua
probe substitutes registration, path lookup, and error display to avoid loading
game resources. The XML helper probe links production serialization code.

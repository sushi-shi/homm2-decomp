# Archived constant taxonomy — first-pass classification of every integer constant

This records the completed heuristic census that preceded the current
file-by-file constants audit. Its manifest and `homm2 enum-types` command were
retired after `H2_ENUM_*` declarations and private TU ownership made the old
source parser and provisional classification ratchet obsolete.

State after the enum-domain unification campaign: the 21 cross-TU value domains
(creature, artifact, spell, map object, building slot, faction, terrain,
resource, …) are unified, dual-mode, and `reviewed` in
`config/integer_domains.toml`. Everything else — 430 declarations carrying
5,286 enumerators — was an unclassified sea of constants. This document records
the sweep that classified all of them, what the categories mean, and the
refinement queues the sweep surfaced.

## Where the classification lives

Every previously unclassified declaration now has a `[[constant_group]]` entry
in `config/integer_domains.toml` (the generated section is marked by a banner
comment). `homm2 enum-types validate` enforces the invariants: every listed
enumerator exists in the declaration, nothing is claimed twice, categories come
from the fixed set. `homm2 enum-types census` reports coverage.

Statuses form a ladder: `provisional` (this sweep's heuristic assignment) →
`reviewed` (individually curated). Only the source-file bookkeeping family was
marked `reviewed` by the sweep itself; everything else is `provisional`.

## Categories and distribution

| Category | Decls | Enumerators | Meaning |
|---|---:|---:|---|
| `value` | 168 | 3,104 | local integer namespaces: state machines, table/text indices, layout coordinates |
| `message-id` | 54 | 670 | dialog widget ids, window commands, button ids (0x7800-range), hotkeys |
| `resource-id` | 45 | 514 | icon frame, sound sample, music track, tileset, video indices |
| `scalar` | 11+ | 494 | sizes, counts, capacities, thresholds (incl. the big anonymous grid/size bags) |
| `flags` | 131* | 348 | bitmask namespaces with bitwise use shapes |
| `implementation` | 18 | 149 | retail compiler/runtime bookkeeping: `__LINE__`/`__FILE__` slot constants |
| `sentinel` | 3 | 7 | pure absence markers (−1, −999) |

*declaration counts are by dominant category of the whole declaration; mixed
grab-bags (below) inflate whichever category dominates them.

Two findings from the investigation:

- **No hidden shared domains remain.** A cross-declaration identical-value-set
  scan found only coincidental overlaps (small dense `0..N` state machines).
  The 14-domain unification already captured every genuine cross-TU value
  domain. The residual constants are honestly local.
- **`storage_locations` is not serialization.** The census records member
  writes, which says nothing about savegame persistence. The `serialized`
  category therefore stays reserved for curated domain entries.

## Refinement queue 1 — mixed `*Constant` grab-bags (118 declarations)

Per-window/per-TU bags mixing widget ids, commands, layout offsets, and sizes
in one declaration. They carry a single dominant-category `provisional` entry;
proper resolution is per-enumerator splitting into multiple `constant_group`
entries (the manifest supports several entries per declaration). Largest first:

`TownManagerConstant` (116), `MapEventConstant` (113), `KbGameTableConstant`
(112), `HeroUiConstant` (100), `NewGameConstant` (98), `OverviewUiConstant`
(65), `CursorConstant` (61), `CombatAIConstant` (58),
`HighScoreManagerConstant` (49), `ViewWorldConstant` (46), and 108 more —
the full list is the census audit `mixed_constant_groups`.

## Refinement queue 2 — `sizeof` re-expression candidates

Some reconstructed literals are almost certainly `sizeof(<struct>)` in the
retail source. Joining size-named, bare-literal enumerators against the
`SIZE(type, bytes)` assertion table (125 assertions) yields 142 value matches;
most are coincidences (any 5-byte struct "matches" every `..._SLOT_COUNT = 5`).
The name-correlated, high-confidence subset:

| Enumerator | Value | Likely retail spelling |
|---|---:|---|
| `OLD_MAIN_NET_SETUP_SIZE` | 212 | `sizeof(OldMainNetSetup)` — name-exact |
| `LISTBOX_SOURCE_FILE_SLOT_SIZE` | 44 | `sizeof(<listbox source-file slot struct>)` |
| `TEXT_ENTRY_SOURCE_FILE_SLOT_SIZE` | 44 | per-TU source-file slot struct |
| `TEXT_WIDGET_SOURCE_FILE_SLOT_SIZE` | 44 | per-TU source-file slot struct |
| `TILESET_SOURCE_FILE_SLOT_SIZE` | 44 | per-TU source-file slot struct |
| `WINDOW_MANAGER_FINAL_SOURCE_FILE_SLOT_SIZE` | 44 | per-TU source-file slot struct |
| `EXEC_TEXT_*_SLOT_SIZE` family | 8–80 | per-message text slot structs |
| `INPUT_MANAGER_NAME_SLOT_SIZE` | 16 | `sizeof(<name slot struct>)` |
| `MIDI_TEXT_*_SLOT_SIZE` family | 4–20 | per-message text slot structs |

Re-expressing these as `sizeof(T)` is codegen-identical (same folded
immediate) but each site still needs its owning struct identified and the
usual build verification; the slot structs are per-TU (`SDropListSourceFile`
is the same-shaped exemplar, not the owner).

## Method

Heuristics, in precedence order: declaration-name markers
(`SourceLine`/`SourceFile`/`SlotSize` → implementation; `flag|mask|attribute`
→ flags; art/audio nouns → resource-id; ui nouns → message-id; size nouns →
scalar), then a ≥60% majority vote over enumerator-name suffixes, then value
shapes (power-of-two + bitwise → flags; sentinel-only values; 0x7800 button
range → message-id), else `value`. Census data (`build/enum-types/census.json`)
provided values, expressions, operations, and use sites; the generator and
detectors were session scripts driven by that JSON.

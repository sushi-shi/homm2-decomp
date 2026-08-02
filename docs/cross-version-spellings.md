# Cross-version spelling ledger

The 2.1 branch's larger purpose is cross-referencing against the 2.0
reconstruction to recover the one true source tree. Whenever this branch's
byte-matching forces a spelling that DIFFERS from the PoL 2.0 spelling, the
divergence is an **unresolved claim**, not a conclusion: matching Buka's VC6
bytes proves the spelling is *sufficient for 2.1*, never that it is what the
devs wrote. Each entry below must eventually resolve to one of:

- **invariant** - the 2.1 spelling also reproduces 2.0's bytes under MSVC
  4.2 (test in the 2.0 tree). The 2.0 reconstruction then adopts it; PoL's
  spelling was just another member of 4.2's equivalence class.
- **mixed** - neither current spelling satisfies both compilers; search for
  a third that does before concluding anything.
- **dev-change** - no single spelling satisfies both, after a genuine
  search: the devs edited the line between 2.0 and 2.1. Document in
  docs/version-changes.md with the evidence.

Resolution protocol: compile the candidate spelling in the 2.0 tree
(homm2-decomp-master, MSVC 4.2 harness) and byte-compare against the 2.0
target the same way `reloc_donation.masked_equal` does here. Until that run
happens, entries stay OPEN. Timing: the verification runs are deferred until
the 2.1 reconstruction is complete - this branch only records divergences
while chasing exactness; the cross-analysis is a later dedicated phase.

## Open entries (2026-08-02)

| Site | PoL 2.0 spelling | 2.1 byte-pinned spelling | State |
|---|---|---|---|
| fullMap::GetNewCellExtra{Overlay,Object} reads | `Row(y)[x]` | `Column(x)[y * width]` (+ new `Column` accessor) | OPEN - test Column form under 4.2; only ~25 arms tried, uniqueness unproven |
| fullMap::Cell body / cell assigns | `cells + width * y + x` | `cells + x + y * width` | OPEN |
| fullMap::Read memcpy args | `cells + width * y + x` etc. | `cells + x + y * width` | OPEN |
| fullMap::GetNewCellExtraIndex locals | `nb, i, j` | `n, i, j` | OPEN - 4.2 hash differs from VC6; both may hold per-compiler with dev rename in either direction |
| fullMap::Read locals | `tmp` | `tmp1` | OPEN (same class) |
| fullMap::ChangeTilesetIndex head | if/else | zero-arm ternary `t = index != MAPCELL_SPRITE_NONE ? tileset : TILESET_NONE;` | OPEN |
| fullMap::ChangeTilesetIndex unused local | `dummy` | `a` (bucket-1 pin) | OPEN |
| mouseManager::Close head | `if (m_active == 1) { ... }` | `if (m_active != 1) return;` unwrap | OPEN - 4.2 lowering of wrap-vs-guard untested |
| inputManager::Close head | wrap | guard-clause unwrap | OPEN |
| heroWindowManager::Close head | wrap | guard-clause unwrap | OPEN |
| AiPrint head | wrap | guard-clause unwrap | OPEN |
| soundManager::AdjustSoundVolumes | n/a (Buka-era body) | `&&` split into guard + if | 2.1-only body, no 2.0 counterpart |
| UpdateSystemOptionsMenu guard | (compare order unrecorded in PoL) | `hmnuApp != hmnuAdv` (reloc-pinned) | OPEN - check 2.0 bytes' operand order |
| ws/dpWaitForExtraGuests first compare | `iWSLastMsg... != giNum...` | `giNumHumanPlayers != i...LastMsg...` | OPEN (same check possible in 2.0) |
| ws/dpnet_rcv first compare | `Tail == Head` | `Head == Tail` | OPEN |
| REQUEST GetMapHeader CRT calls | n/a | `open/read/close` (oldnames) | matches 2.0 house style; uncontroversial |
| Bzip spotBlock locals | upstream `pos, delta, newdelta` | `index, delta, updatedDelta2` | OPEN - upstream names cannot give VC6's slots; either Buka renamed (dev-change) or the 4.2/VC6 hash split resolves it; test upstream names in 2.0 tree |
| PATH FindPath / ValidRange, HERO GetNumSpells / GetExperience, KB CanBuild, TOWN GiveSpells, CURSOR pair, REMOTE calc_crc_long, Newgame GetLossConditionText, wingraph DDInitializePalette locals | PoL names | bucket-pinned renames (see f5d5b4c3) | OPEN as a class - slot names are per-compiler hash pins; resolving needs the 4.2-vs-VC6 hash comparison per name |

| TeleportTo telescope bonus | `>= 1` u32 cast (PoL-era spelling) | `HasArtifact(...) != 0` | OPEN |
| TeleportTo terrain compare | `m_currentTerrain != terrain` | `terrain != m_currentTerrain` | OPEN |
| TeleportTo locals | savedShow11/terrain5/oldCell2/unused47 | savedShow/terrain/cellOld2/unused (bucket pins) | OPEN as slot-name class |
| InsertSound bounds guard spelling | `MAP_WIDTH <= x \|\| MAP_HEIGHT <= mapY` | param-first, axes swapped (see Resolved) | - |
| WGAppPaint scroll adds | `giScrollX + x` (a1-form) | `destX7 + giScrollX` etc. (local first) | OPEN |
| WGAppPaint locals | padding8/destY1/sourceX7/sourceY7/destWidth0 | decl swap + destW/fromY/xSource/destY0 (bucket pins) | OPEN as slot-name class |
| InitCampaignMap priority compare | `bestLocal < best` | `best > bestLocal` (mirror) | OPEN |
| InitCampaignMap faction switch | BARBARIAN, WARLOCK, NECROMANCER | WARLOCK, BARBARIAN, NECROMANCER (je-distance pinned) | OPEN |
| InitCampaignMap carryover product | ternary * counts | counts * ternary (permuter trial 135) | OPEN |

## Resolved

| Site | Resolution | Evidence |
|---|---|---|
| InsertSound loopCount 0 vs 1 | **dev-change** | both values byte-pinned (PoL 2.0 = 0, Buka = 1); ledgered |
| InsertSound bounds guard | **dev-change (or 2.0 recheck)** | retail 2.1 swaps the axes (x vs HEIGHT); verify what 2.0's bytes compare before classifying further |
| In-code UI strings (29 literals) + SExecutiveText | **dev-change (Buka)** | retail CP1251 content differs from 2.0's English; ledgered in version-changes.md |
| EDITOR/mapcell KB.h include (STL pair) | **dev-change (2.1)** | the two new functions exist only in the 2.1 image |

# PoL 2.0 and Gold 2.1/Buka source-structure report

Investigation date: 2026-08-05. The primary goal is to use the exact
  `decomp-gold-2.1-buka` reconstruction as evidence for improving
`decomp-pol-2.0`. The functional-change inventory is included as a secondary
appendix; it is not the matching priority.

## Outcome

- The later branch is a strong source-structure witness. It is exact for all
  1,727 classified game functions under VC6 SP5, while the current PoL branch
  has 1,162/1,514 functions live exact and, after this investigation,
  1,345/1,514 functions exact-max.
- It must not be copied mechanically. Buka uses a different compiler and build
  state, and PoL 2.0 came from a real source fork. Retail PoL bytes remain the
  target.
- One exact shared structure was recovered immediately:
  `MapExtraPosAndAdjacentsSet` moved from a 99.8591% historical maximum to an
  audited 100.0000% maximum for its new source hash.
- One real semantic reconstruction bug was corrected:
  `philAI::TurnCostResource` computed the reciprocal of the retail resource
  factor. The corrected 2.1 form reached audited 100.0000% in a focused replay,
  with retail size, exact bytes and CFG, and the complete ordered relocation
  stream.
- A focused BASE follow-up closed `heroWindowManager::AddWindow` live at
  100.0000%. Retail proves the exact mainline direct-`zOrder`, compact loop,
  and tail-owned first-insertion structure.
- The same structure-first comparison closed two more BASE functions cleanly.
  `SGenRand` mutates `iLastSeed` in place and names the shifted `bitMask`;
  `GetDataEntry` preserves the staged row-layout algebra and owns the initial
  text copy in the null/non-null branches. Both remain exact across their
  complete clean-plus-50-state matrices.
- `textEntryWidget::Read` closes by publishing `m_maxLines` and
  `m_preserveTextOnFocus` directly. The temporary in the earlier PoL
  reconstruction was invented; the 2.1 direct-field phase survives unchanged.
- `CreatePCXFile` does not yet close, but its 2.1 indexed-row encoder supplies
  the stronger PoL parent: 98.2770% becomes 99.2568% clean and 99.4257% in an
  unchanged-source island, with exact CFG and relocation structure.
- `FadeToColorTable` closes cleanly with the 2.1 indexed palette loop and
  nested screen-coordinate loops; the earlier pointer/countdown rewrite had
  preserved behavior but lost the original register and expression ownership.
- `FadeIn` needs both phases: the 2.1 counted loop plus forced-last-level tail
  raises clean output to 99.3827%, and a reproducible declaration-forest island
  closes it at audited 100% with the complete relocation stream.
- `BlitBitmapToScreen` likewise confirms the structure-then-island pattern. The
  2.1 negative no-overlap branch, direct `sourceX` mutation, and repeated mouse
  field reads restore retail size and all 24 relocations; forest/top trial 3
  then closes the remaining two clean-state bytes exactly.
- `FizzleForward` retains the 2.1 per-row pointer recomputation after removing
  later-only loop and allocator differences. Clean output improves from
  99.3026%/1028 bytes to 99.5230% at retail size with exact CFG and relocations;
  a complete state matrix reaches the existing 99.7204% structural island but
  does not yet close the remaining register/SIB ownership.
- `MirrorImage` separates logic from reconstruction. PoL's 46-block retail body
  omits Buka's six source-body exclusion guards, so they stay out of 2.0.
  Buka's `deadline1` slot name and post-increment spelling transfer; the name
  removes the slot swap while pre/post increment is VC4.2-byte-neutral. The
  remaining three bytes are an equivalent loop compare unchanged by a complete
  102-cell source/state matrix.
- `LoadIcons` closes live at 100%. PoL owns a block-local `heroColor` and the
  empty `DebugCheck()` hook only in the player-color arm; Buka owns the inline
  ternary and an outer hook. Reverse matrices reject direct cross-compilation,
  so this is a matching-only source change with no shipping gameplay effect.
- `CatAttack` and `DecodePacket` are bounded non-transfers. CatAttack's Buka
  scope plus 200 fresh states do not beat its 99.9977% MAX. DecodePacket's Buka
  locals emit 293/314 bytes under VC4.2; 11 CRC spellings and 50 states leave
  its exact-frame/CFG/relocation two-byte residual unchanged.
- `ComputerMonsterInteract` is another matching-only non-transfer. The literal
  Buka flee return grows PoL by five bytes; its nested combat-result tail has
  higher fuzzy but worse graph deltas in every one of 51 states. PoL's current
  1088-byte, 48-block parent remains retained.
- `HeroInteractionAtTown` adopts Buka's two `distance < bestDistance`
  shipyard comparisons and x-then-y distance expression. A complete 408-cell
  source/state product isolates the comparison as the useful source choice;
  the Buka town-share and victory-coordinate mirrors are neutral. Forest/top
  seed 1213156658 trial 32 replays at 99.976950%, retail size, complete ordered
  71/71 relocations, and 125/126 exact blocks. The coordinate-load and B109
  branch-target island remains open.
- Two bounded negatives constrain further BASE work. PoL's
  `FillBitmapAreaClip` really owns an inlined fill loop rather than the 2.1
  helper call, and 408 relational/state cells could not move its four-byte
  residual. `PrintMemoryLeaks` also requires the inlined logging body: the
  direct 2.1 call is structurally contradictory, while an explicit inline
  helper compiles identically to the retained open body in all 51 states.
- `font::DrawBoundedString` demonstrates why low fuzzy score does not imply bad
  structure under `/Od`. Its retained PoL algorithm already had retail size,
  the exact 39-block CFG, and all relocations; forest/top trial 1 changed its
  apparent 72.3836% live score into an audited byte-exact 100% island without
  any source edit.
- A justified fresh-seed expansion also closed `sortIt`. Its prior best was
  only three bytes from retail at 99.99363%; forest/top seed 20260805 trial 2
  reached target-local exact bytes and all 68 relocations. Sibling changes in
  that disposable state were deliberately non-gating.
- The Buka comparison also distinguishes reconstruction bugs from bugs in PoL
  itself. For example, PoL retail really tests Bless twice in
  `army::DamageEnemy`; Buka changes the second test to Curse. The PoL bug must
  remain in this byte-matching branch.

## Why the later tree is useful

The available images name four source trees:

| build | source lineage | compiler/build evidence |
|---|---|---|
| Heroes II 1.0 | `D:\heroes2\Source`, `D:\heroes2\Base` | original mainline |
| PoL 2.0 | `I:\Projects\Heroes\Prog\SOURCE`, `...\BASE` | expansion fork, VC4.2 |
| Gold 2.1 | `F:\h2xsrc\Source`, `F:\h2xsrc\Base` | return to mainline |
| Buka | `e:\Users\igorl\VSS\HMM\HMM2\Source\...` | later VSS checkout, VC6 SP5 |

The cross-version frame census found:

| comparison | identical | slot-only | body changed |
|---|---:|---:|---:|
| PoL 2.0 → Gold 2.1 | 855 | 258 | 594 |
| PoL 2.0 → Heroes II 1.0 | 743 | 120 | 860 |
| PoL 2.0 → demo | 764 | 119 | 839 |

Across 811 functions decidable in both 1.0 and 2.1:

| relationship | functions |
|---|---:|
| all three builds identical | 699 |
| 1.0 == 2.1, while PoL 2.0 differs | 112 |
| 1.0 == PoL 2.0, while 2.1 differs | 0 |
| PoL 2.0 == 2.1, while 1.0 differs | 0 |

This is strong evidence that Gold/Buka often preserves the mainline source
shape and PoL is the fork. It does not make Gold/Buka authoritative for PoL
semantics: the 594 changed bodies include genuine feature work and bug fixes.

The divergence is concentrated where an expansion fork would be expected:

| layer | identical | slot-only | body changed | slot-only rate |
|---|---:|---:|---:|---:|
| BASE engine | 216 | 16 | 158 | 7% |
| SOURCE game logic | 458 | 239 | 308 | 34% |
| EDITOR | 7 | 3 | 0 | 30% |
| CRT/runtime | 174 | 0 | 128 | 0% |

## What exact VC6 matching proves—and what it does not

An exact Buka function proves that its retained source is sufficient to produce
that Buka retail body under VC6. It provides unusually strong evidence for:

- semantic phase order;
- loop, guard, shared-tail and switch-body structure;
- real helper and inline-accessor boundaries;
- declaration scopes and meaningful locals;
- operand and expression ownership when VC6 makes them observable;
- external call and global-data ownership.

It does not prove that the same spelling will compile identically under VC4.2.
The largest reasons are:

- Buka compiles almost the whole image `/Od`; PoL has roughly 40 optimized TUs.
- Buka enables `/GX`; PoL has no exception state.
- Buka disables `/QIfdiv`; PoL wraps floating division with adjustment helpers.
- Buka uses VC6 `bool`, STL headers and different TU declaration state.
- PoL and Buka have different data layouts, middleware and literal ownership.
- More than one C++ spelling can be byte-equivalent under either compiler.

### Identifier warning

Both VC4.2 and VC6 `/Od` assign stack slots from the same 16-bucket hash of the
identifier spelling. The measured order is:

```text
bucket = msvc_identifier_hash(name) & 0xf
frame order = bucket ascending, then same-bucket declarations newest first
```

Therefore a Gold/Buka local name is useful evidence about the mainline tree but
is often wrong for matching the PoL fork. Transfer structure first; retain or
solve PoL-specific names from PoL stack displacements.

## Transfer protocol for this branch

For each Buka exact donor:

1. Confirm that the PoL and Buka functions have the same intended semantics.
2. Inspect PoL `rva`, callees, strings, instruction diff, block diff, frame and
   ordered relocations before editing.
3. Build a complete structural matrix containing the current PoL source, the
   exact Buka structure, and any evidence-backed hybrid.
4. Keep small spelling axes independent from the structural axis and run each
   surviving source shape in the clean state and all requested TU states.
5. Select by retail size, exact bytes, ordered relocation identity and CFG—not
   fuzzy score alone.
6. Retain only ordinary semantic source. Generated declarations remain
   disposable.
7. If an exact island appears, replay it against the retained clean source,
   bank the maximum, and harvest the replay evidence.

## Changes landed by this investigation

### `MapExtraPosAndAdjacentsSet`: shared structure, exact closure

The previous PoL source used row-first array syntax and a nested empty `if`:

```cpp
mapExtra[MAP_WIDTH * y + x]
if (checkY >= 0) {
    if (MAP_HEIGHT <= checkY) {
    } else {
        // ...
    }
}
```

The exact Buka source uses pointer-form column-first addition and guard-clause
loops. In PoL the candidate already had 18/20 exact blocks and 6/6 relocations.
A three-arm structural matrix found the exact Buka arm under the same forest
state where the old source stopped at 99.85915%.

Focused replay evidence:

| property | result |
|---|---|
| PoL RVA | `0x0006a0e7` |
| retained source hash | `715ac9bad32c` |
| clean score | 91.549290% |
| exact state | seed 1213156658, forest/top trial 14 |
| size | 246 / 246 |
| byte delta | 0 |
| CFG | 20/20 blocks, exact edges/pred/flow/targets |
| relocations | complete ordered 6/6 stream |
| result | audited MAX 100.0000% |

Evidence lives in
`docs/matching/MapExtraPosAndAdjacentsSet/`,
`docs/matching-matrices/max-observations.tsv`, and the harvested assembly under
`docs/matching-matrices/max-asm/`.

### `philAI::TurnCostResource`: semantic bug fixed, exact closure

The previous source calculated:

```cpp
(fraction / 2.0f + 0.5) / gResourceBaseValue[resource]
```

PoL retail uses `fdivrp`; Buka's exact source spells the opposite ratio:

```cpp
gResourceBaseValue[resource] / (fraction / 2.0f + 0.5)
```

This changes AI resource weighting and is a reconstruction bug, not a cosmetic
source difference. The full matrix tested two resource-value product orders,
two ratio orders, and 51 clean/state shapes: 204/204 variants in 150 seconds.
Its best mixed arm reached 99.863640%. A focused replay against the retained
Buka base-first product form then closed exactly in forest/top trial 1.

| property | result |
|---|---|
| PoL RVA | `0x0003fb8c` |
| old-source matrix baseline | 96.390910% |
| retained-source clean score | 97.081820% |
| exact state | seed 1213156658, forest/top trial 1 |
| audited maximum | 100.0000% |
| size | 384 / 384 |
| byte delta | 0 |
| CFG | 19/19 blocks, exact edges/pred/flow/targets |
| relocations | complete ordered 16/16 stream |
| retained source hash | `7ff4172edb65` |

The two multiplication orders were byte-identical under VC4.2. The Buka
base-first form is retained because it is exact in the mainline witness and
agrees with the PoL target's desired evaluation ownership. The replay evidence
and harvested assembly are recorded with the other matching artifacts. See
`docs/matching/TurnCostResource/`.

### `heroWindowManager::AddWindow`: BASE ownership fix, live exact

The prior source copied `zOrder` to another local, zeroed it before checking
the list tail, expanded the insertion walk, and saved `m_windowListHead` to
decide whether the first inserted window also became the tail. The exact Buka
source instead mutates `zOrder` directly and tests `m_windowListTail`.

PoL retail independently proves both distinctions: it uses a literal zero
move in the automatic-layer arm and loads the tail field at the first-window
test. Retaining the mainline structure produced 188/188 live exact bytes, an
identical 19-block CFG, and the complete ordered 1/1 relocation stream under
the clean VC4.2 state.

The same BASE sweep retained two byte-neutral exact-donor structures:
`button::Deselect` now owns its public body directly, and
`mouseManager::Close` uses the mainline early-return guard. Complete 204-cell
and 51-state matrices respectively left their existing two-byte residuals
unchanged. Two flat 50-state censuses of `executive::DoDialog` initially left
its exact-layout restore loop at 99.4375%. Starting from the later source and
removing its newer stack-object and loop changes exposed the surviving clue:
direct `managerList[managerIndex]` ownership. Retaining that expression inside
PoL's proven decrementing loop closed the clean target at 100%, with 251 exact
bytes, all 15 blocks, and the ordered 16/16 relocation stream.

### Continued BASE island batch

Applying the structure-first, island-second method to additional BASE
residuals produced four more live closures and one exact disposable island:

| function | donor structure | result |
|---|---|---:|
| `inputManager::ForceMouseMove` | repeated mainline re-entrancy guard | live 100% |
| `textWidget::textWidget()` | pointer fields before color/alignment | live 100% |
| parameterized `textEntryWidget` ctor | direct field publication | live 100% |
| `FindIndex` | single-comparison three-way loop | MAX 100% at forest/top trial 3 |
| `FindToken` | counted pointer-form loop | MAX 99.6552% at trial 2 |

The legitimate Misc source changes also moved unchanged `FadeTo` back onto its
previously audited live-exact compiler-state island. `FindToken` remains one
SIB byte from retail; a complete commutative pointer-addition matrix proved the
two source orders byte-identical in every requested state.

### `BlitBitmapToScreen`: cursor-overlap ownership and exact island

The earlier PoL reconstruction introduced `blitSourceX`, rewrote the main
condition as a positive overlap test, cached the saved mouse coordinates, and
returned from the overlap arm. Although semantically close, that source emitted
392 rather than 395 bytes and only 23 of the target's 24 ordered relocations.

The exact 2.1 donor instead mutates `sourceX`, makes the ordinary blit the
negative no-overlap arm, and reads the mouse-manager fields directly when
repairing cursor pixels. A complete 2 x 51 structure/state matrix finished all
102 cells. The donor clean state reached 99.481480%, retail size 395, an exact
18-block CFG, and all 24 ordered relocations. Forest/top trial 3 closed the
remaining two-byte compiler-state residual exactly.

| property | result |
|---|---|
| PoL RVA | `0x000c5ee0` |
| old source | 98.214810%, 392 bytes, 23/24 relocations |
| retained source clean | 99.481480%, 395 bytes, 24/24 relocations |
| exact state | seed 1213156658, forest/top trial 3 |
| audited maximum | 100.0000% |
| CFG | exact 18/18 blocks and topology |
| retained source hash | `0ab236af3f96` |

The focused replay is reproducible from the retained ordinary source; no
generated declaration is retained. See
`docs/matching/BlitBitmapToScreen/cross-version-cursor-overlap.cpp`.

### `combatManager::LoadIcons`: branch-owned inline hook, live exact

| property | PoL result |
|---|---|
| RVA | `0x00091dda` |
| retained source | block-local `heroColor`; `DebugCheck()` in player-color arm |
| bytes / CFG | 999/999; exact 27/27 blocks |
| relocations | complete ordered 58/58 stream |
| live score / hash | 100.0000% / `1a653071cbd3` |

The 9-arm PoL matrix closes only the two player-arm-hook scopes. Buka's exact
inline ternary falls to 98.800900% under VC4.2; the retained PoL form falls to
99.820274% under VC6. See `docs/matching/LoadIcons/`.

## Bugs and bug candidates exposed by the exact branch

### Confirmed reconstruction bug

- `philAI::TurnCostResource`: final resource ratio inverted. Corrected here.

### Confirmed PoL bugs or later behavior changes—preserve for PoL matching

- `army::DamageEnemy`: PoL retail tests Bless in both branches, so cursed
  stacks take the random-damage path. Buka tests Curse in the second branch.
- `ValidHex`: PoL accepts 0..125; the later build uses the 117-cell combat
  count.
- `TransferArtifacts`: PoL's upper artifact bound differs from the later
  ultimate/Golden-Goose exclusion.
- `combatManager::DoVictory`: later necromancy excludes Skeleton rather than
  Ghost.
- `advManager::InsertSound` and `army::LoadResources`: loop-count constants
  changed in opposite directions between the images.
- `advManager::DoVisions`: later code removes the doubled diplomacy fee and
  changes precision/operand order.
- `game::GetLuck`: the Rainbow bonus moved before the clamps in the later
  build.
- `combatManager::MirrorImage`: Buka skips six wide-creature source-body hex
  combinations; the PoL retail CFG contains none of those guards.

These are useful for a playable bug-fix branch, but applying them here without
PoL byte evidence would violate the reconstruction objective.

### High-value reconstruction-bug candidates requiring PoL audit

The exact Buka source corrected the following inherited source. They are not
landed here until PoL's own bytes confirm them:

- `DetermineEffectOfSpell`: Cold Ray is absent from the first
  `SPELL_AI_ENEMY` classification even though its later damage arm exists.
- `DetermineTargetPosition` and `StrategicValueOfPosition`: rival-hero tests
  use current `m_x/m_y` rather than intended `m_destinationX/m_destinationY`.
- `StrategicValueOfPosition`: a friendly-town test uses the sixth-dwelling
  upgrade flag (`0x40000000`) where Buka uses Castle (`0x40`).
- `searchArray::SeedCombatPosition`: Buka tests shooter `shots`, while the PoL
  reconstruction tests `speed`.
- `CampaignHandler`: one campaign map-enable write reverses the array indices.
- `advManager::DoEvent`: obelisk metadata may need the 1-based `-1` index;
  Dragon City may compare a 1-based scenario value.
- `MoveHero`: one south-west deferred-draw expression uses X where the later
  exact source uses Y; runtime values coincide in that arm.
- `CombatSpecialHandler`: the later exact body lacks the inherited
  `ConvertToHover` call.

## Priority donor queue

These live current-hash residuals have exact Buka structures and high retained
PoL maxima. They are the best next cross-version matrices, ordered roughly by
payoff and audit cost:

| PoL function | retained MAX before new work | donor clue |
|---|---:|---|
| `RVOfPosition` | 99.9294% | map access and semantic phase order |
| `FightValueOfStack` | 99.9139% | exact Buka arithmetic ownership |
| `SaveGame` | 99.9084% | exact Buka large-body reference |
| `SmackManagerMain` | 99.8963% | removed speed-test/low-memory branches |
| `HeroQuickView` | 99.8681% | low-memory and quick-info structure |
| `InitMenuHandler` | 99.8268% | exact Buka menu CFG |
| `ControlPanel` | 99.8089% | small, high-confidence donor |
| `ViewSpellsHandler` | 99.7841% | refresh path changed/clarified |
| `CastSpell` | 99.7599% | literal-true Magic Arrow case guard |
| `FizzleForward` | 99.7204% | save/restore update flags; row recomputation |
| `Armageddon` | 99.6871% | shake-pointer expression ownership |
| `DoVictory` | 99.6867% | compare order plus known semantic delta |
| `SetCombatDirections` | 99.6679% | exact Buka phase order |
| `SpecialAttack` | 99.6240% | frame/name donor; localized body differs |

Large optimized functions should use Buka only for semantic CFG families;
their VC6 `/Od` allocation is not a compiler-state donor for PoL `/O2`.

## Secondary appendix: known functional-change inventory

This inventory summarizes all feature-level changes recorded by the exact
Gold/Buka campaign. `[2.1]` means checked against the English Gold 2.1 image,
`[Buka]` means localization/rebuild-specific, and `[open]` means Gold-vs-Buka
provenance is not yet classified. This attribution is secondary to the
structure-mining work above.

### Build and runtime

- `[Buka]` Whole-image `/Od` except optimized `BASE/BITS`; PoL uses mixed
  `/Od` and `/O2` profiles.
- `[Buka]` VC6 SP5, global `/GX`, no `/Gi` line statics and no `/QIfdiv`.
- `[Buka]` Most allocations bypass the PoL debug allocation layer.
- `[Buka]` Asserts survive only in MOUSEMGR, RESMGR, netwin, dpnetwin and
  wingraph; MIDI trace strings are removed.
- `[Buka]` VC6 STL/ctype guard stubs enter most TUs through the Audiere include
  chain.
- `[Buka/build]` String pooling is enabled for most TUs and disabled for a
  small per-TU set.

### Audio, video and media

- `[Buka]` Audiere effects/music backends and refactored Miles wrappers are
  added.
- `[Buka]` MCI/redbook CD-audio methods, the `x_cdtest` TU and `MIDIPoll` are
  removed; OGG tracks replace CD audio probing.
- `[Buka]` `soundManager` shrinks from `0x6ae` to `0x52` and gains a compact
  backend-state head; many fields become globals or disappear.
- `[Buka]` MIDI entry points become free fastcall functions; a new
  `MusicFlags` TU owns MIDI state.
- `[Buka]` Miles wave-driver setup is inlined into backend startup.
- `[Buka]` `sample` grows for an Audiere handle, while its constructor becomes
  single-argument and hardcodes playback defaults.
- `[Buka]` `oldmain` plays additional intro/credit videos; the Smacker speed
  test and low-memory term are removed.
- `[Buka/open]` Cursor footstep sample bookkeeping and timer/CD shutdown calls
  are removed or simplified.

### Localization and text

- `[Buka]` In-code UI, combat, network, end-game, campaign, requester and town
  strings are translated to CP1251 Russian.
- `[Buka]` Window class/title strings and registry keys move to the Buka
  Platinum namespace.
- `[Buka]` A Cyrillic font remapper, line extractor, wrapping and hyphenation
  engine are added; glyph selection becomes CP1251-aware.
- `[Buka]` Keyboard input gains Polish/CP1251 remap tables and special Enter
  scan-code handling.
- `[Buka]` Save filenames, campaign autosave prefixes and player-exit save
  names become localized.
- `[Buka]` Creature-name grammar changes many message sites from singular to
  plural and adds CP1251 upper/lower first-letter helpers.
- `[Buka]` `gMineNames`, plural-monster lookup and localized campaign bonus
  tables are added.
- `[Buka]` Combat text buffers and several quick-info buffers grow to 800
  bytes; resource-manager filename storage grows from 60 to 1000 bytes.

### Adventure map, campaigns and game rules

- `[open]` Environment-sound loop counts change; `DoVisions` no longer doubles
  joining cost and changes floating precision.
- `[Buka]` Low-memory environment-origin, video-refusal, replay-button and hero
  art paths are removed from several dialogs/actions.
- `[Buka/open]` `RandomizeEvents` reorders guardian arrays and expands castle
  metadata one row upward.
- `[Buka]` Random artifact selection permits the Anduran set outside expansion
  campaigns; shrine counting recognizes a town hidden under a hero.
- `[Buka]` Ultimate-artifact rumours gain a missing-artifact fallback.
- `[Buka]` Spell widgets are always refreshed/enabled and restamp message type
  before each broadcast.
- `[Buka/open]` Hero recruitment retains Sirens/Arena visit flags; campaign
  map-enable indexing and remote player matching differ.
- `[Buka]` Town UI gains calendar text, CP1251 capitalization and a MIDI settle
  delay during town changes.
- `[Buka]` Expansion campaign panels, bonus labels, resource format order and
  replay/restart behavior are localized/reworked.
- `[open]` `QuickInfo` gains terrain dig annotations, corrected barrier/tent,
  mine/resource and large-buffer paths.

### Combat, spells and AI

- `[2.1]` Five disabling-spell AI modifiers change from negative to positive.
- `[Buka/open]` Necromancy excludes Skeleton rather than Ghost; combat-hex and
  artifact-transfer bounds are tightened.
- `[Buka/open]` Rainbow luck applies before clamps; several combat arithmetic
  products and switch-body orders change.
- `[open]` `MirrorImage` skips six source-body hex combinations.
- `[open]` `SeedCombatPosition` gates on shots instead of speed.
- `[open]` `ShowSpellMessage` and combat command messages use plural names and
  add shoot-through-wall handling.
- `[open]` Magic Arrow retains a literal-true case guard; Chain Lightning keeps
  identical-arm angle ternaries.
- `[open]` combat drawing copies messages through a file-static scratch and
  several animation loops move from running offsets to indexed `for` loops.
- `[Buka]` combat damage messages, case folding, Russian grammar and buffer
  ownership are substantially rewritten.
- `[open]` PHILAI target selection, resource conversion/order, local naming and
  stack layouts differ throughout the PoL fork.

### UI, widgets, graphics and input

- `[2.1]` `heroWindow` reads six additional widget record types: two text-entry
  variants, drop list, two inset variants and list box.
- `[open]` Widget dispatch stops snapshotting flags; `widget::Dim` owns the
  un-dimmable-kind test.
- `[open]` Button deselection moves into `button::Deselect`; drag completion
  returns the forwarding result literally and hotkey guards change shape.
- `[open]` Border/icon/button reads, constructor store order and local caching
  differ.
- `[open]` Bitmap copy, careful copy, VESA blit, palette cycling and fizzle
  paths use indexed loops, `memset`, or recomputed row pointers.
- `[open]` File requester accepts Cyrillic, fixes filename buffer width and
  swaps knob/gutter dispatch ownership.
- `[open]` Setup-network-game checks for `DPLAYX.DLL`; several palette entry
  points use `__cdecl`.

### Data-model and ownership changes

- `[Buka]` `SAMPLE2` becomes one pointer and `NULL_SAMPLE2` disappears.
- `[Buka]` Four drop-list flags and several global cycle flags narrow to `u8`.
- `[Buka]` `EveryOther`, `gbEveryOtherCycle`, message fields and selected
  accessors change width or return type.
- `[Buka/open]` Several static/global owners move between TUs; compiler-generated
  destructors and STL init funclets are newly emitted.
- `[Buka]` Buka-only tables and scratch buffers are added for mine names,
  Cyrillic combat text, input maps and audio state.

The appendix deliberately groups translation literals and repeated call sites
as features rather than treating each address as a separate feature. For
matching work, every individual site still remains discoverable from the exact
Buka source and its version-delta history.

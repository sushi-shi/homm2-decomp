# Deliberate reading notes

## B01 — base widget event protocol

Read in full: `BUTTON.cpp`, `TEXTWDGT.cpp`, `WIDGET.cpp`, `ICONWDGT.cpp` under
`src/BASE`, and owner headers `button.h`, `textWidget.h`, `widget.h`, `iconWidget.h`,
plus `message.h` and `widgetKind.h` under `include/BASE`.

Constructors/destructors, resource readers, dispatch branches, draw methods and
setters were read, not just repeated blocks. Candidate links: H01 message retyping,
H02 local half-open hit tests, H03 serialized geometry prefix, H04 nested resource
lookups. R01 rejects whole-handler deduplication. In particular, message payload
aliasing and the ordering of the right-button modifier writes prohibit treating
the header macro as a full message initializer.

## B02 — remaining widgets, resource lookup, and window ownership

Read in full: `BORDER.cpp`, `DIMMER.cpp`, `Textntry.cpp`, `listbox.cpp`,
`droplist.cpp`, `RESMGR.cpp`, `RESOURCE.cpp`, `WINDOW.cpp`, and their eight owner
headers. Reconciled 86 function definitions against the index (no header bodies).
Read the complete text-edit and drop-down modal loops, not just message handlers.

H01-H03 recur across the widget readers/handlers. H04 now has a checked owner
contract: `SavePosition` pushes both the active aggregate and that descriptor's
offset to a global ten-entry stack, then `RestorePosition` pops them. Nested font
and icon construction can seek different resources. This is not simply `tell`
and `seek` on a fixed descriptor.

New findings: H05 allocated string copies, H06 existing icon-frame lookup API,
H07 sequential range clamping, H08 scroll-row/drag arithmetic. Read `GetIconEntry`
in `Misc.cpp` as a targeted callee check; that large TU is NOT marked read yet.
R02 records same-TU resource cache boilerplate, R03 rejects whole list-widget
unification, R04 rejects generic ownership/list/background macros.

## B03 — executive, manager, and window dispatch

Read in full: `EXEC.cpp`, `WINMGR.cpp`, `BASEMGR.cpp` and headers `executive.h`,
`EXEC_TYPES.h`, `heroWindowManager.h`, `WINMGR.h`, `baseManager.h`. Reconciled
34 definitions, including the header's `Activate` inline and private
`FadeSavedUpdate`. Read priority/list setup, manager closure, modal manager-list
replacement, all dispatch branches, screen/fade/fizzle work and palette cycling.

R04 is reinforced by manager versus window list/activation differences. R05
records why superficially similar event pumps are not one protocol. Cursor-ready
guards assign false/true rather than save/restore the previous flag; inspect the
mouse owner before proposing an abstraction. Fizzle clipping/addressing supplies
cross-TU occurrences for H09/H10 when read with B04.

## B04 — bitmap/icon/font resources and low-level graphics

Read in full: `BITMAP.cpp`, `bmap2.cpp`, `ICON.cpp`, `miscwin.cpp`, `FONT.cpp`,
`PALETTE.cpp`, `TILESET.cpp`, `Blur.cpp`, `Ripple.cpp`, `TILE.asm`, `BITS.asm`;
headers `bitmap.h`, `bmap2.h`, `icon.h`, `IconEntry.h`, `IconDraw.h`, `miscwin.h`,
`font.h`, `palette.h`, `tileset.h`, `Blur.h`, `Ripple.h`,
`MiscGraphicsConstants.h`, `TILE.h`, `BITS.h`. Reconciled 52 definitions across
25 files, including unannotated private accessors and all four assembly bodies.
Targeted `TOWN.cpp`/`Castle.cpp` packed-bit callers were checked but those TUs
remain unread.

H05 gains an allocated font string copy; H06 gains raw icon-entry consumers.
H09/H10 describe only axis clipping and address formation, not general drawing.
R06/R07 retain different edge conventions, explicit pitch, font character rules,
local filter macros and codegen-relevant declarations. Packed bit access starts
at byte `index / 8` and operates through the assembly interface; scalar flags are
a separate domain. All constructors, resource lifetimes and full font wrapping
loops were read, including unusual legacy behavior; this is not a bug-fix pass.

## B05 — complete icon decoder family

Read in full: `Icon2b.cpp`, `Iconf2b.cpp`, `Icond2b.cpp`, `Icondf2b.cpp`,
`Iconm2b.cpp`, `Iconmf2b.cpp`, `icon2bc.cpp`, `iconf2bc.cpp`, `icon2by.cpp`,
`iconf2by.cpp`, `icon2bs.cpp`, their eleven owner headers, `IconRle.h`,
`IconMonoRle.h`, `IconShear.h`, and `SOURCE/dimPalette.h`. Reconciled 12
definitions in 26 files (eleven decoder/scaling bodies and one existing inline).

Reviewed all setup/fast-path, command, literal/solid/dim, clipping, mirrored copy,
source-advance and row-reset paths, not just the obvious duplicate expressions.
H11 records the eight non-sheared decoders' whole-entry fit predicate. H12 records
the existing unused byte-reader inline and encoded palette selection. H06/H10
have additional concrete consumers. R06 rejects folding the family into macros:
mono encoding, dim gate, flipped clipping, shear timing, file-static state and
scaling's temporary bitmap retain distinct contracts. In particular, current
flipped shadow/fill overlap tests and redundant stores were read as written.

## B06 — audio resources and both sound backends

Read in full: `AudiereEffects.cpp`, `AudiereMusic.cpp`, `MilesSound.cpp`, `Midi.cpp`,
`MusicFlags.cpp`, `SAMPLE.cpp`, `soundmgr.cpp`, plus `soundBackends.h`,
`soundManager.h`, `sample.h`, `sampleData.h`, `MIDIWrap.h`, `Midi.h`, `MusicFlags.h`.
Reconciled 73 definitions in 14 files, including the sample-node constructor and
empty destructor, private Miles format inline, five sound-manager header inlines,
and all sample/MIDI resource lifetimes. Compiler-generated marker entries do not
create additional authored bodies.

H07 gains audio clamps and explicit reverse-order exceptions. Existing backend
predicates, conversion and sample lookup functions are already useful abstractions;
R08 records why whole-backend dispatch, teardown and music-play protocols should
not become macros. Reviewed failure paths, handle allocation, iteration depth,
volume/fade arithmetic, polling and resource release, including distinct global
versus member track/readiness state. Provisional music-flag names are not enough
evidence to rename their semantics during a helper search.

## B07 — Windows input and software/native mouse cursors

Read in full: `INPUTMGR.cpp`, `MOUSEMGR.cpp`, `INPUTMGR.h`, `inputManager.h`,
`mouseManager.h`, plus the empty `DebugCheck.h` inline; reconciled 36 definitions
in six files, including the two
private input inlines, empty methods and both pointer-setting overloads. All
event conversion, ring overflow, scan-code table/CP1251 conversion, capture,
cursor mask construction, dirty bounds, save/draw/restore and nested hide/update
state paths were read.

H13 names the cross-TU client-to-logical coordinate arithmetic, retaining packed
16-bit versus POINT input widths. H14 is the lower-confidence three-store manager
registration prefix, now checked in four TUs. H04 gains broader resource-position
lifetimes as non-exact instances. R09 distinguishes a local ring-commit opportunity
from genuinely shared helpers and rejects generic cursor/bit-mask guards. The
cursor uses separate saved/clipped and raw origins, current/drawn size indices,
and several non-interchangeable reentrancy/readiness fields.

## B08 — complete Misc utility owner

Read all 2,182 lines of `Misc.cpp`, plus `Misc.h` and `MiscEnums.h`; reconciled
49 definitions in three files, including the strict const `FindToken` overload
and constrained `LogInt` bridge. Covered allocation diagnostics, filename hash,
binary search, every registry field read/write, defaults/CD discovery, screen
blitting, logging, palette fades, PCX output, random state and modal data entry.

H01/H07 gain concrete instances and store/axis-order exceptions. H05/H06 now have
complete owner review. H15 names the current graphics-config slot; H16 names
inclusive disjointness shared with the previously read mouse TU. R10 keeps local
registry boilerplate and different file/random/fade contracts separate. Searches
also located potential short-arity `LogInt` and allocation-error patterns in game
TUs; those are leads only until those bodies are deliberately read.

## B09 — complete bundled Bzip

Read all 2,061 lines of `Bzip.cpp` and the complete 139-line `Bzip.h`; reconciled
88 authored definitions, including empty hooks, in two files. Covered CRC state,
bit I/O, arithmetic coder/model updates, MTF/RLE, word access, cyclic sorting,
reversible transform, block spotting, stream framing and filesystem wrappers.
Existing macro bodies were read too; macro expansions are not extra physical
function definitions.

R11 records protocol ownership and why generic bit/RLE/free/normalize macros
would be misleading. The MSB-oriented CRC, arithmetic-coded big-endian accessors,
single-step normalization, returning error stubs, and all-frees-before-null-stores
are explicit contracts. No compression, temporary-file creation, or deletion was
executed during this source-only review. This completes all BASE files in scope.

## B10 — shared types and game/Windows declarations

Read in full: `Ints.h`, `va.h`, `SOURCE/CONFIG_TYPES.h`, `SOURCE/KB.h`,
`SOURCE/X_GLOBAL.h`, `SOURCE/KB_TYPES.h`, `SOURCE/combatTypes.h`,
`SOURCE/kbwin.h`, `SOURCE/wingraph.h`, `SOURCE/NOOPT.h`, and `src/SOURCE/NOOPT.cpp`.
Reconciled 93 definitions in eleven files: 53 fundamental storage/conversion
bodies, 26 KB-type helpers, eight combat-domain helpers, three KB/global inlines,
and three delay functions. All conditional branches, defaulted bodies and macro
definitions were considered; extern tables do not create invented bodies.

R12 separates modern strict-build infrastructure from semantic game helpers.
Existing trigger, side/direction/facing, codepage and map-extra interfaces form
the vocabulary to compare against game consumers. Header reads are complete, but
the large implementing game TUs are still unread; no caller coverage is inferred.
H15's actual config record and global-selection declarations are now checked.
R10 distinguishes pointer versus value deadlines in the already-named delay APIs.

## B11 — complete Windows event/display implementation

Read all 587 lines of `kbwin.cpp` and 1,158 lines of `wingraph.cpp`; reconciled
48 definitions in two files against the owner headers read in B10. Covered
startup, complete window procedure and menu recursion, both message pumps,
DirectDraw surface/clipper/error/retry/lock paths, WinG bitmap/palette paths,
display switching and teardown.

H13/H15 gain display and window consumers; H17 confirms short-arity logging,
H18 isolates palette-component scaling, and H19 records the shared diagnostic
format-then-log protocol across four TUs. R13 rejects whole display/pump
unification and records local HRESULT/descriptor-init leads for later network
owner comparison. Existing odd RECT endpoints, dead/default branches, original
error line numbers and callback-sensitive saved settings remain explicit.

## B12 — map, army-group, hex and town data owners

Read `EDITOR/mapcell.cpp`, `fullMap.h`, `mapcell.h`, `ARMYGRP.cpp`,
`armyGroup.h`, `HEXCELL.cpp`, `hexcell.h`, `TOWN.cpp`, `town.h`, and
`townObject.h` completely; reconciled 50 definitions in ten files, including
eight existing header bodies and the strict-build swap helper. This completes
the EDITOR files, not the game consumers of the map owner.

H20 isolates the town building-mask predicate already repeated in army morale
and town spell/building code. R14 records slot-state, bitfield, serialized-layout,
allocation and draw-order traps. The existing map accessors and army membership,
dismissal and counting APIs should be compared with later consumers before
inventing parallel interfaces. `armyGroup::GetMorale` uses an else-if bounded
clamp, unlike H07's independent sequential statements; retain that distinction.

## B13 — complete hero and shared game/combat owner declarations

Read all 2,131 lines of `HERO.cpp`, plus `hero.h`, `HERO.h`, `playerData.h`,
`game.h`, `GAME.h`, `army.h`, `ARMY.h` and `combatManager.h` completely.
Reconciled 56 definitions in nine files: 41 hero TU bodies and 15 existing header
accessors. The 868-line combat owner was read for the earlier hex-cell consumers
as well as the coming path/combat family; no implementing-TU credit is inferred.

H01 gains the exact status-message prefix. H21 names group slot presence,
H22 proposes reusing the existing embarked accessor, and H23 isolates owner-list
compaction. Targeted reads of `game::HeroIDToHeroPos` / `TownIDToTownPos` at
`GAME.cpp:7604` / `7614` establish first-match rather than last-match semantics;
the complete GAME TU remains unread. R15 records hero UI, random-stream,
spell/skill/flag and lifetime exceptions plus leads for later dialog consumers.

## B14 — complete adventure/combat pathfinding family

Read `SEARCH.cpp` (393 lines), `FINDPATH.cpp` (593), `PATH.cpp` (638),
`searchArray.h`, `PATH.h`, `FINDPATH.h`, `advManager.h`, `ADVMGR.h` and
`CMBTMGR.h` in full. Reconciled 32 definitions in nine files, including six
existing header accessors. Reviewed the complete static seed-state owner,
overlaid queue/cell/path storage, both insertion algorithms, adventure target
and monster reseeding, terrain/water/object restrictions, moat handling, all
wide-creature movement branches and every ordered best-direction fallback.

H24-H28 isolate terrain/object/bounds and indexed-owner access; R16 rejects a
universal search/direction/queue framework. Targeted complete body reads of
`advManager::GetCell` (`ADVMGR.cpp:4448`) and `ValidHex` (`CMBTMGR.cpp:1777`)
confirm out-of-map fallback and actual hex range respectively. Neither full TU
receives a read mark. Existing map-extra operand-order variants stay explicit.

## B15 — complete combat-manager implementation

Read all 2,443 lines and 38 physical definitions in `CMBTMGR.cpp`, including the
full catapult animation/state machine, missile drawing loop, initialization,
army transfer, morale/turn selection, obstacle placement, grid picking and
system options. Constants, data and the non-defining vtable marker were read.
The complete combat and dependent model declarations were covered in B12-B14.

H01/H14/H20/H21/H24/H26/H28 gain combat consumers. H29 names normal spell-point
capacity; H30-H32 isolate fixed dialog arguments, allocation-failure checks and
the existing dialog-completion message protocol. R17 records deliberate reset,
loop/random/state/geometry differences and later animation-helper leads.
No combat/source behavior changed. Continue with the complete ARMY TU, then
the remaining large game/AI/UI implementations and final occurrence/ranking pass.

Targeted complete reads of `MemError` (`KB.cpp:3744`) and the already-reviewed
`heroWindowManager::DoDialog` confirm H31's returning reentry path and H32's
handler-result preservation. No complete KB-TU coverage is inferred.

## B16 — complete combat-army implementation

Read all 3,674 lines of `ARMY.cpp` and the 184-line `SPELLS.h`; reconciled 47
physical definitions in two files, including both unannotated private CP1251
inlines. Covered initialization and resources, all geometry queries, movement,
projectiles and recursive attacks, damage/retaliation/luck, corpse and mirror
handling, complete animation/effect loops, berserk targeting, spell chance and
influence/cancellation paths, and temporary walk/frame metadata construction.
Empty hooks, constant tables and both spell-header macros were included.

H06/H07/H28 gain army consumers. H33-H40 isolate creature classification, combat
extent clipping/update, timer expression, angle-table selection, display names,
pixel-center distance and occupied-hex clearing. These are deliberately smaller
than the surrounding drawing, targeting or death algorithms. R18 records their
boundary exceptions and local-only/reuse leads. A targeted complete read of
`combatManager::UpdateCombatArea` (`DRAWING.cpp:382`) rejects it as an equivalent
for H35: it has additional guards, whole-area coordinates and blit-flag writes.
Neither DRAWING nor KB receives full-file credit from targeted reads.

## B17 — complete combat drawing and status presentation

Read all 1,830 lines of `DRAWING.cpp`; reconciled thirteen definitions, including
both CombatMessage overloads, the entire 543-line DrawFrame and 409-line
DrawSmallView. Read message wrapping/history/expiration, grid shading and cursor
save/restore, background resources, every hero/wall/corpse/obstacle/moat drawing
layer, extent/timer exit paths, and the complete mini-view text/icon/spell layout.
The relevant owners and drawing APIs were already fully reviewed.

H01/H20/H28/H34-H36 gain concrete rendering consumers and operand-order variants.
H41 isolates ordered rectangle growth; H42 proposes defaults for the existing
combat sprite API rather than a replacement renderer. R19 preserves the distinct
visibility/recursion/extent flags, byte/text contracts and layer/timing behavior.
The full-file review supersedes B16's targeted-only DRAWING status; KB remains
unread apart from its separately documented targeted bodies.

## B18 — complete flying movement

Read all 476 lines of `FLY.cpp`; reconciled four definitions, including both
FlyTo overloads. Covered wide-creature fitting and both side attempts, complete
ordered attack-position search, facing-dependent endpoint shifts, occupied-hex
stores, every flight leg/frame/sound path, timer arithmetic and landing cleanup.
All relevant model/path/drawing declarations were already read.

H28/H34-H36/H41 gain flight instances or explicit variants; H43/H44 isolate
interior-hex bounds and integer-result Euclidean length. R20 keeps placement,
movement/sound timing and null-pointer/domain assumptions separate. A flight
occupant reset writes index/side/frame, not H40's side/index-only protocol.

## B19 — complete combat spell implementation

Read all 3,918 lines and 37 definitions of `SPELLS.cpp`, including its constants,
macro variants and trailing data. Covered the entire spell-selection UI and
recursive teleport callback, target and corpse selection, 576-line CastSpell,
every area/individual/mass effect, bolt construction/branching, palette/shake/
ripple drawing, mirror/summon/resurrection and delayed earthquake state changes.
The SPELLS header and complete army/combat/resource owners were already read.

H07/H28/H30/H31/H34-H36/H38/H40/H42 gain concrete spell consumers and exceptions.
H43/H44 gain validation/length instances, H45 names the existing effect-cache
replacement protocol, H46/H47 cover duration-artifact and golem checks, and
H51/H52 cover shorter message calls and existing affected-slot indexing. R21
distinguishes genuine repeated idioms from incompatible cast, damage, resource,
animation and corpse-stack algorithms. No source behavior was corrected.

## B20 — complete combat spell AI

Read all 1,479 lines of `SPELLAI.cpp`; reconciled eleven definitions, including
the full 582-line DetermineEffectOfSpell and 353-line EffectSpellDamage. Reviewed
spell eligibility/scoring/ties, all target modes, duration/cancel modifiers,
creature creation and influence scoring, hex iteration, cure/resurrection value,
damage-area traversal and both final score paths. Static Buka score values and
global tables were included; no other executable version's signs were imported.

H28/H46/H47/H52 gain AI consumers; H48-H50 name narrowly defined dragon and
spell-state predicates across the already-read combat TUs. R22 records where
the evaluator and runtime intentionally cannot share a full formula or iterator.
All three B18-B20 files are now fully read; no snippet-only coverage is credited.

## B21 — complete combat tactical AI

Read all 1,090 lines and nineteen definitions of `AI.cpp`, plus the complete
`PHILAI.h` / `philAI.h` declarations. Reviewed retreat eligibility and value
construction, the entire prioritized tactical decision tree, lich splash scoring,
all seven army-mask builders, best/worst/closest/strength methods and both
front/target approach paths. The implementing PHILAI TU remains unread; reading
its interface establishes call contracts, not full stack-value algorithm coverage.

H21/H28/H39/H49/H50 gain AI instances or explicit variants; H53 isolates a
two-store next-action protocol. H54 records animation-start/Wince reuse discovered
while comparing the complete preceding combat family; H55 records two additional
cross-TU base/upgrade creature predicates. R23 retains mask, scoring, path and
retreat distinctions rather than hiding the large decision tree in macros.

## B22 — command and adventure-cursor packet declarations

Read `CURSOR.h` (110 lines), `COMMAND.h` (44) and `REMOTE_TYPES.h` (68) in full;
zero function bodies in three files. Reviewed cursor/boat/map-change/action
domains, packed SMapChange movement/wire overlay, CombatRemotePacket reserved
fields and typed command payload, network modes/protocols and player-info layout.
These declarations support already-read combat action consumers and the next
CURSOR/COMMAND/remote implementation passes. Those implementing TUs remain unread.

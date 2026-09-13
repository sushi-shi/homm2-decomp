# Final common-helper decisions

This is the final disposition of the audit applied in `750ce56aa`, not a queue
of proposed refactors. **56 families have retained applications, 14 measured
A/B alternatives remain explicit, and 29 lower-gain families remain explicit.**
All 46 exclusions remain in force. Family IDs identify contracts, not necessarily
one helper or an exhaustive list of replaced occurrences.

The rows below consolidate the final implementation decisions. Full historical
call-site witnesses and reading notes remain in the
[archived audit](https://github.com/sushi-shi/homm2-decomp/tree/77b87cd47c847aa5ff6aa95d2c691601d0057f6d/docs/readability); measured source alternatives
and rejection evidence are linked from [implementation.md](implementation.md).
No historical macro spelling is claimed.

## Retained applications

"Retained" includes only the stated subset. Keep other evaluation orders,
conversions, caller policies and source forms explicit unless separately measured.

| Family | Final application and contract |
| --- | --- |
| H01 | One `SET_WIDGET_MESSAGE` in `BASE/message.h`, replacing two private definitions and 75 exact ordered triples in 26 further TUs. Four existing calls remain. Type/command/id order, payload tails, modifier preservation and subsequent id-only broadcasts are unchanged. |
| H02 | Eleven widget-local hit tests in eight TUs use `WIDGET_CONTAINS_LOCAL_POINT`. Coordinates remain narrowed before the call; drop-button and popup rectangles stay explicit. The three-arm probe rejects the inline method (1,270 vs 1,240 bytes) and retains the byte-neutral expression macro. |
| H03 | All eight widget readers share `READ_WIDGET_GEOMETRY`: four ordered signed-word stores. The live resource pointer is reevaluated before each read; id/kind, lookups and payload reads remain outside. |
| H05 | Nine assignment/copy pairs in listbox, droplist, Overview and TOWNMGR use `ALLOC_COPY_STRING`. Existing frees and ownership transfer stay explicit. FONT's local declaration/initialization pair stays readable as two statements; no second declaration-generating macro is introduced. Its non-copy scratch allocation, extra-capacity text buffers and ADVMGR's format-string copy are excluded. |
| H11 | Eight unsheared decoders share `ICON_FITS_CLIP`. It retains left/right/top/bottom comparison order and already-resolved flipped coordinates; clip-mode assignment, inclusive edges and decoder loops remain explicit. |
| H13 | Ten input/source-coordinate expressions in three TUs use `CLIENT_TO_GAME_X/Y`. Signed conversion, original long/int arithmetic, live dimensions and destination narrowing remain. Width/height scaling and inclusive rectangle preparation stay explicit because the new names describe coordinates. |
| H15 | Eighty live lvalue selections in seven TUs use `CURRENT_GRAPHICS_CONFIG`. No reference is cached across callbacks; game/editor slot selection and saved scalar fields remain unchanged. |
| H17 | Six trailing sentinel defaults on `LogInt` and its strict enum bridge; 42 calls shortened at their actual trailing-sentinel boundary. Required first value and meaningful numeric zero slots stay explicit. |
| H19 | The existing one-value `LOG_SUMMARY_VALUE` is shared from Misc.h with statement-safe comma sequencing. Fifteen old calls plus fourteen new calls in SMACKMGR, Bzip and dpnetwin retain formatting into `gText` before logging. Other arities/buffers and subsequent shutdown/error operations remain explicit. |
| H20 | 138 building-mask queries use existing `HAS`, with the same raw `u32l` storage and explicit `IDX`/`BIT`/shifted masks. No normalized-return town API is added. Mutations, the raw captain-mask store and unclassified literal-mask tests stay explicit. |
| H22 | 33 direct `HAS(..., HERO_EVENT_EMBARKED)` queries in eight TUs now use the existing `IsEmbarked`. Its masked integer result, surrounding guards and lookup timing are preserved; flag mutations and boat-object tests are not changed. |
| H24 | 56 pointer-based terrain queries use `CELL_TERRAIN`. It preserves the byte/table lvalue, exact supplied cell expression and one evaluation, including existing GetCell calls and their caller-specific fallback. Index-only and dot/row-view variants stay explicit. |
| H25 | Nine ordered sprite/index/flag conjunctions use `CELL_HAS_NON_SHADOW_OBJECT`. Existing 0x80 aliases share the map owner's shadow-only flag; no passability rule or tileset/frame shadow detection is implied. The repeated-GetCell negation remains explicit. |
| H27 | Five SEARCH node accesses use the existing reference-returning `GetNode`, already used by FINDPATH. The global stride, coordinates and searchNode overlay stay unchanged. |
| H29 | Fifteen `Stats(KNOWLEDGE) * 10` prefixes use `HERO_NORMAL_SPELL_POINTS`. Caller narrowing, further multiplication and each Stats call remain. The method alternative changes instructions; the macro preserves them. |
| H30 | Exact text-only defaults on `NormalDialog`; 238 calls shortened. Mode is required; resource-bearing dialogs and nondefault positions retain their full arguments. |
| H32 | Ten `FINISH_DIALOG_MESSAGE` calls save the original widget id, then store the close id and dialog-select command. Handler returns, the timeout's preceding type write/following reset, SETUP's later cancel override and all other payload remain explicit. The town event handler has the same three-store contract. |
| H33 | Ten exact earth/air/fire/water memberships, including ordered negations, use `IS_ELEMENTAL_CREATURE`. Different comparison orders, the three-elemental frame case and surrounding summon/mirror/undead policies remain explicit. |
| H35 | Sixteen `UPDATE_INCLUSIVE_REGION` calls name endpoint-to-size conversion at the window-manager boundary, with stable local/global bounds. Other drawing/fizzle APIs, clipping and caller flags are untouched. |
| H36 | Fifteen `COMBAT_DEADLINE` expressions retain the `i32` conversion after floating tick addition and the original delay-first product. Long-cast, reversed-product, division and double-factor variants remain explicit, as do timer slots and polling. |
| H38 | Seven `CREATURE_DISPLAY_NAME` calls preserve the lazy table selection and `count <= 1`, including SPELLS' equivalent `PLURAL_QUANTITY_MINIMUM - 1`. Separate-format branches, `== 1` variants and lowercased `gTargetName` remain explicit. |
| H40 | Eight `CLEAR_HEX_OCCUPANT` calls clear side then index only. Repeated cell expressions, front/rear selection and guards are retained; grid initialization, reversed stores and frame/corpse state remain explicit. |
| H42 | Three trailing draw defaults on `CombatClipDrawToBuffer`; 29 calls shortened. Limits/orientation/result tests remain, as do nonzero offsets and palette/shear arguments. |
| H43 | Two complete range/column tests use `IS_INTERIOR_COMBAT_HEX`. Existing ValidHex calls, partial column tests and COMMAND's special/sentinel hex rules stay unchanged. |
| H44 | Eight local-delta expressions use `INTEGER_VECTOR_LENGTH`: signed integer squares/sum, then double sqrt, then i32 truncation. Abs calls and step rounding stay outside. The map-radius repeated-subtraction variant remains explicit. |
| H47 | Eleven iron/steel memberships use `IS_GOLEM_CREATURE`. The inline alternative adds 53 bytes in its probe; the macro is unchanged. Runtime/AI spell coverage, double half-damage arithmetic and destination casts remain outside. |
| H48 | Two green/red/black/bone memberships use `IS_DRAGON_CREATURE`. Three-living-dragon immunity is not changed. |
| H49 | Nine berserk/hypnotize pairs use `ARMY_HAS_BERSERK_OR_HYPNOTIZE`, including the independently measured ordered zero conjunctions. Reversed order and other ownership/action policies remain explicit. |
| H50 | Seven blind/paralyze/petrified triples use `ARMY_HAS_INCAPACITATING_SPELL`, including the independently measured ordered zero conjunctions. Other orders, death, quantity and the separate mind-influence pair are not folded in. |
| H52 | Five flattened affected-army accesses in SPELLS/SPELLAI use the existing two-dimensional array spelling. Both reads and writable stores retain the 20-slot stride and their original chance-query order. |
| H53 | Thirteen `SET_NEXT_COMBAT_MOVE` calls set action before evaluating/storing the destination. Intervening ValidAttack work, previously selected destinations, action-extra stores and returns remain outside. |
| H55 | Fourteen exact lich, vampire and troll base/upgrade memberships use three separate named predicates. Vampire-lord healing remains a single-type test. |
| H56 | Eight `HEX_HAS_OCCUPANT` queries compare side before index, with no requested-index narrowing. Empty-cell alternatives, cell lookup, validity and blocked-state tests remain in the callers. |
| H57 | Seven `CLEAR_ARMY_TARGET` calls preserve side/index order. Attack direction, move/previous-target hexes, save/restore and index-first variants remain explicit. |
| H58 | Two positive PATH and three negative COMMAND gate exceptions use `CAN_PASS_CASTLE_GATE`. Both forms are independently measured. The seven ordered conditions still read gpCombatManager even when the caller reads candidate cells through a different receiver; no passability or TestRaiseDoor policy is added. |
| H60 | Forty-two `DRAW_ADVENTURE_ICON` and seven separately named flipped calls keep the current screen and 480-square adventure viewport. Clipping mode, coordinate/frame expressions and normal/flipped semantics remain explicit. Zero-extent cloud/boat calls and other rendering APIs stay unchanged. |
| H63 | Both scouting-plus-telescope expressions use `HERO_SCOUTING_VISIBILITY_RADIUS`. Signed skill/table indexing, one artifact Boolean, distinct origins and visibility timing remain. Scouting-only GAME sites are excluded. |
| H64 | Both backends use separate initialization, enqueue and array-disposal operations. The first two retain allocation/store/copy order and repeated global reads; the disposal inline retains conditional frees and unconditional null stores. It does not drain payloads or reset indices. The copy/dequeue inline adds eleven bytes (147 to 158) and is removed, leaving that fourth operation explicit after each backend's ProcessMessages. The stored-size/copy-size discrepancy and different backend drain policies remain unchanged. |
| H66 | Five `FREE_NODE_QUEUE` calls preserve the caller's node lvalue, final null, repeated pop and one scalar H2_FREE per node. This is a single while statement, safe as a conditional body; no extra wrapper loop, traversal, lock or shutdown is added. The alternative inline keeps the 233-byte size but changes instructions and is removed. |
| H68 | Retry-dialog/default-message defaults on `TransmitRemoteData`; 25 calls shortened. Reliability remains the explicit fifth argument, including SendMapChange's zero. The explicit non-default message type remains. |
| H69 | Shared uppercase reuse, one companion `CyrillicToLower`, and shared code-point constants replace private/expanded character folds across six TUs. Locals and result-byte stores remain. The callable `toupper(char)` and CRT `toupper(int)` remain distinct and unchanged. Three GAME u8-staged sequences remain explicit after measured non-equivalent helper attempts. |
| H70 | Forty-four reads and 46 writes use exact-size file-value expressions in six TUs, including the plain PCX header and KB's individually sized entry writes. Results, address/sizeof identity, duplicate tent fields and partial-read effects remain. Arrays, wider-global slices, prefixes, KB's oversized entry reads and REQUEST's distinct pointer/sizeof(*header) spelling stay explicit. |
| H72 | Four offered/displayed level expressions use `NEXT_MAGE_GUILD_LEVEL`, preserving saved locals and upper-only signed capping. The value inline adds five bytes to SetupCastle and is removed. KB's uncapped actual cost subscript remains unchanged. |
| H73 | Both completed-building queries use `TOWN_BUILDING_COMPLETE`: mask first, then non-guild or level exactly five. BIT and Castle's signed-long shift are separately measured; both remain 32-bit mask operations on the existing u32l owner. No affordability or raw captain-mask query is changed. |
| H74 | Four `SET_ADVENTURE_BUTTON_FLAGS` calls preserve message type, command, enabled-bit payload, then each id/broadcast from 1 through 6. Active-manager guards remain only where originally present. Receiver/message expressions remain live across callbacks. |
| H76 | Five positive-stack queries in Overview, TOWNMGR and SWAPMGR use `ARMY_GROUP_HAS_POSITIVE_STACK`. Type-first evaluation, signed counts and the unused census remain. H21's type-only queries do not acquire this condition. |
| H78 | Sixteen select/alternate-select command pairs use `IS_WIDGET_SELECTION_COMMAND`. Message type, right-button guards, routing and dialog behavior remain in each caller; Newgame's equivalent command aliases are included. |
| H79 | Both `PRESENT_RESTARTED_CAMPAIGN_MAP` calls preserve invalidation, bottom-view reset, fade-out, origin setup, redraw and fade-in. Different initialization calls and dialog guards remain outside. |
| H81 | Five `ADD_HERO_EXPERIENCE_AND_CHECK_LEVEL` calls preserve add-before-CheckLevel and both receiver evaluations, including the repeated GetHero calls. Setup suppression, cached levels and reward policy remain explicit. |
| H82 | Final `i8 extra = -1` on `GiveArtifact`; 31 calls shortened. End-game policy and the four special spell/metadata grants remain explicit. |
| H85 | Final `waitTime = -1` on `WaitEndSample`; all 23 calls shortened. The negative sentinel still selects the callee's finite 4000-ms default. |
| H87 | Five day-first queries use `GAME_DAY_NUMBER`, with packed u16 promotion to signed i32 and no caching or normalization. PerDay still queries before rollover; month-first campaign arithmetic and its i16-return variant stay explicit. |
| H90 | Five `CLEAR_ARMY_GROUP` calls retain the two array-major clears: type bytes to -1, then signed count storage to zero. Aliased quantity storage has the same address/size. No whole-object memset or per-slot loop is introduced. |
| H92 | Four tent-mask tests use `PLAYER_HAS_VISITED_TENT`, retaining the signed byte promotion and raw mask result. Color decoding and the password-before-flag order remain outside. |
| H94 | Three final zero defaults on `FightValueOfStack`; 24 calls shortened. Hero/raw mode remains explicit. The five nondefault town/enemy cases, including town id -1, stay unchanged. |
| H96 | Twenty-four exact two-delta sums use `MANHATTAN_LENGTH`. Actual deltas, including ResetHeroRVs' two X-coordinate reads, remain unchanged; outer absolute values, shifts, thresholds and tie-breaking are untouched. |

## Measured alternatives left explicit

These are rejected source shapes, not impossible future abstractions. A different
evidence-backed structure needs its own byte/ordered-relocation comparison.

| Family | Decision and boundary |
| --- | --- |
| H21 | Retain the direct type-sentinel comparison. `HasTroop` changes HasAllUndead from 90 to 101 bytes; a macro merely wrapping one already-named comparison would add little clarity. No positive-quantity condition is introduced. |
| H28 | Retain direct combat-array access. The reference-returning GetArmy probe keeps 161 bytes but changes instructions/register use. A three-argument macro would not improve the existing owner/two-index spelling enough to justify another API. Physical, controlling and mixed side/index choices remain visible. |
| H34 | Retain the six exact-order four-`if` clamps. The statement-safe `do/while(0)` macro increases Walk from 2,599 to 2,609 bytes. Its definition is removed; no unguarded multi-statement macro or reordered ternaries are substituted. |
| H45 | Retain the guarded spell-icon dispose/load/cache update. Its expanded statement-safe block changes the probe from 863 to 869 bytes. Resource ownership and reload conditions remain visible. |
| H46 | Retain the two ordered artifact tests and additive bonuses. The statement-safe block changes the probe from 3,918 to 3,924 bytes; neither test becomes else-if or one cached artifact query. |
| H61 | Retain the five ordered independent frame-remap tests and separate final offset. The statement-safe block adds six bytes (9,310 to 9,316). |
| H65 | Retain the explicit allocation/tag/guarded-copy prefix. The tested pointer-return inline adds four bytes (467 to 471) and changes relocation sites, and is removed. Caller locals, u16 promotion, unchecked allocation and Winsock's existing error-return/free behavior are not repaired. |
| H71 | Retain the explicit type-then-count stores. Both existing Dismiss and a separate inline were measured without changing the out-of-line ABI: 775 bytes becomes 767 with a new call relocation, or 783 with the inline. The trial inline is removed. |
| H80 | Retain each campaign-bonus widget protocol. Its statement-safe block changes the probe from 2,268 to 2,278 bytes; selection guards and refresh order are not generalized. |
| H83 | Retain event-resource addition followed by floor-to-zero. The trial player method changes 4,347 to 4,290 bytes and 159 relocations to 153, and is removed. Raw resource indices, overflow and event timing are not repaired. |
| H86 | Retain cheat-flag assignment followed by campaign-conditional assignment. Its statement-safe block adds six bytes (3,587 to 3,593). |
| H89 | Retain the ascending seven-resource cost loops. The trial player method changes the probe from 645 to 649 bytes and removes a relocation, and is removed. No affordability, saturation or transaction layer is added. |
| H91 | Retain presence-first signed quantity summation. The trial CountCreatures inline changes 977 to 983 bytes, and is removed. Count zero, absent types, and the existing unsigned troop alias are not conflated. |
| H93 | Retain the explicit i16 store followed by the named count shift. `RecruitSiteCount(i16)` changes RecruitSiteEvent from 368 to 362 bytes and changes instructions. A second macro merely naming the already-named shift would not justify another API. The rejected inline is removed. |

## Lower-gain abstractions not introduced

These 29 former rank-C observations are final scope decisions, not pending tasks.

| Family | Why the existing source stays explicit |
| --- | --- |
| H04 | Save/lookup/restore remains visible; stored MakeId and larger protected regions are distinct, and a general position guard would obscure that distinction. |
| H06 | Keep the existing GetIconEntry API and explicit addressing at their current sites; no second accessor or unmeasured new out-of-line call is added. |
| H07 | Explicit clamps show lower/upper order and already-narrowed stores; a universal clamp would hide negative-range and interleaved-axis differences. |
| H08 | Parameter-heavy list-layout helpers would move the same arithmetic behind longer calls; row selection and thumb travel remain separate. |
| H09 | Origin adjustment and far-edge shrinking remain explicit, including the interleaved Fizzle axes. |
| H10 | Keep actual base/pitch expressions visible; decoder state and fixed screen pitch do not become bitmap-width assumptions. |
| H12 | Retain existing cursor/palette APIs and distinct decoder expressions. The cursor helper's load/increment order and the two palette domains are not interchangeable. |
| H14 | Three named manager-field stores do not justify a name that could imply registration or resource initialization. |
| H16 | Eight-edge predicates would hide comparison order and saved/raw rectangle choices with little reduction in argument noise. |
| H18 | The component shift is already short; preserve caller-specific promotion/store types rather than add another palette conversion layer. |
| H23 | Keep compaction after the last-match search, old-tail sentinel and external count/selection changes visible. |
| H26 | Map bounds remain local to their actual dimensions, comparison order and access timing; no broad guard is introduced. |
| H31 | An explicit null test followed by MemError is clearer than another macro, especially because MemError is not assumed nonreturning. Unchecked allocations stay unchecked. |
| H37 | Keep the small midpoint scan beside its distinct angle/count/animation context; no generic count repair or projectile-loop extraction. |
| H39 | Existing QuickDistance already names the approximate metric. Coordinate extraction and combat-cell choices remain visible without another forwarding API. |
| H41 | Eight operands and several store-order variants outweigh the gain from an extent-growth macro. |
| H51 | Keep CombatMessage history/update arguments explicit; these are meaningful policies, unlike the applied sentinel defaults. |
| H54 | Retain the existing Wince boundary and explicit sequence/frame pairs; no redundant animation-start API or unmeasured call substitution. |
| H59 | CompleteDraw and UpdateScreen already describe the two operations well. No combined redraw API is added. |
| H62 | Keep guarded pointer detach/delete and the later separate null stores visible, rather than conceal the ownership/lifetime distinction. |
| H67 | Three statements clearly show first-pop then conditional second-pop; no extra priority or locking semantics are implied. |
| H75 | Disable then defer-dim remains explicit, with its message-state differences and special flag. |
| H77 | The reverse defer-dim then disable protocol remains separate; symmetry is not a reason for a second public API. |
| H84 | Six operands would make reward-history bookkeeping harder to follow than the conditional copy and new-primary stores. |
| H88 | Typed guarded scalar deletion and unconditional nulling stay explicit; no deletion-form repair or generic ownership guard. |
| H95 | After H94 shortens the stack-value calls, retain the ratio's visible signed product, double division and float store rather than introduce another wrapper. |
| H97 | The named INFO/QUICK_VIEW ternary remains clear after H30; keep that mode choice visible. |
| H98 | Retain the short rounded-step expression beside each caller's positive-speed/minimum-one policy. |
| H99 | Keep hover equality, state update and consume return together in the handler; no stateful query obscures the dispatch boundary. |

## Excluded broader refactors

The following 46 observations remain outside this change. Local similarities do
not authorize gameplay fixes, validation, container/wire-layout changes, allocator
substitution, resource-lifetime normalization or combined event/rendering pipelines.
The archived candidate ledger contains the detailed examples for each stable R ID.

| ID | Excluded or local-only scope |
| --- | --- |
| R01 | A universal widget event handler |
| R02 | Resource cache getter/aggregate scan boilerplate |
| R03 | Whole list-box/drop-down algorithms |
| R04 | Generic linked-list removal or save/restore-background macro |
| R05 | Shared event polling/dispatch loop |
| R06 | Universal clipping or whole sprite decoder macro |
| R07 | Local-only font/palette algorithms and load-bearing scaffolding |
| R08 | Unified audio lifecycle or backend-dispatch macros |
| R09 | Event queue and cursor state guards |
| R10 | Registry, file, random, and fade utilities |
| R11 | Bundled Bzip implementation is its own protocol boundary |
| R12 | Compatibility machinery versus period helpers |
| R13 | Windows display/error initialization and painting |
| R14 | Data-owner algorithms and deceptively similar state resets |
| R15 | Hero-specific workflows and resolved UI leads |
| R16 | Search algorithms, packed state and direction systems |
| R17 | Combat setup, reset and presentation are not one generic algorithm |
| R18 | Army animation, damage and spell protocols remain explicit |
| R19 | Combat rendering is ordered stateful work, not a scene framework |
| R20 | Flying fit, path and animation differences |
| R21 | Spell execution is not the same as a universal effect pipeline |
| R22 | Combat spell evaluation retains its own formulas and traversal |
| R23 | Tactical AI masks, ranking and movement remain domain-specific |
| R24 | Adventure cursor, movement and map-change protocols remain explicit |
| R25 | Combat commands, reward dialogs and idle cycling retain their contracts |
| R26 | Adventure manager workflows and superficially shared formulas |
| R27 | Remote reliability and DirectPlay/Winsock transport differences |
| R28 | NetBIOS and serial protocols, queues and local-only helpers |
| R29 | Game setup, serialization, calendar and transfer contracts |
| R30 | Town construction, recruitment, strip and trade contracts |
| R31 | Hero exchange and split contracts |
| R32 | Combat detail, score and arena presentation contracts |
| R33 | Kingdom overview and world-view contracts |
| R34 | Smacker playback, palette and campaign selection contracts |
| R35 | Remaining packet/map declarations and expansion data |
| R36 | File-request enumeration, filtering and input contracts |
| R37 | New-game setup, remote exchange, chat and scenario display |
| R38 | Setup dialogs, configuration, menu overrides and local help wrappers |
| R39 | Base campaign graph, switching, presentation and bonus contracts |
| R40 | Expansion campaign state, graph, awards and display contracts |
| R41 | Adventure event rules, local similarities and reward contracts |
| R42 | Combat presentation, network fragments and loss processing |
| R43 | Application orchestration, dialogs, network exits and static game data |
| R44 | AI planning, purchase valuation and event rules |
| R45 | AI battle simulation, transfers and cache lifetimes |
| R46 | Final disposition of remaining early leads |

## Shared-boundary rules

- Keep mask presence separate from completed buildings; type-present slots,
  positive stacks and creature totals are different queries.
- Creature membership sets and spell-influence sets are not universal ability,
  control or action-eligibility predicates.
- Preserve comparison/store order, signed narrowing, packed widths, lazy branches
  and every live receiver evaluation, including calls repeated across callbacks.
- Slot-major dismissal and array-major clearing are different storage protocols.
  Guards, nulling, allocation and ownership transfer remain at their original sites.
- Preserve each geometry metric, bounds convention and caller-supplied dimension.
  Do not repair observed coordinate or width/height oddities during naming changes.
- Receive-storage primitives do not unify backend draining, framing or reliability.
  Existing stored-size/copy-size discrepancies remain outside this refactor.

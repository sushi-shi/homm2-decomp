# Applying the common-helper audit

Work in progress on `refactor/buka-common-helpers`, based on the completed audit
at `77b87cd47` and canonical `decomp-gold-2.1-buka` at `ca2904a91`.
The requested deliverable is the applied readability changes and a pull request
against the canonical branch. Generated/portable-source propagation remains a
separate step.

## Scope and acceptance

Apply the [ranked findings](findings.md) according to their precise contracts in
[candidates.md](candidates.md). Work through the A and B families in small measured
groups; give every H family an explicit final disposition. Rank C observations
are not mandatory abstractions, and R entries remain exclusions. Do not make a
generic helper out of variants with different evaluation, narrowing, mutation,
ownership or event order. Preserve meaningful call policies and existing APIs.

Retained changes require the canonical build and byte/ordered-relocation review.
Before/after compiler-object comparisons supplement the retail comparison; they
are not substitutes for it. Test the strict modern declarations too. Update the
reading inventory for reviewed changes, then publish the branch and open the PR.

Planned groups:

1. Existing widget-message macro and seven optional-argument API families.
2. Existing accessors, affected-army indexing and CP1251 vocabulary.
3. Narrow widget, graphics and resource operations.
4. Army, creature, spell, combat-grid and action contracts.
5. Map, town, player-resource, event and calendar contracts.
6. Small transport storage operations and fixed UI/campaign protocols.
7. Whole-tree caller/contract review, explicit disposition of every finding,
   combined verification, refreshed inventory and PR handoff.

Completed groups and explicit dispositions follow below. Unlisted families are
still pending application review, not implicitly implemented.

## Reproducible verification inputs

- Retail image extracted without running the installer from the existing local
  Buka disc archive: `homm2g-buka.iso`,
  `Autorun/Launch/Setup2/data1.cab` / `data1.hdr` / `data2.cab`,
  `Program Executable Files\\HMM2PL.exe`.
  SHA-256: `bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a`;
  this is the checked-in retail-image pin. All extracted files stay gitignored.
- The published `toolchain-vc6-sp5` archive currently hashes to
  `b243b68a1df8c2c54f7830a14221fa0a73fd3bbca78343fd1dd81a815a32ecec`, not the
  repository's pin. The installer correctly refused it; no checksum was changed.
- Rebuilt with `nix-shell scripts/toolchain/create-toolchain-release.nix`.
  All 12 pinned compiler/linker/assembler/runtime artifacts verified, and the
  produced archive matches the repository pin exactly:
  `accfbb0ab1a63d4b698d0d205950611386c39b81b1868b336dbfdab413bb84c4`.
  `homm2.init.toolchain --check` passes after extraction into `build/toolchain`.

## Setup repairs and controls

Fresh `homm2 build` failed before compiling: the link-graph generator split its
address-sorted objects at data-only X_GLOBAL. Without generated symbols this
placed BASE/Midi in the direct-link group and left no Midi archive member.
Archive membership now follows the BASE unit tier, preserving address order
within each group. Four regressions cover fresh inventories, unanchored X_GLOBAL,
known ordering and fixed OMF assembly members; all four pass.

The first parallel annotation pass raced while initializing lowercase SDK-header
symlinks. Running `python3 -m homm2.init.clangd` serially initialized the mirror;
no SDK file, compiler flag or game declaration was changed for that setup race.

All 98 original TUs compiled. Annotation parsing then rejected OldMain's ternary
of two string literals passed to the legacy mutable-char score API. An explicit
`const_cast<char*>` preserves the existing interface and does not authorize a
write to the literal. Recompiled KB before/after: all 11 non-debug sections have
identical bytes, sizes and flags; all 5,085 ordered relocations retain their
types, sites and targets. Compiler-generated `$SG`/`$T`/`$L` numbers may differ,
but resolve to identical sections/offsets; named symbol identities are unchanged.
The KB annotation pass now succeeds (508 symbols). This is a compatibility
prerequisite, not a newly implemented audit helper or a retail-exactness claim.

The initialized baseline passes `homm2 build`; its current report is 1,727/1,727
exact compared functions and 291,995/291,995 data bytes. Those report totals do
not replace the raw-byte/ordered-relocation controls above. `homm2 selftest`
passes all 888 tests, including the four new link-graph regressions.

## Group 1: existing message and optional-argument APIs

| Family | Retained application |
| --- | --- |
| H01 | One `SET_WIDGET_MESSAGE` in `BASE/message.h`, replacing two private definitions and 75 exact ordered triples in 26 further TUs. Four existing calls remain. Type/command/id order, payload tails, modifier preservation and subsequent id-only broadcasts are unchanged. |
| H17 | Six trailing sentinel defaults on `LogInt` and its strict enum bridge; 42 calls shortened at their actual trailing-sentinel boundary. Required first value and meaningful numeric zero slots stay explicit. |
| H30 | Exact text-only defaults on `NormalDialog`; 238 calls shortened. Mode is required; resource-bearing dialogs and nondefault positions retain their full arguments. |
| H42 | Three trailing draw defaults on `CombatClipDrawToBuffer`; 29 calls shortened. Limits/orientation/result tests remain, as do nonzero offsets and palette/shear arguments. |
| H68 | Retry-dialog/default-message defaults on `TransmitRemoteData`; 25 calls shortened. Reliability remains the explicit fifth argument, including SendMapChange's zero. The explicit non-default message type remains. |
| H82 | Final `i8 extra = -1` on `GiveArtifact`; 31 calls shortened. End-game policy and the four special spell/metadata grants remain explicit. |
| H85 | Final `waitTime = -1` on `WaitEndSample`; all 23 calls shortened. The negative sentinel still selects the callee's finite 4000-ms default. |
| H94 | Three final zero defaults on `FightValueOfStack`; 24 calls shortened. Hero/raw mode remains explicit. The five nondefault town/enemy cases, including town id -1, stay unchanged. |

Total: 412 shorter calls and 75 named message-header protocols. Declaration
parameter types, decorated callable identities and out-of-line implementations
are unchanged. Only changed C++ ranges were formatted, without include sorting.

H01 has two complete four-arm clean-state structural comparisons, documented in
[the measured source dossier](../matching/SetWidgetMessage/shared-macro.cpp).
The original sequence, parenthesized sequence and comma expression all reproduce
the tested retail functions. `do/while(0)` adds instructions at `/Od` and is
rejected. The comma expression is one safe statement at a conditional call site;
its repeated message operand must remain a stable expression.

The combined raw-object control passes for all 98 objects: 1,516 non-debug
sections retain their bytes, sizes and flags, and all 42,433 ordered relocation
records retain their sites, types and destinations. All named symbol coordinates
and storage classes remain unchanged. Only actual defined static-data `$SG`/`$T`
and code-label `$L` counter suffixes may be renumbered, never their destinations. The reusable
`homm2.audit.object_equivalence` control includes 16 negative/positive tests;
it rejects changed instructions, addends, ordered targets, section flags, named
symbols, empty comparisons and missing/added objects.
The full tool suite passes all 904 tests after group 1.

Reproduce after saving the pre-edit raw object directory:

```sh
homm2 build
python3 -m homm2.audit.object_equivalence \
  build/readability/baseline-objects build/objdiff/base
homm2 relocs --fields
homm2 selftest
```

`homm2 build` keeps the baseline report totals. Explicit `homm2 relocs --fields`
passes: 1,727 functions, 38,307 ordered sites and zero structural review items.
The fixed-width-integer gate also passes.

## Existing verification limitations (not suppressed)

The canonical branch has `AUDITS = False` in `homm2/cli.py`; its `homm2 build`
does **not** run the hard gates described by the older build documentation.
This PR does not switch that policy or silently count skipped checks as passes.
The separately invoked gates reveal existing failures:

- `assert_decls`: AudiereEffectsState and SeedPositionState are TU-local structs.
- `assert_defs_declared`: the same five missing-owner-header/declaration reports
  reproduce in the unchanged audit worktree at `77b87cd47`.
- `assert_no_fake_labels`: two existing missing functions and 97 attribution
  reports. The all-object control confirms the entire named-symbol set is
  unchanged by group 1.
- `annotated_functions`: private-function parsing rejects the VC6 STL headers.
- Strict Clang: 95/96 TUs fail in those old SDK headers. Repeating the complete
  check with `-ferror-limit=0` produces 1,615 errors, all confined to eight VC6
  STL headers, and **zero errors in game source/headers**. This is not a passing
  strict compile. No vendor-header patch, error suppression, weakened gate or
  compiler-flag change is retained.

These baseline limitations remain distinct from the passing raw-object and
retail-relocation controls. They are not permission to introduce new failures.

## Group 2: existing accessors and localized byte rules

| Family | Application and boundary |
| --- | --- |
| H22 | 33 direct `HAS(..., HERO_EVENT_EMBARKED)` queries in eight TUs now use the existing `IsEmbarked`. Its masked integer result, surrounding guards and lookup timing are preserved; flag mutations and boat-object tests are not changed. |
| H27 | Five SEARCH node accesses use the existing reference-returning `GetNode`, already used by FINDPATH. The global stride, coordinates and searchNode overlay stay unchanged. |
| H52 | Five flattened affected-army accesses in SPELLS/SPELLAI use the existing two-dimensional array spelling. Both reads and writable stores retain the 20-slot stride and their original chance-query order. |
| H69 | Shared uppercase reuse, one companion `CyrillicToLower`, and shared code-point constants replace private/expanded character folds across six TUs. Locals and result-byte stores remain. The callable `toupper(char)` and CRT `toupper(int)` remain distinct and unchanged. Three GAME u8-staged sequences remain explicit after measured non-equivalent helper attempts. |

The H22/H27 structural pairs preserve target bytes; the combined H22/H27/H52
all-object control also passes. H69's two representative uppercase-reuse pairs
preserve instruction bytes, but their existing function-local retail relocation
residuals are not described as exact closures. The definitive retained-change
control is the before/after all-object comparison, not rounded fuzzy scores.

H69's two calendar sites were tested as an independent 2x2 structural product.
Both explicit u8-store arms retain the 1,277-byte DoNewTurn function; shared
calls produce 1,282/1,284/1,289 bytes. The filename filter's two-arm comparison
retains its 428-byte explicit fold instead of the 433-byte call form. All six
trials completed, source restored. These paths still share code-point constants;
their filtering, source-dot mutation and calendar message protocols are not
hidden by an abstraction.

A VC6-compiled executable including the real KB.h tests all 256 uppercase and
256 lowercase input bytes and passes, including Yo, unsigned interpretation and
unchanged nonletters. The object control now also permits the numeric counter
suffix on VC6 `$sourceLabel$counter` LABEL symbols while preserving the label
name and exact destination; a regression rejects a changed label name.

All four group-2 families are applied. The final build and original-snapshot
object control pass after removal of the remaining town-color private helper:
98 objects, 1,516 sections, 42,433 ordered relocations. The full tool suite now
passes 905 tests. [The measured source dossier](../matching/ReadabilityAccessors/reuse.cpp)
records the structural arms and narrow rejected paths. Reproduce the exhaustive
real-header byte test with `python3 -m homm2.audit.readability_contracts` inside
the build shell; compiler flags come from the KB unit's manifest entry.

## Group 3: narrow widget, resource and display operations

| Family | Application and boundary |
| --- | --- |
| H02 | Eleven widget-local hit tests in eight TUs use `WIDGET_CONTAINS_LOCAL_POINT`. Coordinates remain narrowed before the call; drop-button and popup rectangles stay explicit. The three-arm probe rejects the inline method (1,270 vs 1,240 bytes) and retains the byte-neutral expression macro. |
| H03 | All eight widget readers share `READ_WIDGET_GEOMETRY`: four ordered signed-word stores. The live resource pointer is reevaluated before each read; id/kind, lookups and payload reads remain outside. |
| H05 | Nine assignment/copy pairs in listbox, droplist, Overview and TOWNMGR use `ALLOC_COPY_STRING`. Existing frees and ownership transfer stay explicit. FONT's local declaration/initialization pair stays readable as two statements; no second declaration-generating macro is introduced. Its non-copy scratch allocation, extra-capacity text buffers and ADVMGR's format-string copy are excluded. |
| H11 | Eight unsheared decoders share `ICON_FITS_CLIP`. It retains left/right/top/bottom comparison order and already-resolved flipped coordinates; clip-mode assignment, inclusive edges and decoder loops remain explicit. |
| H13 | Ten input/source-coordinate expressions in three TUs use `CLIENT_TO_GAME_X/Y`. Signed conversion, original long/int arithmetic, live dimensions and destination narrowing remain. Width/height scaling and inclusive rectangle preparation stay explicit because the new names describe coordinates. |
| H15 | Eighty live lvalue selections in seven TUs use `CURRENT_GRAPHICS_CONFIG`. No reference is cached across callbacks; game/editor slot selection and saved scalar fields remain unchanged. |
| H19 | The existing one-value `LOG_SUMMARY_VALUE` is shared from Misc.h with statement-safe comma sequencing. Fifteen old calls plus fourteen new calls in SMACKMGR, Bzip and dpnetwin retain formatting into `gText` before logging. Other arities/buffers and subsequent shutdown/error operations remain explicit. |

[Seven complete source matrices](../matching/ReadabilityWidgets/shared-operations.cpp)
record the precise arms and audit results. H02/H03/H11 reproduce the tested retail
functions. H05/H13/H15/H19 preserve their baseline instructions and relocations
but retain existing function-local retail normalization residuals; rounded 100%
is not reported as closure. The final build and original-object control pass
after the five town/overview string pairs: all 98 objects, 1,516 non-debug
sections and 42,433 ordered relocations. The retail field-relocation review also
passes, with no new structural review items.

## Group 4: domain predicates and precise query boundaries

| Family | Application or measured retention |
| --- | --- |
| H20 | 138 building-mask queries use existing `HAS`, with the same raw `u32l` storage and explicit `IDX`/`BIT`/shifted masks. No normalized-return town API is added. Mutations, the raw captain-mask store and unclassified literal-mask tests stay explicit. |
| H21 | Retain the direct type-sentinel comparison. `HasTroop` changes HasAllUndead from 90 to 101 bytes; a macro merely wrapping one already-named comparison would add little clarity. No positive-quantity condition is introduced. |
| H24 | 56 pointer-based terrain queries use `CELL_TERRAIN`. It preserves the byte/table lvalue, exact supplied cell expression and one evaluation, including existing GetCell calls and their caller-specific fallback. Index-only and dot/row-view variants stay explicit. |
| H25 | Nine ordered sprite/index/flag conjunctions use `CELL_HAS_NON_SHADOW_OBJECT`. Existing 0x80 aliases share the map owner's shadow-only flag; no passability rule or tileset/frame shadow detection is implied. The repeated-GetCell negation remains explicit. |
| H28 | Retain direct combat-array access. The reference-returning GetArmy probe keeps 161 bytes but changes instructions/register use. A three-argument macro would not improve the existing owner/two-index spelling enough to justify another API. Physical, controlling and mixed side/index choices remain visible. |
| H29 | Fifteen `Stats(KNOWLEDGE) * 10` prefixes use `HERO_NORMAL_SPELL_POINTS`. Caller narrowing, further multiplication and each Stats call remain. The method alternative changes instructions; the macro preserves them. |
| H33 | Ten exact earth/air/fire/water memberships, including ordered negations, use `IS_ELEMENTAL_CREATURE`. Different comparison orders, the three-elemental frame case and surrounding summon/mirror/undead policies remain explicit. |
| H47 | Eleven iron/steel memberships use `IS_GOLEM_CREATURE`. The inline alternative adds 53 bytes in its probe; the macro is unchanged. Runtime/AI spell coverage, double half-damage arithmetic and destination casts remain outside. |
| H48 | Two green/red/black/bone memberships use `IS_DRAGON_CREATURE`. Three-living-dragon immunity is not changed. |
| H49 | Nine berserk/hypnotize pairs use `ARMY_HAS_BERSERK_OR_HYPNOTIZE`, including the independently measured ordered zero conjunctions. Reversed order and other ownership/action policies remain explicit. |
| H50 | Seven blind/paralyze/petrified triples use `ARMY_HAS_INCAPACITATING_SPELL`, including the independently measured ordered zero conjunctions. Other orders, death, quantity and the separate mind-influence pair are not folded in. |
| H55 | Fourteen exact lich, vampire and troll base/upgrade memberships use three separate named predicates. Vampire-lord healing remains a single-type test. |
| H76 | Five positive-stack queries in Overview, TOWNMGR and SWAPMGR use `ARMY_GROUP_HAS_POSITIVE_STACK`. Type-first evaluation, signed counts and the unused census remain. H21's type-only queries do not acquire this condition. |
| H78 | Sixteen select/alternate-select command pairs use `IS_WIDGET_SELECTION_COMMAND`. Message type, right-button guards, routing and dialog behavior remain in each caller; Newgame's equivalent command aliases are included. |

[Twenty complete structural matrices](../matching/ReadabilityDomains/shared-operations.cpp)
preserve the attempted source and individual exact/non-exact audit results.
No rejected method or compiler-state probe remains in game headers. Redundant
grouping around primary-expression calls was removed after the measured grouping
pair; control-statement and enclosing-call parentheses are retained.

The final build, original-object control and retail field-relocation review pass:
98 objects, 1,516 unchanged non-debug sections and 42,433 ordered relocation
records. Existing local retail normalization residuals remain distinct from the
passing before/after proof. The real-header VC6 integration check now also passes
all 256 signed-byte inputs for six creature sets and the selection predicate,
plus 32,768 sprite tileset/flag/sentinel combinations, alongside the 512 case folds.

## Group 5: ordered UI and combat operations

| Family | Application or measured retention |
| --- | --- |
| H32 | Ten `FINISH_DIALOG_MESSAGE` calls save the original widget id, then store the close id and dialog-select command. Handler returns, the timeout's preceding type write/following reset, SETUP's later cancel override and all other payload remain explicit. The town event handler has the same three-store contract. |
| H34 | Retain the six exact-order four-`if` clamps. The statement-safe `do/while(0)` macro increases Walk from 2,599 to 2,609 bytes. Its definition is removed; no unguarded multi-statement macro or reordered ternaries are substituted. |
| H35 | Sixteen `UPDATE_INCLUSIVE_REGION` calls name endpoint-to-size conversion at the window-manager boundary, with stable local/global bounds. Other drawing/fizzle APIs, clipping and caller flags are untouched. |
| H36 | Fifteen `COMBAT_DEADLINE` expressions retain the `i32` conversion after floating tick addition and the original delay-first product. Long-cast, reversed-product, division and double-factor variants remain explicit, as do timer slots and polling. |
| H38 | Seven `CREATURE_DISPLAY_NAME` calls preserve the lazy table selection and `count <= 1`, including SPELLS' equivalent `PLURAL_QUANTITY_MINIMUM - 1`. Separate-format branches, `== 1` variants and lowercased `gTargetName` remain explicit. |
| H40 | Eight `CLEAR_HEX_OCCUPANT` calls clear side then index only. Repeated cell expressions, front/rear selection and guards are retained; grid initialization, reversed stores and frame/corpse state remain explicit. |
| H53 | Thirteen `SET_NEXT_COMBAT_MOVE` calls set action before evaluating/storing the destination. Intervening ValidAttack work, previously selected destinations, action-extra stores and returns remain outside. |
| H57 | Seven `CLEAR_ARMY_TARGET` calls preserve side/index order. Attack direction, move/previous-target hexes, save/restore and index-first variants remain explicit. |

[Eight complete two-arm matrices](../matching/ReadabilityOrdered/shared-operations.cpp)
record the exact attempted source and results. H32/H35/H57 match their retail
probes; the other retained expressions preserve baseline instruction and
relocation hashes with the existing local retail residuals. H34's added code is
a measured rejection, not an untested claim that no possible helper could match.

The combined build and original-object control pass: all 98 objects, 1,516
non-debug sections and 42,433 ordered relocations are unchanged. Retail field
review passes for 1,727 functions and 38,307 sites with zero structural items.

## Group 6: combat, town, calendar and distance formulas

| Family | Application or measured retention |
| --- | --- |
| H43 | Two complete range/column tests use `IS_INTERIOR_COMBAT_HEX`. Existing ValidHex calls, partial column tests and COMMAND's special/sentinel hex rules stay unchanged. |
| H44 | Eight local-delta expressions use `INTEGER_VECTOR_LENGTH`: signed integer squares/sum, then double sqrt, then i32 truncation. Abs calls and step rounding stay outside. The map-radius repeated-subtraction variant remains explicit. |
| H56 | Eight `HEX_HAS_OCCUPANT` queries compare side before index, with no requested-index narrowing. Empty-cell alternatives, cell lookup, validity and blocked-state tests remain in the callers. |
| H58 | Two positive PATH and three negative COMMAND gate exceptions use `CAN_PASS_CASTLE_GATE`. Both forms are independently measured. The seven ordered conditions still read gpCombatManager even when the caller reads candidate cells through a different receiver; no passability or TestRaiseDoor policy is added. |
| H63 | Both scouting-plus-telescope expressions use `HERO_SCOUTING_VISIBILITY_RADIUS`. Signed skill/table indexing, one artifact Boolean, distinct origins and visibility timing remain. Scouting-only GAME sites are excluded. |
| H72 | Four offered/displayed level expressions use `NEXT_MAGE_GUILD_LEVEL`, preserving saved locals and upper-only signed capping. The value inline adds five bytes to SetupCastle and is removed. KB's uncapped actual cost subscript remains unchanged. |
| H73 | Both completed-building queries use `TOWN_BUILDING_COMPLETE`: mask first, then non-guild or level exactly five. BIT and Castle's signed-long shift are separately measured; both remain 32-bit mask operations on the existing u32l owner. No affordability or raw captain-mask query is changed. |
| H87 | Five day-first queries use `GAME_DAY_NUMBER`, with packed u16 promotion to signed i32 and no caching or normalization. PerDay still queries before rollover; month-first campaign arithmetic and its i16-return variant stay explicit. |
| H92 | Four tent-mask tests use `PLAYER_HAS_VISITED_TENT`, retaining the signed byte promotion and raw mask result. Color decoding and the password-before-flag order remain outside. |
| H93 | Retain the explicit i16 store followed by the named count shift. `RecruitSiteCount(i16)` changes RecruitSiteEvent from 368 to 362 bytes and changes instructions. A second macro merely naming the already-named shift would not justify another API. The rejected inline is removed. |
| H96 | Twenty-four exact two-delta sums use `MANHATTAN_LENGTH`. Actual deltas, including ResetHeroRVs' two X-coordinate reads, remain unchanged; outer absolute values, shifts, thresholds and tie-breaking are untouched. |

[Thirteen complete structural matrices](../matching/ReadabilityFormulas/shared-operations.cpp)
include both gate forms, both building-mask spellings and the three-arm guild-level
probe. All retained arms preserve their baseline instruction and relocation hashes.
H56/H58/H63 also pass the target-local exact audit; other baseline retail residuals
remain explicitly recorded in the dossier.

The combined build, all-98-object comparison and retail field-relocation review
pass with the same section/relocation totals. FLY now explicitly includes the
math-helper owner header. The real-header VC6 executable additionally passes
2,048 signed tent-mask/color combinations, 2,001 hex/guild-level inputs and
1,089 signed delta pairs against independent bounded integer expectations.

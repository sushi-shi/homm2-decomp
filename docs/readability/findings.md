# Common-helper findings and implementation order

This is a reading-audit result, not an executable refactor or a reconstruction of
the original macro names. The detailed contracts and exclusions live in
[the candidate ledger](candidates.md). Stable H numbers identify finding families,
not necessarily independent helpers: some have alternatives or ordered variants.

All 99 finding families have received the final ranking review. This index keeps
lower-priority observations visible without presenting them as a to-do list of 99
required abstractions.

There are 12 first-experiment families, 58 useful narrow candidates and 29
lower-priority findings. Start with:

- H01: consolidate the identical `SET_WIDGET_MESSAGE` definitions.
- H17/H30/H42/H68/H82/H85/H94: shorten existing APIs with optional-argument
  defaults, keeping meaningful modes and policies explicit.
- H22/H27/H52/H69: reuse existing accessors, array spelling and CP1251 vocabulary.

## How to use the ranking

- **A — first experiments:** large, clear readability gain, usually through an
  existing API or an already duplicated macro. This is not byte-equivalence proof.
- **B — useful narrow names:** credible cross-TU contracts with a natural owner;
  implement only after the proposed name and its generated code earn their keep.
- **C — retain as evidence, not the initial refactor:** lower gain, long argument
  lists, awkward ownership, or materially different source-shape variants. Leaving
  these explicit is a valid outcome; the presence of repetition is not a mandate.

The TU examples below are a confirmed cross-TU witness set, not an exhaustive
replacement list. Names are source basenames, with their exact physical paths and
function boundaries available in [functions.tsv](functions.tsv). Owner paths are
relative to `include/`; a proposed new boundary is explicitly described as such.
The ledger supplies named functions and additional consumers. All proposals retain
caller guards, conversions, evaluation frequency and mutation order unless a
separate measured experiment establishes an acceptable source shape.

## Ranked index

| ID | Rank | Form / natural owner | Confirmed TU examples | Decision and essential boundary |
| --- | --- | --- | --- | --- |
| H01 | A | Existing `SET_WIDGET_MESSAGE` macro; `BASE/message.h` | BUTTON, TEXTWDGT, BORDER, Textntry | Consolidate the two identical definitions first. Write type, command, id only, in that order; preserve the rest of the union. Other store orders are not replacements. |
| H02 | B | Widget predicate; `BASE/widget.h` | BUTTON, TEXTWDGT, ICONWDGT, WIDGET | Name the widget's half-open local extent. Keep screen-to-local narrowing outside; drop-button and popup rectangles are different inputs. |
| H03 | B | Four-read statement macro; widget/resource boundary in `BASE/widget.h` | BUTTON, TEXTWDGT, ICONWDGT, BORDER, DIMMER, Textntry, listbox, droplist | Name the serialized x/y/width/height prefix. Keep four ordered `ReadWord` calls and signed destination stores; do not absorb id, kind or payload reads. |
| H04 | C | Narrow position-preserving lookup macro; `BASE/resourceManager.h` boundary | TEXTWDGT, BORDER, Textntry, listbox, droplist | Direct save / one lookup-assignment / restore is a credible subset. BUTTON/ICONWDGT's stored `MakeId` plus lookup and MOUSEMGR's larger protected regions are not that three-statement contract. No general scope guard. |
| H05 | B | Allocate-copy statement macro; `BASE/Misc.h` | listbox, droplist, FONT | Exact `H2_ALLOC(strlen(source)+1)` then `strcpy`. No implicit free, extra capacity, allocator substitution or source-lifetime change. |
| H06 | C | Reuse existing `GetIconEntry`; `BASE/Misc.h` | listbox, droplist, ICON, ARMY, ADVMGR | Prefer the existing vocabulary over another accessor, but it is currently out-of-line. Do not trade raw addressing for a new call without matching evidence or normalize invalid indices. |
| H07 | C | Ordered scalar-clamp macro; `BASE/Misc.h` | listbox, droplist, soundmgr, Misc, ADVMGR | Lower-then-upper is one contract; upper-then-lower is another. Preserve already-narrowed stores and negative ranges. Do not merge interleaved two-axis clamps. |
| H08 | C | List-layout expressions; proposed shared BASE list-layout boundary | listbox, droplist | Row selection and thumb-to-index arithmetic are separate names. Keep strict first-row boundary and signed multiply/divide order; neither supplies a zero-travel guard. Many parameters limit the gain. |
| H09 | C | Axis-clipping statement idiom; BASE graphics boundary, e.g. `BASE/Misc.h` | WINMGR, Ripple | Negative-origin adjustment then far-edge shrink. Two whole-axis calls would reorder Fizzle's interleaved stores; do not create a universal rectangle clipper. |
| H10 | C | Pixel/row address expression; `BASE/bitmap.h` | BITMAP, WINMGR, Ripple, icon2bs | Name address formation only for a proven base/pitch contract. Fixed screen pitches and precomputed decoder state cannot silently become bitmap width. |
| H11 | B | Icon-fit predicate; `BASE/IconRle.h` | Icon2b, Iconf2b, Icond2b, Icondf2b, Iconm2b, Iconmf2b, icon2bc, iconf2bc | Share the four half-open fast-path comparisons; keep precomputed flipped left edge and clip-mode assignment outside. Shear paths are excluded. |
| H12 | C | Existing cursor inline plus two distinct palette expressions; `BASE/IconRle.h` / `SOURCE/dimPalette.h` | Icon2b, Iconf2b, Icond2b, Icondf2b, bmap2 | Do not count these as one universal RLE helper. Existing `ReadIconRleByte` reverses load/increment source order; encoded-command palette row and plain level/pixel lookup have different inputs. |
| H13 | B | Client-to-game coordinate expressions; input/display boundary in `BASE/inputManager.h` | INPUTMGR, MOUSEMGR, wingraph | Explain the 640/480 logical resolution; preserve signed input conversions, multiply-before-divide and live dimensions. No clamp or new zero-dimension policy. |
| H14 | C | Base-manager field initializer; `BASE/baseManager.h` | WINMGR, soundmgr, INPUTMGR, MOUSEMGR, CMBTMGR, ADVMGR | Mask, priority, active stores only. A name must not imply list registration, resource initialization or name copying, none of which the three stores perform. |
| H15 | B | Lvalue macro or reference inline; configuration boundary in `SOURCE/KB.h` | INPUTMGR, MOUSEMGR, Misc, kbwin, wingraph, Wsnetwin | Name current-executable graphics preferences. Select the live slot on each use; do not cache across callbacks or copy the whole record. |
| H16 | C | Inclusive disjointness predicate; BASE graphics boundary | MOUSEMGR, Misc | Edge contact overlaps. Comparison order differs; passing eight edges may save little. Keep visibility short circuits and saved/raw rectangle choices explicit. |
| H17 | A | Defaults on existing `LogInt`; `BASE/Misc.h` | kbwin, Misc, dpnetwin, Wsnetwin, PHILAI | Keep label and first value required; default only trailing `LOG_UNUSED_VALUE` slots. Update the strict enum bridge consistently. Numeric zero is not the sentinel; no variadic logging framework. |
| H18 | C | Palette-component expression macro; `BASE/palette.h` | Misc, wingraph | Preserve `component << 2` with each caller's signed/unsigned promotion and byte store. A scalar name is possible; a whole palette-conversion loop is not shared. |
| H19 | B | Consolidate existing one-value `LOG_SUMMARY_VALUE` idiom; `BASE/Misc.h` | SMACKMGR, RESMGR, Misc, Bzip, wingraph | Format into `gText`, then log, even when logging is disabled. Start with the exact existing fixed arity; other arities are alternatives, not a new formatter or buffer owner. Distinct from H17. |
| H20 | B | Existing `HAS` vocabulary or town query; `SOURCE/town.h` | ARMYGRP, TOWN, CMBTMGR, DRAWING, ADVMGR | Any-bit building-mask query only. Preserve `u32l`, mask versus slot-index distinction, faction guards and numeric masked-result consumers. Choose one spelling, not competing APIs. |
| H21 | B | Type-present slot predicate; `SOURCE/armyGroup.h` | ARMYGRP, TOWN, HERO, CMBTMGR, ADVMGR | `type != CREATURE_NONE` only. H76 adds a positive quantity and is deliberately different; neither means combat `army::IsAlive`. |
| H22 | A | Reuse existing `hero::IsEmbarked`; `SOURCE/hero.h` | HERO, ARMYGRP, CURSOR, ADVMGR | Retain its masked integer result, existing null guards and query timing. Boat-object/id tests and mutations are not instances. |
| H23 | C | Signed-id compaction; `SOURCE/playerData.h` | TOWN, HERO | Shift after the already-found last matching position and store -1 in the old tail. Do not decrement count, reset selection, use first-match search or repair missing ids. |
| H24 | B | Cell-terrain expression; map/terrain boundary in `SOURCE/KB.h` | SEARCH, FINDPATH, CMBTMGR, CURSOR, ADVMGR, PHILAI | Table lookup from an already-resolved cell, preserving byte result and later conversions. No coordinate lookup, fallback policy or caching inside. |
| H25 | B | Non-shadow object-sprite predicate; `EDITOR/mapcell.h` | FINDPATH, SEARCH, CURSOR | Ordered index / tileset / flag test; name the visual property, not passability. ADVMGR's repeated `GetCell` negation is a variant, not an automatic single-pointer call. |
| H26 | C | Bounds predicate variants; `SOURCE/KB.h` map boundary | SEARCH, FINDPATH, CMBTMGR, CURSOR, ADVMGR | Preserve runtime dimensions, axis/comparison order and `< size` versus `> size-1` spelling. No bounds helper may repair unrelated width-for-height loops or move earlier accesses under a guard. |
| H27 | A | Reuse existing `searchArray::GetNode`; `SOURCE/searchArray.h` | SEARCH, FINDPATH, PHILAI | Keep global stride, real caller coordinates, reference semantics and searchNode type. Explicit-width rows and overlaid searchCell views are different contracts. |
| H28 | B | Side/index army accessor; `SOURCE/combatManager.h` | HEXCELL, FINDPATH, ARMY, DRAWING, SPELLS, AI, COMMAND | Return the existing army object with no checks. Keep the supplied side/index pair; physical side, controlling side and mixed current pairs are not interchangeable. |
| H29 | B | Normal spell-point capacity query; `SOURCE/hero.h` | HERO, CMBTMGR, ADVMGR, PHILAI | Retain `Stats(KNOWLEDGE) * 10`, query count and caller narrowing. Capacity is not a clamp on current spell points or a spellbook check. |
| H30 | A | Defaults on existing `NormalDialog`; `SOURCE/KB.h` | HERO, CMBTMGR, SPELLS, COMMAND, ADVMGR, REMOTE | Keep text and mode required; default the exact no-resource/default-position tail. Formatting, mode choice, result handling and callbacks stay visible; do not add a second wrapper by default. |
| H31 | C | Stored-pointer check macro; `SOURCE/KB.h` | HERO, CMBTMGR, SPELLS, COMMAND, ADVMGR | Only null-test then `MemError`. Low gain relative to an explicit `if`; no allocation, return, throw or nonreturning assumption, and no new checks at unchecked sites. |
| H32 | B | Dialog-completion statement macro; window-manager/message boundary | HERO, CMBTMGR, COMMAND, ADVMGR | Save incoming widget id as dialog result, then set id 10 and command 10. Do not set type, clear payload, interpret the result or hide the handler return. |
| H33 | B | Four-elemental predicate; `SOURCE/KB_TYPES.h` | CMBTMGR, ARMY, COMMAND, ADVMGR, EVENTS, PHILAI | Earth/air/fire/water membership only, with ordered variants recorded. Three-elemental frame cases, summoned flags and ghost exclusions are separate. |
| H34 | B | Fixed combat-extent clamp macro; `SOURCE/X_GLOBAL.h` extent boundary | ARMY, CMBTMGR, DRAWING, FLY | Preserve min-X/min-Y/max-X/max-Y conditional stores to 0/0/639/442. Other axis orders or 479 limits are not instances; keep extent expansion and screen work outside. |
| H35 | B | Inclusive-region short-call macro; `BASE/heroWindowManager.h` | ARMY, CMBTMGR, DRAWING, FLY, SPELLS | Convert endpoints to `right-left+1`, `bottom-top+1` for the existing UpdateScreenRegion call. No clipping or replacement with UpdateCombatArea; fizzle/blit APIs remain distinct. |
| H36 | B | Combat-deadline expression; `SOURCE/KB.h` timer boundary | ARMY, CMBTMGR, SPELLS, COMMAND | Tick plus float-scaled delay, then one final integer cast. Preserve float/double, multiplication/division order and each clock read; reversed products and i32l results are measured variants. |
| H37 | C | Projectile-frame scan inline; `SOURCE/army.h` | ARMY, CMBTMGR | Ordered strict midpoint crossing and last-frame fallback. Keep angle calculation, count domain and shooting-animation choice outside; do not invent a zero-count repair. |
| H38 | B | Creature-name expression; `SOURCE/KB.h` | ARMY, CMBTMGR, SPELLS | Exact two-table `count <= 1` choice. Preserve the caller's actual count and lazy table selection; `== 1`, case folding and localized format branches are separate. |
| H39 | C | Forward existing QuickDistance with combat-cell coordinates; combat/search boundary | FINDPATH, ARMY, AI | Keep max-plus-half-min pixel metric, supplied cell indices and read order. Lower gain than the existing metric's name; not hex distance, H44 or H96. |
| H40 | B | Hex occupant clear; `SOURCE/hexcell.h` | CMBTMGR, ARMY, SPELLS | Side NONE then index -1 only. Frame, corpses, path state and index-first variants stay explicit; distinct owner from H57. |
| H41 | C | Ordered extent-growth macro; BASE graphics extent boundary | ICON, ARMY, CMBTMGR | Left/top/right/bottom conditional growth. Eight operands and several store-order variants make indirection questionable; no normalized rectangle object or clipping. |
| H42 | A | Defaults on existing CombatClipDrawToBuffer; `BASE/icon.h` | ARMY, CMBTMGR, DRAWING, SPELLS | Default offset 0, color table NULL, Y modifier NULL only. Keep limits, orientation and tested return value explicit; no new draw wrapper. |
| H43 | B | Interior combat-hex predicate; `SOURCE/CMBTMGR.h` beside ValidHex | FLY, DRAWING, SPELLS | Range 0..116 excluding columns 0 and 12. Preserve partial-check consumers and existing ValidHex calls until measured; COMMAND's special negative/width-based domain is excluded. |
| H44 | B | Integer vector-length expression; BASE geometry boundary in `BASE/Misc.h` | ARMY, FLY, CMBTMGR, SPELLS | Integer squares/sum, then double sqrt, then i32 truncation. Map-coordinate/repeated-subtraction and reversed-square variants remain explicit; no hypot or overflow-policy change. |
| H45 | B | Conditional combat-effect cache selection; combat-effect boundary in `SOURCE/X_GLOBAL.h` | ARMY, SPELLS | Compare effect, dispose old icon, acquire/store new pointer, store id. Keep already-computed file id and visibility guards outside; filename-overload preparation is a variant. |
| H46 | B | Ordered duration-artifact additions; hero/spell boundary in `SOURCE/hero.h` | SPELLS, SPELLAI | Hourglass +2 then wizard hat +10 on existing power. No stat recomputation, null guard or removal of unused-result calls; wider duration gates have different order. |
| H47 | B | Golem membership predicate; `SOURCE/KB_TYPES.h` | SPELLS, SPELLAI | Iron then steel. Prefer the type query to a damage wrapper; double half-damage casts and runtime/AI spell coverage differ. |
| H48 | B | Dragon membership predicate; `SOURCE/KB_TYPES.h` | ARMY, SPELLAI | Green/red/black/bone in that order. Spell-immunity tests omit bone and must not reuse this set. |
| H49 | B | Exact two-influence army query; `SOURCE/army.h` | ARMY, SPELLAI, AI | Berserk then hypnotize nonzero durations. Reversed/negated forms need separate review; do not name it general action eligibility or controlling side. |
| H50 | B | Exact three-influence army query; `SOURCE/army.h` | SPELLAI, CMBTMGR, AI | Blind/paralyze/petrified in that order. H49's pair is separate; alternate orders, death and quantity conditions are not folded in. |
| H51 | C | Defaults on existing CombatMessage text overload; `SOURCE/combatManager.h` | ARMY, CMBTMGR, DRAWING, SPELLS, COMMAND | Technically credible, but omitted history/update policy can harm readability. Keep history calls explicit and preserve command-overload resolution; lower priority than unambiguous sentinel tails. |
| H52 | A | Existing 2D array spelling; `SOURCE/X_GLOBAL.h` | SPELLS, ARMY, SPELLAI | Use established `gArmyEffected[side][index]` vocabulary, not a new helper. Keep the 20-slot stride, writable lvalue and surrounding chance/random-call order. |
| H53 | B | Next-move statement macro; `SOURCE/X_GLOBAL.h` action boundary | AI, ARMY, COMMAND | Set ACTION_MOVE, then evaluate/store target hex. No extra fields, validation or hidden control flow; intervening-call and grid-first paths are excluded. |
| H54 | C | Existing Wince reuse or generalized army animation start; `SOURCE/army.h` | ARMY, FLY, SPELLS, COMMAND | Sequence then frame zero only. Wince is out-of-line; do not add both redundant APIs or absorb sound, pending animation, timing and offsets. |
| H55 | B | Three separate creature-pair predicates; `SOURCE/KB_TYPES.h` | AI/ARMY (lich), ARMY/FLY/PHILAI (vampire), ARMY/COMMAND (troll) | Name exact base/upgrade memberships, not inferred ranges or ability classes. Single-type vampire-lord healing remains single-type. |
| H56 | B | Hex occupant-identity query; `SOURCE/hexcell.h` | COMMAND, ARMY, FLY | Compare side then index with existing signed-byte promotions. No lookup/check/live-army policy; eager index evaluation is not assumed equivalent to short-circuit source. |
| H57 | B | Army target clear; `SOURCE/army.h` | ARMY, DRAWING, COMMAND | Target side NONE then index -1 only. Preserve other target/direction fields and index-first variants; not H40 occupancy or H53 next action. |
| H58 | B | Castle-gate exception predicate; `SOURCE/combatManager.h` | PATH, COMMAND | Name the exact castle/hex/drawbridge/controlling-side/live/dead condition. Keep blocked and candidate-occupancy checks outside; global manager and caller receiver need not alias. |
| H59 | C | Adventure draw/update pair; `SOURCE/advManager.h` | CURSOR, ADVMGR, EVENTS | CompleteDraw(0) then UpdateScreen(0,0), preserving both calls and live receivers. Already meaningful names limit gain; this is not a forced blit, radar refresh or H79 campaign presentation. |
| H60 | B | Normal/flipped adventure-icon short-call macros; `SOURCE/ADVMGR.h` | CURSOR, ADVMGR | Fixed current screen and 480-square viewport; keep clip mode explicit and normal/flipped names separate. No BASE defaults that impose the adventure viewport on other callers. |
| H61 | B | Flipped hero-shadow remap; `SOURCE/CURSOR.h` | CURSOR, ADVMGR | Preserve the five independent conditional frame writes. The later frame-offset expression is a separate possible name; no table/switch rewrite or boat-shadow reuse. |
| H62 | C | Guarded explicit-pointer detach/delete macro; `BASE/heroWindow.h` | ADVMGR, COMMAND | RemoveWidget then scalar delete under a nonnull test; nulling stays outside. x_arena's unguarded prefix and Overview's guarded prefix with an internal null store are variants, not identical whole blocks. |
| H63 | B | Scouting-plus-telescope radius; `SOURCE/hero.h` with existing table dependency | CURSOR, ADVMGR | Signed skill/table indexing plus one Boolean artifact bonus. GAME's scouting-only uses are excluded; keep coordinates, player, visibility timing and map-origin choice outside. |
| H64 | B | Four distinct shared receive-storage primitives; `SOURCE/dpnetwin.h` | dpnetwin, Wsnetwin | Initialization, enqueue, dequeue and array disposal remain separate. Same 200-slot globals, not a new ring layout; preserve stored-size/copy-size discrepancy and each backend's drain policy. |
| H65 | B | One-byte tagged message construction; `SOURCE/REMOTE_TYPES.h` transport boundary | dpnetwin, Wsnetwin | Allocate size+1, store tag, conditionally copy payload. Keep u16 promotion, unchecked allocation, send/free/error paths and outer wire layer explicit. |
| H66 | B | Drain existing node queue; `SOURCE/comwin.h` | comwin, netwin | Repeated pop-until-null and one H2_FREE per returned node. Preserve final result/anchor behavior; no direct-link rewrite, implicit lock or shutdown. |
| H67 | C | Ordered two-queue pop; `SOURCE/comwin.h` | comwin, netwin | Pop first, then second only if null. Three clear statements may beat another name; no eager double pop, inferred priority or lock/lifetime changes. |
| H68 | A | Defaults on existing TransmitRemoteData; `SOURCE/REMOTE.h` | REMOTE, CURSOR, COMMAND, GAME, Newgame, EVENTS | Default allowRetryDialog=1 and message type DEFAULT. Keep reliable required and explicit; preserve message selection, return handling and narrow argument conversions. |
| H69 | A | Existing CyrillicToUpper reuse; companion lowercase inline in `SOURCE/KB.h` | GAME, ARMY, TOWNMGR, RECRUIT, tradpost, EVENTS | One exact CP1251 byte mapping, including Yo. Preserve u8 input interpretation and result-store conversions; KB's callable char overload and CRT int overload require distinct resolution/ABI review. |
| H70 | B | Exact-size read/write expression macros; `BASE/Misc.h` | GAME, EDITOR/mapcell, RESMGR, HISCORE, KB | Existing CRT call with address and sizeof the same scalar/plain-record lvalue, retaining the result. Exclude array/prefix/wider-global slices and KB's oversized entry reads; no serialization/error framework. |
| H71 | B | Existing armyGroup::Dismiss reuse/inline experiment; `SOURCE/armyGroup.h` | ARMYGRP, GAME, TOWNMGR, SWAPMGR, Campaign, PHILAI | Type NONE then count zero for one slot, with no added guard. Preserve the out-of-line ABI and prove expansion; not H90's array-major clear or count-first/type-only removal. |
| H72 | B | Next mage-guild display level; `SOURCE/town.h` | TOWNMGR, Castle | Signed level +1 capped only above at five. KB's cost lookup uses an uncapped subscript despite computing a capped local; it is excluded. |
| H73 | B | Completed-building query; `SOURCE/town.h` | TOWNMGR, Castle | Building bit first, then non-guild or level exactly five. Slot index input, not H20's mask; no affordability/prerequisite/faction/maximum-level rejection policy. |
| H74 | B | Unconditional six-button broadcast primitive; `SOURCE/advManager.h` | SWAPMGR, ADVMGR | Reuse one message and ordered ids 1..6. Existing DisableButtons/EnableButtons have an active-manager guard absent in SWAPMGR; keep those guards in their callers. |
| H75 | C | Disable-then-defer-dim protocol; `BASE/heroWindow.h` | VIEW, TOWNMGR, ADVMGR | Two broadcasts with exact special dim argument 0x1000. Message-store shapes differ across callers; do not imply text-identical extraction or combine with H77 automatically. |
| H76 | B | Positive signed stack query; `SOURCE/armyGroup.h` | Overview, TOWNMGR, SWAPMGR | Type not NONE first, then signed i16 quantity >0. Distinct from H21; no unsigned count alias, validity repair or removal of an unused census. |
| H77 | C | Defer-dim-then-disable protocol; `BASE/heroWindow.h` | ADVMGR, Newgame | Exact reverse protocol to H75, preserving id reuse and final message state. Treat as an alternative ordered extraction, not a second public API required merely for symmetry. |
| H78 | B | Widget command-pair predicate; `BASE/message.h` | SETUP, ADVMGR, VIEW, Newgame | SELECT then ALTERNATE_SELECT only. No message-type/right-button guard or event-dispatch rewrite; keep the expression at its original evaluation point. |
| H79 | B | Campaign restart presentation sequence; `SOURCE/Campaign.h` | Campaign, X_CAMPGN | Six ordered post-initialization operations, including fade and full adventure setup. Distinct from H59; no outer display guard or generic fade transaction. |
| H80 | B | Campaign bonus-widget protocol; `SOURCE/Campaign.h` | Campaign, X_CAMPGN | Availability chooses frame; selected state is evaluated after the first broadcast and chooses flags independently. Keep message reuse; eager Boolean arguments would change evaluation timing. |
| H81 | B | Add experience then CheckLevel; `SOURCE/hero.h` | Campaign, X_CAMPGN, GAME | Preserve signed addition and existing cached level before one CheckLevel call. EVENTS' GiveExperience resets that cache first and is not a substitute; setup/dialog guards remain outside. |
| H82 | A | Default final GiveArtifact extra; `SOURCE/EVENTS.h` | Campaign, X_CAMPGN, CURSOR, ADVMGR, EVENTS, PHILAI | Default i8 extra=-1, keep checkEndGame explicit and return value intact. Spell scrolls and special metadata grants retain actual extras; no grant workflow wrapper. |
| H83 | B | Event resource add/floor; `SOURCE/playerData.h` | CURSOR, GAME, EVENTS | Add original signed amount, then floor the stored balance at zero. Leave display-amount caps and applicability outside; GiveResource has different validation/end-game behavior. |
| H84 | C | Last-two-rewards bookkeeping; `SOURCE/EVENTS.h` | CURSOR, GAME, EVENTS | Copy old primary to secondary only if present, then store new primary. Six operands limit gain; no generic reward record, sorting or resource-change policy. |
| H85 | A | Default final WaitEndSample interval; `SOURCE/KB.h` | CMBTMGR, SPELLS, COMMAND, EVENTS, TOWNMGR, HERO, CURSOR, ARMY, ADVMGR | All 23 calls pass -1. Keep the two-argument API and sentinel conversion inside; negative means the callee's finite default, not wait forever. |
| H86 | B | Ordinary/base-campaign cheat marker macro; `SOURCE/KB.h` | KB, ADVMGR | Mark ordinary cheating first, then conditionally mark base campaign. Do not include expansion state or ordinary-only forced loss; actual cheat effects remain explicit. |
| H87 | B | Current-map day query; `SOURCE/game.h` | GAME, KB, PHILAI | Day-first signed i32 expression from packed u16 fields, at each original query point. Month-first and campaign accumulated/i16-return forms remain variants, not automatic replacements. |
| H88 | C | Guarded scalar delete then unconditional null; `BASE/Misc.h` | KB, EDITOR/mapcell, BITMAP, MOUSEMGR, WINMGR, TOWNMGR | Typed pointer lvalue with no capture, early nulling or deletion-form repair. Distinct from H62, Dispose and H2_FREE; explicit spelling may better expose ownership. |
| H89 | B | Scaled seven-resource cost deduction; `SOURCE/playerData.h` | PHILAI, EVENTS | Ascending signed multiply/subtract loop with nonaliasing cost array and dead induction variable. No affordability, balance clamp or transaction; RECRUIT's two-resource charge differs. |
| H90 | B | Full army-array clear; `SOURCE/armyGroup.h` | ARMYGRP, EVENTS, PHILAI | Type-array memset(-1,5) then count-array memset(0,10). Preserve both calls; not a whole-object zero, per-slot dismissal loop, temporary construction or stack distributor. |
| H91 | B | Signed creature-count sum; `SOURCE/armyGroup.h` | CMBTMGR, PHILAI | Five ordered type-present slots, signed i16 counts into i32. Negative counts contribute; GetNumArmies counts slots. Interleaved two-army sums are only a variant. |
| H92 | B | Visited-tent-color query; `SOURCE/playerData.h` | EVENTS, PHILAI | Signed byte flags and integer shifted-bit test. Keep existing color mask and password-before-flag order outside; not a universal visited-site or flag-grant helper. |
| H93 | B | Recruitment count decode; `SOURCE/EVENTS.h` | EVENTS, PHILAI | Narrow input to signed i16, shift by three, retain i16 result. Keep the caller's aliased field spelling and separate kind mapping/packing; invalid-kind defaults differ. |
| H94 | A | Defaults on existing FightValueOfStack; `SOURCE/philAI.h` | PHILAI, EVENTS, ADVMGR, TOWNMGR, AI | Default only final town/siege modifiers to zero. Require group, hero and raw/modified mode; do not equate explicit town id -1 with zero. |
| H95 | C | Hero/map-monster strength ratio inline; `SOURCE/philAI.h` | ADVMGR, EVENTS | Signed denominator product, double division, float store. Reassess after H94 removes most call noise; PHILAI's float division is not an instance. |
| H96 | B | Signed Manhattan-length expression; `BASE/Misc.h` | ADVMGR, EVENTS, PHILAI, SPELLS, GAME | Exact `abs(dx)+abs(dy)` with caller deltas preserved, including apparent coordinate mistakes. No metric substitution, reassociation of radius formulas or new INT_MIN policy. |
| H97 | C | Quick-view-to-dialog-mode expression; `SOURCE/KB.h` | HERO, COMMAND, Overview | Zero selects INFO, any nonzero selects QUICK_VIEW. The named ternary is already clear; keep flag derivation and meaningful mode choice explicit after H30. |
| H98 | C | Rounded animation-step expression; `SOURCE/ARMY.h` | ARMY, FLY | `(distance + (spacing >> 1)) / spacing`, signed i32. Keep flight's positive-speed guard and minimum-one correction outside; no generic rounding/zero-divisor policy. |
| H99 | C | Stateful changed-hover-id query; `BASE/heroWindowManager.h` | VIEW, SPELLS, GAME | Return false if unchanged, otherwise store id and return true; leave the caller's consume return explicit. No ConvertToHover, callback, content dispatch or two-field town-cache update inside. |

## Consolidation decisions

Start with one coherent small experiment, not the entire index. H01 is the most
direct macro consolidation. The A-ranked declaration changes (H17, H30, H42,
H68, H82, H85, H94) remove mechanical arguments without adding callable layers;
keep meaningful modes and policies explicit. H22/H27/H52 reuse current vocabulary,
and H69 consolidates duplicated localized-byte rules with an existing owner.
All still require canonical matching verification before retention.

For new names, prioritize packed-data/domain contracts that remove interpretation
work: H24/H25, H33/H47/H48/H55, H61, H72/H73, H83/H87 and H89-H93. Fixed UI
protocols H03/H32/H34/H35/H60 and the small transport operations H64-H66 are
separate, credible experiments. Do not combine these into large refactoring batches.

These relationships prevent duplicate or overbroad proposals:

- H17 and H19 are different logging protocols. Start with existing defaults and
  the already-present one-value format/log macro; do not build a logging framework.
- H20 and H73 distinguish building-mask presence from a completed guild. H21,
  H76 and H91 distinguish a present slot, a positive stack and a creature total.
  H71 and H90 distinguish slot-major dismissal from array-major clearing.
- H33/H47/H48/H55 are explicit creature memberships, not inferred ability classes.
  H49/H50 are two separate influence sets, not a universal action predicate.
- H07/H09/H16/H26/H34/H41 retain different bounds, test orders and mutations.
  H39/H44/H96 retain three different distance metrics. No generic geometry rewrite.
- H12 contains three distinct low-level ideas and H64 four shared-storage
  operations; the family numbers do not imply one combined API. H54 should favor
  the existing Wince vocabulary before considering a generalized animation name.
- H62 never nulls internally. H88 does, after a different deletion protocol.
  H75/H77 are opposite message orders and are not two mandatory new APIs.
- H59 is a draw/update pair; H79 is campaign restart presentation. H80 is bonus
  display state, not either widget-dimming protocol or campaign initialization.
- H81 does not replace GiveExperience. H83 and H89 are different bank mutations.
  H93 shares count encoding, not the differing recruitment-kind switches.
- H95 overlaps H94's call-site benefit: measure the simpler default-argument
  change first, then decide whether the extra ratio name still helps.

Existing GetHero/GetTown/GetPlayerHero/GetPlayerTown accessors also cover private
slot aliases documented in R26/R43/R44; these need no competing index macros.
H97-H99 close earlier small leads without inflating the high-priority list.

The 46 R entries in the ledger preserve local-only leads and rejected larger
abstractions. They are useful negative evidence, not 46 more implementation tasks.
No candidate authorizes repairing observed source oddities, modernizing containers,
adding validation, changing wire layouts or normalizing resource lifetimes.

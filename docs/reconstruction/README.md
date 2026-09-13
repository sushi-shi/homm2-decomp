# King's Field reconstruction-pattern audit

Base: `decomp-gold-2.1-buka` at `ca2904a91`. Work branch:
`audit/buka-reconstruction-patterns`. This is an ongoing review, not a completion
claim. [Progress](progress.md), [every function](functions.tsv), and
[file coverage](files.tsv) distinguish inspected source from pending work.

The supplied checklist is the scope: casts, unions, gotos, artificial address
arithmetic, recovering owners from member pointers, out-of-object pointers,
manual varargs, unrelated variable reuse, stack aggregates/unused members, and
unresolved buffer bounds. Local King's Field references read for the method:
`docs/patterns/reconstruction-debt-review.md`,
`docs/patterns/effect-constructor-varargs.md`, and the cast/union review ledgers.
Their GCC/MIPS code-generation results do not establish VC6/x86 equivalence.

## How to read the ledger

Each physical project function in `src/` and `include/` gets a row, including
unannotated helpers, header bodies, conditional definitions, and assembly
procedures. Vendor dependencies and tool scripts are not reconstructed game
functions. Macro definitions and declarations are covered by a separate file
reading pass. VA markers are independently reconciled with parsed definitions;
retail functions without source definitions are not invented as reviewed C++.

Ten columns give the following manual verdicts:

- `absent`: no instance of that debt pattern found in the reviewed body.
- `retain`: the form is meaningful; the note explains its role or boundary.
- `open`: a candidate or uncertainty remains, with a finding to investigate.
- `applied`: a verified reconstruction change was retained.
- `pending`: the body has not been reviewed, or its content changed afterward.

`bounds` concerns whether the claimed object covers the accesses, including
caller/format assumptions. It does not assert that retail validates malformed
files. `stack` concerns artificial aggregation/unused storage, not ordinary
local arrays. `reuse` distinguishes unrelated roles from a cursor, coordinate,
or tagged message evolving naturally through one operation. A retained cast
is not a failure to reduce a count: wire, SDK, narrowing and tagged boundaries
can require explicit conversions.

`reviews.json` contains function-specific notes and all ten verdicts, keyed by
physical definition identity and SHA-256. `file-reviews.json` records full-file
reading and layout/macro reconciliation. These are manual evidence; the indexer
never upgrades parser coverage into a reviewed verdict. Shared-header edits
also require a caller review and build, even if caller bodies have not changed.

Regenerate using Universal Ctags 6:

```
python3 -m homm2.audit.reconstruction --write
python3 -m homm2.audit.reconstruction --check
```

Retained code changes must pass the Buka compiler/build gates and focused byte
and relocation review. An unsuccessful natural-source experiment remains
documented as unresolved; a fuzzy percentage alone cannot justify a change.

## Findings

- [Global data and annotation reconciliation](data-and-metadata.md): complete
  KB data tail, X_GLOBAL catalogs/runtime owners and va.h metadata read.
  All 1,655 physical functions and all 228 project files now have manual
  reviews; remaining findings retain their individual dispositions.

- C79/C80/O09/S72/S73/R29/G106-G108/A05/B94: all43 EVENTS definitions
  reviewed; 99 gross written conversions removed (95 net), four void-owner
  cast kinds corrected, four hero-tail copies use the actual250-byte owner,
  and two input buffers use proved smaller extents. All98 native objects
  remain unchanged. The58 complete products and per-function bounds,
  rejected alternatives and existing relocation-model gaps are preserved.
  [Evidence](C79-C80-O09-S72-S73-R29-G106-G108-A05-B94.md).

- C73/S66/R25/G87-G90/B88: all16 REQUEST functions reviewed; ten numeric casts
  and three unused scratch bytes removed with all98 native objects unchanged.
  Two-pass list consistency, empty-list access and the retail cross-field
  extension copy remain explicit. [Evidence](C73-S66-R25-G87-G90-B88.md).

- C72/A04/S65/R24/G85/G86/B87: all seven Overview functions reviewed;
  117 casts and one unused mine-count element removed, with both dialects
  checked and all98 native objects unchanged. Retail scroll/selection bounds,
  real heap rows and rejected cleanup families are recorded.
  [Evidence and residuals](C72-A04-S65-R24-G85-G86-B87.md).

- C71/B86: all five HISCORE functions reviewed; both narrowing casts retained
  after a complete four-arm matrix. Real score records reveal unchecked reads,
  an uninitialized cheated field and stored names used as formats.
  [Evidence and bounds](C71-B86.md).

- C70/S64/R23/B85/O06: all seven Campaign functions reviewed; five redundant
  casts removed with complete matrices and98-object native proof. Retail
  crossed-array accesses and campaign/setup reset owners remain explicit open
  findings. [Evidence and per-function boundaries](C70-S64-R23-B85-O06.md).

- C01: implemented `Read13(char*)`, `ReadBlock(void*)` and text name arrays;
  26 casts removed, native before/after bytes and relocation graphs identical.
  [Evidence and passing full-build gate](C01.md).
- S01: `LoadAggregateHeader` now uses the single signed 16-bit count it reads,
  removing an unused array element with [full native/retail proof](S01.md).
- G01: retain the button's shared-dispatch goto; a measured `break` replacement
  deletes the retail tail stub. [Experiment and exact retained proof](G01.md).
- U01: `tag_messagePayload` and its scalar/pointer/modifier subviews describe
  the event protocol. Mouse/keyboard events become widget events in place;
  replacing these unions with casts or fresh whole-message initialization would
  lose preserved payload fields. Retain the tagged views while reviewing each
  consumer's order of reads and writes.
- C02: recovered both cursor byte-buffer owners, removing four repeated views
  and adding two allocation-boundary casts. [Full native and retail proof](C02-G03-C06.md).
- C03: replaced the executive text aggregate overlay with18 actual character
  array owners. [Full native graph and604-byte retail storage proof](C03.md).
- C04/C05: framebuffer SDK publication remains void*, with its normal byte
  conversion corrected; signed palette interfaces and unsigned raw-byte views
  remain explicit boundaries. [Owner review and proof](C04-C05-C08-C11-S11-S13-G14.md).
- C06: retain the fizzle u16 lookup cast; the complete two-arm removal test
  changes native text. G03's structured color-tail alternative also changes
  bytes; retain the cleanup goto. [Measured dispositions](C02-G03-C06.md).
- G02-G06: font next-character joins, mouse cleanup/rectangle joins, palette
  cycle join and input coordinate join need structured-source experiments.
- B01-B09: font extraction, text-entry scratch, dialog saved-manager arrays,
  log formatting, ripple pointer extent, resource names, blit rectangles,
  screenshot names and scan-code table indices need caller-bound evidence.
- D01: [withdrawn address corrections](D01.md). The initial control was a
  reconstructed executable, not retail. All resulting address changes and
  annotation withdrawals were reverted; early build/redelink hash gates now
  reject that mistake before generating evidence.
- O01: recovered the real 72-byte Miles sample-state aggregate, eliminating
  a 72-byte memset through a four-byte scalar. [Exact VC6 evidence](O01.md).
- B10-B12: shared music-track domains, formatted music paths and sample-channel
  allocation assumptions need caller proof. S03/S04 record an uninitialized
  previous-track local and an unread music-fade local for native review.
- B13: registry NetName requests 31 bytes and writes 30 from a field declared
  as 24 bytes. Retail confirms address `0x00526368` and read length 31;
  neighboring allocation evidence must decide layout error versus retail overrun.
- B14-B17: debug-allocation file names, file-ID scratch, log formatting and
  data-entry initial text require caller-length proof. The bitmap-to-screen
  wrapper handles cursor overlap, not general clipping (B07).
- G07/G08: fade terminal-pass jumps and data-entry nested dispatch joins need
  structured-source experiments. S05-S07 cover unused registry scratch and
  unread utility scalars; none is silently removed on source inspection alone.
- O02: split the padding-bearing 140-byte pathfinding scratch record into
  29 objects at the original field addresses; removed two unobserved padding
  arrays. [Native before/after and verified retail evidence](O02.md).
- U02-U04: path-array/pointer overlays and duplicate queue/target-coordinate
  views need shared-owner reconstruction. G09-G11 are the pathfinding common
  completion/store/restore tails. B18/B19 cover map-edge and combat-hex input
  domains; S08 covers unused combat-search scalar slots.
- O03: bitmap helpers form possible beyond-end final row cursors, and dimming
  crossed a palette subarray instead of indexing its real level dimension.
  The palette access is now corrected with [native equivalence proof](O03.md);
  both monochrome icon decoders now use the same level dimension (four more
  accesses corrected). Row-pointer and clipping questions remain open.
- B20: [retail-confirmed receive overread](B20.md), plus transport packet/player
  bounds. C07 applies signed startup fields and actual tagged payload views
  with [complete native and retail proof](C07.md); G12/G13 cover send/input
  retries. S09/S10 record unused trading-message aggregate/send-result scalar.
- B21: trading-ratio indexing can receive an unselected resource (-1);
  caller/UI and retail proof must distinguish reconstruction error from old behavior.
- C08/C09/C11: removed17 written playback/graphics casts with full native
  proof and both dialects checked. C08 uses the real enum-to-index boundary
  before the movie offset. G14, S11 and S13 retain their original forms after
  complete source products reject deletion. [Evidence and contracts](C04-C05-C08-C11-S11-S13-G14.md).
- C10/S12: retain the arena's required const boundary and all measured
  unused slots. Its static-text-to-owning-widget path is a retail ownership
  defect, not repaired by removing a cast. [Lifecycle proof and144 arms](C10-S12.md).
- B23: graphics row pitch, screen extents and signed palette component ranges
  require caller/API contracts; fixed256-entry palette loops themselves fit.
- S14: three world-map offset arrays now use the actual three-scale domain,
  removing unused fourth elements with [complete native equivalence](S14.md).
- B24: the world-map horizontal clamp [writes Y in retail too](B24.md).
  B25 covers map/tileset/resource domains; legend selection is caller-bounded
  and the renderer's fixed framebuffer row pointers remain inside the image.
- S15: combat dialogs have assigned-but-unread local control/layout constants;
  B26/B27 cover hero/army/hex/corpse domains. Corpse append capacity is checked,
  but stored army indices still require lifecycle proof.
- C12/G15/B28: icon RLE palette typing, shared fill joins and clipped/streamed
  pointer bounds need family-wide native and input-domain review.
- B29: both sheared icon decoders read the modifier row before checking Y
  clipping, including initial setup. The two spell producers allocate 480 bytes;
  sprite-extent and stripe/fade accesses still need lifecycle/input proof.
- B30: the scaler's six callers supply only scales 4/6/12 with no clipping.
  Their temporary samples fit the initialized first 2048 bytes of a real
  4096-byte bitmap; destination and input-frame contracts remain B25/B28.
- U05: the monster database record has a single-arm outer union/struct and
  duplicate flag aliases. Initializer topology and all consumers must be checked
  before removing alternate views from the packed 26-byte record.
- B31: all three bit helpers use byte-indexed DWORD accesses; the six-byte
  puzzle mask's final byte access spans through offset 8, where a separate
  hero-screen selection global is claimed. Retail proves the helper widths;
  do not disguise the overlap by padding the mask. Packed town flags also
  require checking the full access width.
- B32: the assembly tile blitter assumes square/multiple-of-eight tiles and,
  on its forward path, a height divisible by 16. Resource and clipping
  contracts remain to be traced.
- C13/G16/S16: Bzip's type conversions, three loop/store-tail gotos and unused
  model/status storage require invariant-source and native proof. Its real
  frequency/permutation tables and sort stacks are retained.
- B33: Bzip `panic` and `cleanUpAndFail` are genuine-retail returning no-ops.
  Its logged overrun/EOF/allocation errors therefore cannot be credited as
  preventing subsequent out-of-range accesses. Decoded origin/run bounds and
  allocation lifecycle remain open.
- B34: Bzip data wrappers accept no destination capacity; path and formatter
  APIs also need caller-length proof. Their fixed generated filenames fit
  the zero-initialized local buffer, including four overwritten terminators.
- B35/S17: map-extra growth first returns index zero although links use zero
  as sentinel; caller reservation/lifecycle proof is required. Two unused
  map helper scalars need native frame checks. Reloading an extra pointer after
  reallocating its owner is necessary, not avoidable pointer reuse.
- C14: retain both recruitment filename casts for the required C++98
  annotation dialect. The [native-equivalent removal experiment](C14.md)
  fails that parser even though the modern interface accepts const text.
- U06: army-group duplicate type/count union aliases need all-consumer review;
  the unsigned troop-count view must not silently change serialized meaning.
- S18-S21: unread morale/recruitment/swap scalars and unknown interior strip/
  recruitment class gaps require native frame/layout evidence. Real snapshot
  arrays and generic swap temporaries are not artificial stack aggregation.
- G17/G18: recruitment's shared close tail and swap's common secondary-skill
  dispatch tail need structured-source matrices.
- B36: army, recruitment and exchange helpers require creature/slot/lifecycle
  contracts. Exchange control IDs bound their two sides, five army slots,
  fourteen artifacts and eight skills. Genuine-retail name and positive-cost
  tables prove recruitment title capacity and nonzero valid gold divisors;
  unchecked input domains and cached-icon lifecycle remain distinct questions.
- C15/S22: [serial receive scalars](serial-scalars.md) replace a casted two-short
  API result with `DWORD`, and a four-byte array with the single byte read.
  Both changes have full native and scoped retail byte/relocation proof.
- G19/G20: modem response and escape-parser joins need structured matrices.
  S23 covers unread serial/network scalars, an unused NetBIOS control block
  and unknown ComPortState interior gaps; genuine SDK records are retained.
- B37: modem setup [uses initialization text as an `sprintf` format in
  genuine retail](B37.md). Fixed dial/response/handshake text fits its real owners;
  negative generic packet lengths and modem's smaller transport limit remain.
- B38: [retail-confirmed NetBIOS wildcard overread](B38.md). The fixed16-byte
  listen-name copy starts at the two-byte `"*"` literal and includes adjacent
  source-path bytes. Enlarging the literal would change copied retail data.
- B39/B40: serial API lifecycle and variable-sized serial/NetBIOS node tails
  need explicit contracts. Known serial callers use slot0; the500-byte error
  formatter is bounded to473 bytes by all callers and genuine retail strings.
  NetBIOS session/payload lengths and short broadcast initialization remain.
- U07/S24: the NetBIOS payload union has an unused word view; event storage
  declares ten handles but only nine are initialized/used. Shared layout and
  native evidence must decide whether those declarations can be simplified.
- V01: NetBIOS session dispatch uses typed SDK `va_arg`; adding its omitted
  `va_end` calls produces extra retail-incompatible stores. [Measured retain
  decision and exact baseline proof](V01.md); portability debt stays explicit.
- C17/G21-G24/S26: remote-message byte-owner casts, shared setup/send/
  receive/completion joins, unread locals and ten extra
  queue slots need native structural review. Actual packed messages stay real.
- S25: both remote checksum arrays are now scalars, with [full native and
  independent retail proof](S25.md); the packet format and algorithm are unchanged.
- B41: remote decoding discards the received count before trusting header
  length; caller payload limits, short/stale data, sender indices and heartbeat
  initialization need protocol evidence. `RemoteMain` clears30 bytes of a
  30-element integer ID table, not its full120 bytes; retail proof is pending.
- C18: DirectPlay receive outputs now use SDK `DPID` locals, removing two
  pointer casts and a scalar cast with [full native/retail proof](C18.md).
- O04: twelve cursor accesses now index the actual two-element configuration
  walk-speed array, with all preference/UI consumers updated and
  [whole-build byte/relocation equivalence](O04.md).
- B42: DirectPlay session enumeration can exceed its ten-element list;
  [retail shutdown-drain capacity hazard is confirmed](B42.md). Packet
  lengths/player mappings and retained provider GUID lifetime/mutable name
  contracts need proof. B20 now includes
  independently verified DirectPlay overread evidence. C07 now replaces
  its false startup-typed carrier with an explicit erased payload boundary.
- S27/S28/S29: DirectPlay unused locals/state, cursor unread flag/counter
  stores, and the system-options unused message need native structural tests.
- G25/G26: cursor movement completion/event joins and map-change queue retry/
  duplicate joins need structured-source matrices. R02 records the movement
  step local reused for boat, terrain, animation and resource roles.
- B43: cursor/map-change direction, hero/player, map coordinate, frame and
  received-record domains remain. Normal preference values fit the actual
  eleven-entry volume and five-entry speed text tables; malformed stored
  values remain unvalidated. B44 now uses array deletion for the visibility
  map, with [complete native and retail equivalence](B44.md).
- C20: system-options scalar boolean conversions/materialization require
  native comparison; event widget payload unions themselves remain meaningful.
- U08: two bottom-view unions and their integer/derived/padding views now
  use [actual widget-pointer arrays with complete native equivalence](U08.md).
  U09 adventure remote payload still has a raw-byte/save/exit union requiring
  remaining consumer review.
- G27-G30: adventure command movement, keyboard cheat/confirmation, search
  completion and default-hover joins await structured matrices. R03/R04 cover
  unrelated dispatcher scalar roles; S30 covers unused adventure locals and
  class padding views.
- C21: overview passes occupied-town pointer bits as HeroView's boolean flag.
  The explicit truth-value candidate is not yet measured: the source generator
  rejects the following unannotated helper in the same RVA marker span.
  B43 also includes zero-town selection and unguarded no-hero search callers.
- G31-G33: radar terrain fallback, quick-info dispatch and bottom-view update
  joins need structured-source matrices. S30 also covers their unread locals
  and the quick-view layout constants retained as stack scalars.
- C22-C24: quick-info signed-byte `toupper`, enemy-turn player truncation and
  manual icon-entry addressing need API/domain evidence and native comparisons.
- B43 correction: adventure `GetCell` clamps invalid coordinates to cell0,0;
  its complete retail byte/relocation proof resolves the earlier concern about
  early wrapper calls in movement/drawing. Radar's direct `fullMap::GetCell(i+1,j)`
  is a separate [retail-confirmed unchecked right-edge access](B43.md).
- B45: quick-info and bottom-view text-as-format, input-string and decimal-label
  capacities require caller/data bounds. [Hero quick-view's five-byte decimal
  allocation is confirmed in retail](B45.md); several date/army-name/debug
  buffers now have explicit safe bounds from real fields and retail text.
- B46: new-turn week/day widgets both occupy tracked slot0/id2100. Parent-window
  ownership [keeps the first widget linked until window closure](B46.md);
  losing the bottom-view reference is not a permanently unreachable allocation.
- B47: the18×18 redraw grid has [retail-confirmed negative neighbor indices
  and a256-byte partial clear](B47.md). Positive border indices fit its real
  owner; no fabricated padding or shifted owner has been introduced.
- G34/R05: mine-guardian switch joins need structured source alternatives;
  town/hero context setters reuse locator-position locals as terrain IDs.
- G35-G39: summon-boat completion, sound disposal, adjacent-monster output,
  control-panel confirmation and visions tails await structured-source matrices.
- C25/C32: [recovered the signed-byte cheat flag](C25-C32.md), removing both
  compensating casts with complete98-unit native equivalence. U09's byte view forwards variable
  network payloads, so its16-byte view does not prove those consumer extents.
- B48: [network-control selection has a retail-confirmed non-advancing loop
  and possible negative index](B48.md). The genuine behavior is retained.
- U10/C26: [the hero patrol owner](U10.md) replaces a misleading boat/patrol
  union with unsigned coordinates and a signed radius, removing two casts.
  All98 units remain native-identical; four affected bodies have retail proof.
- C27: the level-up sample literal now passes directly to the const-correct
  sample API, with [complete native and retail proof](C27.md).
- G40: retain the [level-up skill-search goto](G40.md); the measured break arm
  loses the retail trampoline and ten bytes. G41/G42 secondary-skill status
  and dialog joins still need structured matrices. R06 records the army type/count swap
  sharing a scalar temporary; S31/S32 cover serialized unknown regions and
  unread hero UI/level-up/split/upgrade locals.
- B49: hero/player initialization and spell/skill/experience/selection domains
  require caller contracts. Actual serialized record regions are not disposable
  stack padding.
- B50: [hero formatter bounds](B50.md) fit the real 200/768-byte buffers for
  valid indices and a terminated hero name; complete level-up text is at most
  128 bytes. The next-tier index is intentional, not an off-by-one defect.
- B51/B52: KB startup/menu/player/building domains and text owners are reviewed
  per function. The [mage-cost clamp](B51.md) does not affect the actual table index;
  debug menu IDs exceed the declared hotspot/formation table ranges.
- B53: the [high-score reader](B53.md) requests the full 1000-byte array size at each
  successive 100-byte record. Retail proof confirms this; normal-sized files
  reach EOF after the first read, while oversized files can exceed the owner.
- B54: [sudden-exit transmission](B54.md) includes two uninitialized fields in its actual
  seven-byte packet. Dead-player exit differs: its remaining field is initialized
  by the receiving handler before transmission.
- C28/C33: [applied six resource-cast removals and a numeric payload correction](C28-C33.md),
  with complete native KB equivalence. C30/C31 [apply unsigned text lengths and
  a direct palette-index assignment](C30-C31.md), removing three more casts.
  C29 scalar index conversions remain for measurement.
  U11 records startup's 212/256-byte union;
  S33 covers KB unused locals and reserved scratch space. G43-G46 track startup,
  morale, menu and player-exit joins.
- B52: [selected KB formatter bounds](B52.md) account for complete morale/luck
  append chains, command-line help, ranking/congratulations and memory-error text.
  Other text owners and caller-domain contracts remain individually open.
- U12/U13: game's duplicate castle/town-owner and daily-event/known-town views
  need all-consumer reconciliation. S34 [retains52-byte reserved-file scratch
  after complete native layout matrices](S34-C35.md);
  S35 covers unknown game-layout regions, not proven deletable padding.
  C34 covers bounded narrowing casts in SetupOrigData; B55 tracks game getter,
  saved-player and initialization-domain contracts.
- G47: ultimate-artifact hint retry exit needs a structured-source matrix.
  C34 also covers bounded byte stores in hint/boat setup; C35 covers the
  puzzle interpolator's [applied constant-conversion cleanup](S34-C35.md).
  S36 tracks unread selection locals.
  B56 tracks standard-filename mutation, capacity, aliasing and loop-limit contracts.
- B57: save/load uses cross-member serialized prefixes, duplicated two-byte count
  headers and unchecked file-supplied lengths. S37 tracks save/load/map scratch
  arrays larger than their observed accesses; no padding is silently removed.
- G48/R07: new-map award joins and player/town counters reused as artifact
  coordinates need structural recovery. R08 tracks event random-roll/layer-index
  and town/mine-ID reuse; S38 tracks event layer-index arrays written but not read.
- B58: map-event allocation, terrain, layer-chain and town/mine-edge contracts
  remain open. C36 covers map-coordinate byte owners; C37 covers spell-widget
  boolean conversions and literal constness. S39 records the spell-message
  assignment overwritten by its following clear; B59 tracks spell-view text
  lengths and live widget/spell-index domains.
- C38/C39: [army-view name storage is now text](C38-C41-S41-S44.md), and five variadic numeric
  arguments use normal promotions. B60 covers the complete198-byte detail
  chain in550 bytes. S40 records unread army-dialog locals.
- C40-C43: sprite sentinels, logo upcasts, event-flag conversions and new-turn
  lowercase text ownership are reviewed individually. C41's logo upcast is
  removed; other candidates still require their own matrices.
- S41/S44: dwelling-roll and rumour-order scratch now match ten entries and
  six players. Other random-army/setup storage remains open. S42/S43 cover
  unused turn-update and transfer/compression locals. R09 tracks indices
  reused across distinct player/hero/town roles.
- G49-G54: random-monster bounds, transfer cleanup, score clamping, map-size
  setup, diff completion and rumour fallback joins need structural tests.
- B61/B64/B65: turn-state, town/hero setup, rumour/event and compression
  domains remain function-specific contracts. U14/C44 tracks typed save-packet
  ownership and erased byte-buffer interfaces; no fabricated padding is added.
- B62/B63: [retail confirms](B62-B63.md) the receiver's100-byte acknowledgement fill with
  a200-byte transmission, and diff completion using the old-file remainder.
  These defects are separate from byte-preserving reconstruction fixes.
- A01/S45–S47: [town-manager pointer expressions and four array extents](A01-C45-S45-S47.md)
  are simplified with complete native equivalence. C45 retains its measured
  16-bit boolean conversion; removing it loses three retail bytes.
- B66: [town text bounds](B66.md) prove all80-byte status writes, well-detail
  append chains, resource filenames and complete building descriptions for
  valid table indices. This resolves the selected B52 building-format question.
- B67/U15: [town state and spell-owner contracts](B67-U15.md) remain separate
  from text capacity. [U15's count overlay is removed](U15.md) with all-consumer
  review, all98-object native equivalence and explicit VC6 layout assertions.
  C46/R10/S48/G55–G57 track the remaining conversions, reused roles, unused
  locals and town-dialog joins, with per-function evidence in the ledger.
- C47/R11/S49/B68: the fully reviewed AI town-interaction consumer retains
  its real floating conversions; bounded ID/turn conversions, distance/slot
  reuse, unread scratch and numeric/game-state domains remain separate findings.

- B69/C48/R12/S50/G58/G59/U16: the first20 PHILAI definitions and both owner
  headers now have individual reviews. [Runtime and local-owner evidence](B69-S50.md)
  confirms the pre-guard candidate-grid read and records special-move
  initialization and threat-occupant contracts. The complete six-arm DoAI
  storage family rejects all smaller owners; no source change retained.

- C49/B70/C50/R13/S51/G60/G61: [purchase and target-selection review](C49-B70.md)
  adds21 complete PHILAI bodies. Two numeric cleanups remove eight casts after
  complete129-arm and three-arm exact comparisons. U16's real tagged purchase
  union is retained after its full consumer review. The resource-limit loop's
  last-resource result and the independent seven-resource purchase guard are
  verified separately; movement constants close B69's operand-evidence gap.

- C51/B71/C52/R14/S52: [combat-value and recruitment review](C51-B71.md)
  adds20 PHILAI bodies. Four numeric cleanups remove ten casts after complete
  2/65/12/8-arm comparisons and full native/retail operand proof. A pre-sentinel
  artifact read, duplicated hero-role pointer and slot-based replacement value
  are confirmed retail sequences and preserved, with reachability limits stated.

- C53/S53/B72/C54/G62/R15: [final PHILAI event review](C53-S53-B72.md)
  adds22 bodies and reconciles all84 physical definitions in the full source
  file. Mana evaluation loses one redundant cast with complete16-arm and
  native/retail proof. All16 unused-event-array combinations were measured;
  none was retained. Cache output ordering and mine probability truncation
  are documented separately from reconstruction changes.

- C55/C56/B73/C57/G63-G65/R16/S54: [army review](C55-C56-B73.md)
  completes all47 ARMY definitions and both headers. Sixteen casts disappear:
  eight in C55 and eight in C56 (seven following unsigned-owner recovery and
  one integer-identity cast). Full98-object native proof preserves
  every emitted function and allocated section. Text catalogs establish
  concrete capacities; frame concatenation and Y clipping retain retail
  behavior and explicit input contracts. The sole external writer, CastSpell,
  is also fully reviewed (B74/G66/S55).

- O05/C58/S56/B75/B76: [spell review](O05-C58-S56-B75.md) reconciles all37
  SPELLS functions and its header, plus ClearEffects. Five effect-mask accesses
  recover real array rows, twenty casts are removed, and summon scratch uses
  six entries. Both native TUs are unchanged across all54 emitted functions
  and allocated sections. Fixed catalog/format checks bound messages; signed
  palette indexing, no-show cleanup and corpse bookkeeping preserve verified
  retail behavior. O06's vaporization pointer issue and G67-G71/R17/C59/S56
  residuals remain explicit, not presented as closed.

- C60/C61/S57/R18/B77: [spell-AI review](C60-C61-S57-B77.md) reconciles all11
  SPELLAI functions. Four redundant float casts and one enum cast disappear;
  the latter follows recovery of the lookup table's one-byte spell domain.
  All98 native objects and1826 function instances remain unchanged. Complete
  unused-local and side-role products retain their original source; explicit
  operand checks distinguish verified retail scoring quirks from bounds and
  lifecycle contracts still requiring caller guarantees.

- C62/C63/S58/G72-G74/R19/C64/B78/B79: [combat-manager review](C62-C63-S58-B78.md)
  reconciles all38 CMBTMGR functions and both headers. Thirty-seven casts are
  removed, including two following one-byte castle-hex table recovery. All98
  native objects/1826 emitted functions and allocated sections are unchanged.
  Complete source products retain the original goto and unused-local forms;
  verified retail boundary violations are distinguished from default-data
  short circuits, bounded text and caller contracts that remain open.

- C65/A02/S59/G75/R20/B80: [combat-AI review](C65-A02-S59-B80.md) reconciles
  all19 AI functions and PATH's declaration header. Twenty casts and eight
  manual/reversed array expressions are removed; the boundary owner now has
  nine observed entries. Complete native98-object and fixed retail operand
  checks pass. Nineteen source products retain unsupported goto, unused-local
  and role-split rewrites as measured residuals, not silent gameplay changes.

- C66/S60/B81: all twelve PATH functions reviewed. One signed-byte speed
  restoration cast removed with complete native and retail proof; unused-local
  deletion changes code and is retained as measured debt. Real 117x6 adjacency
  ownership and sentinel/direction/placement contracts are documented in
  [the function-by-function report](C66-S60-B81.md).

- C67/S61/B82: all four FLY functions reviewed. Six assignment casts removed
  with full native proof and a fixed retail double-operand supplement. Unused
  scalar/store deletions were measured and retained. The adjacency producer
  excludes the apparent `CanFit` null case under initialized-table contracts;
  [animation, geometry and timing limits remain explicit](C67-S61-B82.md).
- U17/C68/A03/S62/R21/G76-G81/B83: all36 COMMAND functions and header reviewed.
  Recovered the256-byte command-tagged network packet, removed ten written
  casts (nine scalar), and restored the actual adjacency row expression.
  All32 measured families/176 arms completed; unsupported control-flow,
  unused-scalar/store and role splits retained. Combined all98 native objects
  unchanged; twenty fixed numerical operands and selected text bounds checked.
  [Quantity/network contracts and seven existing EH model gaps remain explicit](U17-C68-A03-S62-R21-B83.md).
- C69/S63/R22/G82-G84/B84: both Castle bodies, the private inline recruit
  helper and owner header reviewed. Nine redundant casts removed; all72 arms
  of nine source families completed, with unsupported joins/storage/role
  changes retained. Native objects unchanged; retail formatter and slot-table
  bounds proved, with lifecycle and one EH model gap
  [explicit in the full report](C69-S63-R22-G82-G84-B84.md).

- C74/S67/B89: all sixteen kbwin functions and header reviewed. Two redundant
  handle/sentinel casts removed; five complete products retain unsupported
  unused-slot/store deletions. All98 native objects and all sixteen retail
  byte/site/ordered-target checks pass without exceptions. Command-line,
  message-table and menu-table bounds are proved; API failure, configuration,
  shared scratch and signed-timer contracts remain
  [explicit in the full report](C74-S67-B89.md).

- C75/S68/R26/G91–G94/B90: all thirteen combat-drawing functions and the
  clipping/shear domain headers reconciled. Nine numeric casts removed;
  thirteen complete products/158 arms retain unsupported control-flow,
  unused-slot/store and role rewrites. All98 native objects unchanged.
  [Real message/table owners and conditional scan bounds](C75-S68-R26-G91-G94-B90.md)
  are distinguished from configuration/resource/input contracts and one
  unchanged private EH/FS model gap.

- U18/O07/C76/S69/R27/G95–G102/B91: all sixteen New Game functions and header
  reviewed. Incoming256-byte and outgoing300-byte packet owners recovered;
  71 numeric casts removed, one enum cast becomes a constructor, and one
  retry goto becomes a structured loop. All46 products/256 arms completed;
  unsupported storage/role/flow changes retained. All98 native objects are
  unchanged. [Packet, text and cursor evidence](U18-O07-C76-S69-R27-G95-G102-B91.md)
  distinguishes bounded default-data cases from unchecked format/index
  contracts and four unchanged private EH/FS model gaps.

- C77/S70/G103–G105/B92: all23 SETUP functions and header reviewed, including
  the earlier modem-entry check. All nine products/24 arms completed; four
  real word conversions, storage reservations and shared tails remain after
  measured alternatives change code. [Entry-capacity and help-table evidence](C77-S70-G103-G105-B92.md)
  proves bounded normal cases while retaining B88's downstream extension
  boundary and nine existing EH/FS model gaps. All98 native objects unchanged.

- C78/O08/S71/R28/B93: all 36 expansion-campaign functions and their header
  reviewed. Two redundant byte casts are removed; two name lookups now index
  their real six-entry owners instead of overrunning the adjacent stable-text
  array. [Exact owner/addend, catalog and per-function evidence](C78-O08-S71-R28-B93.md)
  preserves every retail destination. Eight products/23 arms retain measured
  storage and counter residuals. All 36 byte spans pass; ShowInfo's existing
  EH/FS model gap and invalid-state/serialized-window contracts remain explicit.

## Build setup observations

The public `toolchain-vc6-sp5` archive downloaded during this review hashes
`b243b68a1df8c2c54f7830a14221fa0a73fd3bbca78343fd1dd81a815a32ecec`,
whereas this branch pins
`accfbb0ab1a63d4b698d0d205950611386c39b81b1868b336dbfdab413bb84c4`.
The download was rejected by normal provisioning. Its unpacked contents lack
RC/MASM and contain the older modified MSVCPRT archive. Build setup reconstructs
the missing components from preserved local media and runs the unchanged
per-artifact verifier; no release checksum or validation gate is weakened.

Regeneration with the hash-checked genuine retail image passes `homm2 init`,
`homm2 build`, and all 877 selftests with Universal Ctags available. An additional `homm2 link
--transform` attempt fails in the existing Misc transform: it hard-codes
`$SG56843`, while both the preserved pre-edit and current native Misc objects
name the literal `$SG56847` at section3+0x1c. Both reproduce the same failure;
no source change or renamed fake literal was introduced to bypass the check.
This optional exact-link failure remains separate from the passing native
build and the still-incomplete exhaustive function audit.

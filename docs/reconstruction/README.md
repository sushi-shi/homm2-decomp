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

- C01: implemented `Read13(char*)`, `ReadBlock(void*)` and text name arrays;
  26 casts removed, native before/after bytes and relocation graphs identical.
  [Evidence and remaining full-build gate](C01.md).
- S01: `LoadAggregateHeader` now uses the single signed 16-bit count it reads,
  removing an unused array element with [full native/retail proof](S01.md).
- G01: retain the button's shared-dispatch goto; a measured `break` replacement
  deletes the retail tail stub. [Experiment and exact retained proof](G01.md).
- U01: `tag_messagePayload` and its scalar/pointer/modifier subviews describe
  the event protocol. Mouse/keyboard events become widget events in place;
  replacing these unions with casts or fresh whole-message initialization would
  lose preserved payload fields. Retain the tagged views while reviewing each
  consumer's order of reads and writes.
- C02: mouse color/AND bitplanes use `void*` owners then repeated byte casts.
- C03: executive text is a character array reinterpreted as an aggregate.
- C04/C05: framebuffer and palette interface signedness need owner review.
- C06: fizzle lookup index is cast to u16 although composed of two u8 values.
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
  bounds. C07 covers typed startup-payload access; G12/G13 cover send/input
  retries. S09/S10 record unused trading-message aggregate/send-result scalar.
- B21: trading-ratio indexing can receive an unselected resource (-1);
  caller/UI and retail proof must distinguish reconstruction error from old behavior.
- C08/C09: explicit resource upcasts and promoted-byte casts in movie playback.
  G14 is its shared playback exit; S11 covers unread frame scalars and B22 its
  path, movie-index, volume and rectangle input domains.
- C10/C11: arena static-text ownership casts and graphics HWND/HDC/assertion
  conversions need owner/API-native proof. S12/S13 cover unused arena message
  storage/scalars and graphics scalar slots, not genuine Win32 output records.
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
- C14: two recruitment filename const casts removed through the existing
  const-correct window interface. [Full native equivalence and retail-check
  limits](C14.md) distinguish instruction/ordinary-target proof from EH aliases.
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
  packet lengths/player mappings, shutdown drain capacity, and retained
  provider GUID lifetime/mutable name contracts need proof. B20 now includes
  independently verified DirectPlay overread evidence. C07 also applies to
  its startup-typed carrier reinterpreted as a guest-info record.
- S27/S28/S29: DirectPlay unused locals/state, cursor unread flag/counter
  stores, and the system-options unused message need native structural tests.
- G25/G26: cursor movement completion/event joins and map-change queue retry/
  duplicate joins need structured-source matrices. R02 records the movement
  step local reused for boat, terrain, animation and resource roles.
- B43: cursor/map-change direction, hero/player, map coordinate, frame and
  received-record domains remain. Normal preference values fit the actual
  eleven-entry volume and five-entry speed text tables; malformed stored
  values remain unvalidated. B44 records visibility-map array allocation
  followed by scalar deletion, pending retail allocator-call proof.
- C20: system-options scalar boolean conversions/materialization require
  native comparison; event widget payload unions themselves remain meaningful.

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

# Common-helper candidates

All entries are search findings, not implemented or retail-byte-verified changes.
The whole-tree reading pass is still in progress; occurrence lists grow as bodies
are reviewed. Names below are proposals unless identified as existing.

## H01 — shared widget-message header macro

Disposition: strong candidate; consolidate the existing identical macro first.

`SET_WIDGET_MESSAGE` is defined identically in `src/BASE/BUTTON.cpp:103` and
`src/BASE/TEXTWDGT.cpp:74`. It writes `type`, widget `command`, then widget `id`.
Reviewed uses: `button::Main`, `button::Deselect`, `textWidget::Main`.
Natural owner: `include/BASE/message.h` beside `tag_message`.

B02 adds exact three-store sequences in `border::Main` (release),
`textEntryWidget::Main` (right-click and edit completion), `listBoxWidget::Main`
(right-click), `listBoxWidget::ProcessMouseMessage` (selection event), and
`dropListWidget::Main` (right-click and dialog completion). Their payload tails
remain explicit: list selection adds both index and single/double-click count.

Contract: retype an existing event and set ONLY those three fields, in that order.
Do not clear the union: the right-button modifier survives in another payload slot.
`textWidget::Main` sometimes writes that modifier before retyping; `button::Deselect`
restores the saved modifier afterward. Macro arguments must be side-effect-free
lvalues/values because the message expression is repeated. Parenthesize arguments;
a statement-safe `do { ... } while (0)` form is plausible, but its byte neutrality
must be measured before retaining it in matching source.

`iconWidget::Main` has an equivalent release-event triple in that order. Its
press-event stores and `button::Select` have different ordering and branching;
do not replace those automatically. The icon release arm's existing subsequent
test against `MESSAGE_RIGHT_BUTTON_UP` is unreachable after retyping; a helper
must not silently "fix" this behavior.

B08 adds the initial text message in `GetDataEntry` and the post-copy SET_TEXT
message in `DataEntryWindowHandler`. The latter's GET_TEXT setup orders id before
command and is not an exact store-order instance. Multi-widget broadcasts that
reuse the existing message header should keep their id/payload-only updates.

B13 adds `HeroMessageUpdate`: the HERO_UI names alias the same message type and
SET_TEXT command, with the exact type/command/id prefix. `SetupHeroView` and
`DoHeroSplit` have intervening formatting calls; their scattered stores are not
automatically contiguous instances, even when final message values agree.

Readability gain: name the event conversion once instead of repeating a union
protocol or maintaining two private definitions. Historical confidence: plausible
macro-shaped idiom, but the existing source definitions are reconstruction, not
proof that the original developer used that spelling.

## H02 — widget-local half-open hit test

Disposition: plausible shared expression macro / inline hypothesis; not proved.

Reviewed `button::Main`, `textWidget::Main`, `iconWidget::Main`, `widget::Main`
all test local `i16` coordinates against
`x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height`.
Proposed owner: `include/BASE/widget.h`; candidate spelling `ContainsLocalPoint(x,y)`.

Preserve half-open bounds and the existing `i16` narrowing when converting from
screen coordinates. Do not fold screen-to-local conversion into the helper: it
would change narrowing and possibly overflow behavior. No unsigned subtraction
"optimization" or Win32 `RECT` conversion. Read-only predicate is period-plausible;
an inline may add `/Ob1` expansion structure absent from the current body, so macro
versus method remains a measured matching question. Rectangle-like checks in other
domains are not yet reviewed and are not automatically instances of this helper.

B02 adds `border::Main`, `textEntryWidget::Main`, `listBoxWidget::Main`, and
`dropListWidget::Main` (right-click). The drop button and open list have different
rectangles, so they need an explicitly parameterized rectangle predicate rather
than the proposed widget-extent method. `ProcessMouseMessage` uses `i32` coordinates
from `screenX/screenY`, unlike the entry handler's narrowed `x/y`; preserve that.

## H03 — serialized widget geometry prefix

Disposition: plausible narrow statement macro; more widget readers to review.

`button::Read`, `textWidget::Read`, and `iconWidget::Read` start with four
`gpResourceManager->ReadWord()` calls assigning `m_x`, `m_y`, `m_width`, `m_height`
in exactly that order. Proposed owner: widget/resource serialization boundary.
Possible name: `READ_WIDGET_GEOMETRY` (not an implemented name).

Keep four separate reads and assignments; order and `i16` destinations are part
of the serialized contract. Do not absorb the rest of each `Read`: text has a
length/data payload, font lookup, and different masks; buttons and icon widgets
have different trailing fields. A new callable helper could change `/Od` codegen.

B02 confirms the same four-field prefix in `border::Read`, `dimmerWidget::Read`,
`textEntryWidget::Read`, `listBoxWidget::Read`, and `dropListWidget::Read`: eight TUs
in total. Keep `m_id`/`m_kind` outside the helper, since their positions vary.

## H04 — resource lookup while retaining a resource-file cursor

Disposition: recurring lifetime pattern, defer abstraction judgment.

The three `Read` functions above bracket `GetFont` or `MakeId`/`GetIcon` with
`SavePosition` / `RestorePosition`. It is plausible shared intent, but not yet a
single common statement contract. B02 confirms repeated font/icon pairs in both
list widgets and text entry, and bitmap/icon alternatives in `border::Read`.
The `resourceManager` implementation saves BOTH active aggregate and file offset
on a global ten-entry stack. A narrow `GET_RESOURCE_PRESERVING_POSITION(destination,
lookupExpression)` macro is plausible; both expressions would require side-effect
constraints and exact ordering. Do not introduce RAII, return from inside a macro,
or move the restore past any subsequent `ReadWord`. Deferred until the other
resource consumers are read; a generic callback framework is not warranted.

B07's `mouseManager::SetPointer` overloads also preserve aggregate position, but
the protected regions include cursor selection, disposal, native mask creation,
and nested calls. They are lifetime-pattern evidence, not single lookup-expression
instances of the proposed macro.

## R01 — a universal widget event handler

Rejected. The related `Main` methods intentionally differ: icon replacement can
pass an otherwise-disabled icon widget; text/button disabled-event guards differ
from that exception. Selection, right-button behavior, dispatch return values,
and draw side effects differ. H01/H02 are narrow candidates, not justification
for combining the event state machines.

## H05 — allocate and copy a terminated string

Disposition: credible small cross-TU statement helper; wider occurrence audit pending.

`listBoxWidget::Main` and `dropListWidget::Main` both allocate
`H2_ALLOC(strlen(text) + 1)` into an item slot then `strcpy` into it, for append
and replacement. Proposed name `ALLOC_COPY_STRING(destination, source)`; owner
`BASE/Misc.h` alongside the existing allocator macros is more natural than a UI
header. This is the same byte-oriented, NUL-terminated string contract at each
reviewed site. Destination/source expressions must be stable and side-effect-free.

Do not include freeing the previous string: allocation and source lifetime order
must remain caller-owned, especially with aliases. Do not substitute CRT `strdup`
(different allocator/call) or a modern string class. `textWidget::SetText` and
text-entry buffers use extra capacity (+5/+6), so they are NOT exact instances.
The complete string-memory owner was subsequently reviewed in B08.

B04 adds `font::DrawBoundedString`'s allocated whole-string copy. The buffer
allocation in `font::LineLength` has no immediately corresponding whole-string
copy and is not an exact instance.

B08 confirms `H2_ALLOC` currently expands to `new u8[size]` and `H2_FREE` to the
existing delete expression. `BaseAlloc`/`BaseFree` implement a separate debug
allocation ledger; do not substitute them for the current macros. No existing
allocate-and-copy string helper was found in this owner.

## H06 — icon-frame entry lookup (existing API)

Disposition: favor the existing semantic owner; no new abstraction needed on semantics.

`listBoxWidget::Read`, `dropListWidget::Read`, and
`dropListWidget::ProcessSelectDialog` repeatedly use
`reinterpret_cast<IconEntry*>(m_icon->m_data) + frame`. `iconWidget::Draw` already
calls `GetIconEntry(m_icon, m_frame)`. The existing implementation in
`src/BASE/Misc.cpp:1945` computes the same byte address using `sizeof(IconEntry)`.

Potential readability change: use the established `GetIconEntry` spelling where
matching evidence permits; do not create a second helper with the same contract.
It is currently a real out-of-line function, not an inline accessor. Replacing
raw expressions with calls is NOT known byte-neutral. Invalid/negative indices
and integer-width behavior must be checked, not legalized by a new bounded API.

B04-B05 add raw frame lookups in `font::GetCharacterWidth`, the icon extent
methods, and the ten RLE decoders. Keep their subsequent entry-field loads and
file-static state assignments in place; the scaling wrapper has no raw lookup.

## H07 — sequential clamp to a scroll range

Disposition: shared small idiom; macro versus inline remains an evidence question.

`listBoxWidget::DeleteItem`, `listBoxWidget::Main`, and
`listBoxWidget::ProcessMouseMessage`, plus `dropListWidget::ProcessSelectDialog`,
repeat `if (value < 0) value = 0; if (value > range) value = range;`.
A period-plausible statement macro `CLAMP_RANGE(value, low, high)` would name the
operation. Before sharing project-wide, audit other bounds/widths and existing APIs.

The order matters: if `range < 0`, the second assignment wins; a nested ternary
that returns zero at the first comparison is NOT equivalent. Some destinations
are `i16` fields already narrowed from a drag calculation; others are `i32` locals.
Keep that narrowing boundary, and do not recompute the drag formula in the helper.
Repeated macro operands need side-effect-free arguments. A generic modern clamp
must not import a `low <= high` precondition absent from these bodies.

`Blur` is not an exact instance: it narrows into signed `i8` palette components
before clamping, and applies the upper bound before the lower bound. Preserve
these distinctions if a wider clamp vocabulary is eventually warranted.

B06 confirms lower-then-upper clamps in `SetAudiereMusicVolume` and
`soundManager::ConvertVolume`. Their upper limits are fixed nonnegative constants.
`soundManager::PollSound` instead clamps upper-then-lower after fade arithmetic;
leave the ordering explicit even where ordinary inputs yield the same result.

B08 adds the X/Y window-position clamps in `ReadPrefsFromRegistry`. The source
uses video height for the X limit and video width for Y; a helper must retain
those actual arguments, not quietly correct the apparent axis swap.

## H08 — list row and scrollbar drag arithmetic

Disposition: plausible expression helpers, lower confidence than H01-H03.

`listBoxWidget::ProcessMouseMessage` and `dropListWidget::ProcessSelectDialog`
share the row-offset calculation after a taller first row, and the drag formula
`(y - trackY - 4 - thumbHeight / 2) * (range + 1) / thumbTravel`.
Possible names: `ListRowAtOffset`, `ScrollIndexAtY`; a small shared list-layout
header would be more honest than pretending the classes share an object layout.

Preserve the `offset > firstHeight` boundary (not `>=`), signed division/truncation,
and multiplication-before-division. Clamp remains a separate H07 phase. List-box
and drop-down drawing differ for empty rows and no-scroll thumb positioning;
those whole routines are not equivalent. Zero travel/range and coordinate origins
need explicit caller analysis before a safe common contract can be recommended.

## R02 — resource cache getter/aggregate scan boilerplate

Local-only, outside the cross-TU candidate set. `resourceManager::GetPalette`,
`GetBitmap`, `GetTileset`, `GetFont`, `GetSample`, and `GetMIDIWrap` share
query/refcount-or-construct/add flow. Constructor arguments differ (name versus ID).
`PointToFile` and `GetFileSize` also repeat the aggregate search, but one changes
`m_curAggregate` and seeks while the other records a local aggregate. Keep these
as same-TU observations, not evidence of a shared game-wide macro. Thin typed
resource getters already provide clear caller-level names.

## R03 — whole list-box/drop-down algorithms

Rejected as shared macros. The classes share row skins and some arithmetic, but
list-box input persists between dispatched events and enables mouse-move delivery;
drop-down input is a modal loop with popup background ownership, keyboard navigation,
minimum three visible rows, first-release handling, and different selection guards.
Even `DeleteItem` additionally adjusts scroll/visible state only in list-box.
Replacing either with a generic container would also conceal legacy deletion
behavior. Retain explicit lifetimes and state transitions; extract only measured
small contracts such as H05/H07/H08 if worthwhile.

## R04 — generic linked-list removal or save/restore-background macro

Rejected on currently reviewed evidence. `resourceManager` uses a singly linked
resource list; windows/widgets have doubly linked, ordered lists and separate
ownership flags. `heroWindow::RemoveWidget` has extra post-removal link updates
that cannot be replaced with a textbook unlink silently. Popup background capture
resembles window capture but differs in bitmap type, polling, and guarded redraw.
The named member methods already explain these lifetimes better than a shared macro.

B03 reinforces this rejection: executive manager insertion orders by priority
and activates objects, whereas window insertion updates focus and window flags.
Their link-store sequences and ownership are not interchangeable.

## H09 — clip one coordinate/extent to a zero-based bound

Disposition: small cross-TU statement idiom; distinct from a general rectangle API.

`heroWindowManager::SaveFizzleSource`, `FizzleForward`, and `DoRipple` repeat
`if (x < 0) { width += x; x = 0; }` followed by
`if (x + width > limit) width = limit - x;`, and similarly for the other axis.
A name such as `CLIP_AXIS_TO_BOUND(position, extent, limit)` can expose the intent
without hiding the caller's empty-rectangle return and pixel work. Proposed owner:
a narrow BASE graphics geometry header. Arguments must be stable lvalues/values.

Keep signed arithmetic, adjustment order, and the caller's `< 0` versus `<= 0`
emptiness test. Fizzle interleaves the axes (both lower-bound adjustments before
both upper-bound adjustments); two whole-axis macro calls would reorder stores.
That needs explicit matching evidence or smaller lower/upper helper boundaries,
even though these local coordinates are independent. Ripple clips only the X axis.
Bitmap careful copies additionally adjust a second coordinate
system, so the whole clipping blocks are not identical. `FillBitmapAreaClip`
and RLE span clipping use inclusive right/bottom endpoints and different contact
tests: do not replace them with this helper. No saturating arithmetic, overflow
cleanup, or changes to zero-size behavior are justified by this search.

## H10 — bitmap row/pixel address accessor

Disposition: plausible narrow inline expression; owner and codegen need measurement.

Reviewed instances include `bitmap::CopyToCareful`, `BlitBitmap`,
`heroWindowManager::FizzleForward`, `DoRipple`, `IconToBitmapScale`, and decoder
row initialization: a pixel base plus `y * pitch + x`. A member `PixelAt(x,y)` or
free row accessor could give the address calculation one semantic name.
`BITMAP.cpp` already has private `BitmapWidth`/`BitmapPixels` accessors, presently
unused; they are evidence of an existing vocabulary, not proof of a shared API.

Do not replace explicit `SCREEN_WIDTH` pitches with object width, or discard
existing precomputed pitch/row state. Preserve signed coordinate arithmetic and
the placement of pointer formation relative to clipping. This is address-only,
not a checked drawing operation or a replacement for named blit methods.

## H11 — complete icon fits the clipping rectangle

Disposition: strong repeated predicate; macro/inline hypothesis remains unverified.

`IconToBitmap`, `FlipIconToBitmap`, `DimIconToBitmap`, `FlipDimIconToBitmap`,
`MonoIconToBitmap`, `FlipMonoIconToBitmap`, `IconToBitmapColorTable`, and
`FlipIconToBitmapColorTable` use the same four comparisons to disable clipping
when the whole placed entry fits. Name the predicate, for example
`ICON_FITS_CLIP(left, top, width, height, clipX, clipY, clipW, clipH)`, in the
icon drawing geometry owner. The resulting clip-mode assignment remains explicit.

Preserve the half-open extent comparisons and their order, the already-computed
flipped left edge, and the later inclusive `clipR/clipB` conversion. No repeated
operand may have side effects. Sheared decoders have row-dependent displacement
and do not use this fast-path predicate; scaling delegates to `IconToBitmap`.

## H12 — existing RLE byte reader and dim-palette selection

Disposition: narrow existing inline plus a plausible expression helper; validate
source-state and arithmetic contracts before recommending replacements.

`include/BASE/IconRle.h` already defines `ReadIconRleByte(u8*& cursor)`; the ten
reviewed RLE decoders instead spell byte load and pointer increment separately.
The inline increments first and returns `cursor[-1]`, so source-level store order
is different even where the read value is equivalent. It currently has no callers.
Use its established vocabulary only if byte/relocation evidence supports it;
do not add another cursor helper or put all file-static state into a new object.

The six colored RLE variants also repeat selection of a 256-byte dim-palette row
from the command's masked level bits. A `DimPaletteForCommand(command)` expression
would name the encoding better than a cast plus magic-looking offset. Natural
boundary: `IconRle.h` and `SOURCE/dimPalette.h`, not a generic palette transform.
The color-table forward variant shifts unsigned bits then multiplies by 256;
other variants use the encoded mask times 64. Preserve widths/lowering and do
not conflate this with `DimBitmapArea`'s plain level index. The latter instead
shares `uDimPal[0][0][level * 256 + pixel]` with `DimIconToBitmap` and
`FlipDimIconToBitmap`; a separate `DimColorAtLevel(pixel, level)` expression is
plausible for those three TUs. Preserve the current flattened access, byte result,
and signed index contract until retail-verified owner typing is available.
Application, recoloring, and short/extended run counts remain separate decisions.

## R05 — shared event polling/dispatch loop

Rejected as a common macro on reviewed evidence. Executive, window-manager,
text-entry, and drop-down loops differ in sound polling, Windows message pumping,
mouse dispatch, consume/forward handling, and dialog termination. The repeated
dialog-result test within `EXEC.cpp` is local-only. `BroadcastMessage` accepts a
message type and writes a payload value too, so it is not simply H01.

## R06 — universal clipping or whole sprite decoder macro

Rejected. B04-B05 distinguish half-open extent tests, inclusive span endpoints,
careful source/destination clipping, fixed screen pitches, and shear sentinels.
Mono RLE has a different high-bit/run mask from colored RLE; color-table drawing
has an extra forward-only dim gate, and recolored fills bypass color translation.
Flipped literal runs adjust the source cursor and deferred source skip differently
from solid/shadow spans. Existing restrictive flipped fill predicates (and their
dead inner branches) are behavior to preserve, not deduplication mistakes to fix.
The sheared row-reset reads the current row's shear before incrementing `y`;
do not silently rewrite this as the next row's displacement.

Keep related phase names and small helpers while leaving the actual algorithms,
file-static identities, and dialect differences visible. Assembly bit operations
already have `BitTest`/`BitSet`/`BitClear` interfaces and are not instances of the
scalar `BIT` macro; tile assembly's four orientation paths are not C++ macros to
factor. Resource constructors' seek/allocate/read resemblance likewise does not
justify hiding distinct serialized headers, allocation sizes, or destruction.

## R07 — local-only font/palette algorithms and load-bearing scaffolding

`IsVowel`, `IsHyphen`, and `RemapCyrillicCharacter` already give FONT operations
names. Width lookup maps spaces and periods differently from drawing; do not
merge those whole paths. Similar unused declaration blocks in the line-layout
routines have explicit codegen roles and must not be hidden in a common macro.
`Blur`'s existing `BLUR_TAP_SUM` is a useful local tap-sum macro, not evidence of
cross-TU reuse. Window-manager palette cycles have different lengths/thresholds;
the existing `FadeSavedUpdate` accessor intentionally retains state across calls.

## H13 — client coordinate to logical game coordinate

Disposition: precise cross-TU expression helper; preserve input conversion boundaries.

`MouseMessageHandler` in `INPUTMGR.cpp`, plus `mouseManager::MouseCoords` and
`CheckUpdateMousePos` in `MOUSEMGR.cpp`, compute `clientX * 640 / clientWidth`
and `clientY * 480 / clientHeight`. Proposed names `ClientToGameX`/`ClientToGameY`
would explain the fixed logical resolution; a small input-coordinate owner header
is preferable to a general numeric scaling macro.

The Windows-message path first sign-extends each packed 16-bit coordinate; the
cursor-query paths begin with `POINT`'s signed long fields. Keep these conversions
outside the helper and preserve multiply-before-signed-division, truncation and
global dimension reads. Do not clamp off-screen positions or add a new zero-size
policy. `MouseCoords` additionally queries Windows and converts screen to client;
it cannot replace the message-coordinate expressions wholesale.

B11 adds `DDAppPaint` and `WGAppPaint` source-coordinate calculations. Scaling
width/height uses the same arithmetic but follows distinct rectangle preparation
and inclusive-end adjustments; keep those steps outside an expression helper.
`AppWndProc` forces reported client dimensions to at least one on WM_SIZE, but
that one path is not a proof that every use can discard division preconditions.

## H14 — manager registration fields

Disposition: lower-confidence small statement macro or base-method hypothesis.

`heroWindowManager::Open`, `soundManager::Open`, `inputManager::Open`, and
`mouseManager::Open` assign message mask, priority, then active=true in the same
order. A `REGISTER_MANAGER_FIELDS(mask, priority)`-style name could expose this
three-field base-manager protocol. Proposed owner: `baseManager.h`.

Do not include name copying: the mouse name is initialized in its constructor,
while the other three copy it after these stores. Input keeps a separate requested
priority and installs a fixed manager priority. Do not absorb resource/backend
initialization, list insertion or failure handling; this sequence does none of
those. Because the three current statements are already clear and occur once per
TU, retain only if a named base operation improves readability enough to justify
the indirection and its matching cost.

## R08 — unified audio lifecycle or backend-dispatch macros

Rejected. `IsAudiereBackend`, `IsMilesBackend`, `ConvertVolume` and the free backend
operations already give useful names to the shared contracts. `MusicPlaying`
checks backend kind without the handle checks used by the readiness predicates;
do not substitute them automatically. Readiness globals and disabled/config
guards also differ among operations. Audiere owns reference-counted streams and
an iteration-depth-protected sample list; Miles owns pooled sample handles and
round-robin channels with service/wait behavior. MIDI sequence/resource release
and Audiere stream reset/save-position behavior are not one cleanup protocol.

Same-TU observations stay local: Audiere find-then-operate wrappers already call
`FindAudiereSample`; MIDI stop/release tails have different guards. `MilesSampleFormat`
is an existing private inline that names its backend bit encoding. Shared stereo
and sample-width inputs do not make Audiere buffer-frame setup the same operation.
Compiler-generated static-member lifetime annotations do not count as authored
functions or candidate helper bodies.

## R09 — event queue and cursor state guards

The repeated input ring advancement/overwrite-oldest sequence is a good possible
private `CommitEvent` helper, but all current instances are in `INPUTMGR.cpp`, so
it is not a cross-TU candidate. Existing `ResetEventQueue` and
`InitializeEmptyEvent` already name smaller local contracts. Empty-event setup
does not initialize every union slot; it is not a general event clear or H01.
`ForceMouseMove`'s repeated reentrancy guard is preserved as written.

Reject a universal cursor guard: some call sites save and restore prior flags,
others deliberately force false/true; hide count is nested, and update suppression
uses separate globals and manager fields. Pointer replacement protects resource
position over a larger region than H04's narrow expression. Cursor bitmap masks
are MSB-first two-plane bytes, unlike BITS' packed LSB-oriented interface; no shared
bit-set macro should erase that encoding. Cursor save/draw paths use distinct
raw versus clipped origins and old versus current size indices; R06 applies.

## H15 — current executable's graphics configuration

Disposition: useful cross-TU lvalue expression / reference-accessor hypothesis.

`KeyboardMessageHandler`, `MouseMessageHandler`, `CheckChangeCursor`,
`BlitBitmapToScreenVesa`, `FadeIn`, `FadeOut`, and `ReadPrefsFromRegistry` repeat
`gConfig.gfx[IDX(giCurExe)]` across three TUs. A name such as `CURRENT_GFX_CONFIG`
or `CurrentGraphicsConfig()` would explain that game and editor have independent
window/fullscreen/cursor preferences. The actual record is `exeGfxConfig` in
`CONFIG_TYPES.h`; `gConfig` and `giCurExe` are declared in `KB.h`/`X_GLOBAL.h`.
A small configuration-access boundary should own it, not a generic index macro.

It must remain an lvalue selecting the current slot on each use. Do not cache a
pointer/reference across callbacks, collapse the game/editor settings, normalize
the packed record, or turn nonzero integer flags into new bool storage. The
reference-returning inline and expression macro are alternatives to measure,
not established byte-equivalent spellings.

B11 adds extensive consumers in `kbwin.cpp` (initialization, move/size, resize,
menu changes) and `wingraph.cpp` (clipper setup, display initialization/switching).
The save/restore locals around window changes intentionally preserve selected
fields while callbacks run; H15 does not replace them with a whole-record copy.

## H16 — inclusive rectangle disjointness

Disposition: credible small geometry predicate, separate from clipping and hit tests.

`mouseManager::NewUpdate` compares current versus old cursor rectangles;
`BlitBitmapToScreen` compares the destination update rectangle with the saved
cursor rectangle. Both decide disjointness by four edge comparisons, with edge
contact still overlapping. A name such as `RECTS_DISJOINT_INCLUSIVE` could make
the redraw decision clearer without absorbing visibility or dirty-extent updates.
Proposed owner: the same narrow BASE graphics-geometry boundary considered by H09.

Pass the existing saved/clipped and raw edges exactly; do not reconstruct a new
rectangle from cursor position and current size. Comparison order differs between
these bodies and must be measured or parameterized faithfully. Keep `IsVis()`
outside the predicate and preserve its short circuit. The subsequent test in
`BlitBitmapToScreen` asks whether the update fully covers the cursor; that is a
different containment contract, not another disjointness instance. H02/H11 retain
their separate half-open contracts.

## R10 — registry, file, random, and fade utilities

The many registry queries/writes in `Misc.cpp` are good local readability targets,
but not yet cross-TU helpers. Query byte count is an in/out variable reused across
calls, not reset to four before every DWORD. Initial probing, ignored failures,
string lengths, and handle-close/default-write paths differ; a table-driven rewrite
would hide these details. Read/write/seek/open sequences also differ in error
handling and file mode. In particular, `FileSize` opens `r+b`, so it cannot blindly
replace Bzip's read-only temporary-file measurement.

`Random` and `SRandom` deliberately have different random state and seed mixing;
both already expose inclusive bounds. `MAKEFILEID` folds ASCII only and updates
its unsigned hash by specific shifts/additions, whereas `CyrillicToUpper` handles
CP1251 too. They are not interchangeable case-conversion or rotate operations.
Fade variants differ in threshold table, saturation, palette state and final-step
behavior. Existing `DelayTil` rereads a pointer deadline after message pumping;
`DelayTilMilli` owns a value deadline. Similar loops do not justify merging these
APIs or moving sound/message calls. R05 still applies.

## R11 — bundled Bzip implementation is its own protocol boundary

The complete Bzip TU already has CRC/bitstream/model/packed-word helpers and local
macros (`UPDATE_CRC`, `ERROR_IF_*`, `GETFIRST*`, `SWAP`). Their actual consumers are
within that TU; do not promote them to game-wide utilities just because the names
sound generic. Arithmetic-coded big-endian words are not resource `ReadWord` data;
its RLE counts, EOF marker, CRC order, block normalization and global buffers are
not icon RLE. `NORMALISE` adjusts by at most one block; `STRONG_NORMALISE` loops
and takes a remainder. A generic modulo/clamp replacement changes the contract.

Keep empty error/cleanup hooks as currently written: they do not establish a
nonreturning failure path. Repeated free-then-null groups put all null stores
after all frees, so a per-pointer macro would reorder them. Encode/decode's
temporary-file protocol, random filename append and stream-owned closes are
local opportunities, not evidence for a shared game serialization framework.

## R12 — compatibility machinery versus period helpers

`Ints.h`'s enum storage templates, constrained conversions, assignment bridges,
and generated operators are modern checking infrastructure. They were read and
counted as physical definitions where appropriate, but are not proposed as
historical helper discoveries. Likewise, `va.h` annotations are reconstruction
metadata, not game behavior. Do not factor these into the period macro candidate set.

The game already has small semantic interfaces worth checking at later consumers:
`MAP_TRIGGER_*`, `OppositeMapDirection`, `OppositeCombatSide`, facing/result
conversions, `OppositeCampaignSide`, `NextSpellEffectDisplayType`, and
`CyrillicToUpper`. Their strict/retail branches can differ for invalid domain values;
use the caller's real domain rather than assuming every numeric lookalike is valid.
`MAP_EXTRA_AT` and `_WFIRST` explicitly retain different address operand ordering;
their existing comments prohibit merging them for tidiness. Consumer review and
new matching evidence remain necessary before any additional replacement.

## H17 — short-arity logging calls

Disposition: strong readability candidate using the existing `LogInt` API.

`AppInit` in `kbwin.cpp`, and memory diagnostic routines in `Misc.cpp`, pass one
value followed by six `LOG_UNUSED_VALUE` arguments. A fixed-arity macro such as
`LOG_INT_1(label, value)` could preserve that exact eight-argument call. Default
arguments for the existing function are an equally period-plausible alternative
that avoids new callable wrappers; audit the strict enum overload consistently.
Natural owner: `BASE/Misc.h` next to `LogInt` and the existing sentinel constant.

The implementation chooses output length by trailing sentinel values; it is not
a normal variable-length formatting API. Preserve all seven numeric slots, the
required first value, the sentinel's actual meaning, argument evaluation, and
logging's existing level check inside the callee. Do not add a variadic macro or
replace the function with a stream/logging framework. Other arities remain leads
until their game/network caller bodies are read.

## H18 — palette component from six-bit intensity to output byte

Disposition: small cross-TU expression macro; input signedness must stay explicit.

`CreatePCXFile` in `Misc.cpp` and `DDUpdatePalette`/`WGUpdatePalette` in
`wingraph.cpp` shift each palette component left by two before storing a byte.
A name such as `PALETTE_COMPONENT_TO_BYTE(component)` would expose this conversion
at the palette I/O boundary. Proposed owner: existing BASE palette constants/API.

The PCX source is `u8`; display-update sources are `i8`. Preserve their promotions,
the shift, and the caller's final byte store; do not introduce a common unsigned
cast, saturation, gamma correction, or a different rounding rule. Only the scalar
conversion is shared. WinG also writes RGBQUAD channels in interleaved order,
DirectDraw sets palette flags, and system-palette exclusions are caller-owned.

## H19 — format into the shared diagnostic buffer, then log

Disposition: plausible fixed-arity statement macros; lower priority than H17.

`resourceManager::ReadBlock`, `BaseFree`, `PrintMemoryLeaks`, Bzip model/error
reporters, and `DDSD` repeat `sprintf(gText, format, ...); LogStr(gText);` across
four TUs. Small fixed-arity `LOG_FORMAT_1`/`LOG_FORMAT_2`-style macros could name
that exact two-call protocol. A parenthesized-argument-list macro is another
possible spelling, but may save too little reading effort. Natural owner:
`BASE/Misc.h`'s diagnostic interface; do not introduce a competing buffer owner.

The shared `gText` clobber is part of the contract, including when logging is
disabled. Do not move a debug-level check before formatting: `DDSD`, for example,
subsequently passes the same text to `ShutDown`. Keep format/argument evaluation,
both calls and the caller's later use of the buffer. No modern variadic-macro
requirement, new `vsprintf` call, stream machinery, implicit truncation or hidden
shutdown is justified. H17 remains preferable for existing `LogInt` callers;
these sites have distinct free-form formats, not the fixed integer-label format.

## R13 — Windows display/error initialization and painting

DirectDraw HRESULT checks and `memset` plus descriptor `dwSize` initialization
are local opportunities in `wingraph.cpp`; defer shared ownership until other
Windows-facing TUs are read. Error calls pass original file/line facts and can
return through a reentrancy guard, so a new macro must not assume `noreturn` or
replace these numbers with current source locations.

DirectDraw and WinG painting differ in invalidation rectangles, source clipping,
alignment, scrolling, surface loss/busy retry, lock lifetime and palette realization.
Existing backend dispatch functions already express the choice. Likewise,
palette initialization/update loops have distinct interleaved RGBQUAD stores and
system-entry flags; H18 is not a whole palette-loop macro. Do not impose standard
RECT edge conventions on the source's explicit `+1`/`-1` calculations.

`WinMain` recognizes WM_QUIT in its nonblocking pump; `Process1WindowsMessage`
drains messages, services sound on one interval, and can call blocking GetMessage
on another. Their superficially shared Translate/Dispatch pair does not justify
one universal pump. R05 and the pointer/value deadline distinction still apply.

## H20 — named town building-mask predicate

Disposition: credible small inline or reuse of the existing flag vocabulary.

`armyGroup::GetMorale` and `town::GiveSpells` / `town::BuildBuilding` repeatedly
test `m_buildings & IDX(TOWN_BUILDING_...)` as a Boolean. A narrow
`town::HasBuildingFlags(mask)` (any-bit test), or consistent existing `HAS`
usage, would make those conditions easier to read. Owner: `SOURCE/town.h`.
The mask storage is `u32l`; preserve that width and the distinction between a
`TownBuilding` mask and a `BuildingSlotType` bit index. `BIT(building)` callers
must not pass their slot directly as a mask. Preserve caller short-circuiting.

The special-building masks deliberately share numeric values across factions;
the helper must not infer a faction, select an upgraded dwelling, or turn an
any-bit query into an all-bits query. Only Boolean consumers are candidates:
normalizing the result changes callers that retain a masked numeric value.
Read later game/castle/AI consumers before finalizing the interface. No new
call boundary or inline expansion is assumed byte-neutral.

## R14 — data-owner algorithms and deceptively similar state resets

The map owner already has `Cell`, `GetCell`, `Row`, `Column`, `Extra` and
`mapCell::HasFlag`. Reuse is preferable to another raw-address macro where the
contract and codegen fit. Its object/overlay extra-chain searches are local
duplicates, not a cross-TU framework: allocation can relocate `extras`, so the
saved index and subsequent `Extra(ix)` reacquisition are essential. Do not
retain a stale node pointer or add a zero-index/sentinel repair during cleanup.

`ClearCellExtra` clears selected bitfields without clearing `objectMetadata`;
base map cells have different object/road/tileset and metadata layouts. A
generic zero-struct or unified tile reset changes behavior. Legacy map reads
copy the current-format prefix out of larger old records; they are not ordinary
bulk reads of the destination format. Keep allocation/free and field-write order.

Army emptiness is determined by the creature-type sentinel in membership/count
and garrison queries, not uniformly by quantity. `Dismiss` writes type then
count; `DamageGroup` writes count then type and consumes one random sample per
creature, with a first-stack survival rule. Do not replace that tail with
`Dismiss` or generic damage arithmetic. All-undead intentionally returns true
for an empty group; the homogeneous test tracks successive nonempty types, not
a deduplicated set. Strict `SwapValues` also coexists with retail `i32` temporary
swaps; generic swap modernization is not a historical finding.

Hex corpse drawing's lower range excludes the final corpse; the upper range
contains it. Facing guards, current-army extent flags, parity-dependent tower
coordinates and normal/flipped clipping belong to their explicit draw paths.
Town removal finds the last matching id then shifts the owner list, while town
spells overlap counts in a packed union. Neither deserves a generic container
or serialization rewrite during this audit.

## H21 — army-group slot presence predicate

Disposition: plausible small C++98 inline, lower priority than protocol macros.

`armyGroup::GetNumArmies` / `HasAllUndead`, `town::HasGarrison`, and
`hero::CalcMobility` / `UpdateArmies` / `HeroHandler` test whether a slot's creature
type differs from `CREATURE_NONE`. A proposed `armyGroup::HasTroop(slot)` would
name that repeated sentinel contract and shorten nested member/index expressions.
Owner: `SOURCE/armyGroup.h`; valid slot indexing remains the caller's job.

Do not add a quantity test, alter signed `i8` creature promotion, or replace
`CreatureTypeCount` (which counts matching positive-quantity stacks) with it.
The existing combat `army::IsAlive` requires both a valid type and positive
quantity and is a different domain. `IsMember` asks whether a particular type
occurs anywhere, while `GetNumArmies` counts occupied slots; neither is itself
a slot predicate. Any inline use still needs matching evidence.

## H22 — reuse the existing embarked-state accessor

Disposition: existing small inline worth wider use, not a new flag framework.

`hero::IsEmbarked` already returns `HAS(m_eventFlags, HERO_EVENT_EMBARKED)`.
The same condition is written directly in `hero::CalcMobility` / `Deallocate`
and in the masthead branch of `armyGroup::GetMorale`: two reviewed TUs.
Prefer this name when the caller asks specifically whether the hero is aboard
a boat. Preserve surrounding null guards and short-circuit order.

The accessor returns the masked integer (0 or 0x80), not a normalized 0/1;
do not silently change that existing API. Formation, visited-site and boat-id
tests are different state queries. Replacing source expressions by this inline
may still change `/Ob1` expansion shape and must be measured later.

## H23 — compact an owner's signed-id array without changing its count

Disposition: plausible narrow statement helper; retain only if naming saves effort.

`town::Deallocate` and `hero::Deallocate` both shift later `i8` ids left after a
located position and then write -1 to the old final slot. A period-style
`REMOVE_ID_AT(ids, count, position)` macro or narrow inline could name just this
compaction. Natural owner: the player id-list boundary in `SOURCE/playerData.h`.
It must not decrement count or clear current selection: those operations occur
later and have different intervening effects in the two callers.

Both callers deliberately search through the whole list and retain the LAST
matching position. The existing `game::HeroIDToHeroPos` and `TownIDToTownPos`
return the FIRST match (targeted body reads), so they are not exact replacements.
Do not switch to `memmove`, add absent-id recovery, or conceal an implicit bounds
policy. The current -1 position behavior remains caller-owned; a safe refactor
must preserve the actual input domain and not claim all malformed states are safe.
Macro arguments must be stable; adding hidden locals or an inline boundary needs
the usual codegen audit.

## R15 — hero-specific workflows and pending UI leads

Hero routines use both ordinary and seeded random streams across their related
level/skill/army routines. Keep random-call count/order, low-stat threshold tests,
floating-point-to-integer truncation and bounded skill-search behavior explicit.
`GetLevel` and `GetExperience` have distinct boundary conditions even though
their extrapolation recurrence is similar; this is one-TU sharing at present.
`GetNthSpell` tests its ordinal after `HasSpell` even when the category did not
increment it. A generic filtered iterator can change the zero-ordinal behavior.

`H2_ENUM_CLEAR_FLAG` means clearing a KNOWN-SET flag: retail uses subtraction
inside guards, while strict checking uses bit removal. It cannot replace the
unguarded formation AND-NOT assignments. Skill storage is narrowed before
clamping and effective necromancy levels can exceed the ordinary skill range.
Do not normalize all flag updates, skills or stats through one generic setter.

Hero UI reuses message union contents between broadcasts, interleaves formatting
and dispatch, and sometimes changes globals during callbacks. Do not turn every
set-id/broadcast pair into a helper that initializes fresh state. Merge/split
tails differ from `Dismiss` store order; a preloaded swap temporary also crosses
branch tests before the actual writes. Preserve these phases. `HeroView`'s early
dismissal return does not clear every global cleared on the normal exit; this
audit does not repair it or impose RAII cleanup.

Further leads, not yet promoted cross-TU candidates: shorter `NormalDialog`
arity/defaults, quick-view-to-dialog-mode conversion, maximum spell-point
calculation, skill icon frame calculation, hover invalidation, and allocation
followed by `MemError`. Hero provides concrete instances, but the complete
dialog/game/other UI consumers must still be read before assigning a common
contract. Morale/luck widget layout similarity is currently local, not grounds
for a shared screen algorithm.

## H24 — terrain lookup from an already-resolved map cell

Disposition: credible scalar expression macro or free inline.

`searchArray::SeedPosition` in `SEARCH.cpp` and `TestPossibleDirections` in
`FINDPATH.cpp` repeatedly use `giGroundToTerrain[cell->m_terrainImageIndex]`.
A proposed `CELL_TERRAIN(cell)` names this table lookup without mixing it with
movement cost or pathfinding policy. Owner: the map/terrain boundary that can
see both `mapCell` and the existing KB table; avoid a new copy of the table.

The terrain table's underlying storage is `u8`, and callers also store terrain
in signed/narrow enum slots. Preserve lookup result promotion and destination
conversion. Evaluate the cell expression once and do not add range checks or
cache across callbacks. Keep coordinate resolution outside this helper:
`advManager::GetCell` returns cell (0,0) for out-of-map input (targeted body read),
whereas the full-map accessors directly index their storage.

## H25 — non-shadow, non-dummy object-sprite test

Disposition: promising semantic predicate; final naming needs wider map review.

`TestPossibleDirections` in `FINDPATH.cpp` and the final monster scan of
`SeedPosition` in `SEARCH.cpp` share the exact conjunction: object index is not
0xFF, object tileset is not `TILESET_DUMMY`, and flag 0x80 is clear. A small
`mapCell` predicate could name this visual-object property instead of restating
three packed-field conditions. `mapcell.h` names 0x80
`MAP_CELL_OBJECT_SHADOW_ONLY`; the search code calls the same value BLOCKED.
Record that naming conflict rather than silently choosing a new game rule.

This predicate alone does NOT mean a tile is impassable. Direction masks, the
current/neighbor/below-neighbor choice, trigger handling and terrain all remain
caller-owned. Keep that surrounding short-circuit order, the sprite/tileset/flag
test order, and no extra coordinate lookup. A macro repeats its cell expression,
so it needs a stable local; an inline changes expansion evidence. Do not merge
overlay existence, map-extra visibility, or occupied-hero tests into it.

## H26 — adventure-map coordinate bounds

Disposition: small shared predicate with distinct source-shape variants to audit.

`SeedPosition`'s adjacent scan and `TestPossibleDirections` use signed x/y bounds
against the current global `MAP_WIDTH` / `MAP_HEIGHT`. A proposed
`IN_MAP_BOUNDS(x, y)` could express the four-condition contract at the KB map
boundary, without obscuring what happens when it fails. Keep nonnegative and
upper-bound tests explicit; no unsigned-subtraction rewrite or added clamp.

`PushPoint` instead rejects `x > MAP_WIDTH - 1` (and similarly y), while
`advManager::GetCell` checks both lower bounds before both upper bounds. These
are related but not automatically identical source-shape instances. Preserve
evaluation order and the actual width/height input domain before replacement.
The monster scan's outer y-loop currently uses MAP_WIDTH, not MAP_HEIGHT; a new
helper must not silently repair that separate loop. This is not H02's narrowed
widget-local geometry or combat's existing `ValidHex`.

## H27 — reuse the existing search-node accessor

Disposition: existing inline; do not introduce another flattening macro.

`FINDPATH.cpp` uses `searchArray::GetNode(x,y)` in both point insertion routines.
`SEARCH.cpp`'s `BuildPath` / `SeedPosition` repeatedly spell the same node access
through `GetColumn(x)[MAP_WIDTH * y]`. The existing reference-returning `GetNode`
is the natural readability candidate, owned by `SOURCE/searchArray.h`.

Preserve `searchNode` rather than the overlaid `searchCell`, and retain the
correct global stride and caller coordinates. Same-sized records do not make
their fields interchangeable. These address spellings regroup pointer/index
operations; as with the documented MAP_EXTRA variants, replacement needs byte
and inline-expansion evidence. `GetRow` also takes an explicit width, so it is
not universally replaceable with the global-stride accessor.

## H28 — combat army lookup by side and index

Disposition: plausible small owner inline; no new lookup policy.

`hexcell::DrawLowerDeadOccupants` / `DrawUpperDeadOccupant` / `DrawOccupant`
and `searchArray::SeedCombatPosition` repeatedly address
`gpCombatManager->m_armies[IDX(side)][index]`. A proposed
`combatManager::GetArmy(side,index)` would make this relationship easier to
read and prevent repeating the long storage expression. The complete owner
header currently has no such accessor.

Return the existing object, not a copy. Preserve side/index values, sentinel
guards, reference/address use and evaluation frequency; do not infer a living
army, filter by quantity, clamp the index, or introduce a checked container.
Storage has 21 slots per side while several operational loops visit only 20;
the caller's range is significant. A separate current-army accessor is only a
lead until the corresponding combat consumers are read. Keep raw-byte evidence
as the criterion for retaining a new inline boundary.

## R16 — search algorithms, packed state and direction systems

Adventure directions are eight compass directions; combat has six neighbors
plus two wide-creature pseudo-directions. `OppositeMapDirection` is already the
adventure helper, while `OppositeDirection` has the combat modulo/special-case
contract. Do not unify them or replace all adjacency with a six/eight-way table.
`GetAdjacentCellIndexNoArmy` maps pseudo-directions differently from the
facing-sensitive army method. `GetBestDirection` has ordered fallback priorities;
its repeated tests are local and not a reason to change tie-breaking.

The two priority-queue insertions live in FINDPATH, not separate TUs. Adventure
insertion uses file-static temporary state, a right-shift midpoint, extra visited
flags and whole-node copying. Combat uses locals, division, a high-water count
and selected cell-field stores. Keep queue-count aliases, narrowing, distance
ties and record writes separate. Shared serialization/sort/container machinery
would hide more than it clarifies.

Adventure path reconstruction uses the AND of coordinate mismatches and resets
length on capacity exhaustion; combat uses a different packed path view and
keeps a capped length. Preserve the three/four-byte overlay offsets, including
the explicit adventure starting index, without inventing a generic path buffer.
`SeedPosition` retains static state across continuation calls, target-cost
windows and monster associations. Its processed counter, scan bounds and
apparently redundant conditions are not repaired by this audit.

Combat pathfinding preserves moat state and temporarily changes creature speed
through separate owner routines. Some member reads precede the later null check;
no cleanup macro may assume it can safely move validation or hoist pointers.
Front/rear placement and drawbridge exceptions are local semantic predicates
to compare with later army code, not yet a universal passability API. Existing
`ValidHex` is a 0 <= hex < 117 Boolean predicate (targeted body read), not every
other constant named MAX/COUNT in combat. Monster `HAS` tests already expose
flying/wide flag intent; a generic new flag framework is unnecessary.

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
The full string-memory owner TU still needs deliberate reading.

B04 adds `font::DrawBoundedString`'s allocated whole-string copy. The buffer
allocation in `font::LineLength` has no immediately corresponding whole-string
copy and is not an exact instance.

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

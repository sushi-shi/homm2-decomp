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

B15 adds the initial type/command/id triple in `UpdateCombatSystemOptions`.
Its later broadcasts intentionally reuse command and payload state.

B17 adds `combatManager::CombatMessage(text, ...)`: its COMBAT_MESSAGE aliases
denote the same WIDGET / SET_TEXT values and exact three-store prefix. The second
line's id/text-only broadcast intentionally reuses that header.

B24 adds complete type/command/id prefixes in the combat reward, victory/loss
and surrender panels, and WinCombatHandler's final animation update. Keep the
following text/value write and each broadcast explicit. Its resource-animation
setup writes the text pointer before selecting an id; scattered stores and
subsequent id-only broadcasts are not contiguous instances of the macro.

B25 adds QuickInfo, TownQuickView and TownGate's initial list message in ADVMGR.
Locator updates and options panels often reuse a previously established type or
write id before command. DisableButtons/EnableButtons put data before id; these
are not additional exact three-store prefixes.

B27 adds dpWaitForExtraGuests and wsWaitForExtraGuests, plus Winsock's accepted-
guest status message. Text-buffer choice, broadcast receiver and following
DrawWindow call remain explicit; those are not part of the header macro.

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

B16 adds the raw effect-frame lookup in `army::PowEffect`; `army::SpellEffect`
already uses the API. Keep current frame and icon selection in the caller.

B25 adds UpdBottomViewHero's raw byte-address lookup; HeroQuickView and
TownQuickView already call GetIconEntry. Icon choice, frame, centering arithmetic
and final coordinate narrowing remain explicit.

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

B15-B16 add the upper-then-lower [-20, 20] attack/defense difference clamp in
`combatManager::KeepAttack` and `army::DamageEnemy`. It is a separate ordered
variant, not automatic reuse of the lower-first spelling. The surrounding
damage arithmetic has different floating-point, rounding and quantity stages.

B19 adds lower-then-upper scalar clamps in `AddBolt` / `DoLuck` (upper comparisons
have reversed operands) and an upper-then-lower branch-distance clamp in
`ChainLightning`. `DrawBolt` also updates matching float coordinates on a clamp;
it is not the same scalar-only idiom.

B25 adds the per-axis lower/upper map-origin clamps in ScreenScroll.
ProcessSelect/ProcessHover interleave X and Y bounds differently; do not regroup
their stores into two scalar clamps. SummonBoat's fizzle rectangle instead
applies lower coordinate bounds then shrinks width/height at the far edges;
it is not the same scalar lower/upper clamp.

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

B15 confirms the exact mask/priority/active sequence at the end of
`combatManager::Open`; combat resource setup and name copying stay outside it.

B25 confirms the same final three stores in advManager::Open. Restoring the
saved sound volume and copying the manager name are outside this protocol.

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

B27 adds wsnet_init's current-executable fullscreen test. Its dialog and
SetFullScreenStatus(false) call are separate behavior, not part of the accessor.

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

B27 adds service-provider/session enumeration and guest/startup logging in
DirectPlay/Winsock, with one or two values followed by the established sentinel
tail. REMOTE's failed-send log deliberately includes two numeric zero slots;
do not drop them as though they were LOG_UNUSED_VALUE.

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

B27 adds dpEvaluateMessage's unknown-message diagnostic and DPSD's formatted
error followed by LogStr(gText). Winsock/DecodePacket also use other local or
transport-owned buffers; they are not instances of the fixed-gText protocol.
DPSD's subsequent ShutDown remains outside the helper.

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

B15 adds `combatManager::SetupCombat`, `InitNonVisualVars`, `LoadIcons`, and
`SetupAndLoadObstacles`. Castle/moat Boolean assignments explicitly normalize
the mask; faction-specific shrine and fortification guards remain caller-owned.

B17 adds the knight/rainbow background and moat tests in `DrawBackground` /
`DrawFrame`. Preserve castle and faction short-circuit conditions outside it.

B24-B25 add command turret checks and adventure UpdateTownLocators,
UpdBottomViewKingdom and TownQuickView castle checks. Counting towns, known-town
bits, faction-specific frames and mage-guild information levels remain separate.

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

B15 adds group-presence tests in `CombineGroups`, `Close` and `LoadArmies`.
The count/type reset at the start of `UpdateArmyGroup` instead matches the
existing `Dismiss` type-then-count order; it is a possible existing-method reuse,
not a new reset macro. Preserve initialization and store sequencing if measured.

B21/B25 add tactical AI's group-slot queries and adventure bottom-view /
HeroQuickView / TownQuickView counting and skipping. They still test the type
sentinel alone, not positive quantity or combat army::IsAlive.

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

B23 adds MoveHero's movement/event/adjacent-monster guards and both hero checks
in ValidMoveWithEvent. Preserve the nested lookup of the destination hero and
do not replace the separate boat-object or occupied-boat-id tests.

B25 adds DrawCell's hero-versus-boat cursor selection, SetHeroContext and the
DimensionDoor/TownGate guards. Teleport/demobilize writes to the event flags are
mutations, not occurrences of this read-only accessor.

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

B15 adds `combatManager::SetupCombat`'s battlefield terrain lookup. The source
can assign a null battlefield cell before that lookup; this audit does not add
an early guard or silently change the caller's current preconditions.

B23 adds MoveHero's current/destination terrain queries for walking samples,
water handling and music. Keep those individual cell resolutions and query
times: callbacks and the actual move separate some uses.

B25 adds adventure hover/search, cursor context, travel-spell and environmental
sound consumers. The full GetCell body confirms its (0,0) fallback; raw full-map
and map-extra accesses do not acquire that behavior by sharing a terrain name.

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

B23 adds ValidMove's exact index/tileset/flag conjunctions on destination and
neighbor cells. Its `CURSOR_OBJECT_PASSABLE_FLAG` is also 0x80, another name for
the same stored bit. These sites do not call IsShadow; do not introduce a new
tileset/frame-based shadow test. North/south direction, overlay and trigger
conditions remain outside the proposed three-field query.

B25's FindAdjacentMonster uses the ordered negation (index equals 0xFF OR dummy
tileset OR flag 0x80 set), only for a monster above the origin. Each short-circuit
arm currently calls GetCell again. Passing one resolved pointer to an inline
would change the lookup count; retain this as a source-shape variant requiring
measurement, not an automatic replacement. Digging and boat-placement checks
also inspect other fields and are not this predicate.

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

B15 adds the exact x-lower/x-upper/y-lower/y-upper form in
`combatManager::MoreTreesNear`. Its radius-zero repeated center samples and
tree-versus-mountain classification are outside the bounds predicate.

B23 adds ValidMoveWithEvent's x-lower/x-upper/y-lower/y-upper rejection using
`> MAP_WIDTH - 1` / `> MAP_HEIGHT - 1`, like PushPoint's upper-bound spelling.
These are runtime dimensions, not an assumed fixed 144-square map. ValidMove
instead tests the translated map origin against -7 and dimension-minus-8;
its later center coordinates are not interchangeable helper inputs.

B25 fully confirms GetCell and adds InsertSound's both-lowers-before-both-uppers
form, plus QuickInfo/SummonBoat's axis-grouped variants. FindAdjacentMonster's
strict interior fast path excludes the edge cells; MapExtraPosAndAdjacentsSet
reads its center before checking neighboring bounds. Neither is a universal
bounded-map iterator, and no helper should move the center access under a guard.

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

B16-B17 confirm repeated current, target and mini-view owner lookups in ARMY and
DRAWING. A possible current-army spelling must use `m_currentArmySide`, not
`m_currentSide`: physical and controlling sides are not interchangeable.

B18-B20 add flight targets, spell/corpse/mirror/effect targets and AI lookups.
Some spell routines explicitly combine `m_currentSide` with `m_currentArmyIndex`;
do not silently substitute the physical-current-army pair at those sites.

B21 adds all AI mask/target/strength/approach consumers, including reversed
`index[array]` notation. Preserve that operand ordering when judging exact expansion.

B24 adds the command loop, direction selection, rounds, actions, cycling,
rewards and mini-view callers. RightClick resolves side from its hex argument
but index from `m_selectedHex`; SetupSmallView uses controlling side with the
current army index. Preserve these actual pairs rather than substituting a
current-army or occupant lookup that silently changes them.

B15 adds direct owner-array accesses in `LoadArmies`, `UpdateArmyGroup`,
`CheckApplyGoodMorale`, `CheckApplyBadMorale`, `KeepAttack` and
`ExperienceValueOfStack`. `GetNextArmy` also uses pointer-plus-index spellings;
their operand order requires review before replacing them with the same accessor.

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

## H29 — normal spell-point capacity

Disposition: credible small hero inline, not a spell-point cap.

`HeroHandler` / `SetupHeroView` in HERO and captain setup in
`combatManager::SetupCombat` compute `Stats(HERO_PRIMARY_KNOWLEDGE) * 10`.
A proposed `hero::NormalSpellPointCapacity()` would name the same rule in the
display and initialization paths. Owner: `SOURCE/hero.h`; retain the existing
`Stats` call, signed result promotion and integer multiplication. Captain setup
subsequently narrows to `i16`; display callers do not.

Do not clamp `m_spellPoints`, infer spellbook possession, or reinterpret capacity
as the current balance. Above-normal spell points are explicitly allowed by the
hero UI. No additional minimum/maximum or rounding rule belongs in this helper.

B25 adds HeroQuickView's normal-capacity display with the same Stats call and
integer factor. Its information-visibility conditions stay outside the helper.

## H30 — shorter calls for a text-only NormalDialog

Disposition: strong fixed-argument readability candidate; owner body review pending.

`hero::ViewStat`, `Dismiss`, `CheckLevel`, `HeroHandler` and
`CombatSystemOptionsHandler` repeat `NormalDialog(text, mode, -1, -1, -1, 0,
-1, 0, -1, 0)` across HERO and CMBTMGR. A fixed-arity macro such as
`NORMAL_TEXT_DIALOG(text, mode)` can preserve that exact ten-argument call;
default arguments on the existing declaration are another period-plausible
option. Owner: the `NormalDialog` interface in `SOURCE/KB.h`.

Keep mode explicit, argument evaluation unchanged and the result read by the
caller. Artifact and secondary-skill dialogs have non-default trailing slots
and are not instances. Do not hide `sprintf`, `gText` mutation, sound playback,
button-result interpretation or dismissal side effects. The declaration and
these callers are read; the large NormalDialog body and wider consumer set
still require their full planned review before finalizing defaults.

B19 adds `ViewSpells` and mirror-creation failure. The NORMAL_DIALOG sentinel
names denote the same -1/zero tail; their long message formatting stays outside.

B24-B25 add command help/result dialogs and adventure search, travel, panels,
system-options and visions dialogs. Raw mode 1 remains an explicit argument;
OPTION_DIALOG_NONE and NORMAL_DIALOG_NO_RESOURCE both denote the same -1 tail.
Resource/artifact-bearing calls, including movement-event dialogs, are excluded.

B26-B28 add remote retry/timeout dialogs, DirectPlay/Winsock/NetBIOS setup and
modem command/response waits. These share the exact trailing slots, not one
dialog behavior: keep mode, giWaitType, gbFunctionComplete checks, retry choices
and shutdown calls explicit. An informational dialog is not a network wait.

## H31 — check an already-stored allocation with the existing error handler

Disposition: plausible narrow statement macro; usefulness versus indirection to rank.

`HeroView`, `DoHeroSplit`, `combatManager::Open` and `CombatSystemOptions` use
`new`, store the resulting pointer, then call `MemError()` if it is null.
A proposed `CHECK_ALLOCATION(pointer)` could name only the null-test/error-call
pair. Keep allocation, assignment and destruction outside it; no allocator,
exception, RAII or constructor-argument framework is needed. Owner: the existing
`MemError` interface in KB, without forcing new dependencies into BASE headers.

The check must not inject a return/throw, assume the handler is nonreturning,
check before assignment or evaluate a side-effecting pointer expression twice.
Different pointer store widths and object types remain untouched. Use the
existing error handler rather than replacing it with an assertion or a different
failure path. Allocation sites with no current check are not added instances.

A targeted complete read of `MemError` confirms that reentry immediately returns
when `gbInMemError` is set; otherwise it logs, formats a message and calls
`ShutDown`. A nonreturning assumption is therefore particularly inappropriate.

B19 adds palette allocation checks in `Armageddon` and `DoBolt`; nearby bolt,
flight, ripple and vaporize allocations without checks remain unchanged.

B24-B25 add stored window/widget allocations in command and adventure UI,
including Open, bottom/quick views and adventure/options panels. Unchecked
TownPortalWin, border/fizzle buffers and text buffers are not newly checked.

## H32 — preserve the dialog result and request handler completion

Disposition: strong small statement protocol shared by two reviewed handlers.

At exit, `HeroHandler` and `CombatSystemOptionsHandler` copy the current widget
id into `gpWindowManager->m_dialogResult`, write widget id 10, then write widget
command 10 (`WIDGET_COMMAND_DIALOG_SELECT`). A proposed
`FINISH_DIALOG_MESSAGE(message)` can name just these three stores, in that order.
Owner: the existing window-manager/message boundary. Keep the caller's
`return MESSAGE_DISPATCH_FORWARD` explicit rather than hiding control flow.

The already-read `heroWindowManager::DoDialog` treats handler-forward plus the
dialog-select command as termination without overwriting the saved result;
that differs from its window-broadcast completion path. The helper must not
initialize the message type, clear other payload slots, set result to 10 or
replace the handler result with an executive close event. Both callers already
have the relevant widget-message context; retain it and the original button id.

B24 adds WinCombatHandler's ordinary exit and timeout exit. The latter first
sets the message type, then performs the same three stores, then clears the
timeout; neither extra operation belongs inside the shared completion macro.

B25 adds TownPortalHandler, APanelHandler, CPanelHandler and SystemOptionsHandler.
The latter's SYSTEM_OPTION_FIRST alias is also 10. DimensionDoorHandler only
writes the final id/command pair and deliberately does not copy a global result;
it is not an instance. Confirmation/cancel interpretation remains caller-owned.

## R17 — combat setup, reset and presentation are not one generic algorithm

`CombineGroups` has two passes with existing `IsMember` / `Add` / `Dismiss`
calls; its second inner loop does not break after placement. Generic merging
would change its sequence, including sentinel treatment. Army loading first
clears quantity/type in slot-major order, then calls InitClean in side-major
order. Hex initialization differs from the constructor's occupant index and
field-store order. Do not replace these with one clear-all routine.

Good and bad morale use different random ranges, guards, network handling and
bonus flags. Turn selection distinguishes physical army side from current
controlling side under hypnotize and re-enters deferred rounds. Existing
`OppositeCombatSide` can be compared against XOR/subtraction forms only on the
actual valid-side domain; no generic toggle or living-army filter is implied.

Catapult/keep/wall state values overlap but are not a uniform destroyed flag.
Obstacle rolls include a high endpoint equal to a declared type count; map
setup still makes an otherwise-unused random draw. Neither is corrected here.
Keep targeting prioritizes spell-disabled, shooter/flyer status and value with
strict tie behavior, while grid picking has special hero/ballista regions and
signed remainder geometry. Generic random-choice or rectangle helpers would
hide those contracts.

Animation leads for comparison with ARMY and rendering TUs: inclusive extent
updates, speed-scaled timer deadlines, projectile-angle frame selection, and
quantity-dependent creature names. Shared extents have multiple store/clamp
orders and sometimes 442 versus 479 output limits; never infer a whole drawing
loop macro. Door raise/lower sequences and global visibility/palette lifetime
remain explicit, as do settings that use `1 - value` rather than Boolean NOT.

## H33 — four elemental creature types

Disposition: credible small creature predicate, with ordered variants to preserve.

`combatManager::UpdateArmyGroup` tests that a type is not earth, air, fire or
water elemental. `army::Init`, `LeaveNoBody` and `SpellCastWorkChance` contain
the corresponding four-type positive test. A proposed
`IsElementalCreature(type)` belongs beside the creature domain in KB_TYPES;
it names exactly those four types, not all creatures that share one effect.
An expression macro retains short-circuit comparisons and needs a stable operand;
a small inline is plausible if its expansion/argument evaluation is verified.

Keep summoned, mirror, undead and giant/titan conditions outside this helper.
`ProcessDeath` has the same four exclusions in a different comparison order;
record it as a variant, not an automatic exact expansion. `ModifyFrameInfo`
intentionally includes only three elementals because fire supplies the baseline;
replacing that list with the four-type predicate would be wrong.

B24 adds DoVictory's exact earth/air/fire/water exclusions and AddArmy's exact
positive four-type tail. Skeleton exclusion, light-palette flag, mirror flag,
quantity and slot-reuse policy remain caller-owned.

B25 adds DoVisions' earth/air/fire/water exclusions in that same order. Ghost,
hideous-mask, CanJoin and relative-strength checks do not belong in the helper.

## H34 — clamp the accumulated combat extents

Disposition: strong four-statement macro hypothesis for exact-order sites.

`army::Walk` and the projectile phase of `combatManager::CatAttack` independently
clamp global minimum X, minimum Y, maximum X, maximum Y, in that order, to
0, 0, 639, 442. A proposed `CLIP_COMBAT_EXTENTS()` belongs with the combat
extent declarations. Name only those four conditional stores; the catapult's
subsequent minimum-visible-Y adjustment remains explicit.

Do not reorder axes, normalize an inverted rectangle, or include draw/visibility
flags. The catapult impact phase orders X-min/X-max/Y-min/Y-max differently;
other routines use ternaries, 479 or other limits. Those are not exact instances.
This is screen-bound clipping, not H07's two-sided clamp on a single variable.

B17 adds `combatManager::DrawFrame`'s exact four-store sequence after expanding
all four bounds by one pixel. Keep that expansion and extent-only early exit
outside the clamp. Grid-copy clipping has different limits and local destinations.

B18 adds the exact four statements in `army::FlyTo`. B19's `DoBlast` uses the
same bounds/order but reverses the operands of its upper-bound comparisons;
`DoBolt` also uses local destinations. Keep both as explicit variants.

## H35 — update an inclusive screen region

Disposition: strong short-call macro, distinct from whole-combat redraw.

`army::Walk`, `SpecialAttack`, `PowEffect`, and `combatManager::CatAttack` /
`ShootMissile` call `gpWindowManager->UpdateScreenRegion(left, top,
right - left + 1, bottom - top + 1)`. A proposed
`UPDATE_INCLUSIVE_REGION(left, top, right, bottom)` would name the inclusive
endpoint-to-size conversion at the window-manager interface. Operands must be
stable and side-effect-free because left/top are repeated; preserve integer
promotions and the caller's global versus saved/local extent values.

Do not clip, cache new bounds, or add drawing and timer work. The existing
`combatManager::UpdateCombatArea` is not interchangeable: its fully read body
checks visibility/window state, updates the entire 640-by-443 area and writes
`gbEnlargeScreenBlit` false then true. None of that belongs in this conversion.

B17 adds `UpdateMouseGrid`, the extent branch of `DrawFrame`, and stale-view
erasure in `DrawSmallView`. Their surrounding save/restore/blit-flag protocols
remain separate; the mini-view's final size-based update is not an instance.

B18-B19 add `FlyTo`, mirror sliding, blast segments and earthquake impacts.
SaveFizzleSource/FizzleForward and direct BlitBitmapToScreen calls also convert
inclusive bounds, but have different APIs and side effects; they are not instances
of the same UpdateScreenRegion call macro.

B24's AddArmy has those fizzle-API variants. Full-screen updates in the command
key handler and DoVictory pass 639/479 directly, not endpoint-plus-one sizes;
preserve those arguments instead of normalizing them through this helper.

## H36 — speed-scaled combat animation deadline

Disposition: plausible narrow expression macro; float evaluation is the contract.

`army::SpecialAttack`, `PowEffect` and `combatManager::ShootMissile` compute
`static_cast<i32>(KBTickCount() + delay * gfCombatSpeedMod[gConfig.combatSpeed])`.
A proposed `COMBAT_DEADLINE(delay)` at the combat timer interface can name that
exact expression; leave timer index, assignment and polling outside it. The
catapult has the same-width `i32l` cast and remains a type-spelling variant
until matching verifies any unification.

The tick is converted to floating point for the addition, followed by one final
integer conversion. Casting the scaled delay before adding the tick is not the
same operation. Walking/attack duration sites divide after floating multiplication
by frame count; do not replace that with integer-divided input. `SpellEffect`
reverses the multiplication operands, another explicit variant. No generic
wait/pump abstraction or wraparound-policy change is proposed.

B17 confirms the reversed-multiplication variant in both DrawFrame deadline
sites. Preserve the difference between `updateScreen == 1` and general truthiness
and the distinct timer behavior of extent-only and whole-area branches.

B18's flight deadlines divide after scaling, with an additional double `1.3`
factor on the vampire path. B19 adds direct expression instances in Fireball,
ElementalStorm, Armageddon, ChainLightning, MirrorImage, DoBlast, Resurrect and
Earthquake. MeteorShower uses a double 112.5 multiplier and reversed operands;
DoBolt also reverses operands. Preserve timer 0 versus timer 1 and local deadlines.

B24 adds Main's sound-poll deadline and CycleCombatScreen's final deadline,
using float 75/150 factors in the exact expression order. WinCombatHandler,
victory fades and idle-animation jitter have different timer arithmetic and
random-call contracts; they are not instances.

## H37 — select a projectile frame from angle midpoints

Disposition: small shared loop/inline candidate; lower confidence than expression macros.

`army::SpecialAttack` and `combatManager::ShootMissile` scan from angle index 1
until `(angles[k] + angles[k - 1]) / 2.0f < degrees`, choosing `k - 1`, or the
last frame if no strict crossing occurs. A proposed
`ProjectileFrameForAngle(angles, count, degrees)` belongs with the existing
projectile/frame declarations in ARMY. Preserve the scan order, strict inequality,
float expression parentheses and dynamic byte-sized count versus fixed nine-count
caller contract. Establish the valid count domain; do not silently add bounds repair.

Keep angle calculation and vertical cases outside it. The army needs the angle
for selecting its shooting animation as well as the projectile frame; the generic
missile does not. Their slope conversions differ, including an explicit VC6
floating-instruction constraint. The rest of the two projectile loops is not a
shared algorithm (R18), and a callable helper is not assumed byte-neutral.

## H38 — creature display name for a quantity

Disposition: credible shared expression macro or existing-domain inline.

`army::CheckLuck`, the genie branch of `DoAttack`, and
`combatManager::CheckApplyBadMorale`, `CheckApplyGoodMorale` / `KeepAttack` choose
`count <= 1 ? gArmyNames[IDX(type)] : gArmyNamesPlural[IDX(type)]`.
A proposed `CreatureDisplayName(type, count)` belongs beside those tables in KB.
Retain the actual count (stack size versus killed quantity), <= rather than ==,
table element constness and the unselected arm's lack of evaluation. A macro
requires stable arguments; an inline must not introduce premature evaluation.

This is the game's two-table selection, not a general Russian pluralization or
case-conversion API. The ordinary army damage message lowercases a copied
singular name in `gTargetName`; that conditional is not the same raw-table helper.
CP1251 casing, formatting, sound and text-buffer mutation remain outside it.

B19 adds the cold-ray, magic-arrow and lightning damage messages in CastSpell.
`SpellMessage` and `ShowSpellCastFailure` use `quantity == 1`, not <= 1, so are
not exact instances. Resurrect has separate plural/singular format branches;
do not collapse their different format strings into this table lookup.

## H39 — approximate distance between combat cell pixel centers

Disposition: useful small forwarding helper, owner placement needs dependency care.

`searchArray::FindCombatPath` and `army::GoBerserk` pass the two cells' X/Y
coordinates to the existing `QuickDistance`; the berserk routine repeats it for
nearest-army and two-target selection. A proposed combat-cell distance helper
would name that coordinate extraction and preserve the existing metric/API.
Natural domain: combatManager/searchArray boundary, without creating a header
include cycle or a duplicate implementation of `QuickDistance`.

This metric is max(deltaX, deltaY) plus half the smaller delta, measured on pixel
centers. It is not hex-step distance or Euclidean distance. Preserve actual cell
indices and coordinate-read order; do not add validation, change signed arithmetic,
reorder operands, or absorb targeting/tie-breaking. Calling versus inlining must
be measured before retaining a source change.

B21 adds `GetClosestArmy`. Its subsequent distance weighting minus total hit
points is a separate ranking expression, not part of the shared metric.

## H40 — clear the live occupant identity of a combat hex

Disposition: credible two-store owner method/macro, not a full cell reset.

`combatManager::MakeCreaturesVanish` and `army::ProcessDeath` store
`m_occupantSide = COMBAT_SIDE_NONE` then `m_occupantIndex = -1` into a hex cell.
A proposed `ClearOccupant()` on hexcell, or a narrow statement macro at that
owner, would name those two ordered stores only. Retain the caller's choice of
front/rear cell and its wide-creature/corpse-capacity guards.

Do not clear the occupant frame, corpse stack, path fields or obstacles. The
constructor uses a different index value and `Walk` clears index before side;
neither is an exact instance. Repeated pointer/index evaluation in a macro needs
stable operands; a method's single `this` evaluation and call expansion require
matching evidence. Adding an all-purpose reset would erase these distinctions.

B19 adds the teleport source clear stores in CastSpell; COMBAT_HEX_EMPTY aliases
-1 and occupant-frame reads still follow those stores. B18's flight clear is
index/side/frame instead, and its landing writes the manager's current identity;
neither is this two-store helper.

## R18 — army animation, damage and spell protocols remain explicit

`gbNoShowCombat` and `m_nonVisualCombat` guard different levels of work.
Resource disposal/null stores in `army::FreeResources` stay inside its guard;
combat-manager icon release does not have the same null-store protocol. Animation
flags often reset to fixed false/true values rather than restoring an old value.
Do not add a generic RAII/resource/visibility guard or substitute facing for side
in wide-creature placement and adjacency rules.

Projectile loops differ in background clipping and first-frame save/draw behavior,
and in mage/lich special paths. One army background-Y test uses 640, not its
nominal height; a shared clipping helper must not silently repair it. `PowEffect`
spans multiple armies with non-reset phase counters, consults this army's pending
sequence while drawing another, and orders death before petrification. These
are not grounds for a universal animation-loop macro.

Damage and recursive retaliation preserve ghost/remote/genie/mirror/life-steal
branches, lost-hit-point carry, artifact timing and temporary facing changes.
Keep damage uses a different base-value/rounding calculation. `SpellCastWorks`
still consumes an SRandom(1, 99) draw at both zero and full chance; death uses
the other random stream. Corpse insertion checks both wide-cell capacities,
and immediate/no-body/mirror paths have different vanish and clear schedules.

Spell IDs, influences and visual effect IDs are separate domains. Existing spell
duration is compared before narrowing to a byte; no saturation is introduced.
AFTERMOVE cancellation does nothing; AFTERDAMAGE's hypnotize guard does not imply
an explicit hypnotize cancel. Restoring flying/speed, anti-magic clearing and
stone/steel-skin precedence remain local, ordered behavior.

Local/reuse leads: `BuildTempWalkSeq`'s two memcpy-and-length updates; private
CP1251 folding (promoted to H69 after the complete GAME read in B29; wider
KB/NEWGAME/EVENTS callers still to read); and
`army::Strength` versus keep targeting's reversed multiply operands and signed
destination. Keep byte-conversion boundaries and exact multiplication types.
Three-elemental/upgrade exclusions in frame metadata, empty `WaitSample` /
`ResetPath` calls, STAND-frame-zero geometry and repeated icon lookups must not
be "cleaned up" merely because a different formulation looks conventional.

## H41 — grow an accumulated rectangle in a fixed order

Disposition: genuine shared four-statement idiom; spelling needs a readability test.

`icon::CombatClipDrawToBuffer`, `army::Walk`, and `combatManager::UpdateGrid`
test incoming left/top against accumulated minima, then incoming right/bottom
against maxima, updating in left/top/right/bottom order. A narrow rectangle-grow
macro at the existing graphics extent interface could expose that shared intent.
All eight scalar operands must stay available: callers use global bounds, local
saved bounds and SLimitData/hex fields. Do not introduce a new rectangle temporary
or copy/normalize the storage merely to make a helper call shorter.

Preserve strict comparisons, conditional-only stores and repeated source reads;
aliasing, promotions and destination widths matter. No clipping, +1 conversion,
empty-rectangle policy or visibility flag belongs here. `UpdateMouseGrid` has
the same geometric idea with reversed comparison operands; projectile paths
interleave growth and clipping in a different axis order. Those are variants.
An eight-argument macro may be less readable than the four visible statements;
rank below the stronger fixed-bound and short-call candidates if so.

B18 adds FlyTo's saved-extent accumulation. B19's bolt accumulation instead
orders X-max/X-min/Y-max/Y-min before and after drawing; do not normalize it.

B25's ComboDraw grows X-min/X-max/Y-min/Y-max, then separately converts cells
to pixel bounds and clips them. It is another ordered variant, not automatic
reuse of combat globals. Adventure UpdateScreen uses max-minus-min extents
without +1; it must not inherit H35's inclusive-region conversion.

## H42 — default optional combat sprite drawing arguments

Disposition: strong default-argument candidate on an already-reviewed API.

Army status/effect drawing, `combatManager::CatAttack`, and rendering in
`DrawFrame` / `DrawSmallView` repeatedly call `icon::CombatClipDrawToBuffer`
with final arguments `offset = 0`, `colorTable = NULL`, `yModify = NULL`.
The owner declaration and complete implementation are read. Period-plausible
defaults on those three parameters in `BASE/icon.h` would allow the existing
method name to carry the call, preserving its ABI and full supplied argument list
after compilation. A fixed-arity forwarding macro is an alternative, but adds
another name to an already semantic API.

Keep icon, coordinates, frame, limits and orientation explicit, and preserve the
return value: status and mini-view callers test ICON_DRAW_SKIPPED. Nonzero offsets,
palette translation and vertical modification are not stripped; the full creature
draw uses all three. Do not infer orientation from side/facing, replace the call
with DrawToBuffer, or add drawing flags and screen updates. Even defaults still
need the normal matching verification before retention in canonical source.

B19 adds MeteorShower, ElementalStorm and DoBlast. MeteorShower passes a null
limits pointer: retain it and its existing extent-flag assumptions, rather than
adding a new limits object or treating the shorter call as always safe to compute.

## R19 — combat rendering is ordered stateful work, not a scene framework

CombatMessage copies into private mutable storage before visibility checks and
temporarily terminates the first newline. History expiration has strict versus
non-strict comparisons and separate pending state. Do not add a general line-wrap
or clear-message macro, mutate a caller's literal, or replace NoShowCombatLog's
local buffer with H19's shared gText buffer.

Grid display tracks old/current shading and a separate grid-was-showing flag;
some no-change paths return before copying current state. Mouse-grid validation
rejects edge columns as well as out-of-range indices, unlike ValidHex. Its bottom
clipping is one-axis and its new bitmap currently has no MemError check. These
are not reasons to add validation or failure behavior during a helper extraction.

DrawFrame traverses distinct corpse, obstacle, hero, wall and army phases, reversing
columns for particular castle rows and separately drawing special occupants.
Do not hoist its automatic wall tables or merge all cell traversals. Hero overlay
coordinates and orientations differ between extent and drawing paths. Moat redraw
depends on front/rear movement globals and drawbridge state; geometric simplification
would conceal the actual layer order.

Extent flags sometimes reset to fixed values, while CombatMessage and mouse-grid
paths preserve old values. Cursor readiness and PollSound have both normal and
finish-label writes/calls. Keep early exits and timer scheduling explicit rather
than absorbing them into a guard. Whole-area and extent screen updates remain
different even when their resulting rectangle happens to agree.

DrawSmallView's recursion guard permits calling DrawFrame during erasure; its
old-position erasure and later rendering have different limit-state behavior.
Stat formatting preserves signed/unsigned promotions (including the hit-point
argument with its current %d format). Repeated bounded labels and morale/luck
icons are local leads, not a universal stat-panel macro. Spell scanning trusts the
stored count and searches influences without a new bound; layout tables and
two separate GetIconEntry calls remain intact. Quantity-format selection is not
H38's creature-name table selection.

## H43 — an in-range combat hex outside the edge columns

Disposition: credible narrow predicate, distinct from ValidHex and passability.

`army::CanFit`, `combatManager::UpdateMouseGrid` and `MirrorImage` test both
0 <= hex < 117 and column != 0/12 in a 13-column grid. A proposed
`IsInteriorCombatHex(hex)` belongs beside ValidHex in CMBTMGR. CanFit already
calls ValidHex for the range portion; the other sites spell it out. A shared
Boolean contract does not establish that replacing that existing call with an
inline/macro, or adding a call elsewhere, is byte-neutral.

Keep occupancy, blocked state and fitting-side attempts outside it. HasValidSpellTarget
and area targeting skip border columns under an already-established range; they
are partial/guarded instances, not grounds to add extra checks everywhere.
In CanFit, an interior check controls pointer assignment but the next condition
checks only ValidHex before dereferencing; do not broaden that later check or
repair its null-pointer assumption while introducing a helper. Mirror searching
continues its current inner loop on an invalid candidate; do not turn that into
a break or change target-search order.

B24's ValidHexToStandOn is NOT an instance: -2 succeeds immediately, only -1
is explicitly rejected, and its right-edge test uses `MAP_WIDTH - 1`, not the
combat row length minus one. GetCommand and RightClick have special hero/tower
hexes and partial column checks. Do not add range checks or repair these domains
by applying the interior-hex predicate.

## H44 — integer-result Euclidean length of an existing delta

Disposition: plausible small math helper with explicit integer/float boundaries.

`army::SpecialAttack` / `FlyTo`, `combatManager::ShootMissile`, and spell bolt /
chain / blast code compute
`static_cast<i32>(sqrt(static_cast<double>(dx * dx + dy * dy)))`.
A proposed `PixelVectorLength(dx, dy)` would name that repeated distance operation
at the common combat geometry interface. Inputs are already-computed integer
deltas; callers that take abs do so outside the helper.

Preserve integer multiplication and addition before conversion to double, the
existing sqrt call, and final truncating integer conversion. No hypot, squared
distance substitution, wider overflow policy, pre-conversion to float/double,
or extra absolute-value operation. This is not H39's approximate cell metric.
Movement step rounding, minimum counts and division by frame count stay outside;
they differ among flight, missiles and blast segments.

B25's IsCrystalBallInEffect uses the same integer-square/double-sqrt/i32-truncate
boundary in map-cell units, but repeats coordinate subtraction instead of using
existing delta locals. A broader name such as IntegerVectorLength would need
that domain and source-shape distinction reviewed. Comparing squared distances
directly would lose the current truncation before the inclusive radius test.

## H45 — replace the one cached combat-effect icon

Disposition: credible short state protocol across ARMY and SPELLS.

`army::SpellEffect` and `combatManager::ShowMassSpell` compare the requested effect
with `gCurLoadedSpellEffect`, then Dispose the old icon, assign GetIcon(fileId)
to `gCurLoadedSpellIcon`, and finally assign the effect id. A proposed
`SELECT_COMBAT_EFFECT_ICON(effect, fileId)` can name that exact conditional
three-operation protocol. Owner: the existing combat-effect globals/interface,
not a new general resource cache.

Both callers compute MAKEFILEID before this conditional; keep that computation
and its timing outside the macro. Preserve disposal-before-acquisition, stored
pointer-before-id, operand evaluation and the no-change path. SpellEffect's
visibility guard also stays outside. PowEffect uses the filename overload and
extra valid-effect/draw guards: record it as a variant, not an excuse to move
hashing or unify all effect preparation. No animation state/frame initialization,
reference-count policy or resource error behavior is added.

## H46 — duration bonuses from the hourglass and wizard hat

Disposition: plausible narrow statement helper shared with the spell evaluator.

MirrorImage and the trailing local-power calculation in SummonElemental, plus
DetermineEffectOfSpell, start from the current side's `m_spellPower`, then add
2 for ARTIFACT_ENCHANTED_HOURGLASS and 10 for ARTIFACT_WIZARD_HAT. A proposed
`ADD_DURATION_ARTIFACT_BONUSES(power, hero)` would name just those two ordered
HasArtifact/conditional-add pairs. Owner: the hero/spell interface. Keep the
initial power load and the caller's subsequent storage/usage explicit.

Require stable operands and preserve destination width, sequential additions,
artifact call order and the caller's nonnull assumption. Do not silently add
a null guard or recompute the hero's primary power stat. CastSpell tests each
artifact before that spell's duration attribute; moving a common duration test
ahead of the artifact calls would change evaluation. That is a separate variant.
SummonElemental's final result is unused, but the existing calls are not removed
or repurposed to alter the already-created quantity.

## H47 — iron-or-steel golem classification

Disposition: strong small creature predicate; optional damage wrapper is narrower.

CastSpell, Fireball, ElementalStorm, Armageddon and ChainLightning in SPELLS,
and EffectSpellDamage in SPELLAI, repeatedly test iron then steel golem.
`IsGolemCreature(type)` beside the creature domain would name that exact pair.
Do not substitute the broader notion of spell immunity or resistance.

Several sites then assign `static_cast<i32l>(damage * 0.5)`; a conditional damage
macro is another possible spelling if it improves readability. Preserve the
double multiplier, final conversion and each destination type; ChainLightning
uses i32. Do not replace it with a shift or integer division. More importantly,
do not extend the reduction to new spells: the AI reduces meteor damage here,
while the runtime MeteorShower body does not. Artifact and elemental adjustments
also have different ordering and remain outside the predicate.

## H48 — the four dragon-slayer creature types

Disposition: strong small predicate shared by runtime damage and spell scoring.

`army::DamageEnemy` and `combatManager::RawEffectSpellInfluence` compare green,
red, black and bone dragon, in that order, for dragon-slayer logic. A proposed
`IsDragonCreature(type)` belongs beside the creature domain in KB_TYPES and
retains those four short-circuit comparisons. Stable inputs and a measured
macro/inline choice are required as with H33/H47.

Do not use it for spell immunity: SpellCastWorkChance includes the three living
dragon types but not bone dragon. The evaluator's target-array side and loop-count
side currently differ; a type predicate must not correct that relationship or
change its integer ratio. Classification names do not authorize damage/scoring fixes.

## H49 — berserk-or-hypnotize spell state

Disposition: credible two-field army predicate, not a new controlling-side rule.

`army::SpecialAttack`, `DoAttack`, and the final suppression test in
`combatManager::RawEffectSpellInfluence` read berserk influence then hypnotize
influence as a Boolean OR. Keep that order in a proposed army-owned
`HasBerserkOrHypnotize()` or comparably precise period-plausible name.
Keep retaliation guards, cancellation and drawing outside the predicate.

DetermineEffectOfSpell tests hypnotize before berserk, a reversed-order variant;
KeepAttack's five-state priority test has this two-state tail after H50's three.
Do not collapse all five into a generic CanAct or IsEnemy predicate: berserk and
hypnotize do not share the same action/ownership semantics. Nonzero duration is
the test, not a bit flag, and cancellation does not belong in a query helper.

B21 adds GetTraitorMask's exact positive pair and the pair at the end of
GetBestArmy's five-state test. Shooter/flyer/walker masks use zero-test conjunctions
in the same field order; keep them as negated variants with their other filters.

## H50 — blind, paralyzed or petrified spell state

Disposition: credible three-field army predicate with ordering variants.

DetermineEffectOfSpell and the prefix of KeepAttack's priority test read blind,
paralyze and petrified influence, in that order. A small army-owned predicate
such as `HasIncapacitatingSpell()` can name precisely these three duration tests.
It must not include death, quantity, morale, berserk or hypnotize.

NextArmy tests paralyze/petrified/blind, and DoAttack's second-attack guard has
their three negations in that order. These are logical variants, not exact macro
expansions. CancelSpellType's paralyze/hypnotize/petrified condition is a different
set altogether. Keep surrounding guards, ranking and branch evaluation unchanged;
do not replace full action eligibility with this partial predicate.

B21 adds the exact positive triple in GetOutOfItMask and GetBestArmy, and
zero-test conjunctions in shooter/flyer/walker masks. Those masks do not all
check positive quantity and are not interchangeable with the existing IsAlive.

B24's CycleCombatScreen tests the zero-duration conjunction in
paralyze/blind/petrified order, another explicit ordering variant. Its ability
flag, animation-sequence and deadline guards are not part of this predicate.

## H51 — shorter calls to the existing combat text-message API

Disposition: useful default-argument hypothesis; retain the history choice explicitly.

ARMY attack/luck code, CMBTMGR morale/keep messages, DRAWING's command-message
overload, and SPELLS UI/cast paths repeatedly call the text overload of
`CombatMessage(text, updateScreen, retainPrevious, clear)` with updateScreen 1
and clear 0. Defaults at the existing combatManager declaration can shorten
ordinary status calls `(text, 1, 0, 0)` while retaining explicit arguments for
history messages `(text, 1, 1, 0)` and clear/reset paths. The full owner body is
read in B17; no forwarding function is required just to supply constants.

Keep retainPrevious meaningful: it controls history, timeout and no-show logging,
not only display style. Do not reinterpret false values as defaults, add clearing,
or move text formatting. The separate CombatMessageCommand overload must retain
its resolution; review all call types before changing defaults. This is lower
priority if omission makes a caller's history/update intent harder to see.

B24 adds the four mouse-hover help calls with `(text, 1, 0, 0)`. DoVictory's
empty message uses `(text, 1, 1, 0)` and must retain that history choice.

## H52 — use the existing affected-army array indexing

Disposition: prefer the already-present two-dimensional spelling; no new macro needed.

Fireball/MeteorShower and ClearEffects use flattened
`*(gArmyEffected[0] + IDX(side) * COMBAT_ARMY_SLOT_COUNT + index)` expressions.
ARMY splash damage, ChainLightning and EffectSpellDamage already use
`gArmyEffected[IDX(side)][index]`. The reviewed declaration has exactly two
20-element rows, matching the flattened stride. Owner: X_GLOBAL's existing array.

The direct spelling expresses the same selected slot without inventing a lookup
framework. Preserve lvalue mutation, element type, evaluation order and caller
bounds. Do not return a normalized bool for a writable slot, change the stride
to the army-storage capacity of 21, or alter ClearEffects' side/slot iteration.
In runtime area effects SpellCastWorks is evaluated before duplicate suppression,
whereas the AI checks the slot before its chance query; indexing cleanup must
not move those calls or random draws.

## R20 — flying fit, path and animation differences

CanFit initializes its optional result before validation and has different edge
checks in its first versus opposite-side attempt. A later ValidHex-only test can
dereference a pointer that was assigned under a stronger check; this audit does
not insert a repair. ValidFlight tries current-position attacks before landing
search, uses last matching attack direction in one wide-creature loop, and keeps
its exact ANY_TARGET and 1 - mode handling. No universal nearest/free-hex helper.

FlyTo temporarily shifts both origin and destination when reversing a wide army,
clears index/side/frame before animation and installs the manager's current army
identity on landing. Facing restoration does not rewrite all those stores.
Its initial CopyTo height is ARMY_COMBAT_MAX_Y, not inclusive height; preserve
the actual argument. Frame-count overwrites and the stepCount == 0 branch remain,
even after the earlier minimum-one assignment.

Vampire first/last-leg sound selection has precedence, a fixed unscaled delay and
a double duration factor. Frame-position movement and final leg snapping differ
from missile stepping. The rounded `(length + (speed >> 1)) / speed` expression
is a small arithmetic lead shared with army projectiles, but flight adds positive
speed and minimum-one rules that must stay outside any future rounding helper.

## R21 — spell execution is not the same as a universal effect pipeline

Spell UI target lists, CastSpell's target-pointer exclusions, and AI target modes
are not identical spell sets (including CastSpell's mass-slow omission). Preserve
each switch, fallthrough and default. HandleCastSpell's two-store completion
does not save/set a dialog result like H32; teleport recursively synthesizes a
hover event from screen coordinates and retains its global selection state.

Runtime damage, chance and artifact order differs among direct, area, chain and
mass spells. Area effects can consume repeated random checks for the same wide
army before marking/checking the affected slot; chance queries in AI do not.
Keep explicit SpellCastWorks versus SpellCastWorkChance, SpellType versus influence
versus visual-effect domains, and each use of SPELL_NONE when applying damage.
The current artifact routine is already a shared owner; do not duplicate or
extend it while naming golem/elemental predicates.

Visual/no-show guards do not enclose every later operation. Armageddon restores
through palette pointers outside its guarded allocation block and invokes the
last target pointer without an any-affected guard; ShowSpellMessage's unhandled
creature branch can leave its local message unset. Neither is repaired here.
Preserve byte palette narrowing, local/shared buffers, callback-sensitive update
flags and cursor hide/show order. Shake tables and screen-copy row loops are
local duplication with different scaling and casts, not a shared animation API.

Bolt drawing deliberately makes a discarded Random draw, mixes Euclidean and
Manhattan distances, preserves sin/cos and atan2 operand order, and writes signed
palette indices. Branching iterates only the old bolt count, uses lastBranchX as
a sentinel and has multiple different random streams/rounding stages. Several
call-site angle-looking values occupy DoBolt's unused parameter slot; do not
move them to forceAngle. Zero-distance/denominator and unsigned sentinel behavior
is not replaced with a generic geometry policy.

Ripple/vaporize modify shared row arrays and force quantity display false/true;
they do not restore arbitrary old state. Mass effects animate before applying
influences and have different death cleanup under no-show. Summoning chooses the
last free slot in a rotated scan without checking blocked state; mirror searching
is an ordered multi-part search, not the same slot allocator.

Resurrection walks corpse entries after the match, repeatedly writes live identity,
shifts side/index without an equivalent frame shift and decrements each processed
cell count. Do not substitute a general remove/copy/reset helper. Earthquake
unblocks cells while constructing prospective states, draws random delays for
all impact slots and commits wall states at a later animation frame. A shared
"destroy structure" helper would hide these timing and state-domain differences.

## R22 — combat spell evaluation retains its own formulas and traversal

DoSpellAI preserves repeated mana-cost calls, restricted spell membership and
strict first-best ties. DetermineEffectOfSpell has separate global/area/army/
summed target modes; its area scan stops at 0x2b, not the full hex range. Existing
NextPos/FirstArmy/FirstResurrectable already name the AI traversal protocol;
their inverted found/done return values and edge skips are not a generic iterator.

Duration modifiers have an upper cap but no new lower cap; state can carry across
iterations without an occupied target. Cure passes a hex value to a parameter
used as an army index in the existing evaluator. EffectSpellCreateCreature uses
creature type id times fightValue while its computed spellPower is unused.
Do not reinterpret either as an invitation to reuse a normal stack-value helper.

RawEffectSpellInfluence preserves Buka's positive disabling-spell coefficients,
integer division before float conversion for some ratios, and target-side array
access despite opposite-side loop counts. Cure accumulation can overwrite a
previous negative score for a mirror and clears the positive accumulator in
cure mode; its output signs and per-conversion rounding stay explicit.

Damage estimation builds its area step by step with a different upper hex bound
and golem/elemental order from runtime. It scales chance before reductions,
uses a mirror lethal-damage surrogate and weights partial damage differently
from killed-unit value. Preserve integer multiplication/division without algebraic
cancellation, subsequent score narrowing and decisive/castle score branches.
No shared damage formula, area-list builder or generic buff/debuff scoring macro
is justified merely because spell names and some constants overlap.

## H53 — set the next combat move target

Disposition: credible two-store statement protocol shared by AI and army behavior.

DoCompAI, DoLichShot, AttemptAttack, AttemptAdjacentAttack and the approach
methods, plus GoBerserk in ARMY, write `giNextAction = ACTION_MOVE` followed by
`giNextActionGridIndex = hex`. A proposed `SET_NEXT_COMBAT_MOVE(hex)` belongs
at the COMMAND/X_GLOBAL action interface. Keep exactly that store order and
evaluate the target expression once, after setting the action.

Do not write action-extra or second-grid fields, assign the army's target side/
index, call ValidPath, or hide a return/goto. Some callers set ACTION_MOVE before
an intervening ValidAttack call or only reuse a previously selected grid index;
those are not contiguous two-store instances. ACTION_MOVE can designate an
occupied attack target as well as empty movement, so the helper must not validate
or reinterpret the target.

B24 adds DoCommand's MOVE/FLY/SHOOT group, with a separate following
`giNextActionExtra = -1`. Its ATTACK arm writes the grid first and selects
action/extra later; that arm is not an exact two-store instance.

## H54 — begin a named army animation, favoring existing Wince

Disposition: useful existing-API reuse plus a small generalization hypothesis.

The complete `army::Wince` body sets `m_animationSequence = ARMY_ANIMATION_WINCE`
then `m_animationFrame = 0`. ShowMassSpell repeats that exact pair; compare reuse
of Wince before introducing another name. It is currently out of line, so a new
call is not assumed byte-neutral.

ARMY, FLY and SPELLS also repeat sequence-then-zero-frame pairs for STAND, DEATH
and WINCE_RETURN. A small `StartAnimation(sequence)` member or statement macro
at army.h could name those exact two stores if the expansion and readability
benefit hold. Preserve the original sequence storage type and stable argument
evaluation; keep pending animation, offsets, flags, sound, counters and drawing
outside. Sites that select a sequence but deliberately keep/advance the frame
are not instances. No generic animation state machine is proposed.

B24 adds the army sequence/frame pairs in ResetCyclingCreatures and
CycleCombatScreen. Keep last-animation tick/jitter and standing-animation
selection outside. The separate hero animation state/frame arrays have a
different owner and are not calls to an army method.

## H55 — named lich, vampire and troll base/upgrade pairs

Disposition: additional credible two-type creature predicates, not upgrade inference.

DoCompAI and army::SpecialAttack test lich then power lich for their specific
shot behavior. army::LoadResources, DoAttack and FlyTo test vampire then vampire
lord for extra samples, retaliation exemption or flight presentation. Small `IsLichCreature(type)` and
`IsVampireCreature(type)` predicates beside the creature domain would name those
exact pairs across TUs. Retain stable inputs, comparison order and the caller's
surrounding conditions.

Do not replace these with IsUndead, infer contiguous id ranges, or assume every
base/upgrade pair shares every ability. The helper only names membership; splash
damage, flight delays and first/last-leg precedence remain separate. Vampire-lord
healing is a single-type test and must not be widened to the pair.
Other repeated single-TU creature pairs are leads for later consumers, not yet
additional confirmed shared families.

B24 promotes the troll/war-troll pair: ResetRound's hit-point-loss reset and
army::LoadResources' projectile-resource selection compare those two types in
that order. `IsTrollCreature(type)` can name membership without combining the
unrelated regeneration and resource effects.

## R23 — tactical AI masks, ranking and movement remain domain-specific

The existing mask builders already name meaningful army sets. Their common loop
is local and their filters differ: some omit quantity checks, mirror classification
does not exclude disabled spells, and ability flags are not the IsAlive contract.
Keep the shifted unsigned mask bit, signed mask input and loop bounds rather
than replacing them with a generic filter/container or a living-army iterator.

BestArmy halves strength for five spell states and uses a strict greater-than
test starting at zero; WorstArmy does not use the same adjustment and has a
different sentinel. ClosestArmy is distance-weighted hit-point ranking, not simply
nearest geometry. Existing Strength calls are useful abstractions; don't replace
type-id or differently typed/raw products elsewhere without checking the exact
contract and evaluation. Remaining-hit-point capping in GetModLichDamage is local
so far, not proof of a shared total-health helper.

DoCompAI's priority calls, repeated fly branches, dead constant branch and
post-selection adjacent-enemy rewrite remain explicit. Lich scoring uses its own
local marked array, not the global affected-spell buffer; shooter and flyer bonuses
can both apply. Target/side guards and unused calculations are not removed.

Front-approach and enemy-approach temporarily increase speed but differ in target
assignment, path modes, fallback calls and whether the last path element is walked.
Restore the narrowed speed at the same point, retain moat stopping and preserve
FindCombatPath calls even when a local result is unused. Do not unify both into
a generic walk-until-range function.

AICheckRetreat copies a hero before rewriting a five-slot army projection, performs
explicit double/float and signed/unsigned conversions, and has integer divisions
before float conversion. Group clearing uses type-then-count, like Dismiss, but
projection, quantity estimation, base-artifact scoring and retreat probability
are not a reusable reset/value macro. The later PHILAI implementation review is
still needed before suggesting reuse of any broader battle-value algorithm.

## B23/B24 — extensions to established short-call candidates

ProcessMapChange and SendMapChange add H17's trailing-default logging calls;
the hero-position diagnostic also formats gText then LogStr, an H19 instance.
Its long non-default movement log is not shortened by removing meaningful fields.
ViewBallista adds H20's left/right turret building-mask checks. The ordinary
text-only command/help/retreat/error/ballista dialogs add H30; the cursor death
notification and event-resource dialogs carry actual resource/time arguments
and must keep them. Command panel/window allocations add H31's stored-pointer
MemError checks. Unchecked text buffers are not grounds to add new checks or
replace their fixed capacity with H05's strlen-plus-one allocation.

## H56 — combat-cell occupant identity

Disposition: strong small predicate shared by command, movement and flight code.

SetCombatDirections and CheckSetMouseDirection in COMMAND, MoveAttack in ARMY,
and CanFit in FLY compare a cell's occupant side then occupant index with a
specified pair. A proposed `hexcell::HasOccupant(side, index)` names exactly
that short-circuit conjunction at the already-read packed owner in hexcell.h.
It must compare the stored signed-byte fields using the existing promotions,
without narrowing the requested index before comparison.

Keep hex resolution and ValidHex checks outside; this is not a checked lookup.
The query does not require a nonempty side, positive quantity, unblocked cell
or living army. CanFit separately permits an empty cell before the identity
test. No corpse-list search or occupant-frame comparison is implied. A macro
needs stable cell/side/index expressions; an inline must be measured because
argument evaluation can move ahead of the original side short-circuit.

## H57 — clear only an army's selected target identity

Disposition: credible two-store army protocol, with reversed-order exclusions.

The constructor, Init and MoveAttack in ARMY, SetupGridForArmy in DRAWING, and
SetCombatDirections / GetCommand in COMMAND set target side to COMBAT_SIDE_NONE
then target index to -1. A proposed `army::ClearTarget()` or narrow statement
macro in army.h names only these two ordered stores. It is distinct from H40's
combat-cell occupancy reset and H53's global next-action selection.

Do not clear attack direction, move-target hex or previous-target hex; their
separate stores and retained values remain visible. Save/restore in drawing and
direction setup surrounds calls and cannot be absorbed into a generic guard.
WalkTo(i32) in ARMY and SeedCombatPosition in FINDPATH write index before side;
those are reversed-order variants, not automatic substitutions. No destructor,
callback, hidden return or new validation belongs in this helper.

## H58 — the castle-gate exception to a blocked combat cell

Disposition: useful shared semantic predicate; positive/negative source variants.

The front and rear tests in PATH's army::ValidMove permit a blocked cell when
castle combat is active, the candidate is one of the two gate hexes, and the
drawbridge is not raised OR the current controlling side is the defender with
no live or dead occupant in the approach hex. COMMAND's ValidHexToStandOn,
GetCommand and RightClick spell the exact negation of this gate exception.
The owner declares COMBAT_CASTLE_GATE_OPEN as an alias of
COMBAT_DRAWBRIDGE_RAISED (4); the misleading alias is not a different state.

A proposed `CanPassCastleGate(hex)` at the combat-manager boundary could name
just that exception, preserving castle/hex/state/side/live/dead test order and
the existing global-manager accesses. Blocked-state and candidate-occupancy
queries remain outside: COMMAND reads some candidate fields through `this`
while gate state comes from gpCombatManager. Do not assume the receivers alias
when designing the helper or add an implied movement/flying rule.

Positive and negated forms still require independent byte/CFG evidence before
replacement. Keep the two gate constants, current controlling side, strict
dead-count equality and original hex-domain guards. TestRaiseDoor additionally
examines occupancy/corpses in the gate hex and uses a different drawbridge state;
it is not a reuse site for this predicate.

## R24 — adventure cursor, movement and map-change protocols remain explicit

DrawCursor and DrawCursorShadow locally save the same five cursor fields, but
frame advancement, boat wakes/flags, mirroring, clipping and snapshot timing
differ. Boat flags use no-clip/zero extents while hero flags use clipped extents;
shadow drawing has its own frame remapping. The walking-speed selector through
adjacent config fields is a local lead pending the full adventure-manager pass,
not yet a promoted cross-TU helper. Preserve timer precision, negative half-turn
tie behavior, repeated draws and independent sound guards.

MoveHero computes terrain costs before the mobility guard and sends the map
change before several event/path checks. Forced movement still later deducts
mobility. Embark/disembark, hero interaction and castle branches have different
stop/redraw/event paths; midpoint versus final map-origin/hero-coordinate changes
and deferred northwest X movement are not normalized. Existing global blit and
dialog-menu flags are explicitly set, not restored by a newly introduced guard.
Adjacent-monster callbacks can change the moving hero's owner and invalidate
the pending event cell. Keep these phases visible rather than introducing a
whole movement transaction macro.

Event resource presentation selects the last two nonzero entries, can replace
one with an artifact, and encodes negative displayed amounts differently from
the raw resource addition and floor at zero. Interactive and computer handling
have different event guards. No general resource-award/dialog macro is yet
justified; EVENTS implementation remains unread.

ProcessMapChange advances the sequence before dispatch and changes player
context only for the six-player range. Position mismatch and dead-hero checks
have distinct outcomes; recruit stores preserve trigger/metadata widths and
order. The repeated CompleteDraw/UpdateScreen tail is a local lead until other
adventure TUs are read, not a universal redraw helper.

Incoming map changes distinguish stale, exact-next and future sequences, scan
for duplicates before free slots, and force-unwind one oldest entry on a full
queue before retrying. Batch input is copied and visited from highest array
index down to zero; queue drain has separate forced and exact-next phases. Zeroing all fields,
sorting once, introducing a priority queue or breaking after one exact entry
would obscure/change that protocol. Purge clears only type tags. SendMapChange
guards control then remote state, zeroes its packed record, increments the
sequence, shifts overlapping recent history with memmove and transmits all four
records. This is not COMMAND's four-integer action packet or a generic reliable
transport wrapper; REMOTE implementation remains unread.

## R25 — combat commands, reward dialogs and idle cycling retain their contracts

Main polls sound/cycling, performs castle attacks and win checking, receives
remote commands, then considers local control, berserk and player input. A
received action jumps past local-only handling. Grid-selection input draining,
mouse-move coalescing and reset-event synthesis are not a generic event loop.
gbHumanPlayer and gbThisNetHumanPlayer mean different things; the actual network
control conditions, duplicate previous-command store and sentinel guards are
not replaced with a newly inferred IsHuman/HasControl rule.

Direction setup clears and later restores target identity, builds facing-aware
front/rear cells, then expands six/eight directions into 24 cursor sectors.
The zero-valid fallback, pending-value propagation and next-before-previous
priority remain explicit. Mouse slope thresholds use asymmetric strict tests
and load-bearing float divisor parentheses; do not reuse projectile angle
selection or repair division by zero. ValidHexToStandOn's -2 success and
MAP_WIDTH-based edge check remain as written (see H43).

Round reset, victory qualification, casualties and surrender use different
army counts/filters. ResetRound visits 20 slots and tests quantity; casualties
compare initial and current quantities; surrender calls IsAlive; AddArmy searches
21 slots and reuses only selected excluded dead stacks. DoVictory can restore
one creature after subtracting temporary resurrection, counts necromancy stacks
separately from defeated bodies, and applies multiple truncating conversions.
Do not introduce a universal living-army iterator, generic stack allocator or
shared arithmetic formula. Captains, hero experience, artifact capacity/order,
spell learning and temporary-stat application have separate control paths.

WinCombatHandler preserves the current event payload in timeout completion,
cycles across multiple resources with distinct last/reset frames, and mutates
the incoming message for broadcasts. Reward panels differ in AddWidget order
and which window receiver they draw. Fixed text buffers, format-string versus
%s copying, get/dispose of monster icons and signed quantity formatting remain
visible. Three local pickup-sound tails use SRandom; a searched EVENTS occurrence
uses Random and its complete body is still unread. This is not yet a shared
sound helper. Victory/loss dialog cleanup differs: only the loss path nulls the
window pointer. Do not add cleanup or change callback/resource lifetime here.

ProcessNextAction transmits before executing and has multiple win exits; the
early pre-switch win branch retains the initialized consume result. Action
reset, morale, cycle-timer reset, next-army selection and round reset do not
occur on every path. Keep the spell/move/attack/retreat/surrender/wait/defend
differences and the four-integer wire order. Castle attack loops have fixed
catapult versus three-tower order, not one generic repeat-action operation.

ResetCyclingCreatures marks cycling troops but then resets every non-excluded
troop when any was marked. ResetCycleTimers samples now separately from hero
ticks and uses an inclusive random range only above its delay threshold.
CycleCombatScreen has another spell-test order, weighted animation selection,
random frame-repeat and double-valued jitter; no random/timer calls are cached
or removed. Hero death/idle arrays and army frame state are different owners.
InCombatArea is already a named local-use boundary and does not justify a new
all-screen/hex predicate. Settings updates and AddArmy's fizzle sequence keep
their specific redraw, visibility, resource and preference-write order.

## H59 — redraw the current adventure view and request its update

Disposition: credible small owner method / two-call macro; not a full UI refresh.

ProcessMapChange in CURSOR and Main, ProcessSelect, ProcessSearch,
HeroQuickView, TownQuickView, CheckCastSpell and ScreenScroll in ADVMGR repeat
`CompleteDraw(0); UpdateScreen(0, 0);` contiguously. A proposed
`advManager::DrawAndUpdateView()` would name only that ordered pair. Owner:
SOURCE/advManager.h. Both complete implementations and the one-argument
CompleteDraw overload are read: the overload uses the current map origin,
forceDraw=0 and updateBottomView=1.

Preserve each call, their arguments and repeated receiver/global reads. Do not
add an outer bShowIt guard: drawing and updating each have their own sound,
timer and visibility behavior. UpdateScreen with force=0 can decline a display
update, so the name must not promise a forced blit. Caller radar updates,
mobilization, route changes and hover processing remain outside. Four-argument
CompleteDraw calls are related variants, not this exact source prefix.
RedrawAdvScreen also rebuilds backdrop/border, locators and UI and is not an
existing substitute. Prefer a narrow method only if measured inline expansion
and its name justify hiding two already meaningful calls; no RAII refresh guard.

## H60 — adventure-viewport icon call with explicit clipping mode

Disposition: strong fixed-argument statement/expression macro candidate.

DrawCursor / DrawCursorShadow in CURSOR and DrawCell / PuzzleDraw in ADVMGR
repeatedly call `IconToBitmap(icon, gpWindowManager->m_screen, x, y, frame,
clip, 0, 0, 480, 480, 0)`. CURSOR_CLIP_SIZE, DRAW_CLIP_WIDTH/HEIGHT and
PuzzleDraw's SCREEN_HEIGHT denote that same 480-square adventure viewport.
A proposed `DRAW_ADVENTURE_ICON(icon, x, y, frame, clip)` at SOURCE/ADVMGR.h
could expose the viewport contract while removing six mechanical arguments.
This is an adventure-layer convenience, not a new BASE decoder abstraction.

Keep the normal and FlipIconToBitmap forms separately named; both occur across
these TUs and have the same fixed rectangle but different rendering semantics.
Pass each varying operand once with its existing argument conversion and
evaluation behavior. Frame masking, animation, color choice and mirrored X
offsets stay explicit. Keep clip mode explicit even when the same fixed extents
are ignored in no-clip mode. DrawCursor's non-flipped boat flag instead supplies
zero extents and is not an exact instance. Color/palette, monochrome, scaled and
sheared APIs are excluded. Default arguments on the BASE function cannot encode
this caller-specific screen/rectangle contract without affecting other domains.

## H61 — flipped land-hero shadow-frame remapping

Disposition: credible small ordered remap, with a separate final frame offset.

DrawCell in ADVMGR and DrawCursorShadow in CURSOR apply the same five independent
ifs to an already-resolved i32 frame: 51→56, 50→57, 49→58, 47→55, 46→55, in that
order. A proposed `REMAP_FLIPPED_HERO_SHADOW_FRAME(frame)` or small reference
inline belongs at the shared adventure cursor/frame interface in SOURCE/CURSOR.h.
The two private constant domains have been compared by value; they are not
different animation rules.

Keep the initial frame assignment, mask and cursor frame-count addition in the
caller. Retain conditional-only writes to the existing local, repeated tests
and order; do not replace them with a lookup table, switch or a newly narrowed
frame argument without byte evidence. Both subsequently pass
`frame + (frame >= 9 && frame < 36 ? 50 : 0)` to the normal icon renderer.
That shared expression can be named separately, but storing its result back
into the local would add a mutation absent from the source. The boat shadow
uses offset 36 and no five-frame remap. Non-flipped shadows and flag frames
are not instances. The name explains an otherwise opaque sprite-sheet rule;
the entire shadow draw workflow remains caller-owned.

## H62 — detach and delete one explicitly owned widget pointer

Disposition: plausible small lifetime helper with a deliberately narrow contract.

advManager::ClearBottomView in ADVMGR and combatManager::ClearWinLoseBottom in
COMMAND repeat `if (pointer != NULL) { window->RemoveWidget(pointer); delete
pointer; }` for each of two widget arrays. A proposed
`REMOVE_AND_DELETE_WIDGET_POINTER(window, pointer)` belongs beside heroWindow's
widget operations. The existing RemoveWidget calls Close and unlinks the widget
but does not delete it. Keep that existing call rather than duplicating its list
algorithm, including its current edge-case behavior.

The helper must not clear the array slot: both callers null both pointers only
after both detach/delete sequences. Keep those stores, loop bounds, adventure's
early empty-view guard and its subsequent global resets outside. Macro operands
must be stable and side-effect-free; an inline that captures the pointer once
changes the repeated lvalue-read behavior across RemoveWidget/Close and needs
explicit alias/codegen review. Preserve pointer static type and deletion form.
heroWindow::RemoveAndDeleteWidget(id) is not equivalent: it searches for matching
IDs and deletes only under WINDOW_FLAG_OWNS_WIDGETS, whereas these callers delete
their explicit nonnull pointers unconditionally. No generic owner-container or
cleanup-on-scope-exit framework follows from this shared pair.

## H63 — hero scouting visibility radius including a telescope

Disposition: credible small hero query shared by movement and teleportation.

MoveHero in CURSOR and TeleportTo in ADVMGR pass
`giVisRange[IDX(hero->m_secondarySkills[IDX(HERO_SKILL_SCOUTING)])] +
(hero->HasArtifact(ARTIFACT_TELESCOPE) != 0)` to game::SetVisibility.
CURSOR_VISIBILITY_ARTIFACT aliases ARTIFACT_TELESCOPE. A proposed
`hero::ScoutingVisibilityRadius()` belongs at SOURCE/hero.h with the existing
artifact/skill queries; declaration placement must respect X_GLOBAL's table
dependency. No existing equivalent method was found in the fully read owner.

The table and skill slot are signed i8 storage; preserve their promotion and
signed indexing, the one HasArtifact call and its explicit Boolean conversion,
and the final integer sum. No minimum, clamp, duplicate-artifact count, alternate
table or embarked modifier is added. Coordinates and player are not part of
the helper: MoveHero uses direction-adjusted map origin, while TeleportTo uses
the manager's current origin even when its no-show path leaves that origin
unchanged. Their visibility timing, movement and blackout workflows differ.
Further game/AI consumers remain to be fully read before final interface ranking.

## R26 — adventure manager workflows and superficially shared formulas

The complete 95-definition ADVMGR review retains the following boundaries; no
observed oddity was corrected as part of this source-reading audit.

Open/Close resource groups have different cache, disposal-level, null-check,
entering-town and music conditions. The visibility buffer is allocated with
new[] but currently scalar-deleted; this is not silently repaired. Sample-set
selection's bridge/retail conditional forms differ for negative inputs. Sound
volume and player/watch-player substitutions retain their exact restore order;
draw suppression is not a universal save/restore guard. The final registration
triple is H14, not an abstraction of the whole manager lifecycle.

DoAdvCommand and Main retain route seeding/reversal, mobility, network/input
dispatch and mouse-hide-count draining order. Reseed's actual body only clears
giSeedingValid despite accepting two arguments. ProcessSelect performs some
map-extra reads before GetCell's bounds fallback; radar dragging coalesces two
events under its own protocol. Hover cursor modes depend on action-bit versus
base object type and days-left offsets; not every branch updates selectedCell.
The fixed six-player reveal calls, cheat guards and unusual next-town count
conditions are not normalized by an iteration or selection macro.

CompleteDraw retains PollSound before its show guard, layered whole-view passes,
direction-dependent overlay order, border/cursor/bottom-view ordering and
gbAllBlack's temporary origin stores. DrawCell uses shared file-static state,
GetCell before border selection, watch-player visibility, packed terrain flags,
and distinct base/extra object and overlay rules. Object chains stop at the first
invalid object index, overlay chains at the first invalid overlay index; neither
is a skip-invalid generic iterator. Garden, monster, mine, boat and flag frame
rules differ. H60/H61 name only fixed call arguments and a verified remap.

ComboDraw's 18×18 state is cleared with 256 bytes, not its entire array; retain
the existing partial clear and neighbor indices, including suspicious negative
ones. Its marking traversal and forward overlay passes differ from CompleteDraw.
Extent growth, cell-to-pixel conversion, clipping, empty-rectangle behavior and
UpdateScreen's max-minus-min widths are separate contracts. UpdateScreen also
owns sound polling, timer deadlines, scroll offsets and animation-counter changes.
Save/DrawAdventureBorder use a specific 640-pitch top/sides/bottom copy layout;
they are not a general bitmap rectangle copier or ownership guard.

Radar's 108-map raster uses integer 4/3 with remainders, its frame uses 1.33f,
and selection uses 1.3333f; do not share a supposedly exact scale formula.
UpdateRadar initializes its color once: an other-owner hero can retain the
previous cell's color. Its neighboring castle query has distinct guards and
uses the raw map accessor. Bounds, partial redraw spans, float conversions and
current-player versus watch-player visibility must remain explicit.

QuickInfo's visited-site flags vary between hero masks, player masks and packed
metadata bits. Expansion/dwelling object-versus-overlay selection priorities also
differ; a universal trigger lookup or HasVisited is not justified. Its tent/key
text uses signed-byte toupper, not the combat CP1251 helper. GetArmySizeName's
thresholds, grammar variants and shared diagnostic buffer remain a named owner
algorithm, not a generic number formatter. Resource text with sprintf(source
as format) is not H05's strcpy allocation/copy contract.

Bottom-view widgets retain exact add/update order and buffer capacities. NewTurn
assigns two allocations to the same text slot; enemy-turn logic compares the
player just stored. DoHeroKnob/DoTownKnob differ in their double spans but share
other seemingly odd constants; division occurs before the count guard and the
page is narrowed before the subsequent local limit adjustment. No cleanup helper
repairs those behaviors. HeroQuickView may repeat IsCrystalBallInEffect after
window creation, whereas TownQuickView has different knowledge/debug/guild
levels and radar/window ordering. Their post-dialog local-message checks retain
the type left by widget setup. H62 does not unify their window/resource lifetimes.

SetHeroContext/SetTownContext use last-match list searches and different route,
mobilize, music and hover sequencing. Existing game::GetHero/GetTown and
GetPlayerHero/GetPlayerTown already name unchecked owner-array access;
GetHeroSlot's duplicate local expression is an existing-API reuse lead, not a
reason to add another index macro or bounds policy. Cursor snapshots and its
adjacent-config-field walking-speed expression remain local CURSOR leads; no
second matching TU instance has yet been fully reviewed.

B29 confirms GAME's GetCastle/GetCastleSlot expressions against the existing
game::GetTown accessor. Keep the receiver explicit: GetCastle uses gpGame,
whereas GetCastleSlot accepts an instance. NewMap and ProcessOnMapHeroes use
scouting alone, without H63's telescope bonus; NewMap even reads hero-list slot
zero's skill while placing the hero at the current count. Do not substitute
the broader visibility-radius contract there.

Travel spells retain their own charge/cancel rules: mobility is deducted before
travel UI; DimensionDoor can consume the spell after an accepted invalid terrain
choice, TownGate charges only on success, and SummonBoat failure still reaches
CastSpell's spell-use tail. TeleportTo sends the map change before cell mutation,
clears/restores old occupancy under distinct flag rules and does not restore
bShowIt. Its shown/no-show origins differ (H63). TownGate's nearest selection can
choose an occupied town rather than another free one. SummonBoat chooses the
first qualifying boat, not the nearest; trigger/metadata and packed coordinate
stores retain order. No common movement transaction or travel-spell wrapper.

ShowRoute tests destination X, seeds with its own limit, subtracts movement costs
in stages and encodes u16 frame/reachability flags. HideRoute updates button and
destination state before its visibility guard. SeedTo's related modes already
have an owner method; none of these is a generic path iterator. Scrolling sets
the deadline before clamping and demobilizes only if the origin changes.
FindAdjacentMonster returns the first X-major match with a stricter interior
fast path; its above-origin object test is the H25 negated/call-count variant.
MapExtraPosAndAdjacentsSet tests the center before bounds and revisits it in the
subsequent 3×3 scan.

Environment sound scans process center then ordered perimeter edges in two
layers. Existing sounds update distance only when smaller; replacement chooses
the first strict farthest slot, stops it, stores metadata, lazily loads and
starts playback, then XORs the active mask. TrimLoopingSounds retains ascending
cached slots up to a memory-adjusted quota; this is not LRU or dispose-all.
Backend, mute and stop-versus-volume-update behavior differ from other sound
workflows. The environment-volume, floating-coordinate, remote-payload and
route-visibility macros have existing local contracts; only the correctly
ordered MAP_EXTRA_AT/MAP_EXTRA_AT_WFIRST expansions merit their existing names.

CheckHandleNet accepts reliable and unreliable modes with a typed payload union,
has receive-save failure/shutdown and synthetic player-exit paths, and returns
raw packets for combat/unknown cases. Wait forwarding and executive close
messages are not H32 widget completion. ComputeAdvNetControl's dead-player loop
does not advance its index; the following active-player scan tracks a different
choice. These behaviors are retained, not repaired with a common player loop.
SaveGame has its own human/dead filtering, filename suffixes, disable/dialog/
save/enable ordering and result replacement.

ViewPuzzle keeps its exact piece permutation, coordinate perturbation, tan-map
pixel pass and fizzle-source release choice. PuzzleDraw toggles gbDrawingPuzzle
to true then false, not back to a saved value. Adventure/control/system panels
keep context restoration and confirmation interpretation explicit (DIALOG_OK
cancels the reset here). System option toggles differ: 1-x, Boolean zero test,
modulo cycles and blackout transitions are not interchangeable. The always-true
local music-presence helpers reflect this build's removed readiness checks;
no new generic backend predicate follows. Preference writes, cursor-sample
reloads, interface redraw and remobilization retain their order.

GetMobilityFrame and GetManaFrame share a final threshold ladder locally, but
their initial scaling, negative handling and minimum-visible rule differ.
DoVisions scans through GetCell's fallback without clipping the candidate loop,
uses first strictly nearest Manhattan distance and computes a double ratio
stored as float. Diplomacy fee uses the full count even for a partial join,
and one partial-join message formats count rather than joinNum. H33 does not
absorb these rules. Crystal-ball range truncates sqrt before comparing radius
(H44 variant). StopOnTrigger already names its masked-base-table lookup and
expansion metadata exception; no universal object/trigger classification helper.

## H64 — shared DirectPlay/Winsock receive-buffer primitives

Disposition: strong shared-owner findings; separate small operations, not a
general-purpose ring-buffer or transport framework.

The fully read DPNETWIN and WSNETWIN implementations use the same globals,
declared in SOURCE/dpnetwin.h: ppDPRcvBuffer, piDPRcvBufferSize and their i32
head/tail indices. Both capacities are 200. Narrow free inlines/macros at that
existing shared storage boundary could name the following exact operations:

- Initialization: allocate pointer storage, allocate size storage, zero pointer
  storage, zero size storage, in that order. Neither site resets head/tail here
  or checks these allocations; do not add either action inside a helper.
- Enqueue in dpEvaluateMessage/wsEvaluateMessage: allocate size-minus-1 bytes at
  head, copy that many bytes from rcvBufIn+1, store the original size, advance
  head with `(head + 1) % 200`. Size is u32l here. There is no full-queue test,
  overwrite disposal, source-length validation or new null check.
- Copy/dequeue in dpnet_rcv/wsnet_rcv: return zero on head==tail, read the stored
  size into u32, copy exactly that size, free the slot, advance tail modulo 200,
  and return an i16-narrowed size. The slot and size are not cleared. Each
  backend's ProcessMessages call must remain before this operation.
- Array disposal in dpnet_term/wsnet_term: conditional H2_FREE of pointer
  storage, unconditional null store, then the same pair for size storage.
  This does not free queued payloads or reset indices. DirectPlay drains queued
  packets before this phase; Winsock does not. Keep that lifetime difference.

Possible names include QueueTransportPacket and CopyNextTransportPacket, with
separately named storage initialization/disposal if worth the indirection. Do
not move the existing DATA owners or introduce a new packed/container layout
merely to share them. Preserve all repeated index/global reads and caller-local
size conversions; inline expansion still needs byte/relocation evidence.
In particular, the enqueue's stored size includes the byte omitted from its
allocation/copy, and dequeue later copies that stored size. A helper must not
silently correct this observed discrepancy or acquire a false safety contract.
REMOTE's ordered 128-slot message queue and NetBIOS/serial node queues are
different owners and algorithms, not additional instances.

## H65 — allocate a one-byte-tagged transport message

Disposition: credible narrow construction prefix; lifetime and send remain explicit.

dpSendMessage and wsSendMessage allocate H2_ALLOC(size+1) into a u8 pointer,
write `static_cast<u8>(type)` at byte zero, then copy size bytes to pointer+1
only if size!=0. A proposed ALLOC_TRANSPORT_MESSAGE(destination, type, size,
data) belongs at the NetworkPacketType/transport boundary in SOURCE/REMOTE_TYPES.h
or the shared transport interface, not at the six-byte remote packet layer.

Keep size's u16 input promotion before +1, its conversion at the allocation/copy
calls, the zero-size copy guard, unchecked allocation and caller pointer storage.
Macro arguments must be stable and side-effect-free; a pointer-returning inline
is an alternative only after testing its changed local/return code shape.
Do not absorb backend calls, broadcast expansion, retries, error handling or
H2_FREE: Winsock error returns currently bypass that final free, while DirectPlay
has its own DPSD path. Neither whole send routine is shared by this prefix.
The outer one-byte tag, RemotePacketHeader and RemoteMessage are three distinct
wire layers; no universal packet initializer or serialization template follows.

## H66 — free all nodes from an existing tag_Anchor queue

Disposition: credible small queue-owner operation, not generic object destruction.

com_term in COMWIN and nb_term in NETWIN repeat
`while ((node = pop_node(anchor)) != NULL) H2_FREE(node);` across two and three
queues respectively. A small `free_node_queue(anchor)` beside add_node/pop_node/
init_anchor in SOURCE/comwin.h would name a real operation on that same node
representation. The complete owner bodies are read; no alternate destructor or
payload callback is involved.

Preserve repeated pop-until-null and one H2_FREE per returned node, including
the final null result if a macro uses the caller's node lvalue. Do not replace
it with direct next-pointer traversal or memset the anchor. pop_node currently
updates head and possibly tail without repairing the new head's prev link or
clearing the detached node's links; sharing must retain that behavior.
NetBIOS lock acquisition/release, serial's valid-handle guard, port shutdown and
queue order stay outside. No implicit lock or shutdown belongs in the helper.
The existing init_anchor only writes head then tail to null; it does not drain
allocated nodes and is not an alternative cleanup operation.

## H67 — pop the first available node from two ordered queues

Disposition: plausible small owner helper; lower priority than H64/H66.

comm_wrt_task in COMWIN pops priorityQueue and, only on a null result, normalQueue.
nb_thr_ctl in NETWIN has the exact same three-statement shape for gNbFreeQueue
then gNbSndQueue. A proposed `pop_first_node(first, second)` at SOURCE/comwin.h
would name this ordered choice while reusing the existing pop_node primitive.

Preserve one first pop, the null test and conditional second pop; do not pop
both eagerly, merge the queues or infer a priority from either queue's name.
Keep the caller's result assignment and subsequent empty-queue behavior; serial
returns, whereas NetBIOS clears its loop flag. NetBIOS holds its send lock around
both pops, while serial has no such lock. A macro's repeated arguments need
stable operands, and an inline's local/call boundary must be measured. If the
three explicit statements prove clearer than another name, retain only as a
low-priority source-reading finding.

## H68 — default optional remote-send policy arguments

Disposition: credible shorter call to an existing API; wider caller audit pending.

RemoteMain/TransmitAndWait in REMOTE, SendMapChange in CURSOR and
combatManager::ProcessNextAction in COMMAND pass the final pair
`1, REMOTE_MESSAGE_DEFAULT` to TransmitRemoteData. Default arguments on the
existing SOURCE/REMOTE.h declaration, or a fixed-arity five-argument macro,
could leave data, destination, length, command and reliable explicit.

The complete TransmitRemoteData body confirms that the final two slots mean
allowRetryDialog and default message-type selection. They do not mean reliable
delivery: SendMapChange deliberately passes reliable=0 even though its next
constant is named CURSOR_REMOTE_RELIABLE. Default message type chooses reliable
or unreliable from the preceding argument; an explicit non-default type is
another policy and must remain expressible. Preserve narrowing to the existing
i8/enum parameters, argument evaluation, ID allocation, retries and return
handling. No extra transmit call, early guard, packet conversion or hidden
shutdown is added. Prefer declaration defaults over a second callable wrapper
if matching evidence permits; owner/body reading does not establish byte safety.

## R27 — remote reliability and DirectPlay/Winsock transport differences

REMOTE's complete thirteen definitions and both complete transport TUs establish
three packet layers with different lengths, signedness and checks. EncodePacket
narrows its payload size to char but copies the original length. DecodePacket
reads the global packet, ignores its second argument, converts payload size to
u8 and leaves the CRC field zeroed after verification. Neither adds a received-
buffer-length check. calc_crc_long is the existing feedback-bit/shift/add/sum
algorithm, not a standard CRC library substitution; calc_crc truncates its result.

RemoteCleanup has remote/main/cleanup guards, protocol-specific unload calls and
repeated flag stores; it does not dispose the receive-queue allocations. RemoteMain
zeros player info after first formatting names, clears only 30 bytes of the i32
recent-ID array, sets backend flags in branch-specific ways, and initializes
remote-on before or after backend startup depending on the path. Name input,
preference writes, cursor-color restoration and the two-player setup handshake
remain explicit. A common reset or backend-lifetime guard must not normalize them.

SendRemoteData's NetBIOS error exits its apparent retry loop, while DirectPlay/
Winsock return values are not folded into the same success rule. ReceiveRemoteData
retains each backend's signed/narrow return conversion before DecodePacket.
TransmitRemoteData increments the global ID, constructs fields in order and
can poll reentrantly while comparing confirmations against the current iIDCtr,
not a saved msg.id. Keep the inclusive retry limit, per-confirmation delay,
optional retry dialog and tries=-1 restart. H68 only shortens fixed call slots.

GetRemoteData selects the first strictly lowest arrival order, copies the full
256-byte message into shared rcvBufOut, then optionally frees/nulls its slot.
PollRemote services a backend before its setup guard, sends partially populated
heartbeat/confirmation buffers with distinct lengths, and recognizes only
heartbeat packets for heartbeat time updates. Confirmation or heartbeat receipt
ends that poll; reliable data is acknowledged before duplicate-ID checks, but a
full queue rejects data before acknowledging it. Recent-ID rotation and first
free-slot allocation are not CURSOR's ordered map-change queue or H64's ring.

Timeout paths distinguish host, first guest and later guests. Host completion
clears bInTimeoutFail, while the guest path sets it true after restoring the
sound-poll guard. Synthetic exits do not initialize every SPlayerExit field;
ADVMGR's save-message exit has different flags and store order. No zero-initialized
universal player-exit macro or save/restore guard is justified. TransmitAndWait
consumes unrelated messages, writes the output pointer only on its matching
response path, and can return success while leaving it untouched when remote
handling is disabled. Preserve these contracts rather than introducing a generic
request/reply framework.

DirectPlay and Winsock really share H64's storage, but their startup structures
carry player IDs versus network addresses. DirectPlay declares char count/position;
Winsock declares u8 fields but reads them through char pointers and offsetof.
Do not normalize those promotions. Duplicate guest handling occurs before the
open-game guard in DirectPlay and inside it in Winsock; Winsock additionally
compares a player-record pointer with the incoming pointer, not record contents.
Neither gets a new player/session-capacity guard from a shared helper.

DirectPlay enumerates providers/sessions, stores provider GUID pointers, uppercases
the provided name, and uses its own create/join/accept/startup states. Winsock
changes fullscreen state, resolves/binds a nonblocking socket, validates entered
addresses and has separate automatic-host guest-count behavior. Their wait UIs
share H01/H30 prefixes, not a whole connection state machine. Failed/unknown
message behavior, system-sender filtering and host-accept handling differ.

UpdateNormalDialog is an existing near match to the status-text updates, but a
targeted complete body read shows two separately ranged DrawWindow calls after
its text broadcast. The network callers use a single DrawWindow() call. Favor
H01 for their shared message prefix; do not substitute this broader existing
routine or invent a second universal text-update helper. The full KB TU remains
unread despite this targeted callee check.

Winsock's broadcast send retries without incrementing attemptCount; its error
returns bypass packet disposal. Its receive loop can pass a non-WOULDBLOCK
WSAGetLastError value onward as a length. DirectPlay has distinct accepted-error
codes, receive-result handling and system-message filtering. DPSD sets its
reentrancy flag before the DP_OK early return and does not clear it there;
its error-text switch, three beeps, log and shutdown are not a generic OS-error
macro. H65 shares only message construction and must not repair these paths.
DirectPlay teardown drains the shared receive ring before array disposal;
Winsock does not. CleanupDPVars already names a specific shared reset, not a
reason to merge socket, COM-object and DLL lifetimes.

## R28 — NetBIOS and serial protocols, queues and local-only helpers

Read every definition in Netbios, netwin, Modem and comwin, including all private
callbacks, status stubs, variadic dispatch and data/macro owners. Netbios host
and guest setup differ in status increments, name collision retries, receive
arming and failure behavior. Their narrow enum/byte result conversions and the
registered/active/error masks already have names; a universal wait-state or
network-status predicate would lose those distinctions.

NETWIN's aliases identify existing static storage and NCB_INCLUDED controls an
SDK declaration boundary; neither is a new common-code macro. Its paired
nb_call/nb_listen NCB setup, callback-to-session scans, cancel prefixes and
inactive-session result lists are local-only leads. Do not hide adapter/name/
callback/event/command-complete differences in a generic NCB initializer.
nb_init partially clears its event storage, initializes seven sessions, and
uses its second argument for the narrowed max-session slot. nb_sess retains
per-operation varargs, early returns, move-before-detach order and the final
pending-to-success conversion. nb_format_name copies up to fifteen input bytes,
pads the remainder with spaces and leaves the sixteenth zero; it is not ordinary
strncpy or a string-copy helper.

nb_term closes and nulls event handles before setting event zero. nb_thr_ctl
processes specific receive events, then pops the first queue under a lock and
can retry sending indefinitely after inactive-session errors. Other callbacks
retry, rearm or cancel under different conditions and do not all set the same
adapter/post-routine fields. Allocation-failure guards in nb_recv_complete are
not MemError checks; nb_snd has a different unchecked allocation. No generic
async-operation wrapper, RAII lock, added failure return or timeout is introduced.

COMWIN's existing add_node/pop_node/init_anchor already own the actual link
protocol (H66/H67). NetBIOS nodes allocate length+11 and store a session byte
before payload; serial nodes allocate length+10 and begin payload at that byte.
Their union layouts and copy offsets are not one serialized packet contract.
NetBIOS receive compares node length to a signed parameter before a u16 fallback;
serial receive limits an unsigned request by the driver's count, writes a DWORD
through two i16 slots and returns the first slot. Preserve each truncation and
return convention rather than creating a common receive wrapper.

com_init resets all handle slots before searching for one, preserves driver
configuration/timeouts and uses its own baud enum/default conversion. com_term
drains queues only under a valid-handle guard; NetBIOS teardown holds its own
locks. Zero-length com_snd means a timed break, not an empty data packet.
comm_wrt_task's port pointer does not advance; its empty-queue return and partial
WriteFile loop remain unchanged. ShutdownComError snapshots GetLastError before
its error-name switch and appends localized suggestions; it is not DPSD's
reentrant error protocol or a shared string-format macro.

ModemSetup retains its queue-field reset order, baud-clock calculation, fixed
com_init baud choice, two reset attempts and separate direct/dial/wait paths.
GUIModemCommandExec and ModemCommand use different timing/return protocols;
GUIModemResponseExec reads one byte per callback, filters control characters,
terminates/truncates the buffer through the existing TruncateModemResponse
inline and matches a prefix. write_buffer checks the old queue position but
delegates the actual send; no queue-index update is invented.

Connect and WaitForDirectConnect share a local ID handshake, but blocking
continue/poll/drain and callback-state transitions differ. Tick-derived ID
formatting, six-byte copies without a new terminator, duplicate-ID cleanup and
second-boundary comparison remain explicit. ReadPacket's escape/partial-frame
state and WriteModemPacket's escape duplication, 256-byte limit and ForcePollSound
backpressure form a streaming framing protocol, not RemotePacketHeader or an
icon-RLE helper. No new negative-length/overflow repair, timeout or generic
buffer reader is introduced. These local leads do not inflate the cross-TU list.

## H69 — byte-wise CP1251 case folding

Disposition: reuse the existing uppercase inline; credible shared lowercase inline.

`GAME.cpp:81` and `ARMY.cpp:179` each define a private ToUpperCp1251. GAME's
ViewArmy and ARMY's combat messages use them on a creature name's first byte.
The already-shared `include/SOURCE/KB.h:481` CyrillicToUpper carries the same
byte mapping. `ARMY.cpp:166` ToLowerCp1251 also matches both expanded lowercase
sequences in GAME's DoNewTurn (month and week creature names). Natural owner:
the existing KB.h text-utility boundary, alongside CyrillicToUpper; no locale
facility or new text subsystem is needed.

Uppercase maps ASCII a-z and CP1251 E0-FF down by 32, B8 to A8, and leaves all
other bytes unchanged. Lowercase maps ASCII A-Z and C0-DF up by 32, A8 to B8,
and leaves the rest unchanged. Input must be interpreted as u8, evaluated once;
retain char/u8 result conversions and each caller's subsequent byte store.
CyrillicToUpper accepts char and casts for its comparisons, whereas the private
helpers accept u8. Their byte mappings agree, but local return/assignment CFG
and signed-byte conversion sites still need retail/codegen comparison before
replacement. The lowercase helper returns char while DoNewTurn stages u8.

Do not substitute a locale-dependent CRT function or MAKEFILEID's ASCII-only
folding. GenerateStandardFileName additionally filters characters, truncates
the basename, temporarily mutates the source dot and preserves the extension;
its whole loop is not a case-fold helper. EVENTS' searched lowercase occurrences
remain pending its complete read. Gain: one explicit localized-byte contract
instead of duplicated alphabet rules, favoring an API that already exists.

## H70 — read or write the exact storage size of one file value

Disposition: credible small expression-macro pair, not a serialization framework.

GAME's playerData::Read/Write and game::LoadGame/SaveGame/LoadMap repeatedly
use `read(fd, &value, sizeof(value))` or its write counterpart. Fully read
`EDITOR/mapcell.cpp` fullMap::Read/Write does the same for width, height and
extraCount. `BASE/RESMGR.cpp` ReadByte/ReadWord/ReadLong uses exactly the read
expression, assigning its result to an unused local. Natural owner: the BASE
file-utility boundary in Misc.h, near FileSize/CreatePCXFile, not game.h.

Possible names: READ_FILE_VALUE and WRITE_FILE_VALUE. Keep the CRT call's result
and argument conversions unchanged, so this remains an expression rather than
a void statement macro. Parenthesize arguments; evaluate fd once and the value
lvalue once through its address. The sizeof occurrence is unevaluated. Restrict
initial candidates to exact-size scalar/plain-record lvalues without overloaded
address-taking; no new temporary, initialization, retry, error test, byte swap
or allocation belongs inside. Preserve caller initialization, assertion, field
ordering and partial-read effects. The name states why this particular byte
count is correct and removes the repeated spelling of each field.

Exclude GAME's one-byte slices of wider globals, campaign/state prefixes,
event-header prefixes, arrays and dynamic payload lengths, char scratch buffers
read as i32, and `read(fd, this, sizeof(m_difficultyRating))`. HERO's whole/base
record methods are not field-lvalue instances. KB's searched read of one score
entry with sizeof(the entire entries array) is also not an exact-size instance;
do not silently change its byte count. Leave all such schema decisions visible.
The two consecutive barrier-tent writes/reads stay two calls. R10/R14's file-mode,
error and legacy-record exclusions remain in force.

## H71 — reuse the existing one-slot army dismissal operation

Disposition: existing-API reuse/possible inline, subject to call-site byte evidence.

`ARMYGRP.cpp:131` armyGroup::Dismiss stores CREATURE_NONE in the selected type,
then zero in its count, without checks or other changes. GAME's ClaimTown loop
and ViewArmy's accepted dismissal tail repeat those two stores. The creature/
troop aliases occupy the same packed slots; zero has the same i16/u16 bits.
Natural owner remains `include/SOURCE/armyGroup.h`, not a new global macro.

Keep group and slot stable, type-before-count order, and every enclosing guard,
loop, owner update and dialog result outside. The existing method is currently
out-of-line: replacing open code with a call or moving a body to a header is not
assumed byte-neutral. Preserve its callable ABI and prove any inline expansion.
Readability gain is naming dismissal through the existing domain operation.

Explicit noninstances: SetupTowns' default army reset writes count before type;
SetRandomHeroArmies uses RANDOM_HERO_EMPTY_COUNT = -1, not zero; record memset
followed by type-only initialization is different again. DamageGroup's count-
then-type death tail and CheckHeroConsistency's count-only repair also remain
outside this contract. No generic army reset is proposed.

## B29 — extensions to established candidates

H01 gains exact type/command/id prefixes in ViewSpells, UpdateSpellWidgets and
ViewArmyHandler's animation update. Preserve the earlier redundant type store
before UpdateSpellWidgets' memset; ViewArmy's reused/scattered headers are not
new contiguous instances. H30 gains text-only spell/army/transfer/new-turn
dialogs, excluding icon/resource payloads. H31 gains the checked spell/army
window and widget allocations, not the unchecked detail or transfer buffers.
H17 gains save-transfer/diff/join short logging tails; H68 gains their optional
retry-dialog/default-message tails, with reliability still explicit.

H20 gains BuildingsOwned, neutral reinforcements, ViewArmy upgrades, gold income,
PerDay/PerWeek/PerMonth, SetupTowns, GetNumThievesGuilds and CountShrines.
BuildingsOwned's mage-guild slot/buildState special case and faction gate are
outside the bit predicate. H22 gains GetLuck's embarked-state check. H24 gains
the terrain reads in puzzle/new-map/event/mine/visibility/month/rumour paths;
keep each current-map/adv-manager cell resolution and Column expression.
H26 gains ConvertObject and ComputeUALoc's ordered bounds; SetVisibility uses
a different x/y/lower/upper order, and NewMap uses a border margin.

H29 gains normal mana capacity in initial hero setup and PerDay, preserving
Stats(KNOWLEDGE) calls and i16 stores. H33 gains RandomizeEvents' four-elemental
exclusion, after its separate ghost test and before the conditional random draw.
H44 gains SetVisibility's integer square-root distance (y-square then x-square,
strict radius comparison). H07 gains ComputeUALoc's upper-then-lower clamp and
GetLuck's lower-then-upper clamp; later artifact overrides stay outside.
Existing owner accessors and H63's scouting-only exclusions are recorded in R26.

## R29 — game setup, serialization, calendar and transfer contracts

GAME's WORLDMAP macro is a receiver-local member-address alias, already explained
by game::GetWorldMapData. Its save-transfer local-name macros are scoped spelling
aliases, not reusable operations. RandomMineType, RandomizePassword and the
existing object/overlay/owner accessors already name small local operations.
The town-manager header's SetTown assigns only its pointer; it is not a town
context/activation workflow. No implementation coverage is inferred for TOWNMGR.

Player/save/map formats remain explicit. playerData::Write clears 48 scratch
bytes but serializes 42; cheated is a one-byte slice of the global game field;
barrier tents are written and read twice. SaveGame's expansion flag controls
the leading tag, hero records and artifact-table lengths, while campaign data
uses its own prefixes. Preserve field order, narrowed current-player/human
buffers, marker writes, and zero payloads for null map-extra records. LoadGame
does not validate markers or full read lengths; loadFromFile actually selects
SetupOrigData and returns. Existing wider global bytes and some campaign flags
are not uniformly reset. The filename path checks use localized strings with
legacy ASCII prefix lengths, and some sprintf calls use the filename as format.
GenerateStandardFileName temporarily removes/restores the source dot, filters
the uppercase basename to eight characters and copies the extension unchanged;
without a dot it simply copies. No new string/format repair is hidden in H69/H70.

LoadMap's legacy world-map conversion, one-byte coordinate/type records,
base-format mine limit and final u16 junk differ from LoadGame's chunked stream.
Map-extra allocation orders pointer array, size array, then both clears, and
starts record processing at index one. SetMapSize may skip search-array Init
for unchanged dimensions but always frees/reallocates/zeros mapExtra. Resource
ownership, old-record prefix conversion and unchecked sizes remain visible.

Hero/player selection is not a shared container policy. NextHero scans the
global current player's ids but returns this player's corresponding id; other
queries use this directly. Scan returns the first -1 slot; RandomScan ignores
one apparent retry argument. GetNewHeroId relaxes availability/class/experience/
campaign-portrait filters at different attempt counts and ultimately returns
its last draw, not a failure sentinel. HeroIDToHeroPos/TownIDToTownPos return the
first match, unlike the previously read last-match removal/context searches.
NumOfGivenArtifact counts duplicate artifact slots. GetNumObelisks scans the
boat-count-sized visitor array, not the number of placed obelisks.

Puzzle generation retains both random streams and exact draw timing. The
zero-obelisk division precedes the all-visited special case; justCount returns
before seeding. Piece removal performs its fallback random draws even when
the preferred target is usable. ComputeUALoc generates nonzero x and y offsets
separately and eventually falls back to the real location without validating
it. NewMap's ultimate-artifact search uses margins and early human-distance
restrictions, draws its distance threshold before and during the loop, and
does not share ComputeUALoc's trigger test or retry policy. SetupNewRumour's
direction tests repeatedly use artifact X against both width and height; a
coordinate helper must not silently substitute Y.

Initial player/hero/town resets, campaign awards and placement retain distinct
store order and side effects. Base campaign awards add experience, expansion
awards assign it. NewMap's setup mapping and ProcessOnMapHeroes' assignment,
class and placement passes are whole-map phases, not one per-cell pass.
The latter consumes extras only after placement, adjusts hero/patrol Y above a
town, and applies custom skills after experience/level work. ClaimTown sends
before its same-owner return, may deallocate through global gpGame, clears the
army, writes ownership/list state and changes two flags before optional
visibility/end-game work. ClaimMine always sends, has different resource/flag
coordinates and late-overlay rules, and supports player -1. CreateBoat saves
the old cell trigger/metadata; it is not SummonBoat's relocation workflow.

RandomizeEvents has three ordered map passes: event generation, shadow-only
marking and occupancy. Extra-layer scanning omits the dummy exclusion in H25;
its capped upper/lower lists compare tileset families, not sprite identity.
ConvertObject stops each extra-chain traversal when the next node lacks that
layer's sprite, whereas HasLateOverlay and the overlay conversion routines
continue through all nodes. ProcessMapExtra also has three distinct passes.
Do not introduce a universal layer iterator that changes termination or order.

Random object/town/mine/artifact/army generation is not one weighted-choice
helper. Preserve open monster-strength bounds, the terrain-specific mine
selection retries, random artifact exclusions and uniqueness counter timing,
and the draw consumed even for a spell scroll's early event exit. Monthly
monster spawning uses a different empty-tile test and two troop-count draws;
RandomizeEvents excludes ghosts/elementals from its guard roll, PerMonth does
not. GetRandomNumTroops has per-creature ranges and a no-draw default of three.
Town spell generation seeds guaranteed spells before rejection sampling; its
attempts++ > 500 condition continues rejecting instead of breaking the loop.
Preserve short-circuit draw consumption and packed spell/count aliases.

Calendar/income updates preserve all-player/all-record traversal, including
off-map town ages and hero mana regeneration. Gold counts artifact duplicates,
estates and separate AI/handicap multipliers. Non-gold handicap applies after
the day/week/month update and uses the saved income delta; it is not a second
application of the gold multiplier. Weekly site thresholds are tested before
adding growth and are not saturation bounds. Weekly neutral/AI adjustments
differ from the monthly plague subtraction and halving. PerDay computes
giCurTurn before rollover; the same day-number expression has searched KB/
PHILAI counterparts still awaiting full reads, not a promoted shared helper.
Mana restoration preserves existing over-cap mana and clears the well flag.
GetLuck applies the battle-garb override after its clamp. ExperienceValueOfStack
tests positive quantity, not H21's type sentinel. GiveArmy's explicit slot,
merge, negative-type empty slot and no-room paths remain its own operation.

Spell and army dialogs retain globals, byte narrowing and caller-owned payloads.
Read-only spell selection consumes an event, insufficient mana continues, and
accepted selection forwards a retyped event. Tab command/id mappings and close
rewrites are not H32's completion-flag operation. ViewArmy uses fixed coordinates,
base and live monster records for different statistics, uncapped spell count
for spacing, and capped count for visible icons. The handler clears dismissal/
upgrade flags on every dispatch. Upgrade confirmation charges the current
player and only then forwards the close request. Timer strictness, repeated
GetManaCost/KBTickCount calls and zero-count assumptions remain explicit.

NextPlayer and DoNewTurn order end-game checks, town/hero deactivation, per-day
rollover, context, local/remote UI, save transfer, music and environment updates
differently. ShowComputerScreen temporarily changes local-human state and has
additional bottom-view work absent from WaitForPlayer. TurnOffAIMusic only
sets the ready flag; it is not an inverse sound-lifecycle operation. No generic
context, blackout or audio guard is inferred from superficially paired code.

TransmitSaveGame and ReceiveSaveGame retain compression aliases and different
allocation/free ordering. Sender writes only unacknowledged packets in batches;
receiver consumes messages without sender filtering, uses signed packet indices,
and replies with the payload-size byte count after initializing only a batch's
acknowledgements. Received/transmit CRCs are computed and logged, not enforced.
Timeout dialogs, shutdown paths, save/diff/join ordering, and temporary music
readiness changes remain outside H17/H68. No packet-bounds or cleanup repair is
introduced. CreateDiffFile may read the old file even when a recipient change
forces a full send; forceWhole is a different condition. Its terminal copy uses
oldSize-position, and CreateJoinFile advances existing output for skip commands.
WriteDiffHeaderInfo/GetSkipCopyLen own their short/byte/word local format, not
a generic varint or RLE reader. Compression tests do not assert their CRCs.

CheckHeroConsistency repairs selected counts/cells without normalizing every
type, metadata or owner slot. CheckValidAvailableHeroes changes the available
id but does not add an availability-table update. GetMapEvent checks the first
active coordinate event for the current player's color; time events instead
use date/repeat and human/AI applicability. Time-event resource display amounts
are clamped separately from adding the original reward and clamping the stored
balance; only the last two nonzero resource entries are displayed. CountShrines
walks map triggers/occupying heroes and checks owner, tavern bit and necromancer
faction, rather than simply scanning every town record.

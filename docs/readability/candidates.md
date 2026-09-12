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

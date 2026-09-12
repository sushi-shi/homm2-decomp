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

Next family: manager/window dispatch and low-level resource/graphics consumers.
The BITS assembly has been inspected but remains unmarked pending declaration and
caller review. Other sampled files likewise remain unread.

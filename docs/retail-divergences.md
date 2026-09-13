# Intentional retail divergences

The reconstruction branches preserve the behavior and code generation observed
in the retail executables. `master` starts from that evidence but may correct a
proven defect or replace a platform-bound subsystem when retaining it would make
the maintained port less safe or less portable.

This ledger records deliberate game-source differences from Gold 2.1/Buka. It
does not enumerate the SDL3 platform implementation, UTF-8 localization layer,
or other new portable facilities that have no retail counterpart. Cross-version
differences between PoL 2.0 and Gold 2.1 remain in the retail evidence ledger
linked from [Retail version differences](version-differences.md).

Any future `master` change that intentionally alters a retail game-source
outcome must update this ledger in the same commit. Source-only restructuring
belongs in the section below; observable defect corrections belong in the
table.

## Maintained source structure

The exact Gold/Buka reconstruction retains a few source shapes solely because
they reproduce the Visual C++ 6 executable: redundant labels, explicit Boolean
materialization, and casts at boundaries whose underlying retail value is an
integer. `master` does not preserve those compiler-sensitive spellings when the
same behavior has a clearer portable representation.

In particular, the castle dialog's selection value can hold either a
`BuildingSlotType` value or a UI control identifier. `master` models that mixed
discriminant as an integer and converts to `BuildingSlotType` only when entering
a building API. The overview return path also tests its occupied-town pointer
with `!= nullptr`; the exact branch retains the retail pointer-to-integer truth
expression because the clearer spelling changes VC6 code generation. Redundant
jumps with one natural structured successor are also
expressed as fallthrough, loop exit, or ordinary conditional flow. Jumps that
still represent a real shared tail, cleanup path, retry loop, or multi-loop exit
remain explicit.

Retail also reaches a few logically distinct palette and campaign-name tables
through their linker-defined adjacency. `master` names those tables explicitly,
preserving the selected values without relying on out-of-bounds pointer or
array arithmetic. Repeated UI formatting tails in adventure quick info, radar,
Visions, and the town screen are represented by local helpers or an explicit
outcome selection instead of cross-case jumps.

Numeric enum construction is also explicit on `master`. Public domains cross
integer storage through the named `FromCode` and `FromOrdinal` entry points in
`EnumCode.h`; private protocol enums keep an equivalent decoder next to their
definition. Packed enum storage exposes an already-typed `enum_value()` instead
of re-decoding it. A source-policy test rejects direct numeric enum casts and
bypasses of the shared low-level conversion.

The Overview's dynamic widget tables use their existing 70-slot row types on
`master`, with direct `[row][slot]` access and declarations in `Overview.h`.
Initialization, replacement and cleanup still use four rows; the original
11200-byte allocations and allocation/free pairing are unchanged. This is a
source-ownership improvement, not a claim of an original-game defect. The
corresponding decomp experiment (PR #60, CP03) changed VC6's stride calculation
from multiply-by-70 plus scaled LEA to multiply-by-280 plus ADD, so it was not
retained on the matching branch. `overview_rows` checks every slot of both row
types and their declared global pointer types without depending on a display.

The [complete cast/pointer review](casts-pointers-master.md) records all 148
inherited functions, including dependencies on other open portable PRs and
remaining caller contracts. On master, flat AI arrays use direct indexing,
redundant destination-type casts are removed, and real numeric/API conversions
remain explicit. Palette byte access uses an unsigned object-representation
view where the algorithm requires unsigned channels; the signed brighten path
is unchanged.

Save-event prefixes are encoded as actual four-byte records containing the
count and first index, instead of copying across the two-byte count's boundary.
The repeated first index (including when count is zero) and runtime field layout
are preserved. Legacy packet buffers have real header/payload owners, and queued
message/payload representations are copied into live objects instead of accessed
through struct overlays. This models ownership; it does not establish that
every malformed network command is safe or make portable multiplayer supported.

## Corrected defects

| Area | Retail behavior | `master` behavior |
| --- | --- | --- |
| Diff/join file extents | The terminal diff literal uses the old file's remaining length, overreading a shorter new file or omitting a longer new tail; join skip commands can select bytes beyond the old file. | Uses the new file's remaining length, bounds source file allocations and encoded input, and requires skip commands to stay within initialized old-file bytes. Empty files still have valid allocated storage for zero-length operations. |
| Unrepresentable packet length | Encoding copies a payload before narrowing its length to a single wire byte, allowing a mismatched length or oversized copy. | Rejects negative lengths, lengths above 255, and nonempty null payloads before mutation; valid packet bytes and CRC are retained. No wider wire-length format is invented. |
| Save event and extra sizes | Saving assumes runtime event counts and variable-record sizes fit their arrays and scratch space. | Validates event capacities, extra tables and nonnegative scratch-bounded record sizes before writing. Actual extra allocation extents remain a lifecycle contract and the subject of the variable-record PR. |
| Empty campaign/puzzle state | Campaign award/carryover paths select a first hero without checking that one exists; puzzle interpolation divides by the obelisk count even when it is zero. | Guards the three first-hero paths and avoids zero-obelisk division while retaining the existing all-obelisks puzzle fallback. |
| Ripple source extents | The effect assumes 640-pixel rows and computes displaced source pointers before validating height and displacement. | Requires matching supported widths, valid pixels/heights and bounded strength, and skips empty displaced columns before forming pointers. |
| Adventure control selection | A dead current player's search never advances beyond its first candidate; a no-candidate path indexes the local-human table with -1. | Advances the search and treats no selected controller as false. The existing second-loop selection order is retained; portable multiplayer is still unsupported. |
| Adventure map edges | The radar tests the right neighbor of the final column; the hero/boat redraw path marks column -1 at drawX=1, and resets only 256 bytes of its 324-byte grid. | Guards the right-neighbor lookup and both far-left redraw writes, and clears the complete grid. Missing-current-hero search and invalid map-extra starting coordinates return before indexing. |
| Dimension Door candidates | AI reads a candidate's visited flag before checking whether its coordinates lie in the map. | Checks coordinates first, preserving all valid-candidate ordering. |
| AI absent objects | Obelisk valuation indexes the absent-artifact sentinel and can divide by zero; town threat evaluation forms a hero pointer for an unoccupied town. | Returns zero for no artifact/no obelisks, and evaluates an unoccupied town's garrison without a fabricated hero or double-counted army. |
| Animation concatenation | Positive frame counts are appended to fixed 16-byte frame and offset rows without a cumulative bound. | Copies only the remaining destination capacity; valid sequences are unchanged, oversized sequences are truncated together with their offsets. |
| Spell palette and masks | Red-bolt palette lookup sign-extends pixels >=128; vaporize forms before-owner intermediate pointers, and ripple fades can cross the 480-row mask. | Uses the unsigned pixel as palette index and clips effect-mask writes before pointer formation. |
| Hero/town removal | Missing owner/list membership or empty lists can form negative-index pointers during removal; retreat checks may index the absent available-hero sentinel. | Rejects invalid removal state before side effects and guards both available-hero lookups. Other gameplay lifecycle invariants remain caller obligations. |
| Incomplete trade selection | Selecting the left resource while the right remains -1 indexes before the ratio table. After a trade, stale quantity/range can survive reset selection. | Initializes a disabled trade for invalid selections, guards execution, and recomputes disabled state after a completed trade. |
| Portable font wrapping | The UTF-8 replacement's negative-width empty-word path can index an empty character vector. | Normalizes negative widths to zero; existing callers allocate input length plus hyphen and terminator. |
| Bitset access width | The bit helpers address the byte containing the selected bit but load/store a 32-bit word there, crossing short bitsets and possibly using an unaligned address. | Accesses only the selected byte; the bit number, return value and all neighboring bits remain unchanged. A one-byte allocation reproduces the old heap-buffer-overflow under AddressSanitizer and passes with byte-width accesses. |
| Initial mouse cursor | A newly created configuration starts with the monochrome system cursor, reflecting the original hardware-cursor fallback. | New portable configurations start with the original color cursor artwork. Existing saved preferences remain authoritative. |
| Campaign table bounds | The enabled-map table indices are reversed after switching campaign sides, and the 13-point campaign track reads the 12-entry enabled-map table at its final point. | Indexes the table as `[campaign side][scenario]` and checks the map-table bound before reading track state. |
| Aggregate lookup failure | `resourceManager::PointToFile` and `GetFileSize` continue with an invalid aggregate entry after calling the shutdown path. A shutdown implementation that returns or re-enters can dereference that invalid state. | Returns immediately after reporting the fatal lookup error. |
| Manager destruction | Several dialogs are allocated as a concrete manager and deleted through `baseManager*`, whose retail vtable has no virtual destructor. | Gives the portable manager hierarchy a virtual destructor, so deletion through the owning base pointer is defined and reaches the concrete destructor. |
| Allocation ownership | Map buffers allocated by `H2_ALLOC` are released with `delete`, the adventure visibility array uses scalar `delete` after `new[]`, and text-entry construction can abandon an owned text allocation. | Pairs each allocation with `H2_FREE` or `delete[]` as appropriate and releases the temporary text allocation before replacing it. |
| Invalid encoded game domains | Unknown recruit-site and town-faction values can leave a creature type uninitialized before it is indexed or added to an army. | Rejects invalid values at the owning switch instead of continuing with indeterminate state. |
| AI special-direction movement | The shortcut into the movement loop bypasses initialization of the stop/notification arguments passed to `MoveHero`. | Initializes the movement arguments before either normal or shortcut entry. |
| Spell edge paths | Hero-cast spells assume the current side has a hero, targeted spells assume an occupied source hex, and Armageddon's headless path uses target and palette pointers that may never have been initialized. | Rejects a missing hero at the cast boundary, cancels any targeted spell with no living army target, rejects the same impossible target state during AI evaluation, guards the optional visual target, and skips palette restoration when no visual palette was created. |
| Fixed-size paths, names, and formats | Several retail `sprintf` calls treat external or localized text as a format string or assume campaign, dialog, map, and movie paths fit their local buffers. The map/save requester also copies `"*.MP2"` or a similar six-byte pattern into a five-byte default-extension field. | Routes portable formatting through a capacity-aware UTF-8 helper, copies plain text without interpreting it, builds temporary names dynamically, validates the one fixed-size serialized campaign filename, and passes only the extension (`".MP2"`) to the requester. The requester also bounds its pattern, directory and extension copies by their actual destination capacities. A source-policy test rejects unbounded formatting from the portable tree. |
| Truncated external records | Retail generally ignores host read/write counts, so a truncated AGG header, map header, or preferences file can leave partially initialized state and an AGG entry count can drive an invalid allocation. | Exact-transfer helpers complete short host operations without spinning; the AGG loader bounds the directory by the file length, map headers reject short records, preferences fall back to defaults, and incomplete resource payloads are logged and zeroed. |
| Direct-connect identifier | A six-byte identifier copied with `strncpy` is not terminated when all six source bytes are nonzero. | Copies the fixed-width field and explicitly terminates its seven-byte destination. |
| Remote duplicate-filter reset | Startup clears only 30 bytes of the 30-element `i32` recent-message-ID array, leaving most entries from a previous session intact. | Clears the complete array with `sizeof(iLastIds)`. This belongs to retained legacy transport code; portable multiplayer is not currently supported. |
| UDP send failure | A failed broadcast send tests `attemptCount` but never increments it, so a persistent socket error retries forever; both send-error exits also leak the packet buffer. | Makes at most 20 send attempts, delaying only between attempts, frees the packet on either error path, then reports the error. This belongs to retained legacy transport code; portable multiplayer is not currently supported. |
| Millisecond, cursor, and fizzle timing | Signed absolute comparisons against the wrapping 32-bit tick counter can suppress cursor repaint, palette cycling, and sound polling after the signed boundary, while other signed comparisons can terminate a wait early or extend it across a wrap boundary. `FizzleForward` also starts its first frame from tick zero, so that frame normally receives no delay. | Uses modular deadline comparisons for polling and waits, and starts the fizzle cadence from the current tick. |
| Wagon and lean-to sound | Their event-sound cases select the pickup sound and then fall through, overwriting it with the experience sound. | Stops after selecting the pickup sound, as the otherwise-dead assignment and the event category indicate. |
| AI single-creature stack value | Two consecutive strength thresholds both test for more than two creatures, leaving the `-0.4` modifier unreachable and assigning a one-creature stack the zero-creature modifier. | Uses the evident descending threshold of more than one creature, preserving distinct modifiers for stacks of two, one, and zero creatures. |
| Combat obstacle sentinel | The random obstacle roll includes value 32 although the table and `cobj` resources end at 31. Retail indexes one record past the table, where the adjacent Estates table happens to begin with zero, and retries. | Preserves the same inclusive random roll and random-generator state, but treats 32 explicitly as the retry sentinel before indexing the obstacle table. |
| Neutral-town human lookup | Random dwelling setup indexes the human-player table with the neutral-owner sentinel `-1`. The retail image happens to read adjacent storage, while instrumented portable builds diagnose a global buffer overflow. | Treats the neutral owner as nonhuman before consulting the player-indexed table. |
| Animated-map redraw boundary | Marking a monster in the leftmost visible map column also marks the nonexistent column to its left, writing before the redraw grid. Instrumented portable builds abort when a monster reaches that boundary. | Clips the missing left neighbor while retaining all in-view redraw marks. |

## Replaced subsystem

### Network-save compression

Retail uses the recovered legacy Bzip codec through temporary files and exposes
its implementation globally. `master` instead uses libbz2 1.0.x in memory
through the small `compression::Bzip2*` interface. Callers provide explicit
source and destination capacities, compression uses the documented worst-case
bound, and failures do not continue with an indeterminate length.

The resulting compressed stream is a standard bzip2 stream and is not promised
to be wire-compatible with the retail network-save protocol. Network transports
are not currently supported by the portable platform. The exact reconstruction
and generated retail-source branches retain the original codec and format.

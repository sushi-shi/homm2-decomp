# Intentional retail divergences

The reconstruction branches preserve the behavior and code generation observed
in the retail executables. `port` starts from that evidence but may correct a
proven defect or replace a platform-bound subsystem when retaining it would make
the maintained port less safe or less portable.

This ledger records deliberate game-source differences from Gold 2.1/Buka. It
does not enumerate the SDL3 platform implementation, UTF-8 localization layer,
or other new portable facilities that have no retail counterpart. Cross-version
differences between PoL 2.0 and Gold 2.1 remain in the retail evidence ledger
linked from [Retail version differences](version-differences.md).

Any future `port` change that intentionally alters a retail game-source
outcome must update this ledger in the same commit. Source-only restructuring
belongs in the section below; observable defect corrections belong in the
table.

## Maintained source structure

The eleven ICN drawing entry points share a bounded colour/mask decoder.
Sheared rows retain retail's previous-row displacement; mirrored clipping is
corrected (see the table). The 13-byte frame wire record uses
the complete kind byte, including the mask tag 32, and the drawing path reads
its little-endian fields explicitly. Payload length belongs to the runtime
icon object rather than being discarded after loading.

The exact Gold/Buka reconstruction retains a few source shapes solely because
they reproduce the Visual C++ 6 executable: redundant labels, explicit Boolean
materialization, and casts at boundaries whose underlying retail value is an
integer. `port` does not preserve those compiler-sensitive spellings when the
same behavior has a clearer portable representation.

In particular, the castle dialog's selection value can hold either a
`BuildingSlotType` value or a UI control identifier. `port` models that mixed
discriminant as an integer and converts to `BuildingSlotType` only when entering
a building API. The overview return path also tests its occupied-town pointer
with `!= nullptr`; the exact branch retains the retail pointer-to-integer truth
expression because the clearer spelling changes VC6 code generation. Redundant
jumps with one natural structured successor are also
expressed as fallthrough, loop exit, or ordinary conditional flow. Jumps that
still represent a real shared tail, cleanup path, retry loop, or multi-loop exit
remain explicit.

Retail also reaches a few logically distinct palette and campaign-name tables
through their linker-defined adjacency. `port` names those tables explicitly,
preserving the selected values without relying on out-of-bounds pointer or
array arithmetic. Dim-palette selection indexes the set and level separately;
it does not reach other levels by indexing beyond the first 256-color subarray.
Repeated UI formatting tails in adventure quick info, radar,
Visions, and the town screen are represented by local helpers or an explicit
outcome selection instead of cross-case jumps.

Numeric enum construction is also explicit on `port`. Public domains cross
integer storage through the named `FromCode` and `FromOrdinal` entry points in
`EnumCode.h`; private protocol enums keep an equivalent decoder next to their
definition. Packed enum storage exposes an already-typed `enum_value()` instead
of re-decoding it. A source-policy test rejects direct numeric enum casts and
bypasses of the shared low-level conversion.

The Overview's dynamic widget tables use their existing 70-slot row types on
`port`, with direct `[row][slot]` access and declarations in `Overview.h`.
Initialization, replacement and cleanup still use four rows; the original
11200-byte allocations and allocation/free pairing are unchanged. This is a
source-ownership improvement, not a claim of an original-game defect. The
corresponding decomp experiment changed VC6's stride calculation
from multiply-by-70 plus scaled LEA to multiply-by-280 plus ADD, so it was not
retained on the matching branch. `overview_rows` checks every slot of both row
types and their declared global pointer types without depending on a display.

A review of the inherited cast and pointer findings left `port` with direct
indexing for flat AI arrays and without redundant destination-type casts;
numeric/API conversions, including every narrowing conversion, remain
explicit. Palette byte access uses an unsigned object-representation
view where the algorithm requires unsigned channels; the signed brighten path
is unchanged.

Save-event prefixes are encoded as actual four-byte records containing the
count and first index, instead of copying across the two-byte count's boundary.
The repeated first index (including when count is zero) and runtime field layout
are preserved. Legacy packet buffers have real header/payload owners, and queued
message/payload representations are copied into live objects instead of accessed
through struct overlays. This models ownership; it does not establish that
every malformed network command is safe or make portable multiplayer supported.

Runtime manager, widget, resource, audio playback and combat objects also use
natural alignment. Their pointers, messages and numeric members are passed by
reference in portable C++; retaining byte packing made input initialization and
music playback undefined. Packed map cells, resource headers and configuration
records retain their file layouts. The `runtime_alignment` check rejects
misaligned manager messages, music state and resource pointers at compile time.

## Corrected defects

| Area | Retail behavior | `port` behavior |
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
| Display presentation | F4 switches the legacy fullscreen mode; filtering and VSync are not player preferences. | F4 switches the SDL window in place; Shift/Ctrl+F4 select scaling/VSync. Separate text preferences preserve the retail configuration layout. The `display_settings` CTest checks buffer/cursor preservation, coordinate mapping and persistence. |
| Combat background-cache rebuild | Matching-source lineage clears the entire working screen after rebuilding terrain, even when the next attack redraws only its dirty rectangle. Whether this is visible through the original Windows backend is not yet verified. The native backend's enlarged updates demonstrably expose bare background to the right and below. | Rebuilds the terrain cache separately, then restores only the same rectangle used for sprite redraws. The optional real-asset `homm2_combat_redraw_test` compares attack frames after cache invalidation with complete renders. |
| Mirrored sprite clipping | Solid, shadow, recolour and mask runs crossing a clip edge disappear entirely, leaving background-coloured gaps when battle animations redraw only part of a neighbouring sprite. Attack redraw bounds accumulate a one-pixel expansion each frame, making these gaps follow outward-moving rectangular edges. | Clips the visible portion of mirrored runs in the shared ICN decoder, for every colour, shadow, recolour and mask variant. |
| Combat effect redraw bounds | Recoloured and row-distorted sprites ignore the partial redraw rectangle. Their shadows can be applied repeatedly to pixels whose background was not restored, producing dark fringes. | Uses the same restored rectangle for normal, recoloured and distorted combat sprites. The `combat_sprite` regression compares partial redraws, including expanding rectangles, with a fresh full render in both orientations. |
| Initial mouse cursor | A newly created configuration starts with the game's custom monochrome cursor, rendered by Windows from the original bitmap resources; this is not the desktop arrow. | New portable configurations start with the original color cursor artwork. Existing saved preferences remain authoritative. Monochrome mode now uses the original artwork and hotspots through SDL; the initial native port had omitted custom cursor selection. See [Cursor rendering](cursor-rendering.md). |
| Campaign table bounds | The enabled-map table indices are reversed after switching campaign sides, and the 13-point campaign track reads the 12-entry enabled-map table at its final point. | Indexes the table as `[campaign side][scenario]` and checks the map-table bound before reading track state. |
| Aggregate lookup failure | `resourceManager::PointToFile` and `GetFileSize` continue with an invalid aggregate entry after calling the shutdown path. A shutdown implementation that returns or re-enters can dereference that invalid state. | Returns immediately after reporting the fatal lookup error. |
| Manager destruction | Several dialogs are allocated as a concrete manager and deleted through `baseManager*`, whose retail vtable has no virtual destructor. | Gives the portable manager hierarchy a virtual destructor, so deletion through the owning base pointer is defined and reaches the concrete destructor. |
| Allocation ownership | Map buffers allocated by `H2_ALLOC` are released with `delete`, the adventure visibility array uses scalar `delete` after `new[]`, and text-entry construction can abandon an owned text allocation. | Pairs each allocation with `H2_FREE` or `delete[]` as appropriate and releases the temporary text allocation before replacing it. |
| Invalid encoded game domains | Unknown recruit-site and town-faction values can leave a creature type uninitialized before it is indexed or added to an army. | Rejects invalid values at the owning switch instead of continuing with indeterminate state. |
| AI special-direction movement | The shortcut into the movement loop bypasses initialization of the stop/notification arguments passed to `MoveHero`. | Initializes the movement arguments before either normal or shortcut entry. |
| Spell edge paths | Hero-cast spells assume the current side has a hero, targeted spells assume an occupied source hex, and Armageddon's headless path uses target and palette pointers that may never have been initialized. | Rejects a missing hero at the cast boundary, cancels any targeted spell with no living army target, rejects the same impossible target state during AI evaluation, guards the optional visual target, and skips palette restoration when no visual palette was created. |
| Sphinx answer text | Answers are matched by their first four single-byte characters, ignoring case. | Matches the first four Unicode characters after UTF-8 decoding, retaining the legacy prefix and trailing-space rules. |
| Fixed-size paths, names, and formats | Several retail `sprintf` calls treat external or localized text as a format string or assume campaign, dialog, map, and movie paths fit their local buffers. The map/save requester also copies `"*.MP2"` or a similar six-byte pattern into a five-byte default-extension field. | Routes portable formatting through a capacity-aware UTF-8 helper, copies plain text without interpreting it, builds temporary names dynamically, validates the one fixed-size serialized campaign filename, and passes only the extension (`".MP2"`) to the requester. The requester also bounds its pattern, directory and extension copies by their actual destination capacities. A source-policy test rejects unbounded formatting from the portable tree. |
| Serialized save filename | The extensionless path copies the complete input into a 100-byte scratch buffer; the extension path also copies an unbounded suffix. | Constructs a terminated, zero-padded 14-byte wire field directly, with an eight-character uppercase ASCII stem and at most three ASCII extension characters. Normal retail filenames retain their bytes. Long, extensionless and Unicode names are normalized only in this informational legacy field; the native save path is unaffected. |
| Truncated external records | Retail generally ignores host read/write counts, so a truncated AGG header, map header, or preferences file can leave partially initialized state and an AGG entry count can drive an invalid allocation. | Exact-transfer helpers complete short host operations without spinning; the AGG loader validates little-endian directory entries against the payload area, excluding directory and filename-table bytes. Resource reads stay within the selected member, nested positions preserve member identity, and failed reads stop loading. Fixed-width resource names require a terminator. Map headers reject short records and preferences fall back to defaults. |
| ICN resource and drawing bounds | Retail trusts ICN payload lengths, frame indices and offsets, consumes RLE without an end pointer, and forms destination/shear pointers for off-screen rows. Scaled drawing accepts zero/invalid divisors and ignores its clip rectangle below native scale. | The loader checks the member length and every frame/stream before use. All live drawing variants reject malformed streams before painting, bound source and destination accesses, and use bounded shear spans and defined dim-table indexing. Scaling accepts 1–32, retains retail sample positions and respects both the surface and caller clip. Valid retail output is compared with the established pixel model. |
| Resource failure before cursor selection | Shutdown switches cursor modes while the current type can still be the initial `-1` sentinel, indexing before the cursor-offset table. | A mode change before a valid cursor selection updates the host cursor visibility without reloading a nonexistent frame. The malformed-ICN startup check exercises this path. |
| Serialized text padding | The portable text-encoding scratch buffers for player names and the tavern rumour leave bytes after the terminator uninitialized; whole-field writes include those stack bytes. Retail predates these encoding buffers. | Zero-initializes the serialized fields before encoding, producing deterministic padding without changing their size or decoded text. |
| Save replacement | Opens the destination with truncation before writing the save and does not check close status. An interrupted or failed save can destroy the previous file. | Writes a unique temporary sibling, checks flush and close, then replaces the destination. Failures before replacement preserve the previous save; native file-data flush and browser persistence semantics are documented separately in [Save replacement](save-transactions.md). |
| BMP/TIL allocation and drawing | File dimensions and tile counts participate in unchecked allocation products; tile indices, bitmap copies, fills and dim operations can address beyond their surfaces. `bitmap::CopyTo` assumes a 640-byte stride, and negative careful-copy coordinates can select the wrong source pixels. | Raster loaders validate types, positive dimensions and complete member payloads before allocating. Tile reads validate the selected index/span. Drawing clips against source and destination bounds with widened arithmetic; copies use actual strides and preserve overlapping source data. Dim levels use defined nested palette indexing. Backdrops follow the validated BMP path. The explicit fill-clip helper retains its retail edge-exclusion rule. |
| Invalid map headers and unterminated file text | Fixed-width map/save names and descriptions are consumed as C strings, allowing a missing terminator to read subsequent records. Map header counts and enum values can reach UI arrays without validation. | Bounds text decoding and encoding detection by each field's capacity. Rejects headers with invalid format, dimensions, counts, flags, domains or missing text terminators; save loading also rejects unterminated player, hero, town, rumour and default-player fields before decoding or use. |
| Variable map records and cell chains | Typed event/hero/town accesses trust record indices and lengths; variable text and fixed sphinx answers can run past their allocations. Invalid skill/creature indices reach tables, and cyclic/out-of-range map-cell chains are followed without validation. | Validates referenced records, text terminators, array-index domains, and reachable cell chains at loading boundaries. Runtime accesses retain allocation bounds, including events beneath heroes in saved games. Preserves decorative tiles, unused editor slots, custom portraits and consumed hero/town save records. |
| Direct-connect identifier | A six-byte identifier copied with `strncpy` is not terminated when all six source bytes are nonzero. | Copies the fixed-width field and explicitly terminates its seven-byte destination. |
| High-score record transfers | The recovered update loop requests the size of the complete ten-entry table at each individual entry and ignores end-of-file. Excess data can overwrite the destination array. Stored names are also consumed without verifying terminators. | Reads and writes one explicit 100-byte little-endian record per entry, retains complete preceding records on a short tail, reads an entry with an unterminated name as empty while keeping the records after it, and bounds newly entered names to their UTF-8 field capacities. This also avoids the previous portable exact-read fallback discarding entries after the first whole-table read. |
| Remote duplicate-filter reset | Startup clears only 30 bytes of the 30-element `i32` recent-message-ID array, leaving most entries from a previous session intact. | Clears the complete array with `sizeof(iLastIds)`. This belongs to retained legacy transport code; portable multiplayer is not currently supported. |
| UDP send failure | A failed broadcast send tests `attemptCount` but never increments it, so a persistent socket error retries forever; both send-error exits also leak the packet buffer. | Makes at most 20 send attempts, delaying only between attempts, frees the packet on either error path, then reports the error. This belongs to retained legacy transport code; portable multiplayer is not currently supported. |
| Millisecond, cursor, and fizzle timing | Signed absolute comparisons against the wrapping 32-bit tick counter can suppress cursor repaint, palette cycling, and sound polling after the signed boundary, while other signed comparisons can terminate a wait early or extend it across a wrap boundary. `FizzleForward` also starts its first frame from tick zero, so that frame normally receives no delay. | Uses modular deadline comparisons for polling and waits, and starts the fizzle cadence from the current tick. |
| Wagon and lean-to sound | Their event-sound cases select the pickup sound and then fall through, overwriting it with the experience sound. | Stops after selecting the pickup sound, as the otherwise-dead assignment and the event category indicate. |
| AI single-creature stack value | Two consecutive strength thresholds both test for more than two creatures, leaving the `-0.4` modifier unreachable and assigning a one-creature stack the zero-creature modifier. | Uses the evident descending threshold of more than one creature, preserving distinct modifiers for stacks of two, one, and zero creatures. |
| Combat obstacle sentinel | The random obstacle roll includes value 32 although the table and `cobj` resources end at 31. Retail indexes one record past the table, where the adjacent Estates table happens to begin with zero, and retries. | Preserves the same inclusive random roll and random-generator state, but treats 32 explicitly as the retry sentinel before indexing the obstacle table. |
| Combat stack-count damage clipping | The status bar respects the animation's dirty rectangle, but its digits paint their entire text box. Digits outside the restored area can overwrite an occluding creature; cursor updates then expose white vertical fragments on that neighbour. Reproduced with two adjacent peasants and a Cyclops below them. | Clips count text to the same dirty rectangle as the bar, while retaining the original text box for centering and wrapping. Ordinary full redraws and other text callers are unchanged. |
| Neutral-town human lookup | Random dwelling setup indexes the human-player table with the neutral-owner sentinel `-1`. The retail image happens to read adjacent storage, while instrumented portable builds diagnose a global buffer overflow. | Treats the neutral owner as nonhuman before consulting the player-indexed table. |
| Animated-map redraw boundary | Marking a monster in the leftmost visible map column also marks the nonexistent column to its left, writing before the redraw grid. Instrumented portable builds abort when a monster reaches that boundary. | Clips the missing left neighbor while retaining all in-view redraw marks. |

## Replaced subsystem

### Installed media

The portable startup no longer checks a CD-drive status or requires the obsolete
`Tracks2/02-AudioTrack 02.ogg` marker. The resource manager checks the required
installed AGG archives, music is read from `MUSIC`, and movies use the installed
`HEROES2/ANIM` and `DATA` directories. Missing optional music does not prevent
startup. CD-dependent host/single-player menu restrictions, disc-insertion
messages, unused drive-path globals and registry-forwarding wrappers are
removed. The portable tree contains no floppy-drive implementation.

### Numbered music

Portable playback resolves numbered music through the audio backend for both
legacy music-source settings. The old MIDI-only availability mask does not
describe the installed Ogg tracks and is no longer applied to their playback.

### Network-save compression

Retail uses the recovered legacy Bzip codec through temporary files and exposes
its implementation globally. `port` instead uses libbz2 1.0.x in memory
through the small `compression::Bzip2*` interface. Callers provide explicit
source and destination capacities, compression uses the documented worst-case
bound, and failures do not continue with an indeterminate length.

The resulting compressed stream is a standard bzip2 stream and is not promised
to be wire-compatible with the retail network-save protocol. Network transports
are not currently supported by the portable platform. The exact reconstruction
and generated retail-source branches retain the original codec and format.

### CD-ROM detection

Retail looked for the expansion archive in the installation, then for the CD by
opening `\Tracks2\02-AudioTrack 02.ogg` on each CD drive; without that file it
disabled sound, movies, the campaigns and multiplayer. The Buka disc layout
leaked into `port` as a startup requirement: an installation without a
`Tracks2` folder, such as GOG's or one copied from an English disc, stopped with
"Unable to find the Heroes II data files" even though both archives were there.

`port` treats the installation as the CD. Startup requires only
`DATA\HEROES2X.AGG`. Music is optional and looked up per track in
`MUSIC\NN-AudioTrack NN.ogg`, `MUSIC\TrackNN.ogg` (GOG) and
`TRACKS2\NN-AudioTrack NN.ogg` (the Buka disc). A movie missing from
`HEROES2\ANIM` is logged and skipped instead of ending the game with "Unable to
open animation file".

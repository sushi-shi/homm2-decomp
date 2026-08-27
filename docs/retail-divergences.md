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

## Corrected defects

| Area | Retail behavior | `master` behavior |
| --- | --- | --- |
| Campaign table bounds | The enabled-map table indices are reversed after switching campaign sides, and the 13-point campaign track reads the 12-entry enabled-map table at its final point. | Indexes the table as `[campaign side][scenario]` and checks the map-table bound before reading track state. |
| Aggregate lookup failure | `resourceManager::PointToFile` and `GetFileSize` continue with an invalid aggregate entry after calling the shutdown path. A shutdown implementation that returns or re-enters can dereference that invalid state. | Returns immediately after reporting the fatal lookup error. |
| Manager destruction | Several dialogs are allocated as a concrete manager and deleted through `baseManager*`, whose retail vtable has no virtual destructor. | Gives the portable manager hierarchy a virtual destructor, so deletion through the owning base pointer is defined and reaches the concrete destructor. |
| Allocation ownership | Map buffers allocated by `H2_ALLOC` are released with `delete`, the adventure visibility array uses scalar `delete` after `new[]`, and text-entry construction can abandon an owned text allocation. | Pairs each allocation with `H2_FREE` or `delete[]` as appropriate and releases the temporary text allocation before replacing it. |
| Invalid encoded game domains | Unknown recruit-site and town-faction values can leave a creature type uninitialized before it is indexed or added to an army. | Rejects invalid values at the owning switch instead of continuing with indeterminate state. |
| AI special-direction movement | The shortcut into the movement loop bypasses initialization of the stop/notification arguments passed to `MoveHero`. | Initializes the movement arguments before either normal or shortcut entry. |
| Spell edge paths | Hero-cast spells assume the current side has a hero, targeted spells assume an occupied source hex, and Armageddon's headless path uses target and palette pointers that may never have been initialized. | Rejects a missing hero at the cast boundary, cancels any targeted spell with no living army target, rejects the same impossible target state during AI evaluation, guards the optional visual target, and skips palette restoration when no visual palette was created. |
| Fixed-size paths, names, and formats | Several retail `sprintf` calls treat external or localized text as a format string or assume campaign, dialog, map, and movie paths fit their local buffers. The map/save requester also copies `"*.MP2"` or a similar six-byte pattern into a five-byte default-extension field. | Copies plain text without interpreting it, builds temporary names dynamically, validates the one fixed-size serialized campaign filename, and passes only the extension (`".MP2"`) to the requester. |
| Truncated external records | Retail generally ignores host read/write counts, so a truncated AGG header, map header, or preferences file can leave partially initialized state and an AGG entry count can drive an invalid allocation. | Exact-transfer helpers complete short host operations without spinning; the AGG loader bounds the directory by the file length, map headers reject short records, preferences fall back to defaults, and incomplete resource payloads are logged and zeroed. |
| Direct-connect identifier | A six-byte identifier copied with `strncpy` is not terminated when all six source bytes are nonzero. | Copies the fixed-width field and explicitly terminates its seven-byte destination. |
| Remote duplicate-filter reset | Startup clears only 30 bytes of the 30-element `i32` recent-message-ID array, leaving most entries from a previous session intact. | Clears the complete array with `sizeof(iLastIds)`. This belongs to retained legacy transport code; portable multiplayer is not currently supported. |
| UDP send failure | A failed broadcast send tests `attemptCount` but never increments it, so a persistent socket error retries forever; both send-error exits also leak the packet buffer. | Makes at most 20 send attempts, delaying only between attempts, frees the packet on either error path, then reports the error. This belongs to retained legacy transport code; portable multiplayer is not currently supported. |
| Millisecond and fizzle timing | Signed comparisons against the wrapping 32-bit tick counter can terminate a wait early or extend it across the signed and unsigned wrap boundaries. `FizzleForward` also starts its first frame from tick zero, so that frame normally receives no delay. | Compares the signed modular distance between the deadline and current tick and starts the fizzle cadence from the current tick. |
| Wagon and lean-to sound | Their event-sound cases select the pickup sound and then fall through, overwriting it with the experience sound. | Stops after selecting the pickup sound, as the otherwise-dead assignment and the event category indicate. |
| AI single-creature stack value | Two consecutive strength thresholds both test for more than two creatures, leaving the `-0.4` modifier unreachable and assigning a one-creature stack the zero-creature modifier. | Uses the evident descending threshold of more than one creature, preserving distinct modifiers for stacks of two, one, and zero creatures. |

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

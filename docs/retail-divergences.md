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

## Maintained source structure

The exact Gold/Buka reconstruction retains a few source shapes solely because
they reproduce the Visual C++ 6 executable: redundant labels, explicit Boolean
materialization, and casts at boundaries whose underlying retail value is an
integer. `master` does not preserve those compiler-sensitive spellings when the
same behavior has a clearer portable representation.

In particular, the castle dialog's selection value can hold either a
`BuildingSlotType` value or a UI control identifier. `master` models that mixed
discriminant as an integer and converts to `BuildingSlotType` only when entering
a building API. Redundant jumps with one natural structured successor are also
expressed as fallthrough, loop exit, or ordinary conditional flow. Jumps that
still represent a real shared tail, cleanup path, retry loop, or multi-loop exit
remain explicit.

## Corrected defects

| Area | Retail behavior | `master` behavior |
| --- | --- | --- |
| Campaign branch switch | The enabled-map table indices are reversed after switching campaign sides. Scenario 4 therefore writes outside `m_campaignMapEnabled[2][12]`. | Indexes the table as `[campaign side][scenario]`, matching its declaration and every other campaign table access. |
| Aggregate lookup failure | `resourceManager::PointToFile` and `GetFileSize` continue with an invalid aggregate entry after calling the shutdown path. A shutdown implementation that returns or re-enters can dereference that invalid state. | Returns immediately after reporting the fatal lookup error. |
| Manager destruction | Several dialogs are allocated as a concrete manager and deleted through `baseManager*`, whose retail vtable has no virtual destructor. | Gives the portable manager hierarchy a virtual destructor, so deletion through the owning base pointer is defined and reaches the concrete destructor. |
| Invalid encoded game domains | Unknown recruit-site and town-faction values can leave a creature type uninitialized before it is indexed or added to an army. | Rejects invalid values at the owning switch instead of continuing with indeterminate state. |
| AI special-direction movement | The shortcut into the movement loop bypasses initialization of the stop/notification arguments passed to `MoveHero`. | Initializes the movement arguments before either normal or shortcut entry. |
| Spell edge paths | Teleport assumes an occupied source hex, and Armageddon's headless path uses target and palette pointers that may never have been initialized. | Cancels an invalid teleport target, guards the optional visual target, and skips palette restoration when no visual palette was created. |
| Fixed-size paths and names | Several retail `sprintf` calls assume campaign, dialog, map, and movie paths fit their local buffers. | Builds temporary names dynamically and validates the one fixed-size serialized campaign filename before copying it. |
| Direct-connect identifier | A six-byte identifier copied with `strncpy` is not terminated when all six source bytes are nonzero. | Copies the fixed-width field and explicitly terminates its seven-byte destination. |

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

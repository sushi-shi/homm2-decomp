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

## Corrected defects

| Area | Retail behavior | `master` behavior |
| --- | --- | --- |
| Campaign branch switch | The enabled-map table indices are reversed after switching campaign sides. Scenario 4 therefore writes outside `m_campaignMapEnabled[2][12]`. | Indexes the table as `[campaign side][scenario]`, matching its declaration and every other campaign table access. |
| Aggregate lookup failure | `resourceManager::PointToFile` and `GetFileSize` continue with an invalid aggregate entry after calling the shutdown path. A shutdown implementation that returns or re-enters can dereference that invalid state. | Returns immediately after reporting the fatal lookup error. |
| Fixed-size paths and names | Several retail `sprintf` calls assume campaign, dialog, map, and movie paths fit their local buffers. | Uses capacity-bounded formatting. Truncated input is contained rather than overwriting adjacent storage. |
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

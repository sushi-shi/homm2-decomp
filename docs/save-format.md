# Portable saves and legacy import

The portable game writes and loads `.h2s` files. Scenario, Succession Wars
campaign and Price of Loyalty campaign saves use the same extension and carry
their mode in the payload. The load menu reads human-player slots from the
decoded state, checks the requested player count, and asks before replacing
extra human slots with AI.

Hero, town and player names, rumours and map filenames are owned UTF-8 strings.
The format preserves their bytes without retail code-page conversion or a
thirteen-byte name limit. Rendering still depends on the installed font's
glyph coverage, and existing text-entry widgets retain their UI limits.

## Importing an existing save

The Linux and Windows packages include `homm2-import-save` alongside the game:

```sh
homm2-import-save --encoding windows-1251 old.GX2 imported.h2s
homm2-import-save --encoding windows-1252 old.GM1 imported.h2s
```

The importer accepts the base and Gold/expansion layouts, including `.GMC` and
`.GXC` campaigns. It uses the content's layout tags, rather than the filename,
to select the record layout. Choose the original file's encoding explicitly:
`windows-1251`, `windows-1252` or `utf-8`. Retail files have no reliable charset
tag, so guessing can silently change names. Names already truncated by an older
portable build cannot be recovered from that save.

The tool decodes into an isolated snapshot, validates it, then writes a new
native save. It rejects truncated records, bad section markers and invalid
record references. It refuses an existing destination, uses a temporary output
beside it, and leaves the input unchanged. Copy the result into the game's
writable `GAMES` directory. Browser users can import on a desktop first; the
legacy decoder is not compiled into the browser game.

The importer is linked only into the offline tool and its tests. The running
game never reads or writes retail saves. Retail executables cannot read native
saves, and native files cannot be renamed to a retail extension to convert them.

## Version 1 layout

The envelope contains eight magic bytes (`H2SAVE\r\n`), a 32-bit version, a
32-bit payload length, and a CRC-32 of the payload. All three integers are
little endian. Files over 64 MiB, unsupported versions, incorrect checksums,
truncated data and trailing bytes are rejected before applying state.

The payload has a fixed field order defined by `src/SAVE/Codec.cpp`,
`include/SAVE/GameFields.def` and `include/SAVE/PlayerFields.def`. Each integer,
boolean or enum code occupies four little-endian bytes. Signed values use
two's complement; floats use their IEEE-754 binary32 bits. Strings use a
32-bit byte length followed by validated UTF-8, without a terminator. Embedded
NULs and strings over 1 MiB are rejected. Runtime pointers, GUI state and object
padding are absent. Changes to field order or meaning require a format version
change and an explicit migration.

Map cells and overlay bitfields are encoded as explicit numeric fields. The
save records the original map's text encoding for opaque event blobs; their
bytes retain that provenance. Original map header text crosses a bounded
code-page conversion boundary. This version therefore removes retail limits
from runtime names while retaining the installed map resource format.

Decode builds and validates a separate snapshot before game state changes.
Validation checks map dimensions, live overlay chains, event text bounds and
selected player/hero/town invariants. It is not a general validator for every
possible malformed map resource or gameplay value.

## Network state

Portable protocol version 1 is required in the TCP setup handshake. Combat
state uses explicit hero, town and army records and UTF-8 names. Transfers use
at most 200 state bytes per fragment, with a version tag, total size and byte
offset; inconsistent, oversized and conflicting fragments are rejected.
Network saves carry native `.h2s` content through the existing transfer layer.
Retail and earlier portable peers are incompatible. Live multiplayer testing
is still needed; automated tests cover the codec and fragment assembler.

## Verification

Tests exercise archive bytes and malformed envelopes, full snapshot round
trips, actual engine save/load entry points for all three modes, typed combat
copies, and legacy import. Four synthetic legacy fixtures were written with
the previous 32-bit serializer, including full Cyrillic names in its fixed
fields. Fixture provenance is recorded in `tools/fixtures/legacy-save/README.md`.
Standalone 32-bit and 64-bit builds of the importer produce identical native
bytes for all four fixtures; the full game remains a 32-bit target.

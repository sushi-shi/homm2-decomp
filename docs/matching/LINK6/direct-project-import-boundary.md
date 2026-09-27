# Direct project objects do not preserve the import boundary

Measured on 2026-09-27 in matcher-4, branch `matcher/native-link-recovery`,
from the clean `a016a211b` source checkpoint and its verified 115-byte native
image. The persistent build shell's cwd and `HOMM2_DIR` both resolve to this
worktree. No source, production object, library, or executable was edited.

The three-owner Audiere diagnostic can place its helpers correctly, but its
archive experiment explicitly rooted Purge. This test asks whether ordinary
direct inputs eliminate that extraction requirement while preserving the image.
All seven BASE archives are replaced by their unmodified objects in the order
of their first selected `.text` symbol in the canonical MAP. Assembly symbols
without the MAP `f` flag are included. SOURCE objects, import libraries, resources,
flags, and the four-link historical PDB/clock sequence remain the same. No
`/INCLUDE` option is present.

Two complete native links (four historical passes each) compare the unchanged
115-byte source owner against the preserved three-owner raw objects. Both
executables have 1,208,393 bytes.

| Input ownership | Retail differences | Nonheader differences | SHA-256 |
| --- | ---: | ---: | --- |
| Unsplit control | 240,532 | 240,501 | `62ac7b9343363f0f2bb60670f150e60c51d62116b1f4a26aed4b9b66530ad9cb` |
| Three owners | 240,759 | 240,496 | `9f4d74df145050c073bd4ccabfdd2bb69a5cc945cebcc92d01d5902488a921b0` |

These are diagnostic images, not regressions in the production executable.
The canonical output retains SHA-256
`29064b1aa592911b31345d3f71535f802876f9e974cf35a360530da958dffde2`.

## First displaced contribution

The first BASE constructor moves from `0x4b5660` to `0x4b5390`, exactly 720
bytes earlier. The canonical MAP has 120 six-byte import thunks in that interval;
the direct-object MAP puts BASE immediately after SOURCE instead. Passing an
archive argument before a direct object does not force its members to be loaded
before that object. Sorting only project objects therefore does not preserve the
existing import boundary. This is not merely a change in the Rich header.

The pair of direct-input images differs in 378 bytes: 263 header bytes and 115
nonheader bytes. Relative to the now displaced Purge, the three-owner image has
stream destructor `+0x830`, assignment `+0x860`, guarded initializer `+0x8c0`,
registration `+0x8f0`, and node destructor `+0x910`. These are the required local
relative positions, but their absolute placement and the rest of the image are
wrong. Local order is not whole-image closure.

## Unmodified import-member control

A bounded follow-up copies the exact 120 corresponding archive member payloads
into disposable files, preserving every byte, and places them directly before
BASE. The provenance records each source library, member index, public symbol,
and SHA-256. No import stub is synthesized or modified.

LINK rejects the first control with `LNK1213: unexpected import object
encountered`: the selected set includes short import objects, which this direct
input path does not accept. No successful image or three-owner link is claimed
for this follow-up. Archive extraction and an ordinary direct-object interface
are not interchangeable for these inputs. The experiment does not test a
separately generated long-form import library or every possible build ordering.

## Current ownership census

A fresh read of all 96 current C++ objects finds exactly one without a CRT
contribution: `BASE/DIMMER` (six function symbols). The accepted Misc merge now
places its initializer in `BASE/Misc`; the older ctype-owner audit's pairless
Misc observation describes its earlier source state. This update identifies no
semantically justified owner merger to offset additional Audiere producers.

Artifacts in matcher-4:
`build/link/audiere-direct-ownership/{probe.py,probe.log,results.json,input-order.json,provenance.json,paired-audit.json}`,
two full sets of RSP/MAP/images and linker logs, and
`{import-prefix.py,import-prefix.log,import-provenance.json}` with the rejected
control log. The current owner census is
`build/native-owner-census/{census.py,results.json}`.

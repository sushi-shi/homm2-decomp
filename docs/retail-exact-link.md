# Native final linking and retail comparison

The target remains an executable exactly matching retail, including the six
private `.bss` empty-string cells in REQUEST. A successful native link is not
matching closure. COFF/PE correction machinery and the former `--transform`
mode have been removed; remaining differences require source/build recovery.

## Supported commands

Run inside `nix develop .#build`:

```sh
homm2 build
homm2 link
homm2 link --rsrc
homm2 link --historical
ninja link-audit
```

- `homm2 link` / `ninja link`: raw compiled objects, normal project archives,
  stock runtime libraries, and import libraries built from checked-in ABI
  manifests. No resources or access to the retail executable are required.
  Output: `build/link/generic/HMM2PL.exe`.
- `homm2 link --rsrc` / `ninja link-rsrc`: the same inputs plus resources
  compiled from `res/HMM2PL.rc`. The program icon is extracted from the retail
  control image while the era resource compiler runs; every compiled resource
  payload is compared against retail. Output: `build/link/rsrc/HMM2PL.exe`.
- `homm2 link --historical` / `ninja link-historical`: resource-bearing native
  link with the observed PDB path, creation time, and four-link age history.
  Output: `build/link/historical/HMM2PL.exe`.
- `ninja link-diff` (part of the default graph, so `homm2 build` runs it):
  links the historical image and fails if any region (headers, a section, the
  overlay, the file size) differs from retail in more bytes than
  `config/link_diff_ceiling.tsv` allows. Bank a lower count with
  `python3 -m homm2.build.link_diff --update`; never raise it to admit a
  regression.
- `ninja link-audit`: read-only comparison of the historical native image
  against retail. It writes `build/link/historical/HMM2PL.link.json` and reports
  unresolved differences. It fails while the executable differs from retail;
  the audit never changes inputs or output bytes.

Each output directory also retains the exact response file, unchanged native
MAP, PDB, and linker log. `--transform` is rejected before building anything.
Historical `build/link/HMM2PL.exe` artifacts are no longer supported outputs;
rebuild a supported mode before interpreting a local executable.

## Current residual

The historical image differs from retail in 500 bytes: 496 in `.text`, 3 in
`.rdata` and 1 in `.data`; headers, section geometry, imports and resources
are exact. All of it is two placement walls, attributed in
`docs/linked-function-placement-walls.md` and
`docs/linked-data-section-walls.md`: DIMMER's scalar deleting destructor is
emitted after its first constructor instead of after `Draw`, and the
AudiereEffects node destructor precedes its ctype initializer pair instead of
following it. The ceiling file records the per-region counts.

LINK's import slot order depends on the C runtime that Wine loads for it
(`docs/patterns/import-slot-order-follows-linker-runtime.md`). The ordinary
build keeps Wine's builtin `msvcrt`, which reproduces retail; every Wine child
runs with `TZ=UTC0` (`homm2.core.wine`).

## Source and build evidence

`native_link.py` and the Ninja link graph consume raw `build/objdiff/base`
objects. Comparison-normalized objects never enter the link. The six project
archives (`BASE-prefix.lib`, `Misc.lib`, `MiscRuntime.lib`, `BASE-middle.lib`,
`Midi.lib`, `BASE-suffix.lib`) contain untouched
compiler/assembler outputs, in the reconstructed native archive order.

The response scans SP5 `MSVCPRT.LIB` after `BASE-suffix.lib`; its stock
`delop_s.obj` supplies the early operator-delete body. The subsequent stock
`LIBCMT.LIB` scan resolves the other runtime members. Both archives come from
the pinned toolchain. `BITS.asm` and `TILE.asm` supply ordinary MASM OMF objects;
LINK performs its native OMF conversion. The comparison graph uses MASM COFF
outputs for object-level byte and relocation review.

Import libraries reconstruct the required ABI from `imports/*.def`, including
Smacker ordinal imports and WinG aliases. Audiere uses VC6 to compile the full
15-export ABI surface and retains its native LINK-generated import library;
this also recovers the retail import-descriptor producer records. These are normal linker inputs;
they are not patched copies of linked game code or data. Source-owned resources
and native PDB/debug metadata also remain ordinary build inputs. Generic and resource
links run once with the current process clock and their normal output PDB path.
The historical mode creates the PDB at `2003-02-26 14:51:33`, then links three
more times at `2003-04-04 08:19:23`, using the retail-recorded
`e:\Users\igorl\VSS\HMM\HMM2\temp\release\game\HMM2PL.pdb` path.
LINK itself emits the NB10 signature, age 4, and timestamps; no bytes are patched.

## Verification boundaries

`homm2 build` measures object function/data bytes and ordered relocation
semantics. An exact object report does not establish linked symbol placement,
private storage order, section geometry, or a whole-file match. The native
image audit compares those properties separately and records the candidate
path explicitly. Use current output/report bytes rather than historical totals.

For a strict read-only final comparison:

```sh
python3 -m homm2.build.link_exe --audit-existing --strict \
  --out build/link/historical/HMM2PL.exe
sha256sum build/orig/HMM2PL.exe build/link/historical/HMM2PL.exe
```

The strict audit is expected to fail while native image residuals remain. Only
verified equality of the requested executable can establish completion.

## Historical evidence

Earlier exact-image results relied on disposable corrections to REQUEST's six
empty-string cells, Misc literal storage, and AudiereEffects/DIMMER generated
COMDAT order, together with historical import/PDB inputs. Those corrected
results are not proof that the current source/build reproduces retail.

The removed implementation includes BSS/CRT/Misc/COMDAT adapters, synthetic
COMMON-order objects, post-link import/text/PE normalizers, their orchestration,
the legacy import-library patchers, and the production transform path. Shared
read-only import ABI identities remain available to comparison audits. Matching
dossiers retain the measured
experiments as historical evidence. Conclusions about untested source ownership
must remain open; a failed matrix does not prove all native explanations
impossible.

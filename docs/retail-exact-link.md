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
- `ninja link-audit`: read-only comparison of the resource-bearing native image
  against retail. It writes `build/link/rsrc/HMM2PL.link.json` and reports
  unresolved differences. It fails while the executable differs from retail;
  the audit never changes inputs or output bytes.

Each output directory also retains the exact response file, unchanged native
MAP, PDB, and linker log. `--transform` is rejected before building anything.
Historical `build/link/HMM2PL.exe` artifacts are no longer supported outputs;
rebuild a supported mode before interpreting a local executable.

## Source and build evidence

`native_link.py` and the Ninja link graph consume raw `build/objdiff/base`
objects. Comparison-normalized objects never enter the link. The three project
archives (`BASE-prefix.lib`, `Midi.lib`, `BASE-suffix.lib`) contain untouched
compiler/assembler outputs, in the reconstructed native archive order.

The response scans SP5 `MSVCPRT.LIB` after `BASE-suffix.lib`; its stock
`delop_s.obj` supplies the early operator-delete body. The subsequent stock
`LIBCMT.LIB` scan resolves the other runtime members. Both archives come from
the pinned toolchain. `BITS.asm` and `TILE.asm` supply ordinary MASM OMF objects;
LINK performs its native OMF conversion. The comparison graph uses MASM COFF
outputs for object-level byte and relocation review.

Import libraries reconstruct the required ABI from `imports/*.def`, including
Smacker ordinal imports and WinG aliases. These are normal linker inputs;
they are not patched copies of linked game code or data. Source-owned resources
and native PDB/debug metadata also remain ordinary build inputs. The supported
link runs once, using the current process clock and its normal output PDB path.

## Verification boundaries

`homm2 build` measures object function/data bytes and ordered relocation
semantics. An exact object report does not establish linked symbol placement,
private storage order, section geometry, or a whole-file match. The native
image audit compares those properties separately and records the candidate
path explicitly. Use current output/report bytes rather than historical totals.

For a strict read-only final comparison:

```sh
python3 -m homm2.build.link_exe --audit-existing --strict \
  --out build/link/rsrc/HMM2PL.exe
sha256sum build/orig/HMM2PL.exe build/link/rsrc/HMM2PL.exe
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

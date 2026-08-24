# Retail-exact final link

`homm2 link` drives the pinned VC6 SP5 `LINK.EXE` in one of three modes. In
every mode the file written by LINK is the deliverable: no tool opens it
afterward for mutation.

- `homm2 link` — **generic**: raw compiled objects only. No retail-extracted
  resources, no COFF transforms; one LINK pass with an ordinary PDB. Output
  `build/link/generic/HMM2PL.exe`. This is what the reconstruction honestly
  produces from source alone.
- `homm2 link --rsrc` — generic plus the resources, compiled from source:
  `res/HMM2PL.rc` (menus, dialog, VERSIONINFO as reviewed rc grammar) plus
  `res/heroes.ico` (the one retail-extracted binary, the program icon) through
  the era `RC.EXE` 5.00, gated byte-exactly against `build/orig/HMM2PL.exe` on
  every build (`rc_res.py`). Output `build/link/rsrc/HMM2PL.exe`. See
  `docs/resource-source-reconstruction-plan.md`.
- `homm2 link --transform` — `--rsrc` plus the four reviewed COFF transforms
  declared in `scripts/homm2/build/exact_link/transforms.py`, the historical
  four-pass PDB link, and the strict audit. Output `build/link/HMM2PL.exe`,
  byte-identical to `build/orig/HMM2PL.exe`; the audit fails if any byte
  differs.

The ninja archives (`BASE-prefix.lib`, `Midi.lib`, `BASE-suffix.lib`) hold raw
compiled objects in every mode; `--transform` rebuilds its own prefix/suffix
variants under `build/link/plain-inputs/` with the transformed members swapped
in, so the transforms live in exactly one module and are never baked into
shared build artifacts.

## Compared evidence

Object matching and final-image matching answer different questions:

- `homm2 build` compares 98 reconstructed translation units. The current
  closure is 1,727/1,727 exact functions, complete ordered relocations, and
  291,995/291,995 exact data bytes.
- `homm2 link` compares PE section geometry and payloads, the complete file,
  imports, resources, initialized storage, and reviewed function placement.
- Objdiff uses `functionRelocDiffs=all`. The local Gruntz-derived patch also
  compares symbol-relative COFF relocation addends hidden by ordinary masked
  operand comparison.

The VC6 MAP is retained unchanged. Its project-function summary reports three
displaced duplicate Audiere template COMDAT identities even though the final PE
is exact; the linker selected byte-identical copies under different MAP names.
The per-object comparisons prove every reviewed identity independently, while
the final-image audit proves the selected physical bytes and references.

## What LINK receives

The complete response is retained at `build/link/HMM2PL.rsp`. It contains the
reconstructed project objects, three project archives, generated import
libraries, stock VC6 libraries, and the reconstructed resource object. There is
no `/ORDER` file, synthetic executable padding, or post-link PE normalizer.

Most link inputs are untouched compiler or assembler outputs. Three provenance
residuals remain explicit rather than being mistaken for recovered source:

- `BASE/Misc` uses a disposable COFF copy that separates the Buka track-name
  literal into its retail contribution position.
- `BASE/AudiereEffects` and `BASE/DIMMER` use disposable copies with one
  compiler-generated destructor COMDAT moved to the retail section order.
These are object-input reconstruction debts, not executable
transformations. Removing them through credible source or archive ownership is
the remaining provenance campaign.

`SOURCE/REQUEST` uses the developer-shaped, cross-version-supported
`char* cFRDummy = "";`. VC6 emits that file-scope initializer's backing byte
before five function-body empty strings, while retail places it after them.
Transform mode rotates those six private cells in a disposable REQUEST object;
generic and resource-only modes use the raw compiler output. The rejected
alternative was an invented cross-TU `selectany` owner in SEARCH. See
`docs/matching/fileRequester-cFRDummy/ownership-topology.cpp` for the measured
compiler/linker wall and source disposition.

The early operator-delete owner is no longer reconstructed. The response scans
the untouched SP5 `MSVCPRT.LIB` after `BASE-suffix.lib`; its stock
`delop_s.obj` supplies the retail 11-byte body, `_free` relocation, and
`0x000b1f6f` compiler identity. The later untouched `LIBCMT.LIB` scan resolves
the other 177 selected runtime members. Both archives come directly from the
same pinned SP5 cabinet chain used to assemble the compiler and linker.

Gruntz's independently reversed LINK contribution model agrees with the local
probes: LINK appends whole object contributions and can only relocate a duplicate
COMDAT to the first defining object. A VC6 `/ORDER` experiment using all 2,641
public MAP symbols cannot preserve the retail interleaving because ordinary
NoDuplicates functions are not orderable and object-local `$E` helpers are not
public names. Thus `/ORDER` is not a hidden replacement for the Audiere/DIMMER
input preparation.

`BITS.asm` and `TILE.asm` are ordinary OMF inputs emitted by Microsoft MASM
6.11. LINK itself prints its normal OMF-to-COFF conversion warning. The retail
Rich header independently contains exactly two MASM producer records; compiling
the same bodies as C++ removes those records and changes linked layout.

The import libraries follow the Gruntz model: they reconstruct the ABI exposed
by the missing DLL import libraries. Smacker's ordinal-only exports and WinG's
aliases use reviewed `.def`-syntax manifests, but this is not a claim that the
game developers authored those `.def` files. The generated libraries are
checked against all 240 retail imports before linking.

## Historical VC6 metadata

VC6 derives the NB10 signature from PDB creation time and increments its age on
subsequent links. The retail executable records signature `0x3e5cd475`, age 4,
the path
`e:\Users\igorl\VSS\HMM\HMM2\temp\release\game\HMM2PL.pdb`, and final link
timestamp `0x3e8d400b`.

The build reproduces that ordinary history rather than writing those bytes:

1. create the PDB during a link at `2003-02-26 14:51:33`;
2. run LINK three more times at `2003-04-04 08:19:23`;
3. retain the fourth untouched LINK output, whose PDB age is 4.

`libfaketime` only controls the process clock. The four invocations all use the
same response file and untouched `LINK.EXE`.

## Verification

Run inside `nix develop .#build`:

```sh
homm2 build
homm2 link
sha256sum build/orig/HMM2PL.exe build/link/HMM2PL.exe
```

The strict report at `build/link/HMM2PL.link.json` records four of four PE
sections byte-exact, whole-file `100.000000%`, exact raw import order, exact
resources, and exact NB10 metadata. Both SHA-256 values are:

```text
bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
```

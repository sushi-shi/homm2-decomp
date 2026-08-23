# Retail-exact final link

`homm2 link` reconstructs `build/link/HMM2PL.exe` with the pinned VC6 SP5
`LINK.EXE`. The file written by LINK is the audited deliverable: no tool opens
it afterward for mutation. The strict audit fails if any byte differs from
`build/orig/HMM2PL.exe`.

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

Most link inputs are untouched compiler or assembler outputs. Five provenance
residuals remain explicit rather than being mistaken for recovered source:

- `SOURCE/REQUEST` uses a disposable COFF copy that rotates six distinct
  one-byte empty-string cells into their retail physical order.
- `BASE/Misc` uses a disposable COFF copy that separates the Buka track-name
  literal into its retail contribution position.
- `BASE/AudiereEffects` and `BASE/DIMMER` use disposable copies with one
  compiler-generated destructor COMDAT moved to the retail section order.
- An ordinary one-member archive, made by VC6 `LIB.EXE`, supplies the untouched
  stock `LIBCMT.LIB` `delete.obj` during the first runtime scan. A later scan of
  the unmodified stock archive resolves the other 177 selected runtime members.
  This reproduces the retail Rich producer order, but the original library that
  owned the first `delete.obj` pull is not yet known.

These are object/library-input reconstruction debts, not executable
transformations. Removing them through credible source or archive ownership is
the remaining provenance campaign.

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

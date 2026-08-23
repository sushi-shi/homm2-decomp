# Retail-exact final link

`homm2 link` reconstructs `build/link/HMM2PL.exe` from the current candidate
objects, retail-derived import records, the pinned VC6 SP5 libraries, and the
retail resource tree. The strict audit fails if any byte differs from
`build/orig/HMM2PL.exe`.

## Compared evidence

Object matching and final-image matching answer different questions:

- `homm2 build` compares the 98 reconstructed translation units. Its closure
  requires 1,727/1,727 exact functions, complete ordered relocations, and
  291,978/291,978 exact data bytes.
- `homm2 link` compares PE section geometry and payloads, the complete file,
  imports, resources, initialized storage, and reviewed function placement.
- Objdiff is configured with `functionRelocDiffs=all`. The local Gruntz-derived
  patch also compares symbol-relative COFF relocation addends that would
  otherwise be hidden inside masked operands.

The final-link MAP is emitted by VC6 before the deterministic semantic
normalizers run. It is retained as provenance for the raw link, not rewritten
to claim names the linker did not emit. Consequently, its project-function
summary reports three displaced duplicate Audiere template COMDAT identities.
Those template instantiations have indistinguishable linked bodies; the current
objects prove each reviewed function independently and the final PE proves the
physical bytes and references at every address.

## Durable link inputs

The final link preserves the retail producer and contribution forms instead of
treating all semantically equivalent COFF as interchangeable:

- `config/retail_crt_order.txt` fixes the reviewed order of the 177 selected
  `LIBCMT.LIB` members. One-member archives make that order explicit to LINK.
- The source and CRT `.bss`, `.data`, `.rdata`, COMMON, OMF-converted, and
  selected COMDAT contributions are adapted in disposable final-link copies.
  Candidate object payloads and relocations remain the source of linked data.
- Audiere uses the VC6 short-import form. Gruntz-style disposable filler exports
  place the two retail-used names at their retail hint indices, so no complete
  Audiere export table is a build input. Miles embeds the 29 retail-used names
  and hints directly in the measured regular-COFF form, likewise without a full
  export table. Smacker's ordinal-only imports and WinG's caller/export aliases
  still use reviewed `.def`-syntax manifests; these reconstruct the required
  import-library interface and are not evidence that the developers authored a
  `.def` file. Generated archives are checked back against the retail import table.
- The untouched LINK output is audited with the Gruntz model: DLL descriptors,
  ILT/IAT slot pairs, padded hint/name records, and DLL strings are paired by
  `(DLL, name-or-ordinal)`. At the current checkpoint all 240 imports and all
  6,374 attributable logical bytes are exact. Nine DLLs retain a different raw
  intra-DLL slot order, reported separately as resolution-history evidence.
- The same untouched-image audit finds all 244 six-byte import thunks (240
  identities, including four duplicate occurrences) semantically exact. Their
  raw positions and order remain a separate layout result.
- `normalize_imports` is only part of the derived byte-identical proof artifact:
  it places those already-proved semantic records at retail offsets and retargets
  references. It is not an authentic LINK input and is not used to claim that the
  raw linker reproduced import order.
- `normalize_text` places candidate-authored import thunks, selected CRT tail
  contributions, exception funclets, `matherr`, and `initcoll` by reviewed
  identity. It relocates candidate bytes; it does not copy the retail text.
- `normalize_pe` makes the reviewed deterministic VC6 link timestamp, Rich
  producer order, and NB10 record explicit. These values are metadata, not
  runtime code or data.

## Verification

Run inside `nix develop .#build`:

```sh
homm2 build
homm2 link
sha256sum build/orig/HMM2PL.exe build/link/HMM2PL.exe
```

The strict link report is `build/link/HMM2PL.link.json`. At the current source
checkpoint it reports four of four PE sections byte-exact, whole-file
`100.000000%`, exact import ABI/order, and exact resources. Both SHA-256 values
are:

```text
bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
```

`build/link/HMM2PL.raw.exe` is the actual LINK.EXE output. Its report keeps raw
section/SHA percentages distinct from `semantic_import_bytes`; a semantic 100%
never changes or hides the raw IAT-order diagnostic.

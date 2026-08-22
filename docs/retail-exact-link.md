# Retail-exact final link

`homm2 link` reconstructs `build/link/HMM2PL.exe` from the current candidate
objects, reviewed import definitions, the pinned VC6 SP5 libraries, and the
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
- Audiere uses the VC6 short-import form. Miles, Smacker, and NetBIOS use the
  measured regular-COFF form. WinG uses its older reviewed form. The `.def`
  files remain the semantic export ledgers; generated archives verify used
  names, ordinals, and hints against the retail import table.
- `normalize_imports` places candidate DLL names, hint/name records, ILT slots,
  and IAT slots by semantic import identity, then retargets candidate references.
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

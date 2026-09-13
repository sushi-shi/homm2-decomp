# Buka build-time localization

Game source uses the same `localization::Tr("resource.gold.insufficient")`
notation and semantic IDs as master. `locales/messages.def` is the authoritative
English registry (`HOMM2_MESSAGE(id, text)`); `locales/ru.po` contains readable
UTF-8 Russian translations and validated English snapshots.

Unlike the portable master branch, this matching branch has **no runtime lookup**,
gettext dependency, UTF-8 runtime conversion, or added initialization. The build
replaces each literal-ID expression with a generated literal macro before Clang
and VC6 see it. The macro expands to the exact Windows-1251 bytes. Generated
escapes belong only under `build/`; authored text never contains them.

## Locale builds and generated branches

`homm2 build` / `homm2 build --ru` retain the Russian matching workflow.
`homm2 build --no-match --ru` and `homm2 build --no-match --en` compile and link
ordinary VC6 executables under `build/ordinary/<locale>/`. They use raw objects,
source-backed import definitions and one LINK pass: no retail executable,
delinking, COFF transforms, matching report or MAX updates. English requires
`--no-match`; it must never populate the Russian matching-object directory.
The [decomp README](../README.md#build-without-matching) covers initial setup.

The generated source branch retains the authored IDs, English registry and
Russian PO. Its standalone `./build.py --ru` / `./build.py --en` selects separate
modern-compiler builds; `nix build` defaults to Russian and `nix build .#game-en`
selects English. Only disposable compiler inputs expand IDs into byte literals.
The catalog parser is shared with decomp builds, so validation cannot silently
drift between these workflows. Catalog changes are Ninja dependencies.

Classic is a terminal reading view: `--classic-russian` resolves IDs against
the catalog from `--classic-from`, then materializes high-byte octal/hexadecimal
escapes as Russian UTF-8. It does not inherit locale catalogs or build scripts.
The generated source README documents building; classic's README identifies it
as unsupported for building.

Locale selection covers source catalog text only. Ordinary/generated builds omit
Windows resources and the retail icon, and neither translates external assets.

Existing table IDs remain `table.<symbol>.<index>`. Buka's original grammatical
fragments, fixed save names, punctuation, spacing and even spelling mistakes are
preserved. Where master's whole-message signature is different, an explicit
fragment or `.buka` ID represents the original calling convention. Do not silently
substitute a master translation: matching also covers string bytes and storage.
Packed text storage keeps its explicit `"\0"` separators outside the IDs.

English should reuse the original 2.0 wording wherever the corresponding message
exists, adapting only to Buka's argument order or sentence composition where
necessary. The [English provenance review](localization-english-provenance.md)
records all 91 newly introduced IDs and the correction of 55 English entries;
their Russian translations and generated compiler literals are unchanged.

## Validation

`homm2 build` always runs the localization source/catalog gate, independently of
the older branch's disabled general audit gates. `homm2 selftest` includes the
whole-tree no-hidden-text test and real-Clang formatting/array-layout tests.

```sh
python3 -m homm2.build.localization
python3 -m unittest homm2.build.test_localization -v
homm2 build
```

The source check scans string **and character** literals in all game source and
headers. It rejects hexadecimal, octal and Unicode escapes, as well as inline
non-ASCII text. Use ordinary text in the catalog, ordinary ASCII in nonlocalized
literals, named constants for byte codes, and `\n`, `\t`, etc. for controls.
`\0` remains valid for termination and packed storage. Escaped backslashes in
Windows paths and comments are not mistaken for encoded text. Catalogs likewise
reject hidden numeric escapes, missing/duplicate/fuzzy/stale entries, incompatible
placeholder signatures, and characters unrepresentable in Windows-1251.

Every localized TU is checked by Clang before VC6 compilation, with `-Wformat=2`
and `-Werror=format`. Clang sees literal formats, so the real CRT declarations
catch missing arguments, extra arguments and wrong types such as an integer for
`%s`. No custom typed-format API is introduced. Runtime/nonliteral format calls
remain outside this guarantee; their diagnostics are warnings. Nonlocalized TUs
are not newly format-gated (PHILAI has existing `% 18s` debug-format warnings).
The legacy SDK's known parsing debt is handled with the source-inventory policy:
game-source errors, fatal errors and format errors fail; recoverable SDK errors
are not represented as a passing whole-project strict C++ build.
The selected locale is passed to both compilers, including this format check.

Clang's generated virtual-file view preserves authored byte offsets, line numbers
and paths for source annotations and AST tooling. VC6 receives generated sources
with the original basename and literal macros, using its existing TU flags. Catalog
files are Ninja dependencies, and retained source hashes include expanded Russian
text, so changing a translation cannot retain stale matching evidence.

This is build-time syntax: compile through the wrapper, not by passing an authored
TU directly to VC6. The generated files are disposable and must not be committed.

## Migration verification

Baseline: `750ce56aa` (the verified common-helper branch). The migration covers
2,666 localized sites and 2,495 used IDs. Remaining escaped ASCII literals were
decoded too; the keyboard Escape byte uses a named constant.

All 98 rebuilt objects pass `homm2.audit.object_equivalence` against the baseline:
every non-debug section byte/layout, ordered relocation identity/site/type/addend,
and named symbol is unchanged. This is a before/after equivalence proof, not a
claim that every object matches retail exactly. General strict-Clang/private
annotation checks still encounter the pre-existing VC6 STL errors documented in
`docs/readability/implementation.md`.

The object comparison covers 42,433 relocations. The separate field audit passes
38,307 reviewed sites across 1,727 functions. The normal report remains at 100%
for code, functions and data. All 1,491 retained scores are unchanged; 154 hashes
were re-keyed only after the all-object proof. The tool suite runs 929 tests with
six pre-existing skips, including 24 localization tests.

Local evidence is under `build/localization-*.log` and
`build/localization-object-equivalence.json`; reproduce with a separately compiled
baseline and the pinned VC6 toolchain:

```sh
python3 -m homm2.audit.object_equivalence /path/to/baseline-objects build/objdiff/base
```

The locale-build follow-up runs 949 tool tests (six existing skips), builds both
VC6 locale executables and both generated-source Nix packages, and verifies the
direct generated-source shell command. Repeating both ordinary builds leaves the
matching graph, all objdiff state, retained scores and README unchanged. Russian
matching still passes the same 98-object equivalence proof. Full classic export
is checked for resolved IDs and readable Russian UTF-8. Follow-up evidence lives
under `build/locale-*` and `build/ordinary-{ru,en}.log`.

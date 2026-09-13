# Parameter naming and shipping-symbol evidence

This is the HoMM2 adaptation of [King's Field PR #7](https://github.com/sushi-shi/kings-field-decomp/pull/7).
The useful parts are a cross-function type/name index, declaration reconciliation,
explicit review dispositions, and a check against the actual symbol evidence.
It belongs on `decomp-gold-2.1-buka`; source/classic branches are generated views.

## What the executable actually names

`homm2 audit symbols EXE --output build/symbols.json` reads the shipping PE and
its NB09 stream directly. Every public retains its raw decorated name, segment,
RVA, type index and record file offset. The report includes a SHA-256, all
subsections and record counts, and PE debug-directory payload descriptions.
It never opens a generated PDB or estimates function sizes from adjacent publics.

The measured inputs were:

| Input | SHA-256 | Evidence |
| --- | --- | --- |
| PoL `img-pol/HEROES2W.EXE` | `bc8f362dd49216c9fbcee1eb2e0429b467082a93330dd84ff95757c0182fc8a3` | NB09 embedded publics |
| Buka `HMM2PL.exe` | `bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a` | NB10 external PDB reference |

PoL has **3,541 `S_PUB32` records**, every type index zero, **498 modules**,
**177 `S_COMPILE`**, **177 `S_OBJNAME`**, **182 `S_THUNK32`**, **182 `S_END`**
and **36 `S_ALIGN`** records. Its global type table has zero entries. There
are no procedure, parameter/local, member-layout or source-line records in
this stream. The public mangling preserves function/type spellings, calling
conventions and portions of interfaces; it does not preserve argument names.
The earlier branch's `docs/codeview-contents.md` has the detailed ownership
interpretation. Public presence in PoL is evidence about PoL, not proof that
an inherited name or signature describes the Buka implementation unchanged.

Buka has no embedded NB09 publics, export directory or base relocations. It
**does have a debug directory** at RVA `0xea3f0`, containing one 73-byte NB10
record at file offset `0x127000`. That record refers to
`e:\Users\igorl\VSS\HMM\HMM2\temp\release\game\HMM2PL.pdb` (age 4).
The path is build provenance, not an available PDB or a symbol table. Our
`build/pdb/HMM2PL.pdb` is generated reconstruction metadata and must not be
substituted for the referenced file.

Consequently this review preserves every definition and copies existing
argument identifiers into declarations only. No claim is made to have recovered the
original argument names. The symbol census supports PE32 plus the shipping
NB09 footer format and NB10 references; unsupported/chained NB09 directories
fail instead of reporting a partial census as complete.

## Inventory and check

Inside `nix develop .#build`, after provisioning the toolchain:

```sh
homm2 clangd
homm2 audit parameters -j 4 --format json --output build/parameters.json
homm2 audit parameters -j 4 --by-type --output build/parameters-by-type.tsv
homm2 audit parameters -j 4 --check --output build/parameters.tsv
homm2 audit parameters --tu SOURCE/PHILAI --all
homm2 audit symbols /path/to/PoL/HEROES2W.EXE --output build/pol-symbols.json
homm2 audit symbols build/orig/HMM2PL.exe --output build/buka-symbols.json
```

The strict Clang view preserves the enum identities that `H2_ENUM_PARAM` erases
for the VC6 ABI. `H2EnumStorage` and `H2SteppedEnumStorage` expose their enum
domain through pointers/references. Arrays, callback returns/arguments,
methods, overloads, constructors, dependent templates and macro-generated
operators are inventoried. USRs separate overloads; internal functions retain
a TU scope. Repeated header observations are deduplicated, with their observing
TUs retained. The type index groups names across different functions; it does
not demand that attacker/defender, source/destination, old/new or left/right
parameters have the same spelling.

JSON records exact parameter locations, AST extents, literal excerpts,
callable identities, observation TUs, declaration groups, type groups and
coverage. Macro extents can be empty or omit trailing qualifiers: an AST
extent alone is not a safe replacement range. Snapshot IDs and offsets are
not stable review keys. Review rows use file, callable USR, argument index and
final name. Repeated identical declarations share that naming disposition.

`--check` checks declaration/definition naming for **all parameter types**,
including scalar, character-pointer and enum/record arguments. It rejects
conflicting named declarations and missing declaration names when a definition
provides a name. Unused unnamed definition arguments and callback signatures do
not acquire invented names. Separate inline definitions can retain their own
local argument names; the checker does not impose a rename on function bodies.

The enum/record review in `docs/parameter-naming-review.tsv` additionally rejects
duplicate/stale/missing decisions and empty reasons. Filtered scans cannot
certify the complete review. Parsing errors outside system headers and unowned
parameter sites fail every scan. VC6's known system-header dialect errors remain
in `sdk_errors`; like the existing source-symbol audit, this does not certify
SDK bodies.

## Declaration completion

This change makes **1,785 declaration arguments in 90 headers** agree with their
unique existing definitions, across every parameter type. The full change
inventory is `docs/parameter-declaration-completion.tsv`; each row carries its
callable identity, parameter index, previous/final name and definition file.

For example, `border` now declares `x`, `y`, `w`, `h`, `e`, `kind`, `fillColor`
and `name`, exactly as its existing constructor definition does. `DoRipple`
now declares `source`, `destination`, `height` and `strength`. The pass includes
existing unnamed arguments and pre-existing spelling disagreements.

**No definition, function body, local variable or macro parameter is renamed.**
Names such as `msg`, `src`, `dest` and single-letter arguments are intentionally
retained where those are the definition's current names. Semantic renaming and
cross-function vocabulary standardization belong in a separate follow-up PR.

The PR's PSX resource-record census is not transplanted: its authored-record
versus loader-count question is format-specific. HoMM2 already has
`homm2 audit data-claims extents` and `homm2 data-topology census` for retail
allocation limits and candidate COFF topology. The missing evidence tool here
was the shipping-symbol census above.

## Validation

The complete strict scan covers **96 C++ TUs, 5,870 parameter sites, 2,227
enum/record sites and 145 type groups**. All project headers and C++ sources
are reached; there are zero project parse errors or unowned parameter sites.
The 1,520 known VC6 system-header diagnostic occurrences remain visible in
JSON. The complete declaration check covers all 5,870 sites.

Validation artifacts are retained locally under `build/parameter-*`:

- `git diff e744880d8 -- src` is empty: definitions match the PR base exactly.
- Fresh VC6 parent and final `homm2 build`: 98 units; unchanged 1,727/1,727
  current exact game functions and 291,995/291,995 compared data bytes.
- Strict comparison of all 98 raw objects against the parent: non-debug
  section layout/bytes, symbol identities and ordered relocations agree.
  COFF timestamps and debug bookkeeping are outside this comparison.
- `homm2 selftest`: 970 tests, six skipped, including scalar declaration
  completion/conflict regressions and unused/inline-definition boundaries.
- Complete `homm2 audit parameters --check` and `git diff --check` pass.

The 747 funclet/import/CRT carve-outs remain separately identified; these
comparisons do not assert untransformed full-executable byte equality.

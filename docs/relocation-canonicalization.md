# Candidate-proved relocation target experiment

Objdiff's normal report uses `functionRelocDiffs=none`. That is useful for code-shape
matching, but it ignores a COFF relocation's symbol and implicit addend. The stricter
`data_value` mode exposes two different classes together:

1. real wrong resolved addresses; and
2. harmless delinker spellings such as retail `const_<RVA>+0` versus candidate public
   owner plus a field offset.

`vostok-delinker` emits `const_<RVA>` when the synthesized PDB has no symbol at that
exact address. This is expected behavior, not a `const_` parsing bug. An `S_LDATA32`
record can name an exact address, but it cannot also express that the address is an
interior alias of a public symbol with a nonzero COFF addend.

The primary experiment therefore uses a second synthesized PDB plus a pinned delinker
patch. Candidate objects prove an exact target RVA, public owner, and addend. The PDB
encodes that tuple in an exact-address metadata symbol. The delinker validates the
tuple against the public owner in the same PDB, emits the public owner relocation plus
the proven implicit addend, and never writes the metadata name to COFF. The earlier
`canonicalize_relocs.py` post-COFF mutator remains a comparison prototype only.

## Authorization rule

A target DIR32 relocation is rewritten only when all of these facts hold:

- the function is 100% in the relocation-masked report;
- candidate and target have the relocation at the same function-relative site and use
  the same relocation type;
- the candidate relocation names an exact `cv-public-data` symbol from
  `build/gen/symbol_names.csv`; and
- `public symbol RVA + candidate implicit addend` equals the absolute target read
  directly from the retail PE relocation field.

The last comparison is the authority. The target may currently be named `const_<RVA>`,
a string-like synthetic PDB alias, or another delinker identity; none of those names is
used to infer the address. A compiler-local candidate symbol, a merely similar owner
name, or a wrong addend cannot authorize a rewrite.

The broad resolved-address audit remains independent. Canonicalization makes strict
objdiff compare equivalent symbol/addend spellings; it must never become the evidence
that a candidate address is correct.

## Two-pass pipeline

The sound pipeline is staged because candidate objects do not exist during clean init:

1. `homm2 init` synthesizes the PDB and produces the raw delink target tree.
2. Compile candidate objects and generate the ordinary relocation-masked report.
3. Generate the exact-address owner ledger, synthesize a second PDB, and delink a
   separate strict target tree with the patched pinned delinker:

   ```sh
   python3 -m homm2.build.build_reloc_target \
     --raw-dir build/delink \
     --output-dir build/delink-reloc
   ```

4. Point a second objdiff project at `build/delink-reloc` and generate its report with
   `-c functionRelocDiffs=data_value`.

Never overwrite the raw init output. The driver refuses identical raw/output paths and
replaces an existing derived tree only when its `.homm2-reloc-pdb-target` ownership
marker is present. It also rejects an empty proof ledger and scans every emitted object
to ensure that the private PDB metadata prefix did not leak into COFF.

This pipeline is not yet a hard build gate. The remaining strict reopenings below need
normalization or explicit exclusion first.

## Pinned measurement

Measured from source checkpoint `70a8a68`:

- normal report: 838 exact functions;
- pre-regeneration raw `data_value`: 550 exact functions;
- clean-init raw `data_value`, after the existing explicit-owner synth-PDB pass: 575
  exact functions;
- second-PDB canonical target: 593 exact functions;
- proof ledger: 213 exact target RVAs from 3,568 authorized DIR32 sites;
- alternate owner spellings for a target RVA: 0;
- coverage: 838 paired exact functions and 8,113 base relocation sites;
- same-site/same-type: 8,113; missing target site: 0; type mismatch: 0;
- unresolved base: 1,041; unresolved target: 1; duplicate-string ambiguity: 442;
  unknown compiler-local: 768.

The second-PDB target and the post-COFF prototype have the same 593-function exact set.
Their overall fuzzy scores differ slightly (97.69981 versus 97.69768) because the two
mechanisms retain different nonexact data identities.

Two independent ledger, synth-PDB, and patched-delinker runs produced byte-identical
ledgers and PDBs and the same complete target-tree digest:
`e2c3a149c5e9fbe4c7a78ebba3a56bf360fd67cc37e74d6eb168fa99d865d443`.

The 245 remaining strict reopenings have overlapping categories, so function counts do
not sum to 245:

| Category | Sites | Functions | Representative evidence |
|---|---:|---:|---|
| duplicate string/local literal | 326 | 97 | `Bzip::bitStreamEOF`: candidate `$SG4166`, target content-named `??_C@...` |
| unknown compiler-local identity | 239 | 39 | `mouseManager::SetColorMice`: `_gDefaultCursorName$S32390` versus `const_0011ef24` |
| referenced data value/section identity | n/a | 123 | `getFinalCRC`: ordered relocation tuples agree, but `data_value` still compares referenced target data/section identity |
| same-address external alias | 10 | 8 | `resourceManager::MakeId`: `__stricmp` versus `__strcmpi`, both resolving to the same CRT address |
| resolved-address difference | 10 | 8 | `armyGroup::HasAllUndead`: candidate `0xfaec9`, retail `0xfaec7` |
| folded/local branch identity | 1 | 1 | `heroWindow::RemoveWidget`: `widget::Close` versus delinked `empty_stub` |

Later source fixes can reduce the resolved-address row; it is deliberately reported by
the independent resolved audit rather than allowlisted here.

## Negative control

The corrected `SetupClipper` and `DDInitGraphics` candidate relocations use
`gConfig+0x30` and become 100% under the paired strict report. A copied candidate object
was then changed only at those two implicit addends, from `0x30` back to the known-wrong
`0x1c`. The ledger generator authorized two fewer sites. The global `gConfig+0x30`
alias remained unchanged because twelve independent exact sites still proved it, and
no `gConfig+0x1c` alias was emitted. Strict objdiff retained the expected mismatches:

- `SetupClipper`: 99.92647%;
- `DDInitGraphics`: 99.947914%.

Thus a wrong candidate addend does not bootstrap a matching target alias.

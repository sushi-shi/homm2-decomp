# Completion audit — Buka common-helper search

Historical snapshot: completed 2026-09-12 at `77b87cd47` on
`audit/buka-common-helpers`, based on canonical
`decomp-gold-2.1-buka` at `ca2904a91`. This completes the requested deliberate
reading/search and its function checklist. It does not implement the candidate
refactors or propagate them to generated/ported source.

All evidence and counts below refer to that completed audit snapshot, not the
subsequently modified implementation. See [implementation.md](implementation.md)
for the applied changes, final verification and current inventory reconciliation.
The current checklist intentionally invalidates historical read credit for
changed files; the audit's original all-read inventory is preserved at `77b87cd47`.

## Requirement-by-requirement evidence

| Requirement | Evidence at the audit snapshot | Result |
| --- | --- | --- |
| Use canonical Buka first; propagation later | Audit worktree is based on `ca2904a91`. Whole-branch `git diff ca2904a91 -- src include config` is empty. Changes are audit documentation and inventory/test tooling only. | Satisfied; no propagation. |
| Deliberately read the codebase, not merely search it | B01-B44 in [batches.md](batches.md) document complete file/body/declaration/data reads and function-boundary reconciliation. All 228 [review records](reviews.json) bind to the current whole-file hashes. Final validation rechecked every hash. | 228/228 files read. |
| Make an all-functions read/unread list | [functions.tsv](functions.tsv) has 1,655 unique physical definitions, with path, start/end, name, signature, kind, annotation, status, hash and review. All status fields are `read`; changed files become `unread` on regeneration. | 1,655/1,655 read; zero unread. |
| Include inlines, unannotated and nonordinary bodies | Inventory contains 1,647 C++ bodies, four explicit defaulted/deleted definitions and four assembly bodies. Header/conditional/unannotated definitions were reconciled during reading; all 1,491 VA markers resolve to indexed bodies. | Covered, not limited to retail annotations. |
| Search for common macros across TUs | [macros.tsv](macros.tsv) indexes all 745 definitions, including guards/conditional variants. [candidates.md](candidates.md) distinguishes existing macros from proposed expressions/statements, with named occurrences and noninstances. | Completed search; guards/numeric aliases are not counted as discoveries. |
| Consider small, period-plausible inlines too | The ledger and [ranked index](findings.md) specify existing API reuse, declaration defaults, narrow owner methods/free inlines and macro alternatives. Contracts preserve types, narrowing, evaluation and store order. Modern checking infrastructure and broad frameworks are excluded. | Covered as hypotheses, not recovered original spellings. |
| Helpers must span multiple TUs | Every one of H01-H99 has named witnesses in at least two fully read TUs. Final review checks the contracts, owner declarations, exact versus ordered/protocol variants and existing alternatives. B45 records the additional source checks and corrections. | Cross-TU evidence supplied; witness sets are not bulk replacement lists. |
| Main goal is readability | The final index ranks 12 first-experiment families, 58 useful narrow candidates and 29 lower-priority findings. It prefers existing vocabulary, records overlaps and retains 46 local/rejected observations. Each entry states what a name clarifies and what must stay explicit. | Prioritized handoff, not a mandate for 99 new abstractions. |

The scope is the complete canonical game tree: 98 implementation/assembly files
and 130 headers, totaling 122,441 source lines. A physical file listing, including
ignored/hidden files under `src` and `include`, agrees with Git's 228-file scope;
there are no extra local source files omitted from the checklist. Bundled Bzip
inside `src/BASE` was read. The other tracked C/C++/assembly material is vendor
code, matching experiment/probe documentation, cleaner overrides and a tooling
test fixture; those are not additional canonical game TUs. This scope was set
before the reading pass, not narrowed to obtain completion.

## Validation and limits

Validation used Python 3.13.13 and Universal Ctags 6.2.1 from the local Nix
environment. The reproducible inventory commands and `--ctags` override are in
[README.md](README.md). Final results:

- Inventory regeneration followed by `homm2.audit.readability --check`: pass;
  no stale file/function/macro status or generated report.
- `python3 -m unittest homm2.audit.test_readability -v`: 20 tests pass, including
  all six real-Ctags integration tests; none skipped.
- `python3 -m homm2 selftest audit`: 239 tests pass; none skipped. The explicit
  Ctags path enabled integration tests. An unprovisioned `MSVC_DIR` was used only
  for module imports; no compiler or retail build was invoked.
- Additional read-only checks: exact agreement of tracked files, file checklist
  and review keys; all 228 SHA-256 values current; 1,655 unique definitions;
  complete H01-H99 ledger/index correspondence; R01-R46 and B01-B45 references
  resolve; proposed existing owner-header paths and witness TU paths exist;
  Markdown file links resolve and the finding tables have five columns.
- Independent raw `#define` check: all 745 macro names, locations and physical
  extents match the inventory. The C++ parser had skipped `NextCreatureType`
  in the `#else` following its inline alternative; a separate CPreProcessor pass
  fixes that omission. Regression tests cover this case, both conditional macro
  arms, multiline extents/hashes, comments and quoted text. The previously read
  header and all 1,655 function definitions are unchanged; no new reading credit
  or helper discovery is claimed for the missing index row.
- `git diff --check`: pass. Whole-branch game source/header/configuration diff
  against `ca2904a91`: empty.

The inventory script was itself inspected: it never grants reading credit,
indexes inactive branches, rejects unsupported source types, checks annotated
body association and invalid/missing extents, and invalidates changed reviews.
Its tests prove those mechanics, not that a person or agent read a function or
that Ctags is a complete C++ parser. The deliberate whole-file reads and manual
boundary reconciliation are the reading evidence; hashes establish that the
reviewed contents are still the current contents.

No candidate has newly established retail-byte, relocation or inline-expansion
equivalence. Existing API names and source macros are real current vocabulary;
proposed names and historical likelihood remain judgments. A later implementation
must follow the canonical matching checks for each retained source change,
including affected raw bytes and ordered relocations. Neither test success nor
the similarity of two fragments substitutes for those checks.

No required reading/search work remains for this snapshot. Implementation,
measurement and later propagation are separate next tasks. Start with the small
A-ranked shortlist in [findings.md](findings.md), especially H01's duplicated
widget-message macro and the existing APIs' repetitive optional arguments.

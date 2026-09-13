# Buka common-helper readability

The applied refactor targets canonical `decomp-gold-2.1-buka`. It names narrow
shared operations while preserving Buka's behavior and compiler output; it does
not claim original developer macro names or propagate changes to portable source.

- [Final decisions and contracts](findings.md): all 99 finding families and 46
  exclusions, with retained subsets distinguished from deliberately explicit code.
- [Verification and limitations](implementation.md): reproducible checks, baseline
  prerequisites and links to the eight measured matching experiments.
- [Reading records](reviews.json) and [implementation delta records](implementation-review.tsv):
  compact, hash-bound provenance inputs, not a live progress report.

## Archived audit

The complete pre-implementation reading pass is preserved on
`audit/buka-common-helpers` at
[`77b87cd47`](https://github.com/sushi-shi/homm2-decomp/tree/77b87cd47c847aa5ff6aa95d2c691601d0057f6d/docs/readability). It contains the original candidate
ledger, B01–B45 reading diary, completion report and all-read inventories.
The applied snapshot before this documentation cleanup is `750ce56aa`.
Historical records are not rewritten to claim a new whole-file read.

`reviews.json` records whole-file reads at the audit snapshot. Its B/H/R references
refer to that archive. Changed files become unread when current hashes differ.
`implementation-review.tsv` records the separate caller/contract review between
the audit and applied snapshots; it does not grant whole-file reading credit.

## Generate a local inventory

With Universal Ctags 6 on PATH, run from the repository root:

```sh
PYTHONPATH=scripts python3 -m homm2.audit.readability --write
PYTHONPATH=scripts python3 -m homm2.audit.readability --check
```

Use `--ctags /path/to/ctags` when needed. Outputs are ignored build artifacts:
`build/readability/inventory/{files,functions,macros}.tsv` and `progress.md`.
`--check` compares those local products with the current source and reports
missing/stale files; run `--write` first in a fresh worktree. Neither mode edits
the committed reading records or game source.

The inventory covers tracked game source and headers, including header bodies,
unannotated helpers, conditional macros, fixed assembly and bundled Bzip.
Ctags supplies navigation and VA/body association checks, not proof of complete
C++ parsing, human reading or byte equivalence. A deliberate whole-file review
must reconcile the index before updating its hash-bound reading record.

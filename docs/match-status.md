# Match tracking

Run inside `nix develop .#build`:

```sh
homm2 build
homm2 verify status
homm2 verify check
```

[config/match_baseline.tsv](../config/match_baseline.tsv) (and
`config/match_baseline.editor.tsv` for the editor) records banked progress.
The current report is generated from a real build, not maintained in prose.

- `max_fuzzy`: best observed score for the function's current effective-source hash.
- `src_hash`: the normalized effective-source hash (function body plus its tracked
  codegen dependencies); a changed hash starts a new epoch.

The README status block reports two numbers per function:

- **CUR** is the live score in the current normalized object comparison. Unbuilt
  source edits are not measured, and it is not a raw linked-image equality claim;
  `homm2 build` runs separate raw-object gates and its `link-diff` step compares
  the whole linked image with retail.
- **MAX** is the best score observed for the function's current effective-source
  hash, including audited exact disposable TU-state probes. An unedited dip keeps
  MAX; a genuine source edit resets MAX to CUR. Maxima are historical navigation
  data, not correctness proof or enforcement.

There is no separate historical peak (HIST): the ledger keeps one row per function
for its current hash. `homm2 audit historical-losses` recovers earlier peaks from
the committed ledger epochs.

README headlines and module scores use MAX; a separate CUR / MAX line shows both.
Fuzzy figures are size-weighted instruction matches. Data bytes are not shown in
the README: `homm2 verify check` requires every function and every data byte to be
exact, and `homm2 verify status` prints the data figures.

`homm2 verify bank` is an explicit ledger-writing operation; `homm2 build` also
records maxima and refreshes the README block. `homm2 audit ledger` reports rows
banked against a hash that no longer exists.

## Excluded code

The totals cover every in-`.text` reconstruction target. Identified generated
and library code is carved out of the denominator (`CARVE_OUTS` in
[scripts/homm2/verify/status.py](../scripts/homm2/verify/status.py));
`homm2 verify status` prints the carved function counts:

| Module | Why excluded |
| --- | --- |
| `(funclets)` | compiler /GX EH; match with their parent function |
| `(imports)` | import thunks (`config/retail/functions_imports.csv`) |
| `(libcmt)` | FID-identified static runtime (`config/retail/functions_static_libs.csv`) |
| `(compgen)` | compiler-generated bodies awaiting an owner unit |

Unclaimed reconstruction targets are not carved out; they stay in the denominator.

An exact score is evidence about the compared body and referents, not proof of
all source identities, callers, or runtime behavior.

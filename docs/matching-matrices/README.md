# Matcher experiment matrices

These TSV files are durable no-repeat sets for large exact-preserving searches. Each row is:

```text
whole-source-file SHA-256<TAB>focused function match percentage
```

Before replaying a source state, compare its hash here and confirm the canonical source and pinned
sibling hashes have not changed. A listed hash should not be rebuilt under the same compiler/header
state.

## BASE/Textntry AST searches

All three searches used only `scripts/permute_ast.py`, never the regex permuter, and required all 11
siblings to remain pinned:

- `textntry-ctor-ast-e4e96c0.tsv`: initial constructor pass, 562 unique hashes; SHA-256
  `150e0e735e04b738cb4cf3fe17cf2308e3a5f2ff878f898a65f4b60f65449e75`;
- `textntry-read-ast-ctor8aaf3.tsv`: `Read` pass from the improved constructor state, 356 unique
  hashes; SHA-256 `de0d4815b5c32b62e679891af36c71bb7865589814ea63dd112527786a705b0c`;
- `textntry-ctor-ast-8aaf3-deep.tsv`: constructor continuation, 819 unique hashes; SHA-256
  `0e5a6584c1c4786abf35867a9c1072070ced7586d13f64bd69ac67d0354c9f8a`.

The initial constructor pass found the retained 98.695656% source state. The `Read` pass and deep
constructor continuation found no further improvement.

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

## BASE/Textntry include-surface audit

`textntry-include-surface-8aaf3.tsv` (SHA-256
`d58c40bf716815c90a362b6f0f5fbd6a733105499dd25ea089ee57ad11e13f0a`) records the canonical state
plus eight previously untested declaration/include surfaces. The matrix includes every objdiff
function row and retail size; the two deleting-destructor aliases remain unscored because the
canonical standalone-destructor tradeoff emits one 0x1f base wrapper for the two retail 0x36
aliases. All scored siblings and sizes remained pinned. Canonical and the redundant-`va.h` removal
also produced the same aggregate emitted `.text` dump SHA-256,
`d68541c49e4bf533e079cc8410cc8d24d82b6ad72d3d3010be1fd16c74238d4f`.

The audit established that `BASE/Misc.h` is still required as the owner of `BaseAlloc`/`BaseFree`,
`SOURCE/KB.h` is still required as the owner of the manager globals, `glTimers`, and
`gbTextEntryEscaped`, and `SOURCE/X_GLOBAL.h` and `SOURCE/kbwin.h` own the remaining globals and
functions. Only the direct `va.h` include is redundant, and removing it is byte-neutral, so the
canonical include block is retained rather than creating a no-gain source hash.

## BASE/Textntry fresh SetupDisplayString structure pass

`textntry-setup-fresh-2e351c8.tsv` records the independent retail-structure pass from checkpoint
`2e351c8`. The retained second-loop guard and explicit zero-offset clear raised
`SetupDisplayString` from 95.000000% to 97.128380%, matched the retail `0x1be` size, and kept the
`0x130` frame and 8/8 relocations. The remaining code difference is the whole-function EBX/EBP
allocation swap between `this` and the cursor/second-loop flag lifetime.

The final whole-file source SHA-256 is
`fd22c7692d4e9293c47a13eb83b422b678371fe4ee19755d78b515c9b94fd5cb`; the unchanged
`include/BASE/textEntryWidget.h` SHA-256 is
`f5d08f5a98e96310182cf720733ccb4670b41bfcb24c44ec8ce36fcce86072a3`.

Every row pins the same sibling baseline: default constructor 100%, long constructor 98.695656%,
destructor 100%, `Read` 98.675500%, `Main` 99.852940%, and `Draw` 99.992424%; deleting-destructor
aliases remain unscored. The cursor snapshot, `register` snapshot, and loop-flag declaration-order
states are byte-identical at the new source structure and must not be repeated while the final
source/header hashes agree. No permutation tool was used.

## BASE/Textntry SetupDisplayString AST pass

`textntry-setup-ast-341e775.tsv` records a focused `scripts/permute_ast.py` pass from integrated
checkpoint `341e775` and source SHA-256
`fd22c7692d4e9293c47a13eb83b422b678371fe4ee19755d78b515c9b94fd5cb`. The pass exposed 18 legal
first-order variants and ran a deterministic 300-attempt walk with seed `341775`. Existing matrix
hashes were skipped before compilation, and unsafe inequality +/-1 rewrites were disabled.

The pass compiled 286 new unique whole-file hashes. Every candidate remained at 97.128380% and
kept all 11 objdiff symbols pinned, so the integrated source was restored unchanged. The matrix
SHA-256 is `ec3473c9c38668f263892a0a5868cd8280ec34c01d28c9359b95064be7ec8344`. Do not replay these
hashes while the source and header state agree. The regex permuter was never used.

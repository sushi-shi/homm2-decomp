# Breadth audit tracker

`homm2 breadth-audit` tracks the bounded, closest-to-100 review of every
source-backed non-exact function. Its versioned state is
`config/breadth_audit.json`.

The state is not a second match baseline and does not rename functions. Each
record contains the unit, decorated function name, normalized function source
hash, comparison epoch, outcome, bounded iteration count, before/after score,
and a concise residual or bug note. Recording a new source hash replaces only
that function's row; unrelated rows retain their content.

## Invalidation

The normalized source hash comes from the same function-local API used by
`homm2 status`. A source edit invalidates only the edited function. Compiler-
generated report rows without a source block are outside this audit because
they have no function source hash and may have duplicate decorated identities.

The comparison epoch hashes:

- every normalized target object consumed by objdiff;
- objdiff comparison configuration after removing every `base_path`;
- the `objdiff-cli` executable;
- `canonicalize_data_symbols.py`, which defines comparison-only symbol and
  local-label normalization;
- `assert_relocs.py`; and
- `assert_early_stop_bytes.py`.

Candidate/base objects are deliberately excluded. Recompiling or editing
ordinary source therefore cannot invalidate the full campaign. A target,
comparison configuration, objdiff, canonicalizer, or proof-gate change makes old records
stale. Stale rows remain visible state but do not exclude functions from the
pending queue. Malformed rows, unknown functions, missing source hashes, and
missing comparison inputs fail closed.

Exact functions are omitted from the pending queue without requiring records.
A non-exact function is considered checked only when both its stored source
hash and stored epoch equal the current values.

## Commands

```sh
homm2 breadth-audit epoch
homm2 breadth-audit summary
homm2 breadth-audit summary --module SOURCE
homm2 breadth-audit queue --module SOURCE --limit 35
homm2 breadth-audit queue --module SOURCE --exact-max-only --limit 35
```

The queue is TSV on standard output, sorted from closest to 100% downward. It
does not create a per-run file.

Record one completed audit:

```sh
homm2 breadth-audit record \
  --unit SOURCE/SPELLS \
  --function '?DefaultSpell@combatManager@@QAEXH@Z' \
  --outcome improved --iterations 6 \
  --before 99.861115 --after 99.930560 \
  --note 'one branch-destination byte remains; 3/3 relocations agree'
```

`--source-hash` and `--epoch` are optional assertions for worker handoffs. They
make a stale result fail instead of recording it against newer inputs.

For a batch, pass a tab-separated file or standard input with columns
`unit`, `function`, `outcome`, `iterations`, `before`, `after`, and `note`.
Optional `source_hash` and `epoch` columns provide the same stale-handoff
checks. Outcomes are `exact`, `improved`, or `parked`; iterations must be from
0 through 10. `after` is the score observed during the audit, not necessarily
today's live score: unrelated TU-state changes can perturb codegen without
changing this function's source hash. An `exact` row requires `after=100`, and
an `improved` row requires `after` to exceed `before`.

```sh
homm2 breadth-audit record --input batch.tsv
```

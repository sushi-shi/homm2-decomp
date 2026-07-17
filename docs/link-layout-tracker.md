# Incremental linker-layout tracker

`homm2 link-layout` turns the current final-link report into an address-ordered
contribution queue. It is intended for the phase after reviewed data topology is
closed across all configured units. It does not match functions and does not treat
whole-section objdiff data percentages as source evidence.

## Evidence model

For `.text`, retail TU starts and sizes come from NB09 `sstModule` contributions.
Candidate raw/aligned sizes come from the current COFF object, while candidate starts
come from the LINK MAP. A start is accepted only when its anchor is exactly one
retained public function in `build/gen/symbol_names.csv`, belongs to the expected TU,
and has exactly one definition at the same RVA and owner in `HEROES2W.map`.

This validation deliberately rejects compiler-local names. For example, both PHILAI
and X_GLOBAL currently emit `_$E4`; selecting the first MAP spelling assigns PHILAI
the wrong address. The queue retains PHILAI's local section-size evidence but labels
its start `invalid-anchor` rather than manufacturing an RVA delta.

The queue distinguishes:

- `root`: this TU's aligned contribution size differs from retail;
- `downstream`: the local size agrees, but an earlier root displaced its start;
- `invalid-anchor`: the candidate start is not independently proved; and
- `exact`: both the local contribution and start agree.

When a valid candidate start returns to zero delta, the earlier cumulative blocker is
closed. A later size mismatch begins a new chain. Fix the earliest actionable root,
relink, and regenerate the queue before touching its downstream rows.

## Persistent records

Reviewed rows live in `config/link_layout_audit.json`. Each record is local to one TU
contribution and contains the retail RVA/size/alignment, anchor, full candidate-object
hash, candidate `.text` payload hash, retail contribution payload hash, linker hash,
and a stable link-configuration hash. Thus an unrelated TU edit does not stale the
record. Outcomes are `open`, `exact`, and `blocked`; `exact` is refused unless the
current local size, start, and public anchor all agree.

Generated queue TSVs belong under `build/link/`, not in version control.

## Commands

```sh
homm2 link-layout summary
homm2 link-layout queue --limit 50
homm2 link-layout queue --output build/link/link-layout-queue.tsv
homm2 link-layout explain SOURCE/UNIT
homm2 link-layout record --unit SOURCE/UNIT --outcome open \
  --note 'first actionable contribution; bounded source audit in progress'
```

The queue and `config/link_layout_audit.json` hold the current root and its evidence.
Do not copy live offsets, next targets, or queue snapshots into this document; regenerate
them after every accepted contribution change.

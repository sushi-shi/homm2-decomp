# TODO

## `tu_state_noise --state-summary` perturbs the seeded trial sequence

A full sweep with `--seed N` is reproducible verbatim (Iconf2b 128-trial
target-insertion sweeps reproduce 87.8223% at trial 127 across runs), and
`--only-trial K --retain-best` replays a single trial faithfully. Adding
`--state-summary` to the same full sweep drifts the sequence after some
trial index (the same invocation peaked at a different trial/score). Find
where the summary bookkeeping consumes or reorders generation state, fix it,
and add a regression test comparing full-sweep trial tags and scores with
and without `--state-summary`.

## Segment-aware `DATA` topology ratchet

Verify that the hard build checks require compiler-emitted and linked `DATA(...)`
owners to preserve the reviewed order within each independently ordered data
segment or contribution. Source declaration order is intentionally semantic:
MSVC 4.2 emits function-local statics in name-hash order. The check must include
function-local private statics, not only file-scope globals, and must not compare
owners that belong to different segments.

Add regression coverage for a private-static rename that changes candidate COFF
order and report the owning TU, symbols, segment, and offending physical pair if
the generated model or final link no longer preserves reviewed topology.

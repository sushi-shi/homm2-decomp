# Archived scripts

These scripts preserve one-off recovery and compiler-shape experiments. They are
not part of the supported matching workflow, are excluded from active test
discovery, and may refer to historical reports or commands that no longer exist.

The supported unified compiler-shape search is `scripts/match_variants.py`; it
combines reviewed exact-span choices, conservative AST variants, and disposable
TU-state island probes.

The archived `carcass_fill.py`, `gen_queue.py`, and `verify_carcass.py` belong to
the completed placeholder/bootstrap phase. The live exhaustive residual queue is
implemented separately and remains supported.

`gen_global_decls.py`, `gen_global_defs.py`, and `gen_vtbl.py` are completed
one-time source migration generators. Their generated marker blocks no longer
exist.

The `enum_types/` package and archived constant-taxonomy document preserve the
completed integer-domain census. They predate `H2_ENUM_*`, private TU domains,
and the live file-by-file `homm2 constants` audit, so they are not supported CLI
commands or build gates.

`early_stop_bytes.py` preserves the retired source-marker byte gate. Residuals
are now always live evidence and the queue no longer carries marker state.

Use the current `homm2` commands and build assertions for live evidence. Restore
an archived script only when its experiment is deliberately being reproduced and
its assumptions have first been updated for the current object/report schema.

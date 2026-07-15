# Delinker contribution ownership

The shipping NB09 stream is publics-only, but its `sstModule` records retain every compiland's
linked segment ranges. `python3 -m homm2.build.contribution_manifest` converts the ranges belonging
to configured reconstruction TUs into `build/gen/delink_contributions.tsv`.

The manifest assigns `.text`, `.rdata`, initialized `.data`, and loader-zero `.bss` intervals to the
synthetic delinker object name. Writable ranges are split at the retail PE `.data` raw-size boundary;
the initialized part has bytes in the image while the remaining virtual tail is loader-zeroed. The
generator rejects unknown modules, out-of-section extents, and overlapping contribution intervals.

`vostok-delinker --contribution-manifest` uses these intervals only to constrain fallback data-symbol
selection. When resolving an address in a known contribution, a candidate public/string symbol must
belong to the same compiland and storage class. This prevents a nearest-public search from crossing a
TU boundary and manufacturing a large addend from an unrelated owner. The option is generic and
preserves legacy behavior for addresses outside the supplied ranges.

`--data-manifest` remains the inner, stronger layer. A reviewed exact allocation owns every RVA in
its proven extent before contribution-level fallback runs; contribution ranges are not symbol sizes
and never authorize materializing an entire TU data interval as one object.

Bootstrap `homm2 init` generates and passes `build/gen/delink_contributions.tsv`. Canonical
promotion copies the reviewed result to versioned `config/delink_contributions.tsv`; strict target
regeneration consumes that file without rederiving it. Its digest is part of
`.reviewed-data-stamp.json`. A changed canonical input makes normal build/status fail until the
explicit `homm2 data-topology regenerate` command replaces the target.

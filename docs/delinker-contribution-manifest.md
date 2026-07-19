# Delinker contribution ownership

The shipping NB09 stream is publics-only, but its `sstModule` records retain every compiland's
linked segment ranges. `python3 -m homm2.build.contribution_manifest` converts the ranges belonging
to configured reconstruction TUs into `build/gen/delink_contributions.tsv`.

The bootstrap manifest assigns `.text`, `.rdata`, initialized `.data`, and loader-zero `.bss`
intervals to the synthetic delinker object name. Writable ranges are initially split at the retail
PE `.data` raw-size boundary. Canonical assembly then classifies writable NB09 owner chunks with the
reviewed candidate section/storage evidence and writes the same generated path. The generator
rejects unknown modules, out-of-section extents, and overlapping contribution intervals.

`vostok-delinker --contribution-manifest` uses these intervals only to constrain fallback data-symbol
selection. When resolving an address in a known contribution, a candidate public/string symbol must
belong to the same compiland and storage class. This prevents a nearest-public search from crossing a
TU boundary and manufacturing a large addend from an unrelated owner. The option is generic and
preserves legacy behavior for addresses outside the supplied ranges.

`--data-manifest` remains the inner, stronger layer. A reviewed exact allocation owns every RVA in
its proven extent before contribution-level fallback runs; contribution ranges are not symbol sizes
and never authorize materializing an entire TU data interval as one object.

Bootstrap `homm2 init` generates and passes `build/gen/delink_contributions.tsv`. Canonical
regeneration deterministically refreshes the same generated path; it is never copied into versioned
configuration. Its digest is part of
`.reviewed-data-stamp.json`. A changed canonical input makes normal build/status fail until the
explicit `homm2 data-topology regenerate` command replaces the target.
The whole-image coverage gate loads and validates this exact generated manifest, so Vostok owner
resolution and TU `.data`/`.rdata`/`.bss` coverage cannot silently use different storage classes.

The comparison object and the final-link object have different jobs. Vostok preserves candidate
section topology so function and data comparisons remain reproducible. Immediately before the
final link, `retopologize_data.py` makes a disposable copy of each fully reviewed writable section
and projects its allocations into the owning NB09 contribution. It updates section bytes, symbols,
relocation source sites, section-symbol addends, alignment, and section-definition length together.
Relocations naming a real allocation retain their owner-relative addend, including reviewed
base-minus-N expressions.

This projection accepts only the conflict-free reviewed manifest. Uncovered nonzero bytes,
overlapping allocations, an allocation outside either section, or a relocation which cannot be
mapped to one reviewed owner is a hard error. A loader-zero candidate section becomes raw-backed
only where the retail contribution lies inside the proven initialized stream; it is not inferred
from the PE raw-size boundary alone. The source and canonical comparison objects are never modified.

# Cast and pointer follow-up

Follow-up branch to PR59, starting at `36ace4cae`. Scope is the user's three
priorities: unresolved casts, artificial address arithmetic, and out-of-object
pointers. No new goto, unused-local or unrelated-variable campaign is included.

## Current checkpoint

| Category | Pinned open | Still open | Verified change |
|---|---:|---:|---|
| Casts | 86 | 83 | CP01 removes20 redundant casts in3 functions |
| Artificial address arithmetic | 9 | 9 | CP03 row-owner candidate changes bytes; restored |
| Out-of-object pointers | 66 | 66 | CP02 removes6 cross-member transfer bases in2 still-open functions |

This is a partial follow-up, **not a zero-findings result**. The retained source
preserves all98 native objects against PR59, including1826 emitted function
instances and ordered relocations. The full build remains1727/1727 exact
functions and291987/291987 compared data bytes. Focused retail checker gaps
remain explicitly documented in [CP01](CP01.md) and [CP02](CP02.md).

`fixed` closes only the indicated category. `partial` remains open. A nonempty
`reviewed_body_sha256` binds the focused disposition to that current body;
`baseline_body_sha256` remains the original scope hash. Other categories and
the broader ten-pattern reviews are not recertified by this queue.

## Pinned scope

`scope.tsv` identifies 148 functions and 161 category entries inherited from
the last complete audit and reconciled with the merged source:

- 86 cast entries;
- 9 artificial-address entries;
- 66 out-of-object entries.

The earlier report said eight address entries. `IconToBitmap` had incorrectly
been marked absent despite using the same flattened palette access as its five
siblings; this follow-up includes that ninth case. Counts are function/category
entries, not counts of individual casts or memory accesses. The baseline body
hashes bind the queue to the PR59 source, not to temporary compiler variants.

## Dispositions and proof

A source fix requires the matching build and complete native byte/relocation
comparison. Necessary conversions and genuine runtime-stride indexing must be
explained as such; moving arithmetic into a macro does not establish a better
owner. A documented original-game defect is not a removed out-of-object access.
No case is silently closed to achieve a numerical zero.

The user confirmed the routing policy: preserve the 100% Buka match on decomp.
Verified improvements that change native code belong in separate PRs targeting
`master`; do not merge them into this matching branch. Existing master fixes
and open PRs must be reconciled before proposing duplicates. Uncertain object
boundaries are evidence gaps, not automatically confirmed original defects.

Concrete examples requiring that distinction include `nb_listen` copying 16
bytes starting at the wildcard and into an independently referenced filename
(B38: original C++ source-object extent remains inferred), signed palette indexing in
DrawBolt, and misindexed campaign writes. Recovering real campaign/setup/event
subobjects can fix other crossed-member accesses without changing the format;
it does not fix those separate defects. Caller-bound and malformed-input
questions remain visible even when a representation is improved.

The broader ten-pattern ledger still retains its pre-merge hash boundaries.
This focused queue does not certify the other seven categories in changed
functions or retroactively refresh their manual reviews.

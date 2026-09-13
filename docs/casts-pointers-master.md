# Cast and pointer follow-up on master

This is the complete inherited queue, **not a claim of zero remaining defects**.
The matching follow-up is PR #60; master changes belong to PR #61 or an existing
portable PR explicitly cited in the ledger. Decomp behavior is not changed to
make these numbers smaller.

[The ledger](casts-pointers-master.tsv) pins 148 function identities and 161
category entries: 86 casts, 9 artificial-address cases, and 66 out-of-object
cases. The earlier figure of eight address cases omitted `IconToBitmap`, the
sixth member of its decoder family. A function can belong to several categories.
These are review findings, not 161 independently demonstrated game bugs.

## Complete-review snapshot

| Category | Fixed here | Already replaced on master | Covered by open PRs | Required conversion | Caller contract | Unreviewed |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Casts | 47 | 28 | 6 | 5 | 0 | 0 |
| Artificial address arithmetic | 3 | 0 | 6 | 0 | 0 | 0 |
| Out-of-object pointers | 28 | 11 | 21 | 0 | 6 | 0 |

These are category entries, not distinct functions or newly reproduced defects.
In particular, 33 entries depend on other open PRs, and six retain explicit
caller contracts. The review queue is exhausted; the project is not certified
free of bounds errors. The checker recomputes these totals from the ledger.

## States

- `pending`: not yet dispositioned on current master.
- `fixed`: the identified source issue is repaired in this PR, with limits in
  the evidence column; this does not certify every input and caller of a body.
- `replaced`: master already removed the offending expression or subsystem.
- `covered-pr`: reviewed replacement is in another **open, unmerged** PR.
  It is not included in this PR's build or test claims.
- `required`: remaining conversion expresses a real numeric or API boundary.
- `contract`: a genuine owner is indexed under a caller precondition; it is
  **not** counted as proof that all possible callers are safe.
- `partial` / `open`: known residual work. Never include these in a zero claim.
- `-`: category was not in the inherited queue for that function.

Every disposition records the current master file hash (or `absent`) so later
edits invalidate the review instead of silently inheriting it. Another PR's
commit is named in the evidence. Hashes are review freshness checks, not proofs
of correctness. Findings discovered outside the pinned categories belong in
the evidence and divergence ledger; do not silently change the denominator.

Run `python3 tools/check_casts_pointers_master.py` for coverage and freshness.
`--require-reviewed` additionally fails while any category is still pending.
Even a successful complete-review check is not a zero-defect certificate.

## Existing portable work

Image decoder replacement is PR #37 (on #33); bitmap/tile clipping is PR #40
(on #37). Palette-table access is PR #21. Variable map records are PR #43
(on #28), and runtime/save-record separation is PR #50 (on #43). High scores
are PR #34. These stacks are not duplicated or represented as merged here.

The save-record review found that PR #50 still transfers four bytes through
the address of a two-byte event count. This PR explicitly encodes that prefix
as count plus first index, retaining the legacy repetition even when count is
zero. The three count/index arrays and their runtime layout stay unchanged.

The NetBIOS wildcard finding is not a master fix: master no longer contains
`nb_listen`. Retail bytes show a fixed-length copy spanning text interpreted as
an adjacent filename, but the original C++ object extent cannot be recovered
conclusively from the stripped executable. See the matching B38 evidence;
do not present it as a demonstrated packet failure or invent padding to close it.

## Validation limits

The full native Debug build and CTest suite are run after each batch. Focused
bit, mask, walk-sequence, event-header and packet tests exercise actual implementations
or helpers used by production. The one-byte bit allocation reproduces the old
four-byte read under ASan. GUI/gameplay and multiplayer have not been exercised
by these tests; portable multiplayer remains unsupported. Runtime guard changes
without an isolated engine test are identified as source-reviewed, not as
reproduced gameplay failures.

The packet test checks a fixed CRC/wire fixture, every representable payload
length, invalid-length rejection, destination canaries, and copying an
odd-addressed wire record into live message/payload objects. It does not validate
every received command length, sender domain, or legacy transport handshake.
Diff/join size and tail corrections are source-reviewed; the focused helper
tests are not an end-to-end savegame exchange test.

# Frozen developer-written drafts

The [intent](intent.md) and five complete files in [draft](draft/) were written
before compiling them. [frozen-source.json](frozen-source.json) records their
hashes. The draft uses ordinary member initialization, private implementation
helpers, a single list-removal loop, straightforward playback setup, inline
DIMMER constructors/forwarders, and direct empty-string arguments in REQUEST.
It was not selected from a matching-score matrix.

All four affected raw objects compile with the normal VC6 profiles: Effects,
DIMMER, its WINDOW consumer, and REQUEST. Ordinary LIB and LINK produce a full
game image without input/output corrections. The source hashes still match the
frozen manifest after evaluation. This is a disposable full-game build; the
normal comparison objects and source files are not overwritten.

## Behavior review before further source changes

The test harness extracts the actual Purge bodies from the retained source and
the frozen draft. A small instrumented list supplies playback state, records
queries/releases, and observes the global head during destruction. Across every
list length from zero through eight and every playing/finished combination
(511 cases), the draft preserves retained-node order, release order and query
order. In 255 cases it exposes a different head during destruction: its single
loop unlinks the head first, whereas the previous implementation destroys the
head before updating it. This is observable if destruction calls back into the
module. The real RefPtr destructor calls the stream's virtual `unref`, so a
callback-free destruction contract cannot be inferred from RefPtr alone.

The [second draft](draft-preserve-release-order/) keeps a readable head-removal
loop followed by interior removal, preserving that ordering. All other frozen
source choices remain. Its [manifest](frozen-release-order.json) was written
before its compilation. It passes all 511 cases with zero head-observation
differences and also compiles all four objects and links the full game.
This is a semantic descendant, not a byte-selected spelling change.

These tests model deterministic playback states and observe destruction; they
do not exercise actual Audiere playback, threads, arbitrary reentrant mutations,
or the entire application. No runtime equivalence or play-test claim is made.

## Comparison after freezing

Both drafts differ substantially from retail. The single-loop Purge emits
164 bytes; the release-order-preserving version emits 324; the preceding source
emits 354. Play emits 868 rather than 924 bytes in both drafts. Its member
initialization also removes the OutputStream destructor contribution from Effects.

DIMMER's short methods now belong to WINDOW's emitted inline definitions.
The constructors no longer have standalone bodies, WINDOW's constructing method
changes, and only Read remains emitted by DIMMER. REQUEST's simplified statement
formatting preserves its raw function records. No source choice was undone to
recover any of those matching measurements. Both complete drafts remain available
as structural candidates; they are not claimed as recovered original source.

Section comparisons and image hashes are in
[measured-results.json](measured-results.json). These links have local PDB paths
and one link pass; their headers are not a replay of retail build history.
They do not replace the canonical 10,099-byte checkpoint or claim executable
equality. In particular, moving header methods into WINDOW does not solve the
known retail placement by itself.

## Reproduction

Run from the isolated worktree root with cleanup checkpoint `8428dbd3`'s
normal raw objects built, inside `nix develop .#build`:

```
python3 docs/matching/developer-first/evaluate.py
python3 docs/matching/developer-first/test_semantics.py
c++ -std=c++98 -Wall -Wextra -pedantic build/developer-first-draft/semantics.cpp -o build/developer-first-draft/semantics
build/developer-first-draft/semantics
python3 docs/matching/developer-first/evaluate.py draft-preserve-release-order
python3 docs/matching/developer-first/test_semantics.py draft-preserve-release-order
c++ -std=c++98 -Wall -Wextra -pedantic build/developer-first-draft/semantics-release-order.cpp -o build/developer-first-draft/semantics-release-order
build/developer-first-draft/semantics-release-order
```

All raw objects, baseline snapshots, compiler logs, function bytes and ordered
relocations, libraries, response files, MAPs and images are retained under
`build/developer-first-draft/{evaluation,evaluation-release-order}/`. Semantic
logs and generated harnesses live in the parent directory. The evaluator uses
the read-only [COFF inspector](coff_inspector.py); its function comparison lists
raw relocation-record differences and does not pretend anonymous compiler labels
have been semantically normalized.

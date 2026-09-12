# Buka common-helper reading audit

Scope: the canonical `decomp-gold-2.1-buka` game source and headers, starting at
`ca2904a91`. Propagation to generated source and the cross-platform port is a
later step. This audit searches deliberately for small, period-plausible shared
macros and inline functions that explain code across translation units.

This is a source-reading audit, not a claim to have recovered original source
spelling. Similar code alone does not prove a historical macro or inline.

The deliberate pass is complete: **228 files and 1,655 physical function
definitions read**, including header bodies and assembly. Start with the
[ranked findings](findings.md), then use the [detailed ledger](candidates.md)
for the exact contracts, call sites and exclusions. It records 99 finding
families and 46 local/rejected observations; those are not 99 required new APIs.
The [completion audit](completion.md) states the evidence and its limits.

## Inventory and reading contract

- [Every function definition](functions.tsv), including constructors, destructors,
  header bodies, unannotated helpers, and the fixed assembly routines.
- [Every source/header file](files.tsv), including files with no function bodies.
- [Existing macros](macros.tsv), including conditional variants and include guards;
  these are classified during the owning-file read, not counted as helper ideas.
- [Current progress](progress.md) and [review records](reviews.json).
- [Ranked findings](findings.md), [candidate ledger](candidates.md),
  [batch notes](batches.md), and [completion evidence](completion.md).

`read` means the complete file, all its function bodies, and relevant declarations
were deliberately read; its indexed function boundaries were reconciled. Searches,
call-site snippets, and automated clone detection do not earn that mark. Files
containing a partially read long function remain `unread` until the complete body
and the rest of the file have been covered. Review records bind to whole-file
SHA-256, so changes reset the file and its functions to `unread` on regeneration.
Reading shared declarations/call sites also remains necessary when judging a helper.

Universal Ctags 6 indexes physical definitions using explicit expansions of the
project's enum annotation macros. A separate CPreProcessor pass indexes macros,
including alternatives to inline functions that the C++ parser can skip. Each
source `VA` must have an indexed body.
This is a navigation/completeness aid, not a C++ correctness proof. Human reading
must reconcile unannotated and conditional bodies too. Vendor SDK/library sources,
build tooling, and hypothetical compiler-generated bodies are outside the game
function checklist; bundled Bzip under `src/BASE` remains inside it.

Regenerate/check from the repository root (Python standard library only):

```sh
PYTHONPATH=scripts python3 -m homm2.audit.readability --write
PYTHONPATH=scripts python3 -m homm2.audit.readability --check
```

Use `--ctags /path/to/ctags` if it is not on PATH. Edit review records only after
reading the complete file; each needs its current hash and a batch-note reference.
Inventory generation never marks a file read and never edits game source.

## Decision criteria

A candidate needs named occurrences in at least two TUs, a precise contract, a
natural owner header, and an explanation of what the name makes clearer. Record
argument evaluation, signedness/narrowing, inclusive/exclusive bounds, mutation,
field-store order, aliasing, and control-flow effects where relevant. Prefer an
existing abstraction when one already carries the exact contract.

Separate (1) existing duplicate macro definitions, (2) credible shared expressions
or statement idioms, (3) possible inlines needing retail evidence, (4) one-TU local
cleanup, and (5) rejected similarities. Do not wrap entire algorithms in macros or
hide materially different resource lifetimes, clipping modes, wire formats, or
event protocols behind one name. C++98-era methods/free inlines and narrow macros
are the idiom under consideration, not modern abstraction frameworks.

No executable refactor is considered verified merely because it looks equivalent.
On this branch a later retained change needs the matching build, raw bytes and
ordered relocations; inline expansion can affect `/Od /Ob1` frames and code shape.
The search can proceed without the original executable. Implementation/propagation
must not invent matching evidence if retail inputs remain unavailable.

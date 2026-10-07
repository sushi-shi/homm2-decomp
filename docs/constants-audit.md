# Constants audit

`homm2 verify enum-reuse` maps every evaluated value to every enum member,
`#define` constant and `const` integer of both programs that has it, for
reuse investigation ([enum-reuse.md](enum-reuse.md)). Equal integers are
candidates, not proof of a shared semantic domain.

`homm2 verify constants` inventories numeric constants throughout reconstructed game code. It writes
the complete occurrence list to `build/constants/literals.tsv` and clang-tidy's
`readability-magic-numbers` findings (0 and 1 ignored) to `build/constants/magic-numbers.tsv`.
`build/constants/null-zero.tsv` must remain empty. One census covers both programs: editor-only
units are read with the editor's defines, and a shared unit with `#ifdef HOMM2_EDITOR` code is read
a second time as the editor (`build/clangd/editor-view/`). The generated
`build/constants/README.md` summarizes contexts and orders the current review queue.

A finding in `code`, a `local-table` or a `declaration` (see Classification) is **open** until it
is spelled as a name (an enumerator, a named constant, `NULL`) or a row in
`config/constants.tsv` keeps it numeric with a reason. `build/constants/open.tsv` lists the open
findings (path, line, column, literal, category, owner, context). The committed `#floor` in
`config/constants.tsv` is the open count and only goes down: `homm2 verify constants` (run by
`homm2 build verify`) fails when the open count rises above it, when a kept row keeps nothing
(stale) or is malformed, when a file checked off as `reviewed` still has open findings, or when a
null pointer is spelled `0`.

```sh
homm2 verify constants                       # census and gate
homm2 verify constants --list EVENTS.cpp     # open findings whose file or owner contains it
homm2 verify constants --update-floor        # lower the floor after a batch
```

Kept rows are tab-separated fnmatch globs over file, owner (the enclosing
function), spelling, group (the finding's category) and detail (its source
line), then a reason; the first matching row wins, so put narrow rows before
broad ones. Keep rows for quantities that have no name in the game (pixel
geometry of a retail layout, icon frame numbers, random bounds, delays). A value
that a domain names is not kept; name it.

The per-file checklist is `config/reviews/constants.tsv`. Every reconstructed source and project
header has exactly one row. A reconstructed file stays `pending` until every occurrence has been
reviewed; the gate rejects a `reviewed` file while it still has an open finding. Imported
implementations that are intentionally preserved with their original algorithm spelling use
`third-party`; their findings remain in the inventory but are not counted as open. Each such row
records its provenance and the reason the numeric spelling remains intact.

## Classification

- `annotation` covers `VA`, `VA_COMPGEN`, `DATA`, `DATA_COMPGEN`,
  `DATA_COMPGEN_GUARD`, `VTBL`, `VTBL2`, and `SIZE`. These values are
  evidence, not gameplay constants.
- `enum` is an already declared numeric domain. Shared domains live in their owning header;
  domains used by one translation unit live in that `.cpp` and use prefix-free names where clear.
- `data-payload` is an element of a global initialized object. Review the table's identity, type,
  dimensions, and storage as a unit; do not invent a name for every payload byte.
- `local-table` is an initialized table inside a function. Prefer a named private table and named
  dimensions when that preserves the retail storage and code shape.
- `declaration` includes array extents, storage dimensions, and file-scope scalar initializers.
- `source-line` is an explicit allocation, free, or assertion line operand, or a named retained
  source-line data anchor. It is historical diagnostic and relocation evidence, not a gameplay
  constant.
- `code` is an executable numeric decision, argument, index, scale, mask, timeout, coordinate, or
  sentinel and is the primary cleanup queue.

## Review contract

For each file, inspect both generated lists and the surrounding source. Shared numeric domains use
an explicitly valued enum in the owning header. A domain used by one translation unit stays private
in that `.cpp`; private names need no module prefix unless it adds meaning. Dimensions, physical
units, masks, and algorithm parameters use named constants when they are not value domains. Obvious
arithmetic zero and one may remain numeric after review. Null pointers always use `NULL`.

Do not replace serialized payloads, retail annotations, source-line steering values, or assembly
operands merely to silence the audit. Preserve meaningful evidence and record any necessary retained
literal in the manifest notes. Each source batch is verified with MSVC, raw bytes, and ordered
relocations; a fuzzy score is navigation only and never an acceptance ratchet.

## Storage-split enum domains

Use `H2_ENUM_CLASS_BEGIN_SPLIT(name, storage)` when one semantic domain has an `i32` inventory/API
type but is also stored in narrower fields. Strict Clang builds see a scoped enum with the declared
underlying type; retail MSVC builds keep the domain typedef as `i32`. Declare each stored field as
`H2_ENUM_STORAGE(name, proven_storage)`, which exposes the enum to strict checking while preserving
that field's proven representation (`u8`, `i8`, plain `char`, or `i32`) in both builds. Strict
builds use a width-preserving storage proxy whose assignments and reads are checked as the enum;
retail builds use the proven scalar directly.

Function parameters and semantic locals use the domain type directly. Use
`H2_ENUM_PARAM(name, retail_type)` only where the retail signature has a different scalar type;
strict builds still expose the enum parameter. Raw file/network values are converted where they
enter the domain, and `IDX` remains limited to real indexing or arithmetic.
Generic integer sinks should provide a strict-only enum overload when every enum is a legitimate
input, instead of making ordinary callers encode and decode the domain manually.

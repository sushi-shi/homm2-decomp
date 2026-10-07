# Enum and constant reuse review

`homm2 verify enum-reuse` evaluates every named integer constant of both
programs and groups them by value. Equal numbers are review leads, not
evidence that two domains are one type. The command was ported from HoMM1's
`homm1 verify enum-reuse` (see [tooling-convergence.md](tooling-convergence.md#ported-checks)).

## What it reads

A key is one named constant:

- an enum member: every `H2_ENUM_BEGIN`, `H2_ENUM_CLASS_BEGIN`,
  `H2_ENUM_CLASS_BEGIN_T` and `H2_ENUM_CLASS_BEGIN_SPLIT` block and every raw
  `enum` block under `include/` and `src/{BASE,SOURCE,EDITOR}` (the macro
  machinery in `include/Ints.h` is skipped);
- an object-like `#define` whose body is an integer constant expression; and
- a `const` or `static const` integer (or enum-typed) variable with a constant
  initializer, at namespace, class or function scope.

A lexical inventory finds every enum block and `#define` without
preprocessing. libclang then parses every unit of every image in the
retail-analysis view (`ClangMode.RETAIL_ANALYSIS`, as the libclang audits do)
with that image's defines from
`homm2.manifest.clang_image_defines`: an editor-only unit is read as the editor,
and a unit both programs link is read once as the game and once as the editor,
so `#ifdef HOMM2_EDITOR` code is evaluated. Enum members and `const` values are
evaluated by Clang; each macro is evaluated by an enumerator appended after the
unit's last line (`enum : __int64 { probe = (NAME) }`), and a macro the unit
`#undef`s before its end by its definition's body. Probes that do not compile
(floating-point, string, type or local-alias macros) are not constants.

The two views must cover each other. An enum member or a numeric `#define`
(a body with an integer literal) that no unit evaluates is fatal, as is an
evaluated enum member or macro the inventory does not know. Two exceptions are
not holes: a block under `#if H2_STRICT_ENUMS` exists only for the strict-enum
view the retail compiler never sees (reported as "strict-view only"), and one
macro name of one file is one key, so `H2_STRICT_ENUMS 1` is covered by its
evaluated `0` alternative.

## Reports

The command writes derived reports to ignored `build/gen/`:

- `constant_values.tsv` and `constant_values.json`: the value map, every
  evaluated value with every key that has it. Each key carries its qualified
  name, category (`enum`, `macro`, `const`), domain (the enum, or the file's
  `<macros>`/`<const>` group, or a class's or function's `<const>`), file and
  line, the images that compile it, and its use contexts. Values held by two
  or more keys come first, ordered by how many distinct domains share them.
- `enum_reuse.tsv`: every key with its block kind, storage, expression and the
  units that evaluated it.
- `enum_value_collisions.tsv`: keys grouped by value across domains, with the
  function-body literals of the same value.
- `enum_domain_pairs.tsv`: enum pairs with overlapping value sets.
- `enum_role_pairs.tsv`: enum pairs where at least two equal values also have
  equal member-name suffixes after each enum's common prefix. A search aid only.

```sh
homm2 verify enum-reuse                  # reports, then the ledger check
homm2 verify enum-reuse --by-value       # print the value map
homm2 verify enum-reuse --value 448      # one value (repeatable)
homm2 verify enum-reuse --duplicates     # values two or more domains declare
homm2 verify enum-reuse --json           # the selection as JSON
homm2 verify enum-reuse --extend-ledger  # append new domains as pending
```

`homm2 audit enums` is an alias of the command.

Every key records its use contexts: the declaration identity (field,
parameter, comparison operand, switch subject, array, return) that receives
each reference to it (`scripts/homm2/verify/constant_context.py`). A macro's
uses are its expansion sites. `enum_value_collisions.tsv` lists contexts
shared by two domains of one value (`shared_named_contexts`) or by a key and a
bare function literal of that value (`shared_literal_contexts`);
`enum_domain_pairs.tsv` ranks pairs by shared direct contexts before numeric
overlap. A shared destination is a lead for one domain; a transport that
carries several domains (a widget message id, each window's own controls) is
not.

## Decisions

`config/reviews/enum-reuse.tsv` snapshots each starting domain (each enum
block, and each file's macro and const groups) with its evaluated `name=value`
members and one decision:

- `retain`: the domain stays; its values select a distinct quantity, table,
  operation, representation or state machine.
- `canonical`: this domain owns values that another reviewed domain reuses.
- `reuse`: the members moved to the canonical domain; `member_reuse` maps every
  moved member to `source-enum::MEMBER`. A member that no code names any more
  maps to `-` (retired); the check requires its identifier to be absent from
  every file under `include/` and `src/`.
- `pending`: the producers, consumers and encodings still need review, or the
  reuse is decided but the source move has not landed. Pending rows keep the
  command nonzero, so it runs outside the `homm2 build verify` tier.

Follow both value paths through their fields, callers and tables. Direct
transport of one quantity supports reuse; two zero-based tables that only
share an order support retention. A game and an editor copy of one geometry
or one menu are reuse leads like any other: the editor links the game's shared
headers. Every starting member needs a current home with the same value; new,
removed or changed members need a new decision. Moving members changes C1
symbol numbering in the unit, so do source moves as a reviewed batch and check
the edited functions with `homm2 match`.

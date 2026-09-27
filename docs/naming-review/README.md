# Systematic naming review

This is the work list for a later review. **Every entry starts unreviewed.**
Collection does not judge names or authorize further renames.

Start with [worklist.md](worklist.md): one checkbox per tracked source/header
file, in path order. Finish a file before moving to the next. Within a file,
review declarations in line/column order, including each function's arguments,
locals and nested scopes. Follow callers and shared headers for evidence without
silently marking those other files complete.

## Snapshot and scope

The snapshot is from `ec64499ea47f9a2e1b86105adb6264d1561230b7`, the reviewed
rename branch in PR #67. This work-list PR is based on that branch so its diff
contains only the work list. It does not merge or propagate the renames.

| Inventory | Count | Purpose |
|---|---:|---|
| [Files](files.tsv) | 234 | Every tracked file under `src/` and `include/`, with SHA-256 and coverage |
| [Declarations](declarations.tsv) | 26,211 | Functions, methods, helpers, arguments, locals, members, globals, types, constants and labels |
| [Macros](macros.tsv) | 2,791 | Definitions and their parameter spellings, including inactive conditional branches |
| [Identifier index](identifiers.tsv) | 48,020 | Distinct spelling/file pairs with all occurrence lines; a coverage backstop |
| [Parser gaps](parse-gaps.tsv) | 14 | Distinct errors and the translation units affected |
| [README branches](branches.tsv) | 9 | Separate coverage and eventual propagation tracking |

Declaration counts include prototypes, definitions, unnamed declarations and
macro-generated declarations. They are not counts of distinct symbols or
suspected mistakes. A repeated spelling in different scopes must be reviewed
separately. Shared header declarations observed in several translation units are
deduplicated by source location, kind, name and scope.

The 96 C/C++ sources were parsed using the repository's Clang retail-analysis
arguments (C++98, default Russian locale). Three headers with no declarations
observed through those sources were also parsed directly. All 1,491 source
`VA(...)` function markers appear in the declaration inventory. Unannotated
functions and helpers are included too. This count differs from the compiled
function count because compilation also emits implicit/inline functions.

The snapshot covers this source tree, not every README branch. Other branches
have explicit unreviewed rows in `branches.tsv`; their unique files, scopes and
conditional code still need inventories. The recorded remote-tracking commits
are observations at collection time, not a claim that those branches are current
or contain PR #67. Build tools, third-party `vendor/` code and assets are outside
this first source-name inventory. External APIs, serialized names and strings
must still be consulted where they constrain a game declaration's meaning.

## What to check for every declaration

1. Establish provenance. Record whether the name comes from an actual surviving
   symbol record, an external API, or reconstruction. A `VA`, Ghidra label,
   generated symbol table or exact binary match does not prove an original name.
   Buka's image is stripped. Preserve independently evidenced original function
   names; check reconstructed helpers as well as public functions.
2. Trace the meaning through all definitions, reads, writes and callers. For
   arguments and outputs, follow position and data flow, not the caller's current
   name. For members, check initialization, lifecycle and serialization. For
   constants, check the domain, not just a coincidentally equal numeric value.
3. Check polarity, ownership and units: attacker/defender, source/destination,
   selected/pending, count/index, fraction/value, flags versus state, and AI versus
   human/network behavior. Review ordinary plausible names as well as placeholders.
4. Record an evidence-backed decision in the row. Include source anchors and
   retail RVA/disassembly or caller/data-flow evidence where needed. If a name is
   incorrect, record the replacement and reason for later review. Leave uncertain
   meanings explicit instead of inventing certainty.
5. Check matching aliases and all related declarations/references. An intentional
   retail `#define` can preserve compiler-sensitive spelling; its old spelling is
   not automatically another naming defect. A future rename must preserve
   behavior, ABI and matching and be shown in the rename PR before propagation.

Use these `status` values in the tables:

| Status | Meaning |
|---|---|
| `unreviewed` | No systematic decision yet |
| `verified` | Meaning and current spelling supported by recorded evidence |
| `rename_needed` | Incorrect spelling established; replacement and reason recorded |
| `uncertain` | Review attempted; a specific unresolved question remains |
| `not_applicable` | No independent semantic name to decide, with an explicit reason/reference |

A `-` in decision/evidence cells means no entry yet.
`decision` holds the retain/replace decision; `evidence` holds the supporting
anchors and any later fix/validation commit. Never mark an entire function or
file verified just because one name was checked. The earlier spot checks and
PR #67 changes do not pre-complete this systematic pass.

## Coverage required before checking off a file

- Every declaration and macro row has a supported disposition. No unresolved
  `rename_needed` or `uncertain` item remains. A repeated prototype may reference
  its reviewed definition, but check argument order and spelling consistency.
- Walk the complete source, including every `#if` arm, macro body and nested
  scope. Reconcile the identifier index with the declaration rows. Add explicit
  rows for names missed by parsing; retain declaration/reference distinctions.
- Close relevant parser gaps with evidence that the affected declarations have
  been covered manually or through another valid parse. Record the disposition
  in `parse-gaps.tsv`; a successful build is not proof of AST completeness.
- Verify compatibility aliases, unnamed parameters, header-only helpers and
  generated enum wrappers. Check naming consistency across declarations, uses
  and related owner headers. Record file-level coverage evidence in `files.tsv`.
- Only then set the file's status to `verified` and check its box in `worklist.md`.

The parser reported 14 distinct errors in legacy SDK headers; affected translation
units are listed explicitly. The AST is useful navigation, not an exhaustive
proof. `include/BASE/MiscGraphicsConstants.h`, `include/SOURCE/KB.h`, `include/va.h`,
`src/BASE/BITS.asm` and `src/BASE/TILE.asm` have no observed AST declarations and
require manual coverage. Empty/wrapper headers still need an explicit disposition.

The lexical index retains identifier-like tokens before preprocessing. It
includes keywords, include-path fragments and other noise; it is not a second
symbol table. Comments and string/character literals are excluded. Assembly
syntax, continued macro parameter lists and token-pasted names require manual
source inspection. Index membership never establishes semantics.

## Reading and maintaining the tables

The TSVs are plain text and can be filtered by `path`, `kind`, `scope` or `status`
in a spreadsheet or script. `line`/`column` identify the declaration anchor;
`end_line` helps locate its extent (zero-width unnamed parameter extents use
the anchor line). Nested block scopes carry their source line.
`va` is a source annotation, not name provenance. `name` is the AST spelling;
`anchor_token` is the source token at the expansion location. They may differ for
macro-generated names, anonymous enums and compatibility aliases; inspect both.
`macros.tsv` supplies macro names and parameter lists independently of the AST.

`files.tsv` records the input hash and the parse contexts that observed declarations.
`snapshot.json` records the revision and collection totals. These are a fixed
snapshot, not live completion metrics. Before starting a later review, compare the
current tracked file list and hashes with this snapshot. Add newly introduced
files/declarations; re-anchor moved rows and reopen changed scopes and dependent
callers. Keep evidence for removed/replaced rows instead of silently dropping them.
Do not transfer a verified status based only on an unchanged spelling or line.

After the source review, use `branches.tsv` to inventory branch-specific names and
track approved propagation. Generated branches need regeneration from their proper
parents; independently maintained branches need their own semantic and build
checks. User approval of the rename PR comes before cherry-picking or regeneration
that publishes those renames. No branch is complete merely because a shared name
was reviewed in this snapshot.

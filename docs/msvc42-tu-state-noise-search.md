# Controlled MSVC 4.2 TU-state noise search

MSVC 4.2 can change a later function's allocation/evaluation choices after parsing an
otherwise irrelevant earlier source surface. This makes a bounded TU-state probe useful
after a function is semantically and structurally complete but remains at a compiler-shape
wall. It does **not** make arbitrary dummy C++ declarations acceptable reconstructed source.

The unchanged-source implementation is `scripts/tu_state_noise.py`. Use it only after the target's
semantics, types/layout, frame/slots, CFG, inline boundaries, and external relocations have
been audited. Prefer this disposable search to adding a persistent `OD_STEER` solely to
select one unstable compiler state. Before starting, check the external residual queue and
target-specific matrix so an unchanged state tuple is not searched twice.

```sh
python3 scripts/tu_state_noise.py \
  --source src/BASE/WINMGR.cpp --rva 0xca6d0 --trials 40 --seed 0x484f4d32
```

When the audit leaves several legitimate source spellings to test, use the unified
`scripts/match_variants.py` frontend. Its search is a true product:

```text
reviewed hand choices × (clean + conservative AST shapes) × (clean + TU-state probes)
```

The candidate limit reduces the number of AST source shapes first. Every selected source shape
is still compiled against every requested state; a capped run never silently samples only some
islands for one source idea. Searches also have a 1,200-second wall-clock limit by default, matching
the repository's 20-minute per-function budget. A wall-time stop restores the source and writes an
explicitly incomplete diagnostic census; it is not complete state evidence. `--axes-from` supplies
exact authored choices such as `value++`,
`++value`, and `value += 1`. An option may also contain `extra_edits`, allowing one indivisible
choice to insert an inline helper above the target and replace its call site:

```json
{
  "name": "increment",
  "find": "value++;",
  "options": [
    {"name": "postfix"},
    {"name": "prefix", "replace": "++value;"},
    {"name": "compound", "replace": "value += 1;"},
    {
      "name": "inline_inc",
      "replace": "Inc(value);",
      "extra_edits": [{
        "insert_before": "VA(0x00400000, 0x20)\n",
        "text": "inline void Inc(u8 &value) { value++; }\n\n"
      }]
    }
  ]
}
```

Extra edits use exact unique anchors and may instead specify `find`/`replace` or `insert_after`.
The generator suppresses AST rewrites that overlap a reviewed hand axis. Its automatic control-flow
surface is deliberately narrow: `terminal_return_order` only inverts a terminal
`if (condition) return a; return b;` pair while preserving the branch payloads. Broader early-return
or nesting rewrites require an authored multi-edit option until lifetime, label, and scope safety
can be proved.

The default `forest` trials are deliberately expansive. Their width walks from 10 through
`--max-declarations` (64 by default), then wraps. A width-N forest contains N independently
shaped classes, N typedefs, N function prototypes, and N harmless internal function definitions;
all 4N atoms are shuffled into a recorded permutation. Thus even the smallest default trial has
ten classes and forty independently ordered declaration atoms. The compact single-surface
families remain available through an explicit `--families` selection for isolating a transition
after the broad search discovers it.

The default `--insertion target` isolates declarations immediately before the target metadata.
Use `--insertion top` to place the same disposable block after the TU's leading includes and
preprocessor directives. Top insertion lets the probe state flow through real predecessor parsing
and code generation, which can reach states unavailable to adjacent insertion. Both modes reset
the original logical line at the insertion boundary and preserve the exact authored target suffix.

The tool stops at the first audited exact closure; later generated trials are not compiled. If no
exact closure is found, it reports a target-state census after the compiled sweep. A state is identified by the
target's raw text digest, objdiff function boundary, and ordered relocation stream after replacing
only numeric `$SG`/`$T` compiler-private counters with stable placeholders—not by fuzzy score,
because several distinct objects may share one percentage. Public target identities, relocation
sites/types/addends, and all other spellings remain part of the state. The report also counts the
raw private-label spellings folded into each state. Pass `--state-summary` to preserve a compact
JSON census with one complete reproducible probe body and atom permutation per state while the
sub-100 COFF objects themselves remain disposable. Each state also records the ordered list of
every trial that reached it, so occurrence populations and representative selection can be
reproduced after the temporary objects are removed. This summary is diagnostic evidence only: it
does not update a retained maximum. If one normalized state ever receives multiple fuzzy scores,
the census reports the complete score set instead of silently choosing one.

Treat normalized state cardinality as a diagnostic metric alongside fuzzy score. Record the total
number of states, each state's occurrence count, representative trial, function size, text digest,
ordered-relocation digest, and complete score set. Repeated trials in an already-observed state add
frequency evidence but no new code-shape evidence. Private `$SG`/`$T` counter renumbering must not
inflate the cardinality.

Use `--only-trial N` with the original seed, families, declaration bound, and a `--trials`
generation horizon of at least N to replay one representative without compiling its predecessors.
The generator still advances deterministically through the complete sequence before selecting the
requested index, so the body and permutation are byte-for-byte identical to the census trial.
Repeat `--only-trial` to replay several representatives in one run. This is the preferred way to
re-prove an exact island after intentionally interrupting a long population experiment.

When a sweep is reviewed, compare one reproducible representative of every normalized state both
against retail and against the other states. The console and JSON summary identify the
representative tag, permutation digest, atom count, and complete ordered permutation for every
island. It also stores the representative target bytes, normalized ordered relocation stream, every
retail byte difference, and a pairwise byte-delta matrix across all islands. These are sufficient to
disassemble and compare every target state after the disposable objects are deleted. Attribute each
state transition to concrete instruction or relocation spans; do not inspect only the highest-scoring
state. This distinguishes independent compiler choices that happen to move the aggregate score in
opposite directions. Target-specific matrices under
`docs/matching-matrices/` retain that comparison without retaining disposable source or COFF
objects.

Each baseline and trial compile has a 120-second default ceiling. Override it with a
positive finite `--compile-timeout-seconds`; expiry terminates the complete compiler
process group, including Wine/MSVC descendants, records a rejected timeout trial, and
continues with the next variant. A baseline timeout fails closed. The end-of-run timing
summary and successful manifest separately report compile, target-integrity, objdiff,
COFF-metric, and regression-gate costs.

The default is diagnostic-only. Add `--record-max` only when asking the tool to close an
unchanged target at exact 100.0000%. It never retains a sub-100 improvement. Generated probe
text is never retained in reconstructed source, even when it produces 100%.

An accepted closure contributes to the hash-scoped `functions exact-max` and `fuzzy-max`
metrics. It does not contribute to live `functions exact` or `fuzzy`: those continue to
describe the canonical source under the canonical TU state. This separation makes compiler
instability measurable without pretending that the disposable declarations are reconstructed
program content.

Use `--dry-run` only to inspect generated input without compiling or matching. It is explicitly a
non-matching diagnostic and intentionally retains snippets. Normal compiled mode instead builds in
a temporary run directory. A clean run preserves that directory only after an audited exact closure;
sub-100, interrupted, and fatal runs delete it by default.

The RVA may be image-relative (`0xca6d0`) or an image VA (`0x4ca6d0`), but it must identify
exactly one CodeView function in the configured source TU. The tool recompiles with that TU's
real profile from `config/units.toml` and scores the exact mangled symbol from
`build/gen/symbol_names.csv`.

Every trial is scored through fresh candidate-paired comparison copies: retail relocations are
paired against that trial's raw object, then both sides pass through the repository's reviewed
data-symbol canonicalizer. This is the same semantic normalization pipeline used by the normal
objdiff build. Scoring raw trial objects is invalid because harmless probe declarations renumber
private compiler labels and can otherwise create a false fuzzy loss—or prevent a real exact
closure from being recognized.

## Parser-visible probe classes

The original preprocessor-only sweep was a useful negative control: 120 macro/empty-conditional/
comment/warning variants before `BASE/bmap2::DimBitmapArea` all produced one identical object state.
Comments and whitespace disappear before the C++ parser, and unused macro text is erased by the
preprocessor, so those are not search families.

The default forest permutes all of these VC 4.2 parser-visible surfaces together:

- scalar, pointer, array, const-pointer, and function-pointer typedefs;
- at least ten classes per trial, varying access sections, nested typedefs/enums/unions, arrays,
  bitfields, packed layouts, static/overloaded/virtual declarations, constructors/destructors, and
  simple in-class bodies;
- varied calling conventions, scalar/pointer parameter shapes, and return types in function
  prototypes; and
- harmless internal-linkage function definitions with varied signatures and expression/control
  shapes.

Explicit compact families additionally isolate typedef enums, structs, classes, packed records,
member-bearing classes, extern declarations, initialized static data, prototypes, functions, and
curated includes. The explicit `mixed` family combines the expansive forest with those additional
surfaces.

Every block finishes with a `#line` reset, without a filename, so authored source retains its
existing logical line and logical filename. Identifiers include the seed/trial tag, pack state is
balanced, and include selection is fully recorded. Probe definitions may emit functions or storage
in the disposable candidate object; this is intentional compiler-state input, not reconstructed
program content. The generator never emits calls, volatile accesses, labels, or mutation of an
external object. The regex permuter is never invoked.

Includes are restricted to the tool's audited allowlist. Before compiling an include-bearing
candidate, the tool recursively resolves that header and its transitive includes, collects every
macro definition conservatively, and rejects unresolved headers or any macro name that appears as
an identifier in the canonical target block. This is intentionally stricter than the real include-
guard state: it prevents a header from winning by macro-rewriting target tokens. The manifest stores
the selected headers, transitive count, macro count, conflicts, and canonical identifier digest.

## Acceptance gates and artifacts

The original source is copied into the artifact directory and restored in a `finally` block after
every trial and again at tool exit (including Ctrl-C and handled termination). Candidate objects
are compiled under the artifact directory, so the normal `build/objdiff/base` object is not
overwritten. The target size acceptance gate uses objdiff's CodeView-delimited function size.
The separately logged COFF `size`/text-hash diagnostics span each function symbol start to the
next function symbol, or to the section end, and can therefore include trailing section padding.
The ordered relocation stream records each function-relative offset, relocation type, symbol
identity, and encoded addend. Unknown relocation widths fail exact closure closed.
Because extra probe-only symbols/storage are allowed in the temporary object, whole-object symbol
equality is not an acceptance rule. Instead, each trial fails closed unless the canonical target's
exact authored suffix from its real line-start `VA(...)` marker is unchanged, the target remains
uniquely identifiable to objdiff, and its retail size/score/relocations are extracted. This exact
suffix gate is stronger than recomputing the normalized target hash and avoids a repository-wide
source-hash census for every trial. The canonical normalized hash is still captured once for the
retained-max audit. Target resolution ignores incidental `VA(...)` text in trailing comments and
vtable annotations. Include-bearing trials must additionally pass the allowlist/macro-intersection
guard above. The tool evaluates only the requested target function. It does not inspect or gate on
sibling scores, sibling bytes, or predecessor metrics: the generated declarations and their whole
candidate object are disposable, and no sibling live state or maximum is updated. A target exact
closure requires the unrounded objdiff score to equal numeric `100.0`, the CodeView-delimited size
to equal retail, and the complete ordered relocation offset/type/identity/addend stream to equal
retail. Comparing every target relocation is deliberately stronger than checking only external
references; a local-label/delinker identity caveat remains evidence-only until it is byte-proven
under the matcher guide.

Only a successful exact-100 compiled run preserves `manifest.json`. It records the commit, source
hash, compiler flags, each seed-derived tag and complete generated probe body, every score, target
text/relocation metrics, rejection reasons, and the best observed disposable trial from that
successful run. `trials.tsv` is its compact score/family/rejection log. No generated `.cpp` or patch
is emitted. The preserved exact artifact retains the seed, tag, and complete winning snippet for
audit and reproduction, but never a source patch. Without exact closure, the tool prints only a
concise path-free summary and removes all snippets, manifests, objects, and logs.

`--record-max` is the only optional repository mutation. It runs after byte-for-byte source
restoration and uses the project's normalized `source_hashes()` API. The mode requires exactly
one target row in `config/match_baseline.tsv`, requires its stored hash to equal the current hash,
and never lowers a maximum. Only an exact closure satisfying the size and ordered-
relocation proof above can replace that row's max field with literal `100.0000`; every other byte
and field in the ledger is preserved. Missing/duplicate rows and hash mismatches are refused. Any
sub-100 run—including 99.99 above the stored maximum—leaves the entire baseline byte-identical.

This strict rule prevents a synthetic sub-100 score from becoming a phantom retained maximum that
the canonical checked-in TU never produces. Such a maximum would distort hardest-first queue
priority and teach later agents to fear legitimate structural changes as apparent regressions.
Exact 100 is the sole exception because it proves that the unchanged target source can emit the
retail-sized function with the retail relocation stream under a reproducible controlled TU state
and therefore closes the target.

Never apply or commit generated TU-state noise. Sub-100 artifacts are disposable diagnostics: do
not cite them as a retained maximum or commit them as durable evidence; compiled mode deletes them
at clean exit. Preserve the successful exact-100 manifest as the reproducible closure audit; it
contains the seed and complete probe body regardless of whether `--record-max` was requested.

Related: [msvc42-tu-declaration-state.md](patterns/msvc42-tu-declaration-state.md),
[o2-tu-cumulative-register-steering.md](patterns/o2-tu-cumulative-register-steering.md), and
[tu-cumulative-eval-order.md](patterns/tu-cumulative-eval-order.md).

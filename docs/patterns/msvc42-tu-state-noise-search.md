# Controlled MSVC 4.2 TU-state noise search

MSVC 4.2 can change a later function's allocation/evaluation choices after parsing an
otherwise irrelevant earlier source surface. This makes a bounded TU-state probe useful
after a function is semantically and structurally complete but remains at a compiler-shape
wall. It does **not** make arbitrary dummy C++ declarations acceptable reconstructed source.

Use `scripts/tu_state_noise.py` only in the last-mile phase, after the target's semantics,
types/layout, frame/slots, CFG, inline boundaries, and external relocations have been audited.
Before starting, check the function's `@match-note` and target-specific matrix so an unchanged
state tuple is not searched twice.

```sh
python3 scripts/tu_state_noise.py \
  --source src/BASE/WINMGR.cpp --rva 0xca6d0 --trials 40 --seed 0x484f4d32
```

The default is diagnostic-only. Add `--record-max` only when asking the tool to close an unchanged
target at exact 100.0000%. It never retains a sub-100 improvement. Generated probe text is never
retained in reconstructed source, even when it produces 100%.

Use `--dry-run` only to inspect generated input without compiling or matching. It is explicitly a
non-matching diagnostic and intentionally retains snippets. Normal compiled mode instead builds in
a temporary run directory. A clean run preserves that directory only after an audited exact closure;
sub-100, interrupted, and fatal runs delete it by default.

The RVA may be image-relative (`0xca6d0`) or an image VA (`0x4ca6d0`), but it must identify
exactly one CodeView function in the configured source TU. The tool recompiles with that TU's
real profile from `config/units.toml` and scores the exact mangled symbol from
`build/gen/symbol_names.csv`.

## Parser-visible probe classes

The original preprocessor-only sweep was a useful negative control: 120 macro/empty-conditional/
comment/warning variants before `BASE/bmap2::DimBitmapArea` all produced one identical object state.
Comments and whitespace disappear before the C++ parser, and unused macro text is erased by the
preprocessor, so those are not search families.

The default deterministic families now exercise compiler state that VC 4.2 actually parses:

- plain typedef aliases and typedef enums with varied explicit values/order;
- unused structs, classes, packed records with balanced `#pragma pack(push/pop)`, and classes with
  member declarations plus simple in-class bodies;
- extern/global declarations and internal-linkage initialized data definitions;
- function prototypes and harmless internal-linkage function definitions;
- deterministic selections from a curated VC 4.2-compatible project/standard include list; and
- mixed bundles combining those declaration surfaces.

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
normalized source hash is unchanged, the target remains uniquely identifiable to objdiff, and its
retail size/score/relocations are extracted. Include-bearing trials must additionally pass the
allowlist/macro-intersection guard above. The tool also rejects a candidate from best-candidate selection when:

- any non-target sibling's objdiff score regresses;
- any already-exact sibling's raw text or ordered relocation digest changes;
- an earlier function's raw text or ordered relocation digest changes;
- target code-size distance from retail worsens; or
- target relocation-count distance from retail worsens.

Those gates make a trial useful as a diagnostic. Exact closure is stricter: the unrounded objdiff
score must equal numeric `100.0`, the CodeView-delimited size must equal retail, and the complete
ordered relocation offset/type/identity/addend stream must equal retail. Comparing every relocation
is deliberately stronger than checking only external references; a local-label/delinker identity
caveat remains evidence-only until it is byte-proven under the matcher guide.

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
and never lowers a maximum. Only an eligible exact closure satisfying the size and ordered-
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

Related: [msvc42-tu-declaration-state.md](msvc42-tu-declaration-state.md),
[o2-tu-cumulative-register-steering.md](o2-tu-cumulative-register-steering.md), and
[tu-cumulative-eval-order.md](tu-cumulative-eval-order.md).

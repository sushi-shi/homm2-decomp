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

The default is evidence-only. To retain an eligible improvement as the maximum for the
target's unchanged normalized source hash, add `--record-max`. Generated probe text is never
retained in reconstructed source, even when it produces 100%.

The RVA may be image-relative (`0xca6d0`) or an image VA (`0x4ca6d0`), but it must identify
exactly one CodeView function in the configured source TU. The tool recompiles with that TU's
real profile from `config/units.toml` and scores the exact mangled symbol from
`build/gen/symbol_names.csv`.

## Deliberately narrow probe classes

The generated blocks contain only:

- unused `#define`/`#undef` pairs;
- empty true conditional groups;
- balanced MSVC warning push/disable/pop groups;
- comments and lexical padding;
- deterministic combinations of those groups.

Every block finishes with a `#line` reset, without a filename, so authored source retains its
existing logical line and logical filename. The blocks create no declaration, symbol, storage,
call, volatile access, relocation, or runtime expression. They are inserted before the target's
whole comment/`VA` metadata block and removed immediately after each compiler invocation.

The tool intentionally does **not** generate unused enums, typedefs, structs/classes, member
declarations/bodies, functions, globals, labels, active assertions, pragmas that affect layout or
optimization, dummy/volatile loads, or fake relocation references. Experiments prove that some of
those declarations perturb MSVC, but they also pollute the source model and can change ABI,
promotion, layout, or linkage. Real recovered declarations are valid TU-state triggers; fabricated
ones are not a last-mile technique. The regex permuter is never invoked.

## Acceptance gates and artifacts

The original source is copied into the artifact directory and restored in a `finally` block after
every trial and again at tool exit (including Ctrl-C and handled termination). Candidate objects
are compiled under the artifact directory, so the normal `build/objdiff/base` object is not
overwritten. The target size acceptance gate uses objdiff's CodeView-delimited function size.
The separately logged COFF `size`/text-hash diagnostics span each function symbol start to the
next function symbol, or to the section end, and can therefore include trailing section padding.
The tool rejects a candidate from
best-candidate selection when:

- any non-target sibling's objdiff score regresses;
- any already-exact sibling's raw text or ordered relocation digest changes;
- an earlier function's raw text or ordered relocation digest changes;
- target code-size distance from retail worsens; or
- target relocation-count distance from retail worsens.

`manifest.json` records the commit, source hash, compiler flags, exact snippets, every score,
target text/relocation metrics, rejection reasons, and the eligible best trial. `trials.tsv` is
the compact search log. No generated `.cpp` or patch is emitted. A 100% objdiff score is still
relocation-masked and requires raw-byte and relocation review, but the probe itself remains only
evidence.

`--record-max` is the only optional repository mutation. It runs after byte-for-byte source
restoration and uses the project's normalized `source_hashes()` API. The mode requires exactly
one target row in `config/match_baseline.tsv`, requires its stored hash to equal the current hash,
and never lowers a maximum. Only when the eligible best exceeds the stored maximum does it replace
that row's max field; every other byte and field in the ledger is preserved. Missing/duplicate
rows and hash mismatches are refused, and a neutral search leaves the baseline unchanged. The
manifest records the old maximum, new maximum, and hash.

Never apply or commit generated TU-state noise. Preserve the manifest as the reproducible evidence
for the retained maximum and record its state tuple in the target's durable matrix.

Related: [msvc42-tu-declaration-state.md](msvc42-tu-declaration-state.md),
[o2-tu-cumulative-register-steering.md](o2-tu-cumulative-register-steering.md), and
[tu-cumulative-eval-order.md](tu-cumulative-eval-order.md).

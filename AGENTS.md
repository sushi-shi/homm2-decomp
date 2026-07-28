# Codex Matching Guide

Read `CLAUDE.md` and `.claude/agents/matcher.md` before changing reconstructed code.
For orchestration, also read `.claude/agents/orchestrator.md`. Keep this file durable:
do not add current assignments, queue snapshots, percentages, or next actions.

## Objective

- Recover correct C++ structure, layouts, storage, control flow, and relocations, then
  drive every function and data object to exact comparison where retail evidence permits.
- Continue autonomously while useful work remains. A completed function or batch is a
  checkpoint, not the end of the campaign.
- Integrate verified work linearly on `master`. Matcher lanes may use persistent
  worktrees, but their generated reports are not integration payloads.
- Carry each accepted change through build, byte/relocation review, and a focused commit.

## Queue Policy

- During coverage, prioritize high unmatched-byte weight and drain each chosen TU in
  retail order. Recover all predecessors that can affect cumulative compiler state.
- During the exhaustive residual campaign, keep every live non-100% function in a
  persistent hash-scoped queue. First process historical-exact losses whose current-hash
  maximum is below 100%, then all other live functions at 93% or higher, then the lower
  residuals. Within each tier order by fuzzy percentage descending, then RVA.
- Source comments never remove a live residual from an exhaustive queue. Reproduce all
  evidence from current objects.
- Do not spend unlimited time on compiler shape. Once semantics, frame, CFG, slots, and
  relocations are credible, try the obvious spellings or at most ten non-improving
  last-mile attempts, record the residual, and continue.
- Spend at most 20 minutes total on one function, including inspection, source-shape
  generation, compilation, and island classification. Size permutation matrices to fit.
  A wall-time-truncated matrix is diagnostic only and does not count as complete state coverage.

## Matching Loop

1. Inspect before editing:
   `homm2 sema rva`, `xref --callees`, `strings`, `disasm --diff --lite`, and
   `disasm --blocks --diff --lite`.
   Use cached Ghidra decompilation only when the structure remains unclear.
2. Reconstruct real types, fields, enums, scopes, locals, and inline accessors. Do not
   begin with raw offset arithmetic or compiler steering.
3. Build the TU with `ninja`, refresh `homm2 status`, and advance from the first real
   disassembly divergence. Keep stack displacements visible. Use
   `disasm --blocks --diff` to localize differing block bodies;
   `--blocks --base/--target --lite` for either skeleton; and `--blocks --dot` when
   graph topology is clearer than a listing.
4. Run `homm2 od-frames` to expose `/Od` frame and slot drift, then use
   `scripts/od_slots.py` for individual name-sensitive layouts. For large switches,
   recover body order separately from case values and compare ordered jump-table
   destinations.
5. Audit external targets and owner-relative addends with `homm2 relocs 0x<RVA>`.
   Compare raw `llvm-objdump -r` ranges when a provisional boundary confuses the helper.
6. Use relocation-masked bytes only to isolate code-shape differences. They do not prove
   correctness; ordered relocation identity/addend/destination checks still apply.
7. Do not test small evaluation-order, commutative/relational, parenthesization, identifier-
   spelling, or similar compiler-state-sensitive source changes by editing and compiling them
   one at a time. Enumerate them with `scripts/match_variants.py` and run a bounded complete
   permutation matrix. Manual source edits are reserved for evidence-backed structural recovery
   such as types, fields, scopes, locals, inline accessors, CFG, and switch body order. When a
   targeted source shape is justified at the first real divergence, add it as a reviewed
   permuter axis and test it against the clean state and every requested TU-state probe.
   **Structural versions come first:** build and compare the credible high-level control-flow,
   loop/tail, inline-accessor, and switch-order alternatives before generating internal small
   transformations. Select a retail-compatible semantic CFG family, then permute internal
   spellings within each surviving structural version. Emitted basic-block count, numbering,
   and partition are diagnostic rather than correctness invariants: unchanged source can split
   or merge blocks across legitimate MSVC TU states. Re-run the block view for retained islands,
   use it to classify structural orbits, and require exact bytes plus complete ordered
   relocations for closure. Fuzzy score is not monotonic across structural changes: a lower-
   scoring version with credible semantics, retail size, relocations, and CFG remains an active
   branch seed. Explore evidence-based structural descendants and their bounded permutation
   matrices before pruning it. Prune a branch for contradictory topology or semantics, not
   merely because its parent or first compiler-state census scores below the current MAX.
8. For a structurally aligned compiler-state residual, island search is the current
   last-mile policy. Run `scripts/tu_state_noise.py` for an unchanged-source census. When
   several legitimate source shapes must also be tested, use `scripts/match_variants.py` so
   reviewed exact-span choices, conservative AST variants, and TU-state probes are independent
   Cartesian dimensions. A selected source shape must be compiled against the clean state and
   every requested probe state; never rely on a truncated mixed mutation list. Use the expansive
   declaration-forest default, with at least ten varied classes, typedefs, prototypes, and
   functions per trial. Default to 50 TU-state trials per structural version; expand beyond
   50 only when a near-closing state or unusually sparse state census justifies the extra
   time. Stop at the first audited target-local exact closure. TU-state probes
   may perturb siblings; report those changes as diagnostics, but never reject or withhold the
   target's exact MAX because of them. If no exact state appears, retain both independently
   selected clue pairs under `build/`: best fuzzy score and best block topology. The topology
   rank compares block-count delta, canonical labeled-edge and predecessor deltas, flow-kind
   mismatches, shifted targets, and size-only blocks before using fuzzy score as a tie-breaker.
   Inspect every state's recorded topology rather than assuming the fuzzy winner represents the
   structural orbit.
9. Treat every retained `OD_STEER` expression as migration debt: it manually selects a compiler
   state while contributing no program semantics. Remove it incrementally, search islands for
   the clean unchanged source, and retain no generated probe declarations or functions. The
   recovery loop performs this migration immediately when an exact target itself contains
   `OD_STEER`; it must never remove steering from a predecessor or sibling. A higher observed
   score may update the clean function's hash-scoped maximum after source
   restoration; exact closure additionally requires retail size, exact-100 bytes, and complete
   ordered-relocation identity. The reconstructed source remains free of compiler-state hacks.
10. If the clean-source sweep has no exact island, make one evidence-based targeted source change
   at the first real divergence, then resweep and compare every resulting island. A legitimate
   change can expose a different compiler-state orbit. Never invent a source change merely to
   perturb the compiler. Generated source is never retained; best compiled clue artifacts may
   remain under `build/`, and a higher sub-100 observation may advance hash-scoped MAX.
   Preserve each reviewed source-shape experiment under
   `docs/matching/<function-name>/<attempt-name>.cpp`, with the attempted code, the generated
   matrix/artifact path, and comments recording the result and disposition. One file may describe
   all arms of a single permutation matrix; TU-state probes do not need separate source files
   because they leave the effective function source unchanged.
11. Before committing, run `homm2 build`, the focused relocation review, and
   `git diff --check`. Normal one-unit builds take roughly 4-5 seconds; investigate build
   performance only when it exceeds 10 seconds consistently.

## Tooling Reference

- Enter the build environment from the assigned worktree with `nix develop .#build`.
  Before trusting any result, verify `pwd`, `git branch --show-current`, and
  `readlink -f "$HOMM2_DIR"` all identify that worktree. Keep one persistent shell when
  running many matrices so every compile uses the same environment.
- Locate files and source anchors with `rg`/`rg --files`. Use exact, unique source spans
  for generated axes; `match_variants.py` deliberately rejects ambiguous or overlapping
  edits. Generate JSON manifests with a small reviewed script when quoting a source
  product by hand would be error-prone.
- Start a function dossier with:
  `homm2 sema rva 0x<RVA>`,
  `homm2 sema xref --callees 0x<RVA>`,
  and `homm2 sema strings 0x<RVA>`.
  Use `homm2 sema strings --find <text>` for reverse literal lookup. These are read-only;
  use cached `homm2 ghidra` output only when the retail structure is still ambiguous.
- Use `homm2 sema disasm 0x<RVA> --diff --lite` for the compact instruction diff and
  remove `--lite` when bytes, offsets, stack slots, or relocation sites matter.
  `--base` and `--target` inspect one side. `--rich` interleaves candidate source lines.
  Add `--blocks` for the canonical basic-block view:
  `--blocks --diff --lite` is the compact skeleton,
  `--blocks --diff` shows per-block instruction changes,
  `--blocks --base/--target` shows one complete skeleton, and
  `--blocks --dot` emits Graphviz CFG input.
- Interpret the block report as a multi-objective diagnostic. Track exact graph/edge
  topology, block count, size-only blocks, shifted targets, flow-kind mismatches, and the
  leading structural frontier separately from fuzzy score. A lower-fuzzy island with
  fewer structural mismatches remains a valid descendant. Compiler state can change block
  partition and register allocation nonlocally, so rerun the block census after every
  retained source shape and across state probes.
- Audit relocations with `homm2 relocs 0x<RVA>`. It reports missing/extra owners and
  count differences. Use
  `llvm-objdump -r build/objdiff/base/<unit>.obj` and
  `llvm-objdump -r build/delink/<unit>.c.obj`
  to compare exact source offsets, types, identities, and addends when one repeated owner
  is ambiguous. Equal relocation counts alone are not closure.
- For `/Od` work use `homm2 od-frames` for frame/slot drift and
  `scripts/od_slots.py` for an individual name-sensitive layout. Keep stack
  displacements visible in the non-lite disassembly. Do not apply `/Od` slot steering to
  optimized functions.
- Build through `homm2 build` after retained edits. It configures the affected units,
  compiles raw objects, and refreshes the normalized objdiff copies. If source line
  movement makes function spans stale, first run
  `python3 -m homm2.build.annotated_functions`. A raw `ninja` compile without normalized
  refresh can leave `homm2 sema`/objdiff reading stale objects. Run `homm2 status` before
  quoting live repository totals.
- Describe reviewed source products in a schema-1 exact-span axes file and run them with
  the public permuter, for example:

  ```
  python3 scripts/match_variants.py src/OWNER/File.cpp 0x<RVA> \
    --axes-from build/function-axes.json \
    --min-depth 0 --max-depth 0 --limit <complete-product> \
    --state-trials 50 --state-families forest --state-insertion top \
    -o build/function-manifest.json --run \
    --batch-output build/source-variant-batch/function
  ```

  `--limit` must fit the full exact-axis product times every emitted source/state shape;
  a reported truncation is not coverage. Use `--compile-timeout` and
  `--wall-time-seconds` to bound the complete matrix, not to accept a partial one. The
  results JSON retains separate `best`, `best_topology`, and structural-frontier
  selections. Do not choose only the printed fuzzy winner.
- For an unchanged-source compiler-state census use:

  ```
  python3 scripts/tu_state_noise.py \
    --source src/OWNER/File.cpp --rva 0x<RVA> \
    --trials 50 --families forest --insertion top \
    --state-summary build/function-state-summary.json \
    --output build/tu-state-noise/function --retain-best
  ```

  The tool restores source on success/failure and retains paired best-fuzzy,
  best-topology, and best-frontier objects/assembly/diffs. Use `--seed` for reproducible
  state sets, `--only-trial` for a reviewed replay, and `--layer-state-summary` plus
  `--layer-trial` for a second state layer. Probe declarations are disposable and never
  become source. Use `--record-max` only for an audited exact closure; never edit
  generated MAX/status configuration by hand.
- Read matrix results from `results.json`/`results.csv`, not terminal snippets alone.
  Preserve each source matrix under `docs/matching/<function>/<attempt>.cpp` with code,
  artifact paths, completeness, topology, relocation result, and disposition. Mark
  planned ideas explicitly `UNTRIED`; do not mix them with measured rejections. General
  optimized-code lessons and multi-objective island selection are recorded in
  `docs/msvc42-optimized-nonlocal-islands.md`.
- Before a focused commit run `homm2 build`, `homm2 relocs 0x<RVA>`,
  the appropriate disassembly/block diff, and `git diff --check`. Inspect
  `git status --short`, stage only the declared source/header/docs plus intentional
  root-generated status files, and leave build probes, worktrees, queues, and unrelated
  dirty files unstaged.

## Evidence Rules

- Retail bytes, relocations, and public RVAs are authoritative. Embedded CodeView does
  not prove function lengths, private symbols, TU ownership, or layouts.
- Objdiff fuzzy score is prioritization evidence, not correctness proof. In particular,
  wrong `(%ebp)` displacements and relocation operands can still score highly.
- A `jmp $+0` under `/Od /Ob1` is often an inline-accessor continuation. Reconstruct the
  accessor and expression context before considering it a wall.
- TU-wide compiler state can perturb unchanged siblings. Only current raw bytes and
  relocations decide correctness.
- Retained MAX is historical evidence for unchanged effective function source, not the
  current/live score. Never lower it merely because a live object regresses. Carry it
  through hash-schema or dependency-hash migrations when the function body and all
  tracked codegen dependencies are unchanged; reset it only for a real effective-source
  change. MAX is the durable campaign memory: when a disposable island has exact target
  bytes, retail size, and complete ordered-relocation identity, record 100 for that target
  even if the same TU state perturbs siblings. Recover each sibling through its own search.
- Interpret negative optimized-code experiments narrowly. Byte-identical arms prove only
  that the compiler erased that distinction for the tested parent source and TU state;
  they do not prove which spelling was original. Likewise, a complete flat matrix rejects
  those axes only in that parent orbit. A distant ownership, helper-boundary, declaration-
  order, or compiler-state change can make the same axis observable, so retry it only when
  an evidence-backed structural parent changes.
- An exact CFG, exact block sizes, exact total size, or equal relocation count is still
  partial evidence. A relocation appearing in the wrong block often identifies the wrong
  semantic owner even when the target and total count agree. Compare ordered sites,
  identities, addends, block contents, and bytes before calling an island closed.
- Keep an ordinary source comment only when it records an enduring semantic or codegen fact.
  Do not encode queue state, scores, retained maxima, or completion claims in source comments.
- Never retain generated labels, globals, aliases, padding, or fake code in reconstructed
  source solely to improve a metric. The controlled TU-state probe is disposable compiler
  input; its best object/assembly may remain under `build/` as clue evidence. Its contract is documented in
  `docs/msvc42-tu-state-noise-search.md`.

## Source Conventions

- For a family of closely related functions, recover and preserve a consistent
  semantic phase structure, control-flow ownership, and narrow inline-helper
  boundaries. Keep real dialect differences explicit. Do not replace copied
  retail-family implementations with shared macros merely to remove source
  duplication, especially when doing so loses established byte or CFG islands.
- Class data members start with `m_`; plain struct fields need not.
- Replace `fieldN` placeholders once their meaning is known. Use real tagged layouts when
  one serialized record has multiple forms.
- C-style casts are forbidden. Prefer reconstructed types; otherwise use the appropriate
  C++ cast explicitly.
- Prefer repository naming over secondary projects. Use named constants instead of magic
  numbers.
- Use the fixed-width aliases from `Ints.h` throughout reconstructed game code: `i8`/`u8`
  through `i64`/`u64`. Plain `char` remains the text character type. Reserve `i32l`/`u32l`
  for proven retail `long` declarations whose distinct C++ type identity affects mangling or
  overload resolution, and retain SDK aliases such as `DWORD`, `WPARAM`, and `U32` at external
  ABI boundaries. The fixed-width build assertion enforces this scope.
- Put type declarations and shared enum domains in headers. A `typedef enum` used by
  exactly one TU is private and lives in that `.cpp`. For known serialized numeric
  domains, prefer a `typedef enum` with explicit values while preserving storage width
  and packed layout.
- Preserve proven layouts with packed records and `SIZE` evidence. The retail MSVC build
  keeps `SIZE` byte-neutral; do not turn it into emitted declarations.
- Use inline accessors where retail `/Ob1` traces prove them. Do not replace modeled fields
  with `reinterpret_cast<unsigned char *>(this)[offset]` merely for a local score gain.
- In the optimized icon-decoder family, begin from the
  `FlipIconToBitmapYModify` structural idiom: semantic file-static decoder
  state and comparable declaration order, direct compact cursor operations,
  few incidental state-copying locals, shallow early-exit/continue flow, and a
  scoped `do { ... } while (0)` candidate for small two-join goto clusters that
  plausibly came from a multiline macro. Preserve normal semantic gotos and
  treat every family resemblance as a hypothesis requiring byte, relocation,
  and CFG evidence. Enumerate uncertain declaration orders rather than editing
  them manually one by one.
- Do not search for unavailable original source. Secondary references are for
  naming/semantic guidance only; adapt useful names to this repository.

## Data And Linking

- Candidate objects supply reconstructed COFF symbol spelling and topology; retail PE,
  NB09 contributions, bytes, and relocations supply placement evidence. Vostok emits the
  reviewed model; it does not discover private identities automatically.
- The canonical data manifest is generated from source `DATA(...)`,
  `DATA_COMPGEN(...)`, `VTBL(...)`, and `VTBL2(...)` annotations. Candidate COFF
  supplies physical topology; source supplies semantic identity. Missing or ambiguous
  private placement warns normally and fails strict assembly instead of falling back to
  a synthetic name or a second hand-maintained ledger.
- Preserve storage class and section alignment. Padding is not a symbol. Do not insert
  giant arrays to reproduce final-image gaps, and do not classify `.data`/`.bss` solely
  from the PE raw-size boundary.
- Compare relocation source site, target identity, and owner-relative addend. Never hide
  an interior-field mismatch by aliasing overlapping storage.
- Anonymous `$SG`, `$T`, and compiler-counter names are normalized only in disposable
  comparison copies. Never link or derive layout from those normalized objects.

Detailed contracts live in `docs/delinker-contribution-manifest.md`,
`docs/data-symbol-normalization.md`, `docs/coff-data-relocations.md`, and
`docs/static-storage-link-audit.md`. The compiler-generated data contract lives in
`docs/candidate-data-topology.md`.

## Orchestration

- Every assignment names the absolute worktree and expected branch. Before editing, a
  worker verifies `pwd` and `git branch --show-current`; before compiling, it enters
  `nix develop` only after changing to that worktree and confirms `HOMM2_DIR` resolves
  there. Evidence from a sibling worktree or a shell opened in the wrong directory is
  invalid for integration.
- Keep one TU per active lane. Workers may change shared owner headers when correctness
  requires it; the orchestrator resolves overlap and integrates one result at a time.
- Reuse a worker after substantive verified work, especially on the same TU. Return
  deferred work to that worker. Replace comments-only or stub-only lanes.
- Refill completed lanes promptly. Do not poll active lanes repeatedly; wait roughly
  five to six minutes between unsolicited checks.
- Stage only declared source/header changes plus root-generated status files. Never stage
  `.claude/worktrees/`, worker queues, or unrelated dirty files.

## Git Discipline

- Never revert user or concurrent-agent changes.
- Commit focused units of work with messages such as
  `match: reconstruct EVENTS EraseObj` or `tools: verify relocation addends`.

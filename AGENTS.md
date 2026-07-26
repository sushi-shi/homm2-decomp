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
   relocations for closure.
8. For a structurally aligned compiler-state residual, island search is the current
   last-mile policy. Run `scripts/tu_state_noise.py` for an unchanged-source census. When
   several legitimate source shapes must also be tested, use `scripts/match_variants.py` so
   reviewed exact-span choices, conservative AST variants, and TU-state probes are independent
   Cartesian dimensions. A selected source shape must be compiled against the clean state and
   every requested probe state; never rely on a truncated mixed mutation list. Use the expansive
   declaration-forest default, with at least ten varied classes, typedefs, prototypes, and
   functions per trial. Stop at the first audited target-local exact closure. TU-state probes
   may perturb siblings; report those changes as diagnostics, but never reject or withhold the
   target's exact MAX because of them. If no exact state appears, retain only the best paired
   object, disassembly, and diff under `build/` as clue evidence.
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
11. Before committing, run `homm2 build`, the focused relocation review, and
   `git diff --check`. Normal one-unit builds take roughly 4-5 seconds; investigate build
   performance only when it exceeds 10 seconds consistently.

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
- Keep an ordinary source comment only when it records an enduring semantic or codegen fact.
  Do not encode queue state, scores, retained maxima, or completion claims in source comments.
- Never retain generated labels, globals, aliases, padding, or fake code in reconstructed
  source solely to improve a metric. The controlled TU-state probe is disposable compiler
  input; its best object/assembly may remain under `build/` as clue evidence. Its contract is documented in
  `docs/msvc42-tu-state-noise-search.md`.

## Source Conventions

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
- Do not search for unavailable original source. Secondary references are for
  naming/semantic guidance only; adapt useful names to this repository.

## Data And Linking

- Candidate objects supply reconstructed COFF symbol spelling and topology; retail PE,
  NB09 contributions, bytes, and relocations supply placement evidence. Vostok emits the
  reviewed model; it does not discover private identities automatically.
- The canonical data manifest is generated entirely from source `DATA(...)`,
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

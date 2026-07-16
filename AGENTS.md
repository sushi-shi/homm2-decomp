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
  worktrees, but their generated reports and baseline files are not integration payloads.
- Carry each accepted change through build, byte/relocation review, and a focused commit.

## Queue Policy

- During coverage, prioritize high unmatched-byte weight and drain each chosen TU in
  retail order. Recover all predecessors that can affect cumulative compiler state.
- During the exhaustive residual campaign, queue every live non-100% function by fuzzy
  percentage descending, then RVA. Recheck exact-max/live-nonexact functions under the
  current target, data, and verifier epoch.
- A prior `@semantic`, `@early-stop`, or retained maximum does not remove a live residual
  from an exhaustive queue. Reproduce its evidence from current objects.
- Do not spend unlimited time on compiler shape. Once semantics, frame, CFG, slots, and
  relocations are credible, try the obvious spellings or at most ten non-improving
  last-mile attempts, record the residual, and continue.

## Matching Loop

1. Inspect before editing:
   `homm2 sema rva`, `xref --callees`, `strings`, and `disasm --diff --lite`.
   Use cached Ghidra decompilation only when the structure remains unclear.
2. Reconstruct real types, fields, enums, scopes, locals, and inline accessors. Do not
   begin with raw offset arithmetic or compiler steering.
3. Build the TU with `ninja`, refresh `homm2 status`, and advance from the first real
   disassembly divergence. Keep stack displacements visible.
4. For `/Od` slot mismatches, use `scripts/od_slots.py`. For large switches, recover
   body order separately from case values and compare ordered jump-table destinations.
5. Audit external targets and owner-relative addends with `homm2 relocs 0x<RVA>`.
   Compare raw `llvm-objdump -r` ranges when a provisional boundary confuses the helper.
6. Use relocation-masked bytes only to isolate code-shape differences. They do not prove
   correctness; ordered relocation identity/addend/destination checks still apply.
7. On structurally aligned near-exact functions, use `scripts/match_variants.py` for a
   shallow, bounded exact-only search. Inspect every retained mutation. Never retain a
   sub-100 probe or record its score as a canonical maximum.
8. Before committing, run `homm2 build`, the focused relocation review, and
   `git diff --check`. Normal one-unit builds take roughly 4-5 seconds; investigate build
   performance only when it exceeds 10 seconds consistently.

## Evidence Rules

- Retail bytes, relocations, and public RVAs are authoritative. Embedded CodeView does
  not prove function lengths, private symbols, TU ownership, or layouts.
- Objdiff fuzzy score is prioritization evidence, not correctness proof. In particular,
  wrong `(%ebp)` displacements and relocation operands can still score highly.
- A `jmp $+0` under `/Od /Ob1` is often an inline-accessor continuation. Reconstruct the
  accessor and expression context before considering it a wall.
- TU-wide compiler state can perturb unchanged siblings. Retained maxima preserve prior
  evidence, but raw current bytes and relocations decide correctness.
- `@early-stop` requires the exact byte span and a permitted, reproducible artifact.
  `@semantic` requires complete semantics plus the first residual, relocation state, and
  attempted spellings. Neither marker may conceal incomplete structure.
- Never invent source objects, labels, globals, aliases, padding, or fake code solely to
  improve a metric.

## Source Conventions

- Class data members start with `m_`; plain struct fields need not.
- Replace `fieldN` placeholders once their meaning is known. Use real tagged layouts when
  one serialized record has multiple forms.
- C-style casts are forbidden. Prefer reconstructed types; otherwise use the appropriate
  C++ cast explicitly.
- Prefer repository naming over secondary projects. Use named constants instead of magic
  numbers.
- Put type and enum declarations in headers. For known serialized numeric domains, prefer
  a header-level `typedef enum` with explicit values while preserving storage width and
  packed layout.
- Preserve proven layouts with packed records and `SIZE` evidence. The retail MSVC build
  keeps `SIZE` byte-neutral; do not turn it into emitted declarations.
- Use inline accessors where retail `/Ob1` traces prove them. Do not replace modeled fields
  with `reinterpret_cast<unsigned char *>(this)[offset]` merely for a local score gain.
- Do not search for unavailable original source. Secondary projects are
  secondary naming/semantic references only; adapt useful names to this repository.

## Data And Linking

- Candidate objects supply reconstructed COFF symbol spelling and topology; retail PE,
  NB09 contributions, bytes, and relocations supply placement evidence. Vostok emits the
  reviewed model; it does not discover private identities automatically.
- The canonical data manifest is the conflict-free union of generated source `DATA(...)`
  definitions and reviewed `config/delink_data_supplemental.tsv` rows. Skip unverified
  private placement instead of guessing or falling back to synthetic names.
- Preserve storage class and section alignment. Padding is not a symbol. Do not insert
  giant arrays to reproduce final-image gaps, and do not classify `.data`/`.bss` solely
  from the PE raw-size boundary.
- Compare relocation source site, target identity, and owner-relative addend. Never hide
  an interior-field mismatch by aliasing overlapping storage.
- Anonymous `$SG`, `$T`, and compiler-counter names are normalized only in disposable
  comparison copies. Never link or derive layout from those normalized objects.

Detailed contracts live in `docs/delinker-contribution-manifest.md`,
`docs/data-symbol-normalization.md`, `docs/coff-data-relocations.md`, and
`docs/static-storage-link-audit.md`.

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

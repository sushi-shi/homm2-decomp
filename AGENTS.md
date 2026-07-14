# Codex Matching Guide

Read `CLAUDE.md` and `.claude/agents/matcher.md` before changing code. They are
authoritative. This file is the short, restart-ready Codex workflow.

## Objective

- Work on campaign branch `source-matching`, created from the latest integrated `master`.
- Reconstruct and byte-match functions in `SOURCE`, hardest/largest unresolved functions first.
- Continue the matching campaign autonomously for as long as useful work remains. Finishing one
  function or batch is a checkpoint, not a reason to stop; rebuild the queue and continue.
- Carry each change through implementation, build verification, relocation review, and a focused
  commit. Do not leave completed work uncommitted.
- Legacy `.claude/worktrees/matcher-1` through `matcher-4` content is unrelated and must not be
  staged or changed; the persistent `source-matcher-1` through `source-matcher-4` pool is managed
  by the orchestration campaign below.
- Keep this guide durable and restart-ready. Do not add transient progress, current assignments,
  queue snapshots, or "next actions"; reconstruct those from the report and repository state.

## Matching Workflow

1. Build a queue from `build/objdiff/report.json`. Select the largest 20% of SOURCE functions by
   retail byte size, then prioritize the lowest size-weighted match percentage within that set;
   this is where matching work usually recovers the most bytes. Parse `size` numerically, never
   lexically, and recompute the cutoff whenever the report changes materially.
2. For every selected function, add all functions preceding it in the same translation unit to the
   queue and match them in source order. These predecessors can control cumulative compiler and
   register-allocation state even when they are smaller or already have a high fuzzy score.
3. Inspect the target before writing source:
   - `homm2 sema rva 0x<RVA>`
   - `homm2 sema xref --callees 0x<RVA>`
   - `homm2 sema strings 0x<RVA>`
   - `homm2 sema disasm 0x<RVA> --lite`
   - `python3 -m homm2.analysis.decomp 0x<RVA>` when semantic structure is unclear. The documented
     `homm2 sema decomp` command is currently not registered, but the module works with the cached
     Ghidra project and must be run without reanalysis.
4. Reconstruct real types, fields, enums, and inline accessors before using pointer arithmetic.
5. Compile rapidly with `ninja` while iterating. Run `homm2 status` before trusting
   `homm2 sema match`, because a bare `ninja` leaves `report.json` stale.
6. Use `homm2 sema disasm 0x<RVA> --diff --lite` to advance from the first structural divergence.
   The campaign has two global phases. Until **every SOURCE function has a reconstructed body**,
   recover complete bodies, real types/layouts, frames, stack slots, CFG, inline accessors, and
   relocation targets across the whole program.
   At a compiler-shape wall, try only a few obvious semantics-preserving spellings, record the
   byte-level residual and exhausted attempts in a durable source marker, and move on. Use
   `@early-stop` only for a byte-proven permitted wall; otherwise use `@match-note` for a complete
   coverage-phase structural checkpoint. A function is ready for integration when its recovered
   structure is credible and all external relocations match, even if a documented compiler-shape
   residual keeps it below 100%. Do not run extended compile searches or permutation passes:
   later shared-header/layout recovery can invalidate carefully tuned matches. Only after the
   syntactic-placeholder census reaches zero, start the last-mile phase and use
   `scripts/permute_ast.py` on structurally aligned 96-99% functions.
   For a remaining compiler/TU-state wall after those gates, use
   `scripts/tu_state_noise.py` as a bounded diagnostic. It inserts only reversible
   preprocessor/lexical probes, compiles outside the normal object path, and rejects sibling,
   exact-function, predecessor, size, and relocation regressions. Generated noise is never
   applied or committed. Audit the generated manifest and raw bytes; the optional guarded
   `--record-max` mode may retain only the eligible score for the unchanged source hash.
   Sweep every SOURCE TU in that phase. The campaign is complete only when every function's
   retained fuzzy maximum is 100%, and therefore every TU's fuzzy-max is 100%. If a byte-proven
   delinker artifact prevents the raw metric from reaching 100%, normalize or explicitly bless
   that proven artifact in the comparison/baseline instead of distorting correct source.
   Its ranges are syntax-aware, but audit every retained mutation. Inequality +/-1 rewrites are
   disabled because they are invalid for floats and can cross integer overflow boundaries. Do not
   use the regex permuter unless every retained mutation receives the same audit.
7. Run a relocation-masked raw-byte comparison for near-exact functions. objdiff masks relocation
   bytes and can report less than 100% for delinked local-label identity even when every code byte is
   identical.
8. Audit relocation targets with `homm2 relocs 0x<RVA>`. If the helper misidentifies a delinked
   boundary, compare `llvm-objdump -r` entries manually over the function ranges. Jump-table local
   labels may be delinked as the containing function; external globals and callees must agree.
9. Run the full `homm2 build` and `git diff --check` before committing. A one-unit full build is
   about 4-5 seconds; do not optimize the build unless it exceeds 10 seconds in real shell time.
10. For large switch functions, recover case-body order independently from case values. MSVC often
    emits bodies in source order, so use retail call/relocation order to reorder blocks, coalesce
    empty case labels, and place shared bodies before changing expressions inside them.
11. When a delinked local symbol truncates `llvm-objdump --disassemble-symbols`, disassemble the
    explicit object-address range instead. Read jump-table offsets and sizes from
    `build/gen/jump_tables.csv`, exclude those data ranges, and compare the remaining instruction
    streams. Normalize only branch destinations and relocation identities, never operands or stack
    displacements.

## Orchestration Campaign

- When the active goal is campaign orchestration, the root agent does not match functions itself.
  It builds the weighted queue, dispatches matcher workers, reviews and integrates their diffs,
  verifies master, blesses the baseline, commits, and immediately refills the freed lane.
- Reuse four persistent worktrees with independent incremental `build/` and Wine state. The active
  SOURCE pool is `.claude/worktrees/source-matcher-1` through `source-matcher-4`; reset a lane to the
  latest `source-matching` only after its prior result has been integrated. Do not modify or reset the
  older dirty `matcher-1` through `matcher-4` BASE worktrees.
- Keep each translation unit in one lane and never run workers on the same source file. Assign
  bounded retail-order batches, including required predecessors, and keep the lane on that TU until
  it is complete.
- Workers may modify any necessary owner or shared header even when another lane also touches it;
  they must not avoid a correct type/layout reconstruction to reduce overlap. The orchestrator owns
  semantic conflict resolution, combined-layout review, serial application, and rebuild validation.
- For a hardest-first campaign, rank SOURCE work by unmatched weighted bytes using the retained
  source-hash maximum, not a transient live dip. Prefer `/Od` units until they are drained; start
  `/O2` units when only optimizer/register-allocation work remains.
- While a substantial syntactic-stub backlog remains, dedicate one persistent lane to untouched
  bodies. Start with the highest-weighted stub-heavy TU and drain it in source order, including
  required predecessors, before moving that lane to the next stub-heavy TU. A stub lane changes
  priority only: it must still recover complete semantics, types, layouts, frames, CFG, and relocs.
- Until every SOURCE function has a reconstructed body, prioritize structural recovery over wall
  grinding.
  A semantically complete function at a proven compiler wall leaves the active queue after only a
  few obvious steering attempts, even if it is 96-99%. Systematic wall breaking and AST permutation
  begin only after the placeholder census reaches zero. Total SOURCE fuzzy is a progress signal,
  not the phase-switch trigger. This prevents later shared-header and class-layout discoveries from
  invalidating carefully crafted byte matches.
- Every integrated non-100 function must carry a durable marker immediately above its `VA()`.
  `@early-stop` is the byte-proven artifact set. `@match-note` is a provisional coverage-phase
  checkpoint:
  record the first retail/ours assembly divergence or byte span, frame/slot/CFG and relocation
  status, the obvious spellings already tried, and the condition for revisiting it. During coverage,
  do not repeat attempts listed in `@match-note`; after all placeholders are gone, build the
  last-mile queue from those notes.
  Remove a stale `@match-note` when the function reaches 100% or gains a valid `@early-stop` proof.
- Integrate exactly one worker at a time. Apply only its declared source/header files; never copy a
  worker's `README.md`, `config/match_baseline.tsv`, or transient queue. On `source-matching`, run the
  full build, verify the reported functions, run `homm2 status update`, stage the worker files plus
  the root-generated README/baseline, and create one linear `match:` commit.
- Refill a lane immediately after serial integration. Reuse a worker with `followup_task` when its
  prior batch produced substantive, verified matching code, preferably continuing the same TU. If
  its handoff was comments-only, stub-only, or otherwise lacked real matching progress, reject that
  result and assign the lane to a fresh matcher agent instead.
- A worker does not escape deferred targets. Return every unfinished or improperly parked function
  to that same worker and lane before assigning new work. During coverage, advance only after each
  target
  is structurally complete and is either 100%, a byte-proven permitted `@early-stop`, or a fully
  evidenced `@match-note` checkpoint under `.claude/agents/matcher.md`; a note cannot excuse missing
  semantics, layout, frame/slot, CFG, or relocation work.
- Do not repeatedly poll worker worktrees, reports, or percentages while a batch is active. Rely on
  matcher milestone/completion messages and let workers tune uninterrupted. Inspect a lane only for
  serial integration, a reported blocker/coordination need, or after a genuinely long silent period
  that suggests the worker or build has failed. Use waits on the order of 5-6 minutes between
  unsolicited checks, not frequent sub-minute polling cycles.

## Compiler Constraints

- Retail is MSVC 4.2 with `/Od /Ob1`. Inlining is enabled even though optimization is disabled.
- A `jmp $+0` is commonly an inlined accessor continuation, not an unbreakable compiler wall.
  Reconstruct the accessor and its expression context. These traces should not simply be accepted.
- Local stack positions are controlled by MSVC identifier hashes. Use `scripts/od_slots.py` and
  semantic suffixes to place locals in increasing bucket order. Do not brute-force random names.
- Arrays/aggregates can explain unused four-byte words. Example: `EraseObj` needed two `int[2]`
  coordinate temporaries; scalar or struct-member spellings changed later inline code generation.
- Source condition polarity controls `/Od` block layout. Match the retail positive arm and `else`
  order directly; logically equivalent negation often reverses compare operands and introduces a
  different trampoline pattern.
- Local names only order allocations within their lexical scope. If a target slot appears missing,
  inspect the retail decompile/local-use sites for a real initialization or a local declared in a
  wider scope before adding padding. Moving the real local to its original scope can shift all
  later slots while preserving the frame size.
- TU-cumulative compiler state can move fuzzy scores after an unrelated type/source addition. The
  source-hash max model intentionally preserves prior maxima. Always use raw bytes for final proof.
- When a later change lowers the live fuzzy score of an already matched function, keep its retained
  source-hash maximum and continue forward. Do not spend time restoring the live percentage unless
  raw-byte or relocation evidence proves that the function itself actually regressed.
- When an intentional shared type/layout edit changes an affected sibling's normalized source hash,
  run `homm2 status update --accept-regressions` so the new hash inherits the retained maximum.
  Use plain `homm2 status update` for direct function edits that should establish a new maximum.
- Mark `@early-stop` only for 100% matches or a byte-proven residual such as delinked local-label or
  constant-pool naming. Document the exact byte span and reason.
- Mark a structurally complete but not yet byte-proven coverage-phase wall with `@match-note`,
  immediately above `VA()`. Record the first differing instructions/offsets, confirmed frame/slot/CFG and
  relocation state, and every obvious steering attempt already exhausted. This is a restart
  checkpoint, not an early-stop proof and not permission to leave reconstruction work unfinished.
- Before the placeholder census reaches zero, a wall proof requires complete
  semantics/frame/slots/CFG and relocation review, but only a few obvious source-shape attempts.
  Do not exhaustively permute or brute-force it.
- A jump-only early stop must be proved after excluding known jump-table data: every non-jump
  opcode and operand must match, the remaining size delta must equal the counted five-byte
  continuation/trampoline jumps, and external relocation targets must be audited manually when
  `homm2 relocs` stops at a delinked local boundary.

## Source Conventions

- Every class data member introduced or refined here must start with `m_`. Struct fields do not need
  the prefix.
- Do not preserve offset placeholders such as `field4` once their role is understood. Recover
  semantic member names, and use genuine tagged views when one physical record has distinct event
  layouts; do not retain anonymous compatibility aliases merely to avoid updating callers.
- Do not use C-style casts. Use a real reconstructed layout first. Where a cast is genuinely what
  the source requires, use `static_cast`, `reinterpret_cast`, or `const_cast` as appropriate.
- Prefer this repository's naming conventions over names from other projects.
- Use named constants instead of unexplained numeric values. Typedef enums are ABI/byte neutral in
  suitable expressions, but MSVC's TU-cumulative behavior can still perturb later code; verify.
- For serialized or deserialized numeric domains with known retail values, prefer a header-level
  `typedef enum` with explicit values over scattered integer constants. Preserve the proven storage
  width and packed layout; do not change a field's representation merely to introduce the enum.
- Type and enum declarations belong in headers, never locally in a translation unit.
- Preserve layout with packed records and `SIZE` evidence where available. Do not invent classes,
  functions, globals, or labels solely to improve objdiff.
- Use inline accessors when retail emits their `/Ob1` continuation traces. Do not replace them with
  raw offset arithmetic just because the arithmetic is equivalent.
- The original source code is not available. Do not search the web or other repositories for it;
  reconstruct from retail disassembly, CodeView, data bytes, relocations, and local project evidence.
- Secondary projects may be used only as
  secondary naming references for enums, object IDs, and serialization formats. Retail disassembly,
  CodeView, data bytes, and relocations are authoritative. Adapt useful names to this repo's style;
  do not copy its implementation structure.
- Project Ironfist is available read-only at `/path/to/homm2/project-ironfist`. It is
  not original NWC source: `src/raw_decompiled/` is Hex-Rays output,
  `src/sergei_reversing_dump/` is a community manual-reversing corpus, and `src/cpp/shared/`
  contains cleaned but potentially mod-altered code. Use it only as secondary evidence for
  semantics, source-shaped loops, constants, enums, field names, and related-version behavior.
  Check its commit and working-tree state when provenance matters; never copy decompiler
  temporaries, raw pointer arithmetic, guessed signatures, or mod behavior. Retail evidence
  remains authoritative, and this repository's naming conventions still take precedence.

## Static Data And Link Resolution

- Do not use objdiff's aggregate `matched_data` or `matched_data_percent` as a static-data coverage
  metric. The delinker fragments, duplicates, synthesizes, and zero-fills per-TU data sections, so
  its target objects are not faithful byte-for-byte images of retail `.data`/`.rdata`. Matching
  those synthetic sections globally is neither meaningful nor a campaign objective.
- Unresolved data symbols are still actionable link and layout work. CodeView-backed globals must
  be declared in their canonical owner header and defined in the owning TU with the proven type.
  Recover retail initializers and pointer relocations when runtime semantics require them; compare
  those bytes against the original PE at the authoritative VA, not against a synthetic delinked
  data section.
- Never resolve an interior alias by emitting overlapping independent storage. Replace aliases into
  `gConfig`, monster tables, formation/elevation records, or other known objects with real member or
  table access. Refine the owning packed layout when necessary and then remove the synthetic model
  declaration.
- Keep unresolved-symbol cleanup separate from data-byte matching. Its proof is: no unresolved
  reconstruction-object reference, correct owner/address/layout, correct call-site semantics, and
  a passing full build/relocation audit. It does not require improving objdiff's data percentage.
- `DATA(<VA>)` is audit metadata and does not pin a linker address. Model the retail storage class
  in the definition: a value present in initialized storage needs the corresponding initializer
  (including an explicit zero when retail emitted it), while true loader-zeroed storage remains an
  uninitialized definition. Verify what MSVC 4.2 actually emits rather than assuming modern
  compiler behavior.
- One TU contributes independently to initialized `.data` and zero-fill `.bss`; the linker combines
  each contribution with the corresponding contributions from every other object. Two globals
  owned by the same TU can therefore be hundreds of kilobytes apart in the final image. Never add
  a giant padding object inside a TU to reproduce that final-image gap.
- Exact final RVAs require the retail object/library order, per-object section sizes and alignment,
  storage-class membership, and linker options. Validate the eventual link with a map/symbol-RVA
  comparison. If natural MSVC 4.2 contributions cannot reproduce the order, use generated ordered
  COFF section contributions as a final-link mechanism rather than contaminating reconstructed
  source layouts with address-sized padding arrays.
- Delinked target objects synthesize data sections and may duplicate a symbol once per reference;
  their section membership is not evidence for original `.data` versus `.bss`. Use the retail PE's
  raw/virtual section extents, stored bytes, CodeView addresses, and neighboring symbols instead.

## CodeView Function Boundaries

- Treat the original NB09 per-module `sstAlignSym` procedure records as the authoritative function
  inventory and length source. Parse both `S_GPROC32` and `S_LPROC32`, preserving their start,
  `pLen`, name, and compiland ownership; also retain `S_THUNK32` records as distinct code ranges.
- `S_PUB32` is a public alias or fallback only. It must never replace a matching procedure record's
  start or length. Computing a function length as the distance to the next public symbol causes
  public functions to absorb private helpers and creates artificial objdiff/delinker walls.
- After all CodeView procedures and thunks are materialized, audit uncovered `.text` ranges. Carve
  a truly CodeView-absent function only with executable entry evidence such as a direct-call target
  or a validated thunk/disassembly boundary. Never carve alignment padding, jump tables, or embedded
  data as functions; record deterministic synthetic names and provenance for every such range.

## Git Discipline

- Never revert user changes or stage `.claude/worktrees/`.
- Stage only the files belonging to the current matched function/type refinement.
- Use focused commit messages such as `match: reconstruct EVENTS EraseObj`.

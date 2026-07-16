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
   `@early-stop` only for a byte-proven permitted wall; otherwise use `@semantic` for a complete
   semantic/structural checkpoint. A function is ready for integration when its recovered
   structure is credible and all external relocations match, even if a documented compiler-shape
   residual keeps it below 100%. Do not run extended compile searches or permutation passes:
   later shared-header/layout recovery can invalidate carefully tuned matches. Only after the
   syntactic-placeholder census reaches zero, start the last-mile phase and use
   `scripts/match_variants.py` on structurally aligned 96-99% functions. It is the exact-only
   public frontend for hardened libclang AST mutations, parser-visible TU-state variants, and
   optional hand-authored exact-span axes. Keep searches shallow and bounded, use repeatable
   `--require-mutation` filters for the specific residual, and inspect every retained candidate;
   syntax-aware mutations are not automatically semantically valid. The family-balanced enumerator
   prevents one axis from consuming the finite candidate budget. `scripts/permute_ast.py` is only a
   legacy compatibility wrapper, while standalone `scripts/tu_state_noise.py` remains a bounded
   diagnostic for a specifically suspected TU-state wall.
   If modern libclang rejects a reviewed MSVC 4.2 construct in another function, use the repeatable
   `--allow-external-diagnostic` exact-substring option. It cannot allow target-local, fatal, foreign-
   file, or unmatched diagnostics, and every allowance is recorded in the generated manifest.
   TU-state trials temporarily insert deterministic parser-visible declarations, harmless
   definitions, or curated includes and compile outside the normal object path. Probe-only
   symbols/storage may exist in that disposable object, but the canonical target hash, identity,
   score/relocation extraction, siblings, predecessors, and size/relocation distance are guarded.
   Curated include probes also fail closed on unresolved headers or any supplied macro name
   referenced by the canonical target block. Generated probes are never applied or committed.
   Sub-100 results are disposable diagnostics and must never enter the retained maximum: they are
   phantom scores not produced by the canonical checked-in TU and would distort queue priority and
   make later structural edits look like regressions. Guarded `--record-max` may set only the target
   row to 100.0000, and only for an unrounded exact 100 with exact CodeView size and a complete
   ordered relocation offset/type/identity/addend match under the unchanged source hash. Otherwise
   it leaves the baseline byte-identical. A clean compiled run deletes its entire temporary output
   unless such an audited exact closure exists; interruption and fatal exits also clean by default.
   Exact closure preserves the seed/tag/body manifest for reproduction regardless of `--record-max`.
   Explicit `--dry-run` is the only non-matching mode that intentionally retains generated snippets
   for inspection.
   Sweep every SOURCE TU in that phase. The campaign is complete only when every function's
   retained fuzzy maximum is 100%, and therefore every TU's fuzzy-max is 100%. If a byte-proven
   delinker artifact prevents the raw metric from reaching 100%, normalize or explicitly bless
   that proven artifact in the comparison/baseline instead of distorting correct source.
   Its ranges are syntax-aware, but audit every retained mutation. Inequality +/-1 rewrites are
   disabled because they are invalid for floats and can cross integer overflow boundaries. Do not
   use the regex permuter unless every retained mutation receives the same audit.
   Use `generate_ast_variants.py` plus `batch_source_variants.py` only after semantics, CFG, frame,
   slots, and relocations align at roughly 96-97% or better. Select a small bounded family/depth,
   inspect every emitted `find`/`replace` edit for value and lifetime preservation, then batch-score
   it. Discard every sub-100 candidate; retain only strict exact closure, followed by `homm2 build`,
   a raw-byte review, and the broad `homm2 relocs 0x<RVA>` audit before integration.
7. The generated objdiff report uses `functionRelocDiffs=data_value`. For near-exact functions,
   also run a relocation-masked raw-byte comparison to isolate code-shape differences, but never use
   that masked result as target-correctness proof. The resolved owner/addend gate and relocation
   audit remain authoritative, especially for equal-valued zero-fill storage.
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
12. For every switch table, compare the ordered case-index mapping by resolving each `DIR32` entry
    to its function-relative destination. Candidate `$L` labels and Vostok's containing-function
    aliases may spell the same destination differently; relocation masking alone cannot prove that
    the same case index reaches the same body.

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
- For an exhaustive closest-first residual audit, queue by the current live report, not retained
  maxima. A function with exact-max 100% but live below 100% must be re-reviewed under the current
  target/data/relocation epoch. A currently live-exact function needs no separate record because its
  exactness was recomputed against the current normalized target objects.
- While a substantial syntactic-stub backlog remains, dedicate one persistent lane to untouched
  bodies. Start with the highest-weighted stub-heavy TU and drain it in source order, including
  required predecessors, before moving that lane to the next stub-heavy TU. A stub lane changes
  priority only: it must still recover complete semantics, types, layouts, frames, CFG, and relocs.
- During the exhaustive residual campaign, build the queue from every live non-100% function and
  order it by fuzzy percentage descending, with retail RVA as the deterministic tie break. Closest
  to exact is handled first. Prior `@early-stop`, `@semantic`, and retained-max results do not remove
  a live nonexact function from this queue; reproduce their evidence from current objects.
- Every integrated non-100 function must carry a durable marker immediately above its `VA()`.
  `@early-stop` is the byte-proven artifact set. `@semantic` is the current linking/runtime
  checkpoint:
  record the first retail/ours assembly divergence or byte span, frame/slot/CFG and relocation
  status, the obvious spellings already tried, and the condition for revisiting it. Do not repeat
  attempts listed in `@semantic` while linking and runtime testing are the active priority. The
  marker does not claim byte exactness or a proven wall and never suppresses exhaustive residual
  work. Remove it when the function reaches 100% or gains a newly reproduced valid
  `@early-stop` proof.
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
  target is structurally complete and every live residual byte and external relocation is accounted
  for under `.claude/agents/matcher.md`; a marker cannot excuse missing semantics, layout,
  frame/slot, CFG, relocation work, or another attempt during the exhaustive residual campaign.
- Do not repeatedly poll worker worktrees, reports, or percentages while a batch is active. Rely on
  matcher milestone/completion messages and let workers tune uninterrupted. Inspect a lane only for
  serial integration, a reported blocker/coordination need, or after a genuinely long silent period
  that suggests the worker or build has failed. Use waits on the order of 5-6 minutes between
  unsolicited checks, not frequent sub-minute polling cycles.

## Compiler Constraints

- Retail is MSVC 4.2 with `/Od /Ob1`. Inlining is enabled even though optimization is disabled.
- Compile objects with the pinned VC 4.2 toolchain, but final-link with the separately pinned VC
  4.0 LINK 3.00.5270 component under `build/toolchain/link300`. The build shell selects it through
  `HOMM2_LINK_EXE`. Keep its sibling CVPACK/CVTRES/MSPDB40 directory first on `PATH`; mixing LINK
  3.00 with VC 4.2 CVPACK fails. Provision/verify it with `scripts/make-linker.sh`, and use the
  combined one-shot `scripts/create-toolchain-release.nix` for reproducible releases.
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
- Mark a structurally and behaviorally complete but nonexact function with `@semantic`,
  immediately above `VA()`. Record the first differing instructions/offsets, confirmed frame/slot/CFG and
  relocation state, and every obvious steering attempt already exhausted. This is the current
  linking/runtime stop, not an early-stop proof and not permission to leave reconstruction work
  unfinished.
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

- Treat compiled reconstruction objects as the base and Vostok-delinked retail objects as the
  target. Candidate COFF supplies real symbol spelling, TU ownership, section ordinal/offset,
  scope, alignment, and storage class. Retail NB09 contributions, public RVAs, PE bytes, and
  relocations supply placement evidence. Vostok does not discover candidate identities: it emits
  the names and topology provided by the reviewed manifests.
- Canonical Vostok symbol input is exactly the conflict-free union of generated source
  `DATA(...)` definitions in `build/gen/delink_data_from_source.tsv` and explicitly reviewed facts
  absent from source DATA in `config/delink_data_supplemental.tsv`. The union is generated as
  `build/gen/delink_data_manifest.tsv`. Generated CSV/TSV files belong under `build/gen`, never
  `config`.
- Skip any symbol which lacks reviewed placement. Discovery tools may propose a real candidate
  symbol, TU, retail RVA, and supporting evidence under `build/gen`, but proposals never mutate the
  supplemental manifest, canonical union, or target objects. A human-reviewed supplemental row is
  the only promotion path for a non-DATA symbol. Keep contradictions and missing evidence explicit.
- Replay candidate data sections in COFF section-table order and with encoded alignment inside the
  exact NB09 contribution chunks for their TU. Public and DATA RVAs are non-steering breakpoints:
  they validate the replay and report drift but never move its cursor. Independent initialized,
  read-only, and zero-fill contributions can be far apart in the linked image.
- Padding is not a symbol, allocation, or matching target. Preserve it only through COFF section
  size and alignment so the following real symbol retains its offset. Never add padding rows,
  anonymous section identities, or address-sized arrays to imitate linked-image gaps.
- Canonical delinking has no nearest-symbol or synthetic fallback. Target objects must contain no
  `const_*`, `string_*`, `data_*`, `bss_*`, `[section-N]`, `empty_stub`, or unresolved-range
  identities. An uncovered retail relocation or unassigned section is a hard diagnostic, not
  permission to invent a name.
- Compare candidate and delinked COFF identities directly after the independent anonymous-name
  normalization described below. For every relocation, the source section/site, referenced symbol,
  and owner-relative addend must agree. Do not use a paired-object name or addend mapping to make
  HoMM2 comparisons pass. `homm2 data-relocs` independently audits raw COFF relocation topology
  because ordinary code scoring may mask relocation fields; objdiff's `data_value` comparison
  remains the per-symbol payload view.
- Never resolve an interior alias by emitting overlapping independent storage. Replace aliases into
  `gConfig`, monster tables, formation/elevation records, or other known objects with real member or
  table access. Refine the owning packed layout when necessary and then remove the synthetic model
  declaration.
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
- Do not classify logical `.data` versus `.bss` solely at the PE raw-size boundary. File-alignment
  padding can contain logically zero-fill contributions. Reconstruct storage from candidate COFF
  class, exact NB09 ownership chunks, public/DATA anchors, and retail bytes; conflicting evidence
  remains a diagnostic until the declaration or contribution model is corrected.

## Anonymous Data Comparison

- Objdiff compares disposable canonical copies under `build/objdiff/normalized/`, not the raw
  compiler and delinker objects directly. `$SG`, `$T`, and `name$S<number>` identities are rebuilt
  independently from each object's bytes and relocation topology. See
  `docs/data-symbol-normalization.md`.
- Never pass normalized copies to the linker, disassembler, layout recovery, or hard-gate tooling.
  Raw objects in `build/objdiff/base/` and `build/delink/` remain authoritative; the canonicalizer
  enforces that only symbol names and the COFF string table change.
- Do not restore compiler counters or use source order, retail RVAs, the opposite object, or a
  persistent manifest to make anonymous names agree. Identical allocations use deterministic
  section/offset occurrences; ambiguous float widths remain `$anon_data` and are still compared.
- Treat canonical-name or normalized-record differences as evidence to inspect, not permission to
  alias relocations. A wrong relocation target between identical writable allocations must remain
  visible.

## CodeView Function Boundaries

- The shipping NB09 stream is a linker-produced minimal/publics-only image. All 3,541 retained
  named symbols are `S_PUB32` records with type index zero. Game compilands contain no
  `S_GPROC32`, `S_LPROC32`, `S_LABEL32`, locals, types, or line records. Never attribute procedure
  lengths or private compiler information to this stream or to the synthetic PDB generated by the
  reconstruction.
- `S_PUB32` proves a public symbol's start and name, but carries no length. A next-public span is
  only a provisional delinker boundary: it may absorb an unlisted static helper and create an
  artificial objdiff wall. Preserve that uncertainty in diagnostics rather than calling the span
  CodeView-authoritative.
- Retain the 182 real shipping `S_THUNK32` records as distinct ranges. Audit the remaining uncovered
  `.text` ranges and carve private/static functions only with executable entry evidence such as a
  direct-call target and a validated disassembly boundary. Never carve alignment padding, jump
  tables, or embedded data as functions; record deterministic synthetic names and provenance for
  every carved range.

## Git Discipline

- Never revert user changes or stage `.claude/worktrees/`.
- Stage only the files belonging to the current matched function/type refinement.
- Use focused commit messages such as `match: reconstruct EVENTS EraseObj`.

# Codex Matching Guide

Read `CLAUDE.md` and `.claude/agents/matcher.md` before changing code. They are
authoritative. This file is the short, restart-ready Codex workflow.

## Objective

- Work on branch `source-decomp`.
- Reconstruct and byte-match functions in `SOURCE`, hardest/largest unresolved functions first.
- Carry each change through implementation, build verification, relocation review, and a focused
  commit. Do not leave completed work uncommitted.
- Existing `.claude/worktrees/` content is unrelated and must not be staged or changed.

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
7. Run a relocation-masked raw-byte comparison for near-exact functions. objdiff masks relocation
   bytes and can report less than 100% for delinked local-label identity even when every code byte is
   identical.
8. Audit relocation targets with `homm2 relocs 0x<RVA>`. If the helper misidentifies a delinked
   boundary, compare `llvm-objdump -r` entries manually over the function ranges. Jump-table local
   labels may be delinked as the containing function; external globals and callees must agree.
9. Run the full `homm2 build` and `git diff --check` before committing. A one-unit full build is
   about 4-5 seconds; do not optimize the build unless it exceeds 10 seconds in real shell time.

## Orchestration Campaign

- When the active goal is campaign orchestration, the root agent does not match functions itself.
  It builds the weighted queue, dispatches matcher workers, reviews and integrates their diffs,
  verifies master, blesses the baseline, commits, and immediately refills the freed lane.
- Reuse four persistent worktrees with independent incremental `build/` and Wine state. The active
  SOURCE pool is `.claude/worktrees/source-matcher-1` through `source-matcher-4`; reset a lane to the
  latest `source-decomp` only after its prior result has been integrated. Do not modify or reset the
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
- Integrate exactly one worker at a time. Apply only its declared source/header files; never copy a
  worker's `README.md`, `config/match_baseline.tsv`, or transient queue. On `source-decomp`, run the
  full build, verify the reported functions, run `homm2 status update`, stage the worker files plus
  the root-generated README/baseline, and create one linear `match:` commit.
- Refill a lane immediately after serial integration. Reuse a worker with `followup_task` when its
  prior batch produced substantive, verified matching code, preferably continuing the same TU. If
  its handoff was comments-only, stub-only, or otherwise lacked real matching progress, reject that
  result and assign the lane to a fresh matcher agent instead.
- A worker does not escape deferred targets. Return every unfinished or improperly parked function
  to that same worker and lane before assigning new work; advance only after each target reaches
  100% or a byte-proven permitted wall under `.claude/agents/matcher.md`.
- Do not repeatedly poll worker worktrees, reports, or percentages while a batch is active. Rely on
  matcher milestone/completion messages and let workers tune uninterrupted. Inspect a lane only for
  serial integration, a reported blocker/coordination need, or after a genuinely long silent period
  that suggests the worker or build has failed.

## Compiler Constraints

- Retail is MSVC 4.2 with `/Od /Ob1`. Inlining is enabled even though optimization is disabled.
- A `jmp $+0` is commonly an inlined accessor continuation, not an unbreakable compiler wall.
  Reconstruct the accessor and its expression context. These traces should not simply be accepted.
- Local stack positions are controlled by MSVC identifier hashes. Use `scripts/od_slots.py` and
  semantic suffixes to place locals in increasing bucket order. Do not brute-force random names.
- Arrays/aggregates can explain unused four-byte words. Example: `EraseObj` needed two `int[2]`
  coordinate temporaries; scalar or struct-member spellings changed later inline code generation.
- TU-cumulative compiler state can move fuzzy scores after an unrelated type/source addition. The
  source-hash max model intentionally preserves prior maxima. Always use raw bytes for final proof.
- When a later change lowers the live fuzzy score of an already matched function, keep its retained
  source-hash maximum and continue forward. Do not spend time restoring the live percentage unless
  raw-byte or relocation evidence proves that the function itself actually regressed.
- Mark `@early-stop` only for 100% matches or a byte-proven residual such as delinked local-label or
  constant-pool naming. Document the exact byte span and reason.

## Source Conventions

- Every class data member introduced or refined here must start with `m_`. Struct fields do not need
  the prefix.
- Do not use C-style casts. Use a real reconstructed layout first. Where a cast is genuinely what
  the source requires, use `static_cast`, `reinterpret_cast`, or `const_cast` as appropriate.
- Prefer this repository's naming conventions over names from other projects.
- Use named constants instead of unexplained numeric values. Typedef enums are ABI/byte neutral in
  suitable expressions, but MSVC's TU-cumulative behavior can still perturb later code; verify.
- Type and enum declarations belong in headers, never locally in a translation unit.
- Preserve layout with packed records and `SIZE` evidence where available. Do not invent classes,
  functions, globals, or labels solely to improve objdiff.
- Use inline accessors when retail emits their `/Ob1` continuation traces. Do not replace them with
  raw offset arithmetic just because the arithmetic is equivalent.
- fheroes2 is available read-only at `/path/to/homm2/fheroes2`. Use it only as a
  secondary naming reference for enums, object IDs, and serialization formats. Retail disassembly,
  CodeView, data bytes, and relocations are authoritative. Adapt useful names to this repo's style;
  do not copy its implementation structure.

## Git Discipline

- Never revert user changes or stage `.claude/worktrees/`.
- Stage only the files belonging to the current matched function/type refinement.
- Use focused commit messages such as `match: reconstruct EVENTS EraseObj`.

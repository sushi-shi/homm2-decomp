# Codex Matching Guide

Read `CLAUDE.md` and `docs/matcher-instructions.md` before changing code. They are
authoritative. This file is the short, restart-ready Codex workflow.

## Objective

- Work on branch `source-decomp`.
- Reconstruct and byte-match functions in `SOURCE`, hardest/largest unresolved functions first.
- Carry each change through implementation, build verification, relocation review, and a focused
  commit. Do not leave completed work uncommitted.
- Existing `.claude/worktrees/` content is unrelated and must not be staged or changed.

## Matching Workflow

1. Build a queue from `build/objdiff/report.json` using size-weighted match deficit. Prioritize
   large functions with the lowest weighted match percentage, where each improvement moves the
   overall score most. Parse `size` numerically, never lexically.
2. Inspect the target before writing source:
   - `homm2 sema rva 0x<RVA>`
   - `homm2 sema xref --callees 0x<RVA>`
   - `homm2 sema strings 0x<RVA>`
   - `homm2 sema disasm 0x<RVA> --lite`
   - `python3 -m homm2.analysis.decomp 0x<RVA>` when semantic structure is unclear. The documented
     `homm2 sema decomp` command is currently not registered, but the module works with the cached
     Ghidra project and must be run without reanalysis.
3. Reconstruct real types, fields, enums, and inline accessors before using pointer arithmetic.
4. Compile rapidly with `ninja` while iterating. Run `homm2 status` before trusting
   `homm2 sema match`, because a bare `ninja` leaves `report.json` stale.
5. Use `homm2 sema disasm 0x<RVA> --diff --lite` to advance from the first structural divergence.
6. Run a relocation-masked raw-byte comparison for near-exact functions. objdiff masks relocation
   bytes and can report less than 100% for delinked local-label identity even when every code byte is
   identical.
7. Audit relocation targets with `homm2 relocs 0x<RVA>`. If the helper misidentifies a delinked
   boundary, compare `llvm-objdump -r` entries manually over the function ranges. Jump-table local
   labels may be delinked as the containing function; external globals and callees must agree.
8. Run the full `homm2 build` and `git diff --check` before committing. A one-unit full build is
   about 4-5 seconds; do not optimize the build unless it exceeds 10 seconds in real shell time.

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
- fheroes2 is available read-only at `/home/sheep/Projects/homm2/fheroes2`. Use it only as a
  secondary naming reference for enums, object IDs, and serialization formats. Retail disassembly,
  CodeView, data bytes, and relocations are authoritative. Adapt useful names to this repo's style;
  do not copy its implementation structure.

## Git Discipline

- Never revert user changes or stage `.claude/worktrees/`.
- Stage only the files belonging to the current matched function/type refinement.
- Use focused commit messages such as `match: reconstruct EVENTS EraseObj`.

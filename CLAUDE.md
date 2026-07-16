# HoMM2 Decomp Project Guide

Binary-matching decompilation of Heroes of Might and Magic II: The Price of Loyalty.
The goal is C++ that reproduces the retail MSVC object code and can be linked into a
working executable.

## Ground Truth

- Retail `HEROES2W.EXE` is authoritative for code, data, relocations, resources, and
  linked addresses.
- The embedded NB09 CodeView stream is minimal/publics-only. Its 3,541 named symbols
  are `S_PUB32` records with type index zero. It proves public names and start RVAs,
  not lengths, TU ownership, private helpers, types, locals, lines, or layouts.
- A next-public span is a provisional function boundary. It can contain an unlisted
  static helper, jump table, or embedded data.
- `build/delink/` contains Vostok-delinked retail target objects.
  `build/objdiff/base/` contains objects compiled from this source tree. Do not call
  either side "original source"; all source structure remains a reconstruction.
- Secondary references may help with names and semantics, but retail evidence wins:
  Project Ironfist, if present, is a read-only reference.

See `docs/codeview-contents.md`, `docs/compiler-detection.md`, and
`docs/candidate-data-topology.md` for the supporting evidence.

## Toolchain

- Most game TUs use MSVC 4.2 `/Od /Ob1 /MT /Gr /G5 /QIfdiv`; 39 of the 95
  configured TUs use an optimized profile instead. `config/units.toml` is the
  authoritative per-TU assignment.
  `/Ob1` remains active under `/Od`; inlined accessors commonly leave `jmp $+0`.
- Optimized TUs use the `o2` profile. Their register allocation and instruction
  selection cannot be solved with `/Od` stack-slot techniques.
- `/Gr` makes free functions `__fastcall` by default.
- The retail build has no `/GX` exception state and no RTTI.
- `/G5` commonly lowers unsigned zero-extension with `and` rather than `movzx`;
  preserve that distinction when reading retail intent.
- `/Od` stack positions depend on identifier hashes. Use `scripts/od_slots.py` and
  `docs/od-stack-layout.md`; do not brute-force local names.
- Compile with the pinned VC 4.2 compiler. Final-link with pinned VC 4.0
  LINK 3.00.5270 under `build/toolchain/link300`.

## Build

Enter the worktree before the build shell because `HOMM2_DIR`, `MSVC_DIR`, and the
Wine prefix are fixed when the shell starts:

```sh
cd /path/to/worktree
nix develop .#build
homm2 init                 # one-time target/config generation
homm2 build                # configure, compile, compare, and run hard gates
homm2 status               # current and retained-max metrics
homm2 status --force-refresh
```

Use `ninja` for rapid TU iteration, then refresh status before trusting objdiff
metrics. Run `homm2 build` before integration. The report cache is content-addressed;
unchanged units are reused. The hard gates and their rationale live in
`docs/build-asserts.md`.

## Navigation

Use RVAs for semantic tools. A full `VA(...)` address is accepted where documented.

```sh
homm2 sema rva 0x<RVA>
homm2 sema disasm 0x<RVA> --diff --lite
homm2 sema xref 0x<RVA> --callees
homm2 sema strings 0x<RVA>
homm2 sema match SOURCE/UNIT
homm2 sema symbol Name
homm2 relocs 0x<RVA>
homm2 relocs --addends SOURCE
```

`python3 -m homm2.analysis.decomp 0x<RVA>` provides cached Ghidra-assisted structure
when ordinary disassembly is unclear. `homm2 ghidra` creates the optional project;
`homm2 ghidra --no-analyze` reapplies names without reanalysis.

## Repository Model

- One TU lives at `src/<TIER>/<TU>.cpp`; its declarations live in
  `include/<TIER>/<TU>.h`. Define functions in retail-RVA order.
- Mark functions with `VA(0x........, 0x..)`, global definitions with `DATA(<VA>)`,
  and owned vtables with `VTBL(...)`. These are audit metadata, not linker placement
  directives.
- Definitions and declarations follow the owner-header model. Do not add local
  `class`, `struct`, `enum`, `extern`, or forward declarations in `.cpp` files.
- Comparison-only anonymous-data normalization happens under
  `build/objdiff/normalized/`. Raw compiler and delinker objects remain authoritative
  for linking, disassembly, and hard gates.
- Reviewed static-data topology is described by source `DATA(...)` definitions plus
  `config/delink_data_supplemental.tsv`. Generated manifests belong in `build/gen`.

See `docs/data-symbol-normalization.md`, `docs/delinker-contribution-manifest.md`,
`docs/reviewed-data-objdiff.md`, and `docs/static-storage-link-audit.md`.

## Proof Vocabulary

- Exact means raw function bytes, size, and ordered relocation semantics agree after
  only the repository's reviewed target normalization.
- `// @early-stop` is reserved for a byte-proven permitted artifact. A
  relocation-only claim also uses `// @early-stop-reloc-only` and must pass its hard
  gate.
- `// @semantic` marks structurally and behaviorally complete code with a documented
  residual. It is not exactness and never excuses missing semantics or relocations.
- Objdiff fuzzy percentages guide the queue but are not proof. They can hide wrong
  stack displacements and relocation fields.

## References

- `.claude/agents/matcher.md`: detailed worker procedure and byte-proof rules.
- `.claude/agents/orchestrator.md`: parallel lane and serial integration protocol.
- `AGENTS.md`: concise Codex campaign policy and source conventions.
- `docs/patterns/INDEX.md`: known MSVC lowering patterns.
- `docs/od-stack-layout.md`: solved `/Od` local-slot model.
- `docs/build-asserts.md`: enforced repository invariants.
- `scripts/match_variants.py`: exact-only last-mile variant search.

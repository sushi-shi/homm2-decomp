# Matching tooling

How the reconstruction is checked against retail, and where each tool lives.
Contributor rules are in [AGENTS.md](../AGENTS.md); the command map is
[tooling-map.md](tooling-map.md); the matching loop is [workflow.md](workflow.md).

## Ground truth

- Retail `HMM2PL.exe` is authoritative for code, data, resources, and linked
  addresses.
- The image is STRIPPED: no debug stream, no export directory, and no
  base-relocation directory (data directory 5 is 0/0). Nothing in the binary
  names a symbol or lists a DIR32 site.
- `config/retail/functions.csv` is Ghidra's function inventory (2,472 candidate
  boundaries): ANALYSIS OUTPUT, edited as understanding improves, never retail
  evidence. A boundary becomes a claim only when a source `VA(...)` marker
  names its address; `build/gen/symbol_names.csv` is that claimed inventory.
- Every function the retail inventory lists that no marker claims delinks into
  the `(unmatched)` module, so the whole `.text` is always comparable.
- `config/retail/absolute_relocations.tsv` is the absolute-relocation site list — the only
  DIR32 site channel (it substitutes for the missing `.reloc` directory, for
  both the delinker and the Python tooling). It is generated, not hand-edited:
  `homm2 audit reloc-sweep --write` regenerates it with the delinker's
  `scripts/find_relocs.py`, whose rules are scored against PoL 2.0's surviving
  `.reloc` directory. See `docs/reloc-manifest-sweep.md`.
- `build/delink/` contains Vostok-delinked retail target objects.
  `build/objdiff/base/` contains objects compiled from this source tree. Do not
  call either side "original source"; all source structure is a reconstruction.
- Secondary references (the PoL 2.0 reconstruction, Gold 2.1 binaries) may help
  with names and semantics, but this image's bytes win. Diff against Gold 2.1
  before calling a divergence Buka-specific.

## Toolchain

- The target was built with VC6 SP5; the flake provisions that exact toolchain
  (compiler and linker in one tree) under wine. `config/units.toml` is the
  authoritative per-TU flag assignment. The whole game build is /Od-family
  (frame census: every retail function keeps the full /Od frame): 91 TUs on
  `base`, plus `base_oi` (FONT, RESMGR), `base_gx` (/GX: AudiereEffects,
  AudiereMusic, BITMAP), and the single optimized unit `o1_frame` (BITS).
- `/Gr` makes free functions `__fastcall` by default. The retail build has no
  `/GX` exception state and no RTTI.
- The `/Od` stack-slot model and lowering catalogs in `homm2/core/od_slots.py`
  were solved against MSVC 4.2 (cl 10.20) on the PoL line. Treat every such
  prediction as a hypothesis until the `od-frames`/`od-oracle` harness
  re-validates it against VC6 objects; do not brute-force local names either
  way.

## Build

Enter the shell from anywhere inside the worktree; the flake resolves its Git root before
setting `HOMM2_DIR`, `MSVC_DIR`, and the Wine prefix:

```sh
cd /path/to/worktree
nix develop .#build
homm2 init --exe /path/to/HMM2PL.exe [--editor-exe /path/to/EDT2PL.exe]
homm2 delink [--force]     # rebuild delinker inputs and the target (idempotent)
homm2 build                # configure, compile, compare, link-diff, README block
homm2 build verify         # the build, then every gate of the tier
homm2 match SOURCE/KB      # the selected-unit compile and compare loop
homm2 verify status        # current metrics plus observation-only retained maxima
homm2 verify bank          # explicitly record maxima for current function hashes
homm2 compare              # regenerate the objdiff report from scratch
homm2 verify behaviour     # game-behaviour tests of the reconstructed code
```

`homm2 build` validates the delink stamp against every input (exe, PDB,
inventories, reviewed manifests, delinker binary, generator scripts) and refuses
a stale target by naming the changed input; `homm2 delink` is the one
regeneration entry point.

Use `homm2 match <unit>` for rapid TU iteration. Run `homm2 build verify` before
integration. The report cache is content-addressed; unchanged units are reused.
The gates and their rationale live in `docs/build-asserts.md`. `HOMM2_JOBS=N`
caps every ninja the tooling starts.

## Navigation

Use RVAs for semantic tools. A full `VA(...)` address is accepted where documented.

```sh
homm2 sema rva 0x<RVA>
homm2 sema disasm 0x<RVA> --diff --lite
homm2 sema xref 0x<RVA> --callees
homm2 sema strings 0x<RVA>
homm2 sema match SOURCE/UNIT
homm2 sema symbol Name
homm2 verify relocs 0x<RVA>
homm2 verify relocs --addends SOURCE
```

`python3 -m homm2.sema.decomp 0x<RVA>` provides cached Ghidra-assisted structure
when ordinary disassembly is unclear. `homm2 ghidra` creates the optional project;
`homm2 ghidra --no-analyze` reapplies names without reanalysis.

## Tooling layout

`scripts/` holds `homm2/` (the CLI and its role packages) and `toolchain/`
(VC6 SP5 release builder, Wine game prefix and runner). Retired one-shot
scripts were removed after `f0ae961d2`; recover one with `git show`. The
packages mirror the command structure:

| Package | Role |
| --- | --- |
| `core/` | shared library: `paths` (image-aware roots), `inputs` (retail pins), `coff`, `pe`, `image`, `tsv`, `usage`, `od_slots`, `retail` |
| `graph/` | `homm2 configure/build/link`: the ninja emitter (`emit`, `rules`, `compile_graph`, `link_graph`), compiler/assembler/resource wrappers, import libraries, the native link, localization and locale builds |
| `retail_labels/` | `homm2 labels`: source `VA`/`DATA`/`VTBL` claims, symbol providers, `build/gen/symbol_names.csv` |
| `delink/` | `homm2 delink`: synthetic PDB, reviewed data and relocation manifests, Vostok delink |
| `compare/` | disposable comparison copies (canonical names, freshness stamps) |
| `verify/` | `homm2 verify`: status, banking, README block, every gate, the behaviour tests |
| `sema/`, `lsp/` | `homm2 sema`, `homm2 lsp`: retail and source navigation |
| `walls/` | the residual queue |
| `permute/` | `homm2 permute`: source-variant and TU-state searches |
| `audit/` | `homm2 audit <tool>`: on-demand campaign audits |
| `clean/` | `homm2 clean`: the generated source trees |
| `workflow/` | `homm2 workflow format`: header and enum formatting |
| `tool/`, `ghidra/` | era-tool drivers (`wine`); the optional Ghidra project |
| `manifest`, `toolchain`, `init`, `cli` | `config/units.toml`; `homm2 toolchain`; `homm2 init`; the command line |

The only tests are game-behaviour tests (`scripts/homm2/verify/behaviour/`, run by
`homm2 verify behaviour`): they check what the reconstructed code does, not how the
tooling matches it. Add a tool to its role package, not to a new top-level file.
Every entry point keeps `homm2.core.usage.logged` (`homm2 audit usage`).

## Repository model

- One TU lives at `src/<TIER>/<TU>.cpp`; its declarations live in
  `include/<TIER>/<TU>.h`. Define functions in retail-RVA order.
- Mark functions with `VA(0x........, 0x..)`, global definitions with `DATA(<VA>)`,
  primary vtables with `VTBL(Class, <VA>)`, and secondary base-specific vtables with
  `VTBL2(Derived, Base, <VA>)`. These are source-owned audit and delinker metadata,
  not compiler placement directives.
- Definitions and declarations follow the owner-header model. Do not add local
  `class`, `struct`, `extern`, or forward declarations in `.cpp` files. A
  `typedef enum` used by exactly one TU is private and lives in that `.cpp`;
  shared enum domains live in the owner header.
- Reconstructed game integers use `H2/Ints.h` aliases from `i8`/`u8` through
  `i64`/`u64`; plain `char` remains textual. Use `i32l`/`u32l` only where retail
  `long` type identity is proven to affect C++ ABI behavior, and keep native SDK
  aliases at external API boundaries. The build assertion rejects raw integer
  spellings in game-owned source and headers.
- Comparison-only anonymous-data normalization happens under
  `build/objdiff/normalized/`. Raw compiler and delinker objects remain authoritative
  for linking, disassembly, and hard gates.
- Reviewed static-data topology is described entirely by source `DATA(...)`,
  `DATA_COMPGEN(...)`, `VTBL(...)`, and `VTBL2(...)` annotations. Candidate COFF
  supplies the physical topology for compiler-generated objects. Generated manifests
  belong in `build/gen`; there is no hand-maintained private-data supplement.
- `config/match_baseline.tsv` ([score tracking](match-status.md)) retains each function's best observed fuzzy score for its
  current normalized source hash. A changed hash starts a new current-score epoch. It is
  queue evidence only: no build or command rejects a regression against an older maximum.
  A controlled run from `homm2.permute.tu_state_noise` may raise the unchanged function's maximum
  after generated predecessor input is removed. Best paired objects, disassemblies, and diffs
  may remain under `build/`; generated source input is never retained.
  The ledger is only rewritten by a build, so a source edit committed without one leaves rows
  banked against a hash that no longer exists. `homm2 audit ledger` reports that drift without
  needing a build.

See `docs/data-symbol-normalization.md`, `docs/reviewed-data-objdiff.md`, and
`docs/static-storage-link-audit.md`.

## Matching method

- Do not test matching hypotheses as one-off manual edit-compile cycles. Localize the
  divergence with `homm2 sema disasm --blocks --diff` first, then run
  `homm2 permute` as one bounded complete matrix whose independent
  dimensions are: generated conservative AST transformations (`--min-depth 1
  --max-depth 2` with the relevant `--families`), reviewed exact-span axes for
  hypotheses the generator cannot express (`--axes-from`, with the full candidate
  family per site in one file — never laddered across runs), and TU-state probes
  (`--state-trials ... --state-families forest`). Judge survivors by block topology
  and ordered relocations as well as fuzzy score.
- For a structurally aligned residual with unchanged source, run
  `homm2.permute.tu_state_noise` island censuses (forest family). Record MAX only on an
  audited exact closure.
- Manual edits are reserved for integrating the winning arm of a measured matrix and
  for mechanical fixes pinned directly by byte/relocation evidence (a wrong constant,
  field, or call target).
- Every banked maximum keeps its evidence, not just its score: after banking runs,
  run `homm2 audit harvest-max` to append replay coordinates (seed,
  trial, probe tag) to `build/matching-matrices/max-observations.tsv` and preserve
  the winning bytes as disassembly under `build/matching-matrices/max-asm/`. That
  disassembly is the structural reference for later source-shape recovery; it is
  generated state, not documentation.

## Proof vocabulary

- Exact means raw function bytes, size, and ordered relocation semantics agree after
  only the repository's reviewed target normalization.
- Source comments may record enduring semantic or codegen facts, but never queue state,
  scores, retained maxima, or a claim that a live residual is complete.
- Objdiff fuzzy percentages guide the queue but are not proof. They can hide wrong
  stack displacements and relocation fields.
- Most reconstruction work was produced with GPT-5.6 Sol and Claude Fable 5.0. An
  exact function is still independently checkable against retail bytes and ordered
  relocations, so accepting it does not require trusting the model's prose or intent.
  The cross-platform port is a semantic rewrite, not a byte-matching result, and still
  requires ordinary code review and play-testing.

## Usage history

Every Python entry point, direct module invocation and CLI child logs to
`build/homm2_usage.jsonl` (start/finish events with invocation and parent IDs,
UTC time, arguments, cwd, duration, exit code and a bounded failure tail) and a
copyable completion line to `build/homm2_usage.log`. Logging never changes a
command's result. `homm2 audit usage` checks that every entry point keeps it.

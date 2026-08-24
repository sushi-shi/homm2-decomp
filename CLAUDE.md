# HoMM2 Decomp Project Guide

Binary-matching decompilation of Buka's Heroes of Might and Magic II release,
built from the HoMM2 Gold 2.1 tree. The goal is C++ that reproduces the retail
MSVC object code and can be linked into a working executable.

## Ground Truth

- Retail `HMM2PL.exe` is authoritative for code, data, resources, and linked
  addresses.
- The image is STRIPPED: no debug stream, no export directory, and no
  base-relocation directory (data directory 5 is 0/0). Nothing in the binary
  names a symbol or lists a DIR32 site.
- `config/retail_functions.csv` is Ghidra's function inventory (2,472 candidate
  boundaries): ANALYSIS OUTPUT, edited as understanding improves, never retail
  evidence. A boundary becomes a claim only when a source `VA(...)` marker
  names its address; `build/gen/symbol_names.csv` is that claimed inventory.
- Every function the retail inventory lists that no marker claims delinks into
  the `(unmatched)` module, so the whole `.text` is always comparable.
- `config/delink_relocs.tsv` is the absolute-relocation site list — the only
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
homm2 init                 # one-time toolchain fetch + first delink + editor config
homm2 redelink [--force]   # rebuild delinker inputs and the target (idempotent)
homm2 build                # configure, compile, compare, and run hard gates
homm2 status               # current metrics plus observation-only retained maxima
homm2 status update        # explicitly record maxima for current function hashes
homm2 status --force-refresh
homm2 selftest             # the tool test suite (run after changing anything in scripts/)
```

`homm2 build` validates the delink stamp against every input (exe, PDB,
inventories, reviewed manifests, delinker binary) and refuses a stale target by
naming the changed input; `homm2 redelink` is the one regeneration entry point.

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

## Tooling Layout

`scripts/` holds three directories and no loose files. Every package under
`scripts/homm2/` is one role, and the folder structure mirrors the command structure:
`core/` (shared library — `paths`, `manifest` the one flag-assembly truth, `coff` the
one COFF/RES reader, `wine` the one tool layer, `od_slots`), `analysis/` (`homm2 sema`),
`permute/` (`homm2 permute`), `audit/` (`homm2 audit <tool>`), `match/`, `build/` (the
always-on gates; `build/configure/` generates build.ninja behind the root shim),
`clean/` (`homm2 clean`), `format/`, `init/`, `ghidra/`. Provisioning lives in
`scripts/toolchain/`, retired tooling in `scripts/archive/`.

Tests live beside what they test; `homm2 selftest` runs them and asserts a case-count
floor, because a package missing `__init__.py` is skipped by discovery silently rather
than failing. Add a tool to its role package, not to a new top-level file.

## Repository Model

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
- Reconstructed game integers use `Ints.h` aliases from `i8`/`u8` through
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
- `config/match_baseline.tsv` retains each function's best observed fuzzy score for its
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

## Matching Method

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
  trial, probe tag) to `docs/matching-matrices/max-observations.tsv` and preserve
  the winning bytes as disassembly under `docs/matching-matrices/max-asm/`. That
  disassembly is the structural reference for later source-shape recovery.
  (The directory starts empty on this branch; only VC6-measured evidence goes in.)

## Proof Vocabulary

- Exact means raw function bytes, size, and ordered relocation semantics agree after
  only the repository's reviewed target normalization.
- Source comments may record enduring semantic or codegen facts, but never queue state,
  scores, retained maxima, or a claim that a live residual is complete.
- Objdiff fuzzy percentages guide the queue but are not proof. They can hide wrong
  stack displacements and relocation fields.

## References

- `.claude/agents/matcher.md`: detailed worker procedure and byte-proof rules.
- `.claude/agents/orchestrator.md`: parallel lane and serial integration protocol.
- `AGENTS.md`: concise Codex campaign policy and source conventions.
- `docs/build-asserts.md`: enforced repository invariants.
- `scripts/archive/`: retired experiment/search tools retained for historical reproduction.

The VC4.2 pattern catalog and matching matrices from the PoL 2.0 line were
deliberately not carried over: that catalog's own rule is "nothing ports from
other decomps". A fresh VC6 catalog grows here from measured evidence.

# homm2-decomp — plan (port of the Gruntz matching-decomp system)

**Goal.** A binary-matching decompilation of **HoMM2 "Price of Loyalty" `HEROES2W.EXE`**
(New World Computing, 1997-05-05, MSVC 4.x): C++ sources that, compiled with the *original*
MSVC 4.x toolchain, produce object files **byte-identical** to the retail `.text`, verified
with **objdiff**. Same architecture as `/home/sheep/Projects/gruntz`, ported and simplified.

## Why this is dramatically easier than Gruntz

Gruntz's target has **no symbols** — half their machine (Ghidra DB, pyghidra, FID library
identification, `synth_pdb` from Ghidra exports, name/boundary recovery, the `classifier`
agent) exists only to answer *"what is this function and what are its types?"*. **Our target
ships a CodeView NB09 debug stream**, so we already know, authoritatively:

| Gruntz recovers via Ghidra (slow, fuzzy) | We already have (exact, from CodeView) |
|---|---|
| function names | 3,541 demangled symbols → `orig/symbols.txt` |
| function boundaries/sizes | exact `size` per function (next-symbol − padding) |
| which TU a function belongs to | `pol-symbols-by-module.txt` (498 compilands) |
| class layout / vtables / inheritance | `include/` (46 class headers, vtable order, OVERRIDE) |
| the source tree | `src/` BASE/SOURCE/EDITOR tiers (validated 35/35 vs asserts) |
| compiler/toolchain | MSVC 4.x (MSDEV, LINK 2.60, CVTRES 4.00, LIBCMT) |

So we **delete the entire Ghidra pipeline** and feed `vostok-delinker`/`objdiff` directly
from CodeView. The carcass (`src/`+`include/`) is **already generated**. What remains for the
swarm is (a) the **infrastructure** (this plan), then (b) the **matching campaign** — writing
function *bodies* that compile (MSVC 4.x) to the retail bytes. Identification is free; only
codegen-matching is work.

## Target layout (port of gruntz, Ghidra-ectomy)

```
homm2-decomp/
  src/        {BASE,SOURCE,EDITOR,_external}   reconstructed C++           [HAVE]
  include/    {BASE,SOURCE,EDITOR} + rva.h + Ints.h   class headers        [HAVE; add rva.h/Ints.h]
  orig/       HEROES2W.EXE, symbols.txt, compilands, libraries             [HAVE]
  vendor/     import-lib headers if any third-party SOURCE appears         [likely empty]
  config/     units.toml, symbol_names.csv (gen), match-queue.md, flags
  scripts/h2/ the python package (python -m h2): cli, configure, build/, match/
  tools/      cv-nb09-*.py  (our CodeView "delinker front-end" / symbol source)  [HAVE]
  docs/       build-system, toolchain-vc4x, compiler-detection, matching-patterns, patterns/
  editor/nvim the :H2 plugin (vtable/diff/status keybinds)
  flake.nix   two dev shells (default = analysis+diff+clang; build = +wine+MSVC4)
  configure.py  units.toml -> build.ninja + objdiff.json
  CLAUDE.md, .clang-format, .clangd, .githooks/pre-commit
  .claude/    agents (matcher, orchestrator, parallel-orchestrator) + commands/match
  build/      (gitignored) base objs, target objs, synth PDB, clangd db, wineprefix, report
```

## The pipeline (identical to gruntz, minus Ghidra)

```
config/units.toml (per-TU)  +  config/symbol_names.csv (rva,name,unit,size,kind)
   |                                    ^-- generated ONCE from CodeView by tools/cv-nb09 (no Ghidra)
   |  configure.py
   v
build.ninja  +  build/objdiff/objdiff.json
   |  ninja
   +-- BASE  : src/<unit>.cpp --(wine cl /O2 /MT ...)--> build/objdiff/base/<unit>.obj
   +-- TARGET: symbol_names.csv --(synth_pdb -> vostok-delinker)--> build/objdiff/target/<unit>.obj
   v
objdiff (pairs base vs target BY SYMBOL NAME) -> per-function exact/fuzzy % -> README match block
```

**On the PDB (vostok-delinker eats a PDB, not CodeView).** vostok reads the PE **plus a PDB**
to know each symbol's RVA/size so it can cut `.text` into per-symbol COFF target objects. So we
still **synthesize a PDB** (`synth_pdb`: `symbol_names.csv` → PDB-YAML → `llvm-pdbutil yaml2pdb`),
exactly as gruntz does. The difference is **we do it ONCE**: gruntz re-synths every time Ghidra
renames/discovers a function, but our CodeView publics are complete and authoritative from the
start — `symbol_names.csv` is generated one time from the debug stream and frozen, so the synth
PDB is a build artifact, not a moving target. (We could alternatively transcode the NB09 stream
straight to PDB, but synth-from-CSV is simpler and reuses gruntz's proven path.)

`clang --target=i686-pc-windows-msvc` (unwrapped) compiles the *same* sources for **clangd**
(navigation) and **label/type checks** (`rva.h` annotations, `SIZE`/`OVERRIDE`). The matching
`.obj` is **MSVC 4.x under wine**. `rva.h` macros vanish under MSVC so they never perturb bytes.

---

## Work items (the "carcass" for the swarm) — each is a discrete deliverable

### Critical path — I bootstrap these first (single-threaded spine)

**B0. Toolchain provisioning (the one true blocker).** Source the exact **MSVC 4.x** (`cl.exe`,
`link.exe`, `cvtres.exe`, `C2.DLL`, headers, `LIBCMT.lib` + Win32 SDK of that vintage). Evidence
points to VC++ 4.x (MSDEV, LINK 2.60.5112, CVTRES 4.00). Steps:
  - **compiler-detection spike** (`docs/compiler-detection.md`): compile one tiny leaf function
    with candidate `cl` versions (4.0 / 4.1 / 4.2) and objdiff against a known retail function to
    pin the exact build that reproduces bytes. (Mirrors gruntz `docs/toolchain-vc50-sp3.md`.)
  - package it as a tarball `{msvc/{bin,include,lib}, ninja/}` → host (GH release) → `flake` fetches
    it → run under `wineWow64Packages.staging`. Port `scripts/create-toolchain-release.py`.
  - **DECISION NEEDED:** do you have a VC++ 4.x ISO/install on hand, or should B0 include sourcing it?

**B1. `flake.nix`** — two shells, copied from gruntz minus jdk/ghidra/pyghidra:
  - `default`: `objdiff(-cli)`, `vostok-delinker`, `clang-unwrapped` (clangd/format/tidy), python,
    ripgrep/file/xxd/jq. Exports `$H2_EXE`, `$H2_CLANG`. **No Ghidra, no JDK.**
  - `build`: + `wineWow64Packages.staging`, `+ $MSVC_DIR` from the toolchain tarball, `ninja`.
  - runtime DLLs (`MSS32.DLL`/`SMACKW32.DLL`/`WING32.DLL`) fetched for *running* the rebuilt EXE
    (not needed to build). We already have `SMACKW32.DLL` locally.

**B2. `config/symbol_names.csv` + `config/units.toml`** — generated from CodeView by a new
`tools/gen_manifest.py` (wraps cv-nb09): emits `rva,name,unit,size,kind` (raw MSVC-mangled names
from `pol-fargus-cpp-symbols.txt`) and one `[[unit]]` per compiland with `source` = its `src/` path.
Flag profiles seeded from the binary (`base` = VC4 `/c /O2 /MT`; refine via `docs/linker-flags.md`).
**This is the step Gruntz needs Ghidra for; we do it from the debug stream.**

**B3. `include/rva.h` + `Ints.h`** — port gruntz's macros verbatim (they're compiler-table no-ops):
`RVA(addr,size)`, `RVAU`, `SYMBOL(mangled)`, `DATA(addr)`, `SIZE(type,bytes)`, `OVERRIDE`. Our
headers already use `OVERRIDE`; src already uses `RVA`/`DATA` lines. Adjust clang target to MSVC4.

**B4. `configure.py` + `scripts/h2/`** — port the gruntz package, *dropping* `ghidra/`,
`build/{ghidra_metadata_generate,synth from ghidra}`, `analysis/{fid_generate, gen_class_stubs, …}`
(we generated those from CodeView already). Keep/port: `configure.py`, `build/{cc_wrap, delink,
synth_pdb, link, ninja_syntax, codeview}`, `match/{status, fingerprints}`, `init/clangd`, `cli`.
`synth_pdb` is fed our `symbol_names.csv` directly (no functions.csv overlay). **End state of the
critical path: `h2 build` compiles + delinks + diffs at least one TU end-to-end.** ("configure"
can be a thin step — since the manifest is generated, we can also emit `build.ninja` directly.)

### Parallelizable — independent swarm items once B0–B4 land

**P1. `vostok-delinker` integration** — `build/delink.py`: synth_pdb(`symbol_names.csv`) →
vostok-delinker → collect per-unit `<unit>.obj` into `build/objdiff/target/`. Pin the same
`fix/absolute-data-relocs` branch gruntz uses (DIR32 for abs refs, REL32 for branches).

**P2. `objdiff` usage** — `configure.py` emits `objdiff.json` pairing base↔target by symbol name;
`match/status.py` runs `objdiff-cli report generate` → per-unit/function exact+fuzzy %.

**P3. clang / clangd** — `.clangd`, `init/clangd.py` emits `compile_commands.json` (clang
`--target=i686-pc-windows-msvc /imsvc <toolchain>/include`). `.clang-format` (house style) +
`.githooks/pre-commit` auto-format. **clang only for tooling; never for the matching build.**

**P4. `editor/nvim`** — port `editor/nvim/{plugin,lua}`: `:H2` commands + keybinds for
vtable view, per-function objdiff, match status, jump-by-RVA. Auto-loaded via the flake `nvim`
shim.

**P5. README + auto match-status** — `match/status.py --write-readme` regenerates the
per-module exact/fuzzy table (denominator = engine `.text` reconstruction targets; exclude CRT/EH).

**P6. work queue** — `analysis/gen_match_queue.py` → `config/match-queue.md`: unmatched functions
ordered leaf-first / by TU. We can pre-rank by call-graph + size since we have every boundary.

**P7. docs** — `build-system.md`, `toolchain-vc4x.md`, `compiler-detection.md`, `matching-patterns.md`,
and **`docs/patterns/` SEEDED EMPTY**. ⚠️ **Gruntz's pattern catalog is VC5.0 /O2 idioms — they will
NOT port to VC4.x** (different optimizer/regalloc/EH). We start a fresh VC4 catalog and grow it
during matching. (This is the "not sure how well those apply" you flagged — answer: the *mechanism*
ports, the *contents* don't.)

**P8. `CLAUDE.md` + `.claude/agents` + `commands/match`** — port `matcher`,
`orchestrator`, `parallel-orchestrator`; **drop `classifier`** (it identifies functions — we have
names). Adapt the matcher loop: skip "pull Ghidra decomp / identify"; go straight from
`symbols.txt`+`include/` (known name, signature, class, size, RVA) to body reconstruction →
`h2 build` → objdiff → iterate. Keep STOP-EARLY, worktree pool, serial integration, the
RVA-order + zero-padded-address conventions.

## Execution phasing

1. **Phase 0 (me, now → on your approval):** B0–B4 — provision the toolchain, scaffold the
   flake/package/manifest, and get **one real TU green** end-to-end (proves the whole loop).
   This de-risks everything; nothing else matters if the toolchain can't reproduce a byte.
2. **Phase 1 (infra swarm):** P1–P8 in parallel (independent files/dirs, low conflict).
3. **Phase 2 (matching swarm):** the `/match` campaign — matchers reconstruct bodies, leaf-first,
   verified by objdiff, growing `docs/patterns/`. This is the long tail (cf. gruntz at 40%).

## Decisions (resolved)

1. **Toolchain:** **spike first** — pin the exact MSVC 4.x sub-version (statically from the
   retail CRT's CodeView + banners; then empirically) before committing to a provisioning route.
2. **Target binary:** **PoL `HEROES2W.EXE`** — the only symbol-bearing build.
3. **Package name:** **`homm2`** → `python -m homm2`, `$HOMM2_EXE`, `:Homm2`.
4. **Vendored source:** `basewin.lib` is NWC's own (already in `src/BASE/`); `LIBCMT` is the CRT
   (excluded from %, matched via the toolchain lib). Miles/Smacker/WinG are DLL imports, not source.

## Risks
- **R1 (critical):** sourcing the *exact* VC4.x build — wrong sub-version → nothing matches.
- **R2:** VC4 codegen patterns are undocumented (no inherited catalog) — slower early matching.
- **R3:** EH model / CRT differences vs VC5 — `_external/` CRT identification needs the real LIBCMT.

# HoMM2 decomp — project guide

Binary-matching decompilation of **Heroes of Might and Magic II — The Price of
Loyalty** (`HEROES2W.EXE`, New World Computing, 1997). Goal: C++ that, compiled
with the original **MSVC 4.2** toolchain under wine, produces object files
**byte-identical** to retail `.text`, verified with **objdiff**.

## What makes this decomp different (read before matching)

- **CodeView is authoritative.** PoL ships an NB09 debug stream, so function
  **names, RVAs, sizes, owning class/TU/tier, vtables, and class layouts are
  ground truth** — already extracted to `build/gen/symbol_names.csv` and the
  recovered `include/` headers. **Never guess or re-derive them; there is no
  Ghidra / FID / name-recovery stage**.
- **The build is `/Od` (debug), not `/O2`.** Flags: **`/nologo /c /Od /MT /Gr`** —
  unoptimized (full `ebp` frames, every local spilled), static LIBCMT,
  **`__fastcall` default** (free fns mangle `@@YI`; 1st/2nd int args in ECX/EDX).
  **No `/GX` → no C++ exceptions / no EH state. No RTTI.** So the EH wall and the
  /O2 regalloc/scheduling walls of a typical decomp **do not exist here** — most
  functions reconstruct to 100% directly. Lowering is literal: body maps ~1:1 to asm.
- **The one real /Od lever is SOLVED: stack-slot naming.** `/Od` assigns each
  local's frame offset by a **hash of its name** (per-scope 16-bucket table), not
  declaration order. This is fully reverse-engineered → **`scripts/od_slots.py`**
  *computes* local names for a target retail frame (no compile-loop brute force).
  Full model: **`docs/od-stack-layout.md`**; pattern: `docs/patterns/od-hash-slots.md`.

## Build loop

Everything runs in the build dev-shell: **`nix develop .#build`** (wine + MSVC 4.2 +
the CLI). `HOMM2_DIR`/`HOMM2_EXE`/`MSVC_DIR`/`WINEPREFIX` are fixed at shell entry to
`$PWD` — so always **`cd` to your tree BEFORE entering the shell**, and prefer running
many commands inside ONE open shell over `nix develop … --command` per call.

```sh
homm2 init      # ONE-TIME: CodeView -> manifest -> ??_C@ names -> synth PDB -> delink -> configure
homm2 build     # configure + ninja (wine cl per TU) + objdiff vs target + refresh README %
homm2 status    # per-unit + overall match %   (also: status update | status check)
```

`homm2 build` compiles each `config/units.toml` unit to `build/objdiff/base/<unit>.obj`
and diffs it against the delinked retail target `build/delink/<unit>.c.obj`.

## Conventions

- One TU per CodeView compiland under `src/<TIER>/<TU>.cpp` (TIER ∈ BASE, SOURCE,
  EDITOR), shared headers in `include/<TIER>/`. Define functions in **retail-RVA order**.
- Above each function: **`VA(0x........, 0x..)`** (8-hex-digit zero-padded address,
  unpadded size); globals/vtables: **`DATA(0x........)`**. Macros vanish under MSVC
  (`include/va.h`). A *placeholder* is `VA(...)` then a `// signature;` comment; a
  *reconstruction* has a real body.
- Mark a parked-below-100% match with **`// @early-stop`** + a byte-level reason line
  (see matcher.md). `rg '@early-stop' src` is the deferred set.

## Matching campaign (parallel by default)

- **`/match [n]`** drives the campaign (default n=4). It builds the queue, fans out
  `n` **matcher** agents across a reused `matcher-1..n` worktree pool, and integrates
  results **serially into linear `master`**. See `.claude/agents/orchestrator.md`.
- **matcher** (`.claude/agents/matcher.md`) reconstructs one TU; uses `od_slots.py`
  for stack naming; reports a diff. Single worker — never spawns subagents.
- Queue: `python3 -m homm2.match.gen_queue` → `config/match-queue.md` (CodeView funcs
  not yet bodied, size-band order).
- Baseline: `homm2 status update` blesses best fuzzy% to `config/match_baseline.tsv`;
  `homm2 status check` gates regressions at integration.
- Status shows **Functions exact** (completion) + **Fuzzy** (weighted progress — the
  signal that moves as you grind). No exact-byte "Code" column (it's a restatement of
  Functions-exact that stays ~0 until the hard functions finish).

## Key references

- `docs/od-stack-layout.md` — the /Od name-hash + per-scope tables (the matcher's superpower).
- `docs/patterns/INDEX.md` — codegen idiom catalog (grep by symptom/tag when a diff row sticks).
- `docs/compiler-detection.md`, `docs/linker-flags.md` — toolchain facts.
- `scripts/od_slots.py` (predict/solve slots) · `scripts/od_oracle.py` (verify vs real cl).
- `editor/nvim` — `:Homm2` in-editor diff/build/status (auto-loaded by the dev-shell).

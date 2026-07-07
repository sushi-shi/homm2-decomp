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
- **Most TUs are `/Od` (debug), but optimization is PER-TU — 39 of 95 ship `/O2`.**
  Default flags: **`/nologo /c /Od /MT /Gr /G5 /Ob1 /QIfdiv`** — unoptimized (full `ebp`
  frames, every local spilled), static LIBCMT, **`__fastcall` default** (free fns
  mangle `@@YI`; 1st/2nd int args in ECX/EDX), **`/G5`** (Pentium target:
  zero-extends with AND, never MOVZX — `docs/patterns/od-debug-build.md`).
  **No `/GX` → no C++ exceptions / no EH state. No RTTI.**
  - **`/O2` TUs** (flags `/nologo /c /O2 /MT /Gr /G5` — `o2` profile): the
    **basewin.lib UI framework** (most of BASE — BITMAP, BUTTON, WIDGET, WINMGR,
    MOUSEMGR, Misc, droplist, listbox, icons, …) **+ SOURCE/{FINDPATH,SEARCH}**.
    Here lowering is NOT literal: **FPO (no `ebp` frame), register allocation,
    strength-reduction `lea`, intrinsic `strlen` via `repne scasb`**, and the
    optimizer reassociates/CSEs. The `/Od` levers below DON'T apply; the wall is
    matching the optimizer's register allocator + instruction selection (steer with
    source structure: separate statements to block factoring, `>>1` not `/2`, etc.).
  - **`/Od` TUs**: all SOURCE game logic (GAME, HERO, ARMY, CMBTMGR, SPELLS, AI, …),
    EDITOR/mapcell, and 7 BASE TUs (BITS, Bzip, FONT, RESMGR, TILE, WINDOW, soundmgr).
    No EH/regalloc wall — lowering is literal, body ~1:1 to asm.
  - Opt level is **not in CodeView** (S_COMPILE omits it). `gen_manifest.py`
    classifies each TU by prologue (`push ebp;mov ebp,esp` = /Od, FPO = /O2) →
    `flags="base"|"o2"` in `config/units.toml`. See `[[optimization-is-per-tu]]`.
- **But inline expansion is ON (`/Ob1`) — separate axis from `/Od`.** "Unoptimized but
  inlined." The retail `.text` is littered with `jmp $+0` (`e9 00000000`, jump-to-next)
  that plain `/Od` never emits: they are the per-call-site continuation jumps of
  **inlined in-class accessors**. A cluster of `jmp $+0` around repeated field/array
  access is the fingerprint that the original used inline getters (e.g. `fullMap::Row`/
  `Extra`) — reconstruct them, don't hand-inline to a raw expression (that structurally
  caps the match). Fully-inlined accessors emit no symbol, so CodeView lists none.
  Pattern: **`docs/patterns/inline-accessors.md`**.
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
and diffs it against the delinked retail target `build/delink/<unit>.c.obj`. ninja **tracks
header deps** (MSVC 4.2 has no `/showIncludes`, so `cc_wrap.py` scans each TU's `#include`
graph into a depfile) — editing a shared header recompiles exactly its includers, so a header
change can't leave a stale obj. It then runs **five hard gates** (a red gate fails the build):
`assert_decls` (no local `class/struct/enum`/`extern`/fwd-decl in any .cpp), `assert_no_fake_labels`
(no emitted fn symbol absent from CodeView), `assert_globals_data` (every global carries a unique
`DATA(<VA>)`; none in a .cpp), `assert_defs_declared` (every free-fn definition is declared in its
owner header), `assert_globals_defined` (every extern global has a definition in its owner TU —
link-completeness). Full catalog + rationale: **`docs/build-asserts.md`**.

## `homm2 sema` — semantic navigation (matcher's read-only toolbox)

One discoverable entrypoint for source/target navigation (**`homm2 sema -h`** lists all;
every invocation is logged to `build/homm2_sema.log`). SEMANTIC questions go here — grep is
lexical-only. Each subcommand is a thin wrapper over a `homm2.analysis.<...>` module.

```sh
homm2 sema xref   0x0004a3c0        # who calls this fn (--callees | --tree | --raw)
homm2 sema decomp 0x0004a3c0        # Ghidra decompiler C (names from our CodeView seeding)
homm2 sema disasm 0x0004a3c0 --diff # base(compiled) vs target(retail) asm (--base|--lite|--rich)
homm2 sema strings 0x0000126d       # a fn's string set   (--find TEXT = reverse lookup)
homm2 sema match  SOURCE/KB         # per-fn % of a unit (or an 0x RVA)
homm2 sema rva    0x0004a3c0        # address dossier (claim / src loc / ghidra / match %)
homm2 sema symbol combatManager     # fuzzy workspace-symbol search (clangd)
homm2 sema def|refs|hover src/… L C # clangd LSP at a point;  rename … --dry-run
```

Addresses are **RVAs** (as in `symbol_names.csv`); `rva`/`decomp` also accept a full VA (the
`VA(0x..)` macro form). xref/disasm/strings/match/rva/clangd need **no** Ghidra.

**Ghidra (optional, powers `xref` boundaries + `decomp`).** `homm2 sema decomp` and the
library-boundary half of `xref` need a one-time Ghidra project. Build it with:

```sh
homm2 ghidra          # ONE-TIME: import HEROES2W.EXE -> auto-analyze -> apply OUR CodeView
                      # names -> export build/ghidra/exports/functions.csv  (SEVERAL MINUTES)
homm2 ghidra --no-analyze   # re-apply names + re-export instantly (no re-analysis)
```

Ghidra is read-only here — it never *discovers* names (CodeView is authoritative); it's fed
the EXE + our known symbols. Needs the dev shell's Ghidra env (in the flake).

## Conventions

- One TU per CodeView compiland under `src/<TIER>/<TU>.cpp` (TIER ∈ BASE, SOURCE,
  EDITOR), shared headers in `include/<TIER>/`. Define functions in **retail-RVA order**.
- Above each function: **`VA(0x........, 0x..)`** (8-hex-digit zero-padded address,
  unpadded size); globals/vtables: **`DATA(0x........)`**. Macros vanish under MSVC
  (`include/va.h`). A *placeholder* is `VA(...)` then a `// signature;` comment; a
  *reconstruction* has a real body.
- Mark a parked-below-100% match with **`// @early-stop`** + a byte-level reason line
  (see matcher.md). `rg '@early-stop' src` is the deferred set.
- **Header discipline (owner model, gate-enforced):** a symbol defined in `<TU>.cpp` is
  declared ONLY in its owner header `include/<TIER>/<TU>.h`; callers `#include` that. A .cpp
  carries **no** local `class/struct/enum`/`extern`/forward-decls — types come from the recovered
  class headers, cross-TU functions from their owner header. **Globals** live on their owner TU:
  `DATA(<VA>) extern T g;` in `<TIER>/<TU>.h` (CodeView says which TU owns each) + a plain `T g;`
  **definition** in that owner `.cpp`; globals with no CodeView symbol go in `_globals_model.h`.
  Win32 from the custom minimal `include/win/windows.h`, CRT from real `<io.h>`/`<string.h>`.
  Include the **specific** headers a TU uses — there is no `_all.h` or `_globals.h` umbrella (both
  dissolved). The five build gates (`docs/build-asserts.md`) enforce all of this; bootstrap an
  owner header with `gen_module_header.py`, global defs with `gen_global_defs.py`.

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
- **⚠️ Fuzzy% lies about frame slots.** It gives partial credit for a wrong `-0xN(%ebp)`
  displacement, so a function with EVERY local mis-slotted can read 97%+. When grinding
  the last few %, **diff your obj vs the target with `(%ebp)` displacements VISIBLE**
  (normalize jump/call targets only) — a "stuck on jmps" plateau is often a hidden
  slot-hash miss. Fix names with `od_slots.py`. See `docs/patterns/od-hash-slots.md`.

## Key references

- `docs/build-asserts.md` — the five hard build gates (what `homm2 build` enforces + why).
- `docs/od-stack-layout.md` — the /Od name-hash + per-scope tables (the matcher's superpower).
- `docs/patterns/INDEX.md` — codegen idiom catalog (grep by symptom/tag when a diff row sticks).
- `docs/compiler-detection.md`, `docs/linker-flags.md` — toolchain facts.
- `scripts/od_slots.py` (predict/solve slots) · `scripts/od_oracle.py` (verify vs real cl).
- `homm2 sema <cmd>` — semantic navigation (xref/decomp/disasm/strings/match/rva/clangd; see
  Build loop). Modules in `scripts/homm2/analysis/`; Ghidra pipeline in `scripts/homm2/ghidra/`.
- `editor/nvim` — `:Homm2` in-editor diff/build/status (auto-loaded by the dev-shell).

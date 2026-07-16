# HoMM2 decomp — project guide

Binary-matching decompilation of **Heroes of Might and Magic II — The Price of
Loyalty** (`HEROES2W.EXE`, New World Computing, 1997). Goal: C++ that, compiled
with the original **MSVC 4.2** toolchain under wine, produces object files
**byte-identical** to retail `.text`, verified with **objdiff**.

## What makes this decomp different (read before matching)

- **The shipping CodeView is minimal/publics-only.** PoL embeds an NB09 stream whose
  3,541 named symbols are `S_PUB32` records with type index zero. It authoritatively
  supplies each retained public symbol's name and start RVA, but not its length, TU
  ownership, private/static helpers, local symbols, types, lines, vtables, or class
  layouts. Those are reconstruction results in `build/gen/`, the manifest, and
  `include/`; preserve their evidence and uncertainty instead of attributing them to
  CodeView. A next-public span is only a provisional function boundary and can absorb
  an unlisted helper.
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
  - Opt level is **not in the game CodeView records** (the game compilands have no
    `S_COMPILE`; surviving records describe linker import-thunk modules). `gen_manifest.py`
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
homm2 init      # ONE-TIME: public symbols + recovered manifest -> synth PDB -> delink -> configure
homm2 build     # configure + ninja (wine cl per TU) + objdiff vs target + refresh README %
homm2 status    # per-unit + overall match %   (also: status update | status check)
```

The objdiff report is content-addressed over `objdiff.json`, every configured base/target object,
and the resolved `objdiff-cli` binary. An unchanged `homm2 build` reuses the verified report;
`homm2 status --force-refresh` explicitly regenerates it. When only base-object content changes,
the cache regenerates complete reports for those units and merges them into the prior verified
report. A target/config/tool/topology change, corrupt cache, or explicit force refresh still runs
objdiff over the whole project.

`homm2 build` compiles each `config/units.toml` unit to `build/objdiff/base/<unit>.obj`
and diffs it against the delinked retail target `build/delink/<unit>.c.obj`. ninja **tracks
header deps** (MSVC 4.2 has no `/showIncludes`, so `cc_wrap.py` scans each TU's `#include`
graph into a depfile) — editing a shared header recompiles exactly its includers, so a header
change can't leave a stale obj. It then runs **seven hard gates** (a red gate fails the build):
`assert_decls` (no local `class/struct/enum`/`extern`/fwd-decl in any .cpp), `assert_no_fake_labels`
(no emitted fn symbol absent from the recovered inventory), `assert_globals_data` (every global's **definition**
carries a unique `DATA(<VA>)`; no `DATA()` on a header `extern`),
`assert_defs_declared` (every free-fn definition is declared in its
owner header), `assert_globals_defined` (every extern global has a definition in its owner TU —
link-completeness), `assert_vtables` (every class vtable is claimed by a `VTBL()` census marker in
its owner TU — no drift, no fake classes), and ordered `assert_relocs --fields` (an exact function
cannot silently address the wrong field of a recovered public DATA owner). Full catalog + rationale:
**`docs/build-asserts.md`**.

**`homm2 relocs` — OPT-IN broad reloc-target audit.** The narrow owner-field subset is a hard gate;
the order-independent whole-target review remains opt-in. The generated objdiff report uses
`functionRelocDiffs=data_value`, but that does not prove every target or owner-relative addend:
equal-valued BSS fields and unresolved aliases can still hide or manufacture differences.
`homm2 relocs` resolves every
near-exact fn's reloc targets (via `symbol_names.csv` + definition `DATA()` VAs) and flags any address
base references that retail never does. It's OPT-IN, not a hard gate, because incomplete functions
can still carry legitimate relocation-shape differences. Canonical targets retain real folded
function identities and synthetic relocation names are errors. `homm2 relocs 0x<rva>`
reviews one function. Manual relocation-masked raw-byte proof remains useful for separating code
shape from target identity.

`homm2 relocs --addends [SOURCE|BASE|EDITOR]` compares every report function independently as
`relocation name -> sorted encoded COFF addends`. It intentionally ignores function-relative sites
and fuzzy percentage, so code movement in incomplete functions does not hide a missing or wrong
owner-relative displacement. The complete mismatch queue is written to
`build/gen/function_reloc_addends.json`. Rows are classified as `value-set` (an addend value exists
on only one side), `count-only` (the same values occur with different multiplicities), or
`one-sided` (the relocation identity is absent from one object), and `code-local` (a self-symbol
relocation whose addend names a local label or jump-table entry); stdout stays focused on the
actionable external `value-set` rows while JSON retains all structural differences.

## `homm2 sema` — semantic navigation (matcher's read-only toolbox)

One discoverable entrypoint for source/target navigation (**`homm2 sema -h`** lists all;
every invocation is logged to `build/homm2_sema.log`). SEMANTIC questions go here — grep is
lexical-only. Each subcommand is a thin wrapper over a `homm2.analysis.<...>` module.

```sh
homm2 sema xref   0x0004a3c0        # who calls this fn (--callees | --tree | --raw)
homm2 sema disasm 0x0004a3c0 --diff # base(compiled) vs target(retail) asm (--base|--lite|--rich)
homm2 sema strings 0x0000126d       # a fn's string set   (--find TEXT = reverse lookup)
homm2 sema match  SOURCE/KB         # per-fn % of a unit (or an 0x RVA)
homm2 sema rva    0x0004a3c0        # address dossier (claim / src loc / ghidra / match %)
homm2 sema symbol combatManager     # fuzzy workspace-symbol search (clangd)
homm2 sema def|refs|hover src/… L C # clangd LSP at a point;  rename … --dry-run
```

Addresses are **RVAs** (as in `symbol_names.csv`); `rva` also accepts a full VA (the
`VA(0x..)` macro form). xref/disasm/strings/match/rva/clangd need **no** Ghidra.

**Ghidra (optional, powers `xref` boundaries).** The
library-boundary half of `xref` needs a one-time Ghidra project. Build it with:

```sh
homm2 ghidra          # ONE-TIME: import HEROES2W.EXE -> analyze -> apply retained/recovered
                      # names -> export build/ghidra/exports/functions.csv  (SEVERAL MINUTES)
homm2 ghidra --no-analyze   # re-apply names + re-export instantly (no re-analysis)
```

Ghidra is read-only here. It is fed the EXE plus retained public and recovered names; use its
analysis as evidence for private-helper boundaries, never as invented source identity. Needs the
dev shell's Ghidra env (in the flake).

## Conventions

- One reconstructed TU under `src/<TIER>/<TU>.cpp` (TIER ∈ BASE, SOURCE,
  EDITOR), shared headers in `include/<TIER>/`. Define functions in **retail-RVA order**.
- Above each function: **`VA(0x........, 0x..)`** (8-hex-digit zero-padded address,
  unpadded size); globals: **`DATA(0x........)`** on the **definition** in the owner `.cpp` (the
  header `extern` is plain); a class
  vtable: **`VTBL(class, 0x........)`** census marker at the owner TU's tail. Macros vanish under MSVC
  (`include/va.h`). A *placeholder* is `VA(...)` then a `// signature;` comment; a
  *reconstruction* has a real body.
- Mark a byte-proven artifact with **`// @early-stop`** + a byte-level reason line
  (see matcher.md). Mark a behaviorally and structurally complete nonexact function with
  **`// @semantic`** + its remaining compiler/code-shape residual. `@semantic` is the current
  linking/runtime-testing stop: it does not claim byte exactness or a proven compiler wall.
  `rg -n '@(?:early-stop|semantic)' src` is the completed nonexact set.
- **Header discipline (owner model, gate-enforced):** a symbol defined in `<TU>.cpp` is
  declared ONLY in its owner header `include/<TIER>/<TU>.h`; callers `#include` that. A .cpp
  carries **no** local `class/struct/enum`/`extern`/forward-decls — types come from the recovered
  class headers, cross-TU functions from their owner header. **Globals** live on their owner TU:
  a plain `extern T g;` in `<TIER>/<TU>.h` (the recovered owner model says which TU owns each) + the **definition**
  `DATA(<VA>) T g;` in that owner `.cpp` (the VA rides the definition, not the extern). Storage with
  no CodeView public symbol is a module-private `DATA(<VA>) static T g;` definition in the one TU
  that references it. A cross-TU external must have a retained public symbol and belongs in that
  symbol's owner header; do not create def-less synthetic externs.
  Win32 from the custom minimal `include/win/windows.h`, CRT from real `<io.h>`/`<string.h>`.
  Include the **specific** headers a TU uses — there is no `_all.h` or `_globals.h` umbrella (both
  dissolved). The six build gates (`docs/build-asserts.md`) enforce all of this; bootstrap an
  owner header with `gen_module_header.py`, global defs with `gen_global_defs.py`.

## Matching campaign (parallel by default)

- **Two global phases:** until total SOURCE fuzzy reaches **95%**, prioritize complete body,
  type/layout, frame/slot, CFG, inline-accessor, and relocation recovery. At a compiler-shape
  wall, try only a few obvious semantics-preserving spellings, document the byte residual, and
  move on; do not run extended permutation or compile-search loops because later shared-header
  recovery can invalidate that tuning. At 95% total SOURCE fuzzy, start the systematic last-mile
  wall-breaking and audited AST-permutation pass.

- **`/match [n]`** drives the campaign (default n=4). It builds the queue, fans out
  `n` **matcher** agents across a reused `matcher-1..n` worktree pool, and integrates
  results **serially into linear `master`**. See `.claude/agents/orchestrator.md`.
- **matcher** (`.claude/agents/matcher.md`) reconstructs one TU; uses `od_slots.py`
  for stack naming; reports a diff. Single worker — never spawns subagents.
- Queue: `python3 -m homm2.match.gen_queue` → `config/match-queue.md` (recovered functions
  not yet bodied, size-band order).
- Baseline: `homm2 status update` records each function's **max fuzzy% keyed by a hash of its
  SOURCE block** to `config/match_baseline.tsv` (`unit<TAB>fn<TAB>max%<TAB>src_hash`). Max% only
  resets when THAT function's own source changes; a sibling edit that perturbs it (tu-cumulative)
  leaves the hash — and the max — intact. So **you don't chase %**: match a function to 100%, and
  even if a later sibling drops its live %, its max% stays 100% and current recovers in a second
  pass. **max% == 100% for every function ⇒ essentially done.**
- `homm2 status check` fails ONLY when an *edited* function (hash changed) fell below its former
  max% — i.e. your edit lost ground. tu-cumulative dips are not flagged (no more blanket
  `--accept-regressions`). README shows **Fuzzy** (live) and **Fuzzy-max** side by side.
- Status shows **Functions exact** (completion) + **Fuzzy** (weighted progress — the
  signal that moves as you grind) + **functions-at-max-100%**. No exact-byte "Code" column (it's a
  restatement of Functions-exact that stays ~0 until the hard functions finish).
- **⚠️ Fuzzy% lies about frame slots.** It gives partial credit for a wrong `-0xN(%ebp)`
  displacement, so a function with EVERY local mis-slotted can read 97%+. When grinding
  the last few %, **diff your obj vs the target with `(%ebp)` displacements VISIBLE**
  (normalize jump/call targets only) — a "stuck on jmps" plateau is often a hidden
  slot-hash miss. Fix names with `od_slots.py`. See `docs/patterns/od-hash-slots.md`.

## Key references

- `docs/build-asserts.md` — the six hard build gates (what `homm2 build` enforces + why).
- `docs/od-stack-layout.md` — the /Od name-hash + per-scope tables (the matcher's superpower).
- `docs/patterns/INDEX.md` — codegen idiom catalog (grep by symptom/tag when a diff row sticks).
- `docs/compiler-detection.md`, `docs/linker-flags.md` — toolchain facts.
- `scripts/od_slots.py` (predict/solve slots) · `scripts/od_oracle.py` (verify vs real cl).
- `scripts/match_variants.py` — **preferred exact-only last-mile search frontend**. It combines
  hardened libclang AST mutations, parser-visible TU-state variants, and optional hand-authored
  exact-span axes in one validated, family-balanced search. Use shallow bounded searches and
  repeatable `--require-mutation` filters only after semantics, CFG, frame/slots, and relocations
  align. AST-correct ranges do not prove semantic equivalence, so inspect every candidate and
  retain generated source only for audited exact 100% closure. It will not fix a slot miss; use
  `od_slots.py` for that. `permute_ast.py` and `tu_state_noise.py` remain compatibility/diagnostic
  frontends, not the default campaign entry point. A reviewed MSVC 4.2 compatibility error outside
  the target may be named exactly with repeatable `--allow-external-diagnostic`; target-local,
  fatal, foreign-file, and unmatched diagnostics still fail closed and allowances enter the manifest.
- `homm2 sema <cmd>` — semantic navigation (xref/disasm/strings/match/rva/clangd; see
  Build loop). Modules in `scripts/homm2/analysis/`; Ghidra pipeline in `scripts/homm2/ghidra/`.
- `editor/nvim` — `:Homm2` in-editor diff/build/status (auto-loaded by the dev-shell).

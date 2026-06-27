# homm2.nvim

In-editor [objdiff](https://github.com/encounter/objdiff) for the **homm2**
binary-matching project (HEROES2W.EXE). For the function under your cursor: its
**target** (retail) asm, its **base** (recompiled) asm, a **side-by-side diff**,
and its **match %** — without leaving Neovim to start the objdiff TUI. Plus a
one-key **build** that recompiles and tells you what moved.

Presentation only — no extra tool. Every asm/diff view is one
`objdiff-cli diff … --format json` invocation, and the function under the cursor
resolves through the data the pipeline already emits:

- `build/gen/symbol_names.csv` — the join table: retail address → mangled symbol
  + unit.
- `build/objdiff/report.json` — per-function match %.

The retail **address** in each function's `VA(0x…, 0x…)` macro (see
`include/va.h`) is the key. It moves with the function text, so cursor→function
resolution never goes stale — unlike line tables. (Those addresses come from the
EXE's CodeView debug info via the labeler; there is no Ghidra in this project.)

## Interface

`:Homm2 {target|base|diff|status|hints|autobuild|autoformat}` (tab-completable)
and `:Homm2Build [args]`.

Buffer-local chords on C/C++ buffers:

| chord | action                                                         |
|-------|----------------------------------------------------------------|
| `vt`  | **target** asm of the function at the cursor                   |
| `vb`  | **base** asm                                                   |
| `vd`  | **diff** — target ∣ base side-by-side (the "objdiff look")      |
| `vs`  | **status** — match-% overview, current unit's functions listed |
| `vB`  | **build** (`:Homm2Build`)                                      |
| `vq`  | **close** all open homm2 views                                 |
| `V`   | **peek** — match % + metadata (size, rva, unit) in a float     |

`vt`/`vb`/`vd` keep the cursor in your source buffer (the view opens without
stealing focus), and a new view **replaces** the previous one (so `vd` on a new
function closes the old `vd`). The diff shows the **whole function** (folding off),
target on the left and base on the right.

- **target / base** open a reusable scratch split (`filetype=asm`, so mnemonics/
  registers/labels are syntax-highlighted); the header carries the symbol and its
  match %. Intra-function jump targets get synthetic labels (`.L1:` …) and
  branches reference them (`jne short .L1`) instead of raw offsets. When a **base**
  view is open and a build changes the base, it turns into a `previous build → now`
  diff — you see exactly what your edit did to the compiled output. (Target asm is
  fixed by the retail binary, so it just re-renders.)
- **diff** opens two scratch buffers in a native Neovim **diff** split (target
  left, base right): scrollbound, with diverging instructions highlighted. Each
  pane's winbar labels it `TARGET (retail)` / `BASE (recompiled)` with the % so
  the sides are never ambiguous. The jump labels also keep the diff tight — a
  branch matches on both sides even when its byte offset shifts.
- **status** lists every started unit by match % (worst first); the current
  file's unit is marked `>` and expanded to its functions. `<CR>` on a function
  row opens its diff.
- **peek** floats the current function's match % and metadata (size, retail
  address, unit progress) — instant, no objdiff call; inside any plugin view,
  `<CR>`/`V` instead follow a `0x…` (or status row) to that function's diff.
  `q` closes.

## Inline % hints

Like coc's inlay hints: every `VA(0x…, 0x…)` line in a source buffer is tagged
with its function's match % as end-of-line virtual text, colored by how close it
is (`✓ 100%` green, partial yellow, low red, `— n/a` if not built). So the match
state is visible *as you read the code*, no command needed. Refreshed on
enter/save and after `:Homm2Build`. Toggle with `:Homm2 hints`, or disable by
default with `hints = false` in `setup`.

`:Homm2Log` shows the resolved `objdiff-cli` path and every invocation — handy
when a stale dev shell ships an old binary.

## `:Homm2Build` — build and report what moved

`vB` snapshots the current `report.json`, runs `homm2 build`, and on success
shows a **compact corner popup** with the build time, the overall % change, and
the current unit's `before → after` deltas for the functions that moved
(`↑`/`↓`/`+`, capped — `vs` has the full table). The popup never steals focus
(handy when the async build lands while you're typing elsewhere) and fades after
a few seconds. Extra args pass through to the build (forwarded to `ninja`), e.g.
`:Homm2Build build/objdiff/base/SOURCE/GAME.obj` to limit it to one TU.

> **Launch nvim from `nix develop .#build` for fast builds.** The build then runs
> `homm2 build` **directly** (the MSVC 4.2 + wine toolchain is already on the
> env). Launched from the plain `nix develop` shell it still works, but each build
> is wrapped in `nix develop .#build`, which adds shell-setup overhead *per
> build*. The plugin auto-detects which shell it's in (via `$MSVC_DIR` +
> `$WINEPREFIX`).

### Build on save (`:Homm2 autobuild`)

Off by default. When on, **saving a TU** (`*.c`/`*.cpp`/`*.cc`) recompiles **only
that file's unit** (`homm2 build build/objdiff/base/<unit>.obj` — a single
`ninja` target) and re-diffs it with `objdiff-cli diff -u <unit>` against the
**cached target objs** — **no log window**, just a small `building …` corner note
that closes into a result popup — so the **inline %s update right after you save**
(and any open `vd`/`vt`/`vb` view for that unit re-renders in place, keeping your
cursor and scroll). The tight loop: edit a function, `:w`, watch its `%` move.
Enable per session with `:Homm2 autobuild`, or by default with
`build_on_save = true` in `setup`. **Rapid saves supersede:** a new save cancels
the in-flight one (latest wins), so saving several times quickly doesn't queue
builds or race `ninja` on `build/`.

The overlay %s use `function_reloc_diffs=none`, the same setting
`objdiff-cli report generate` uses, so they match the report's
`fuzzy_match_percent` exactly (no number jump). It only re-diffs the **edited
unit** — the target side is fixed by the retail EXE, so a body edit can't change
it.

> **Note.** `homm2 build` always re-runs `configure.py` and regenerates the
> all-units `report.json` (+ the README % block) after `ninja`, regardless of the
> target — the `homm2` CLI has no "single-unit, skip the report tail" fast path.
> So a save-build does more work than the bare `cl` for the edited unit; the live
> per-unit overlay still updates immediately, and a follow-up full build stays
> consistent. **Tradeoff:** between full builds the **overall %** and **other
> units'** numbers (the `vs` status view) reflect whatever the last `homm2 build`
> regenerated. Run **`vB` / `:Homm2Build`** for the full pipeline; that's also
> what picks up a **brand-new function** you just added (and a save in a file with
> no `VA(...)` yet falls back to a full build automatically, to wire the new unit
> into the graph).

### Format on save (`:Homm2 autoformat`)

Off by default. When on, **saving a source file** under `src/` or `include/`
(`*.c`/`*.cpp`/`*.cc`/`*.cxx`/`*.h`/`*.hpp`/`*.hh`) runs `clang-format` on **just
that file** in place before it hits disk — using the root `.clang-format` (the
same config the repo's `.githooks` pre-commit auto-format uses, so the same
whitespace-only, matching-neutral result), scoped to the file you're editing.
It's a no-op on already-formatted files (the buffer and its undo history are left
untouched), skips `src/_external/` and anything outside `src/`/`include/`, and
preserves your cursor/scroll. Enable per session with `:Homm2 autoformat`, or by
default with `format_on_save = true` in `setup`. Needs `clang-format` on `PATH`
(the dev shell provides it via `llvmPackages.clang-unwrapped`).

The `autobuild`, `autoformat`, and `hints` toggles are **remembered per checkout**
in `build/homm2-nvim.json` (gitignored), so each worktree keeps its own setting
across restarts; a remembered toggle wins over the `setup` default.

## Requirements

`objdiff-cli` on `PATH` — launch nvim from the project dev shell (`nix
develop`). A populated `build/` (run `homm2 build` once if it's a fresh
checkout). The build command additionally needs the `.#build` shell (MSVC 4.2 +
wine), which it enters itself.

## Install

**You usually don't have to.** The project dev shell wraps `nvim` so it
auto-loads this plugin — when you `nix develop` (either shell) you'll see:

```
[homm2] nvim       : WRAPPED -> auto-loads editor/nvim (:Homm2).
```

The wrap is a small script on `PATH` (`build/nvim-shim/nvim`) that execs your own
nvim with `--cmd "set rtp^=…/editor/nvim"` (so your config and plugins are
untouched, and plain `nvim` outside the shell is unchanged). Defined in
`flake.nix` (`nvimShimHook`).

To load it **everywhere** (outside the dev shell too), add it to your plugin
manager instead — e.g. lazy.nvim, pointing at this in-repo directory:

```lua
{ dir = "~/Projects/homm2/homm2-decomp/editor/nvim", ft = { "c", "cpp" } }
```

Optional config (defaults shown):

```lua
require("homm2").setup({
  keymaps = true,            -- set false to bind your own
  hints = true,              -- inline match-% after each VA(...)
  build_on_save = false,     -- rebuild quietly on every TU save
  format_on_save = false,    -- clang-format the saved file (src/ + include/)
  split = "botright vsplit", -- where asm/status views open
})
```

Outside a homm2 checkout (no `config/units.toml` above the buffer) the plugin
stays inert.

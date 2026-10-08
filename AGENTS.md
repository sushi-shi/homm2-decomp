# HoMM2 Buka reconstruction

Byte-matching C++ reconstruction of Buka's Gold 2.1 Windows `HMM2PL.exe`
(`config/retail/targets.json`), built with the pinned Visual C++ 6.0 SP5
toolchain. Retail bytes are the authority. Every function and data byte
matches, and both the game and the scenario editor (`EDT2PL.exe`, the second
image: `homm2 --image editor`, [docs/editor.md](docs/editor.md)) link
byte-identical to retail.

## Build and gates

- Work inside `nix develop .#build` in the assigned worktree (`HOMM2_DIR`
  resolves there; `HOMM2_JOBS` caps parallel work). Run `homm2 build` after any
  source, claim, config or tooling change; run `homm2 delink` first after a
  claim, signature or data-owner change.
- Every commit keeps `homm2 build verify` passing (every function and data byte
  exact, both linked images byte-identical) and `homm2 clean --verify`
  building the generated tree. Never weaken a gate to pass it.
- `homm2 build verify` includes `homm2 verify behaviour`, the game-behaviour
  tests (`scripts/homm2/verify/behaviour/`). They assert retail behaviour, so
  change one only on purpose.
- The README status block is generated (`homm2 verify readme`); never edit
  it by hand. `homm2 verify bank` updates the score ledger
  ([score tracking](docs/match-status.md)).

## Source rules

- Ordinary C++ with real types. No decompiler output, byte blobs, naked
  assembly, dummy bodies, address masking, C-style casts, or compiler-state
  steering in retained source; game code uses the `H2/Ints.h` aliases.
- One TU is `src/<TIER>/<TU>.cpp` with its declarations in
  `include/<TIER>/<TU>.h`, functions in retail-RVA order, marked `VA(...)`,
  globals `DATA(...)`, vtables `VTBL`/`VTBL2`. Data identities, types and
  initializers come from retail bytes and their code users.
- Compiler profiles live in `config/units.toml`; changing one needs
  retail-backed evidence.
- Comments describe behaviour or an enduring codegen fact, never queue state,
  scores, maxima or how a match was achieved.
- What a score or retained MAX proves: [evidence](docs/workflow.md#evidence).
- The full rules: [source](docs/workflow.md#source-rules) and
  [data and linking](docs/workflow.md#data-and-linking).

## Layout

| Path | Contents |
| --- | --- |
| `src/{BASE,SOURCE,EDITOR}`, `include/` | reconstructed source, resource scripts and owner headers |
| `imports/`, `locales/` | reviewed import ABI, text catalogs |
| `vendor/` | third-party SDK headers (Audiere, Miles, Smacker, WinG) |
| `config/`, `config/retail/`, `config/reviews/` | build contracts; retail facts; review ledgers |
| `scripts/homm2/` | tooling; keep `homm2.core.usage.logged` on entry points (`homm2 audit usage`) |
| `docs/`, `docs/patterns/` | reference docs; measured compiler mechanisms |
| `build/` (ignored) | retail images, toolchain, Wine prefix, generated files |

## Branches

`decomp-gold-2.1-buka` (this branch) generates `source-gold-2.1-buka` and
`classic-gold-2.1-buka` with `homm2 clean`; never edit generated branches. The
cross-platform `port` builds on `source-gold-2.1-buka`. Commit and staging
rules are in [docs/workflow.md](docs/workflow.md#git).

## Skills

`.agents/skills/` (linked from `.claude/skills`): `matcher` for
reconstruction, `permute` for compiler-state experiments, `orchestrator` for
parallel lanes with serial integration.

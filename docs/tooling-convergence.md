# Tooling convergence plan

This repository and the HoMM1 reconstruction (the sibling `homm1-decomp`
repository, branch `decomp-buka-2003`) reconstruct two Buka 2003 releases
built with the same VC6 SP5 toolchain. HoMM1 reconstructs two linked programs
(the game and the scenario editor); this repository starts the second program,
the editor `EDT2PL.exe`, from the same disc and build as `HMM2PL.exe`. The
plan below converges the two toolchains so that one person can work in either
repository with the same commands, the same configuration layout and the same
documentation shape.

This document is the migration ledger. Each row records a decision:

- **adopt** - take the HoMM1 shape (command, layout or module) and fill it with
  this target's facts;
- **keep** - keep this repository's implementation, with the reason;
- **merge** - adopt the HoMM1 surface (name, options, location) over this
  repository's implementation;
- **defer** - adopt later, when the work that needs it starts.

## Target end state

- The `homm2` CLI has the `homm1` command surface: `init inspect toolchain
  configure build link match play labels model delink compare audit sema walls
  permute lsp ghidra verify workflow clean localization tool`, with
  each old spelling kept as an alias while other branches use it.
- `homm2 --image editor <command>` selects the editor for any command and is
  exported to child processes as `$HOMM2_IMAGE`. Every authoritative fact is
  keyed by `(image, rva)`: the game keeps `config/retail/` and `build/`; the
  editor uses `config/retail/editor/` and `build/editor/`.
- `config/retail/targets.json` pins both executables; `homm2 init --exe
  --editor-exe` stages and verifies them.
- `config/` holds build contracts; `config/retail/` holds retail facts by kind
  (`functions`, `absolute_relocations`, `data_compgen`, `link_order`, ...), one
  set per image.
- `config/units.toml` lists each unit's `images` and per-image defines
  (`[images.editor]`).
- Every entry point logs its use (`homm2.core.usage.logged`); `homm2 audit
  usage` checks coverage.
- The README carries a generated status block per image (`homm2 verify
  readme`, refreshed by `homm2 build`).
- `AGENTS.md` has the short contract shape (build and gates, source rules,
  layout, branches, skills); `CLAUDE.md` links to it. Long-form workflow lives
  in `docs/` and `.agents/skills/`.
- Gates run through `homm2 build verify` per image, including the linked-image
  comparison (`config/link_diff.tsv`, `config/retail/editor/link_diff.tsv`).

## Invariants at every step

Every migration step is committed only when all of these hold on the game:

| Guarantee | Check |
| --- | --- |
| the build and its hard gates | `homm2 build` |
| 1727 / 1727 functions exact, 291,995 / 291,995 data bytes, 97 / 97 data units | `homm2 build` (README block and `homm2 status`/`verify status`) |
| strict data allocations | `homm2 strict-allocations` (later `homm2 verify strict-allocations`) |
| linked `HMM2PL.exe` byte-identical to retail (ceiling 0 in every region) | `ninja link-diff` (later `homm2 verify link-diff`) |
| game behaviour | `homm2 verify behaviour` (later run by `homm2 build verify`) |
| generated source | `homm2 clean --verify` |

Steps run with at most four parallel jobs (`HOMM2_JOBS=4`, nix `--max-jobs 1
--cores 4`): the machine is shared.

## Commands

| HoMM1 command | This repository today | Decision |
| --- | --- | --- |
| `init --exe --editor-exe` | `init` (toolchain fetch, redelink, clangd) | merge: HoMM1 options and `targets.json` staging; keep the toolchain fetch, delink and clangd steps |
| `inspect [--target]` | none | adopt |
| `toolchain install\|check` | `python3 -m homm2.init.toolchain [--force\|--check]` | merge: verb adopted; keep the pinned VC6 SP5 release fetch |
| `configure` | `configure` (`configure.py`) | keep; image-aware |
| `build` (all images), `build verify` | `build` (ninja, gates, status, README) | merge: `build` builds every image; `build verify` runs the gate tiers; keep `--no-match --ru/--en` (locale builds have no HoMM1 counterpart) |
| `link` | `link [--rsrc\|--historical]` | merge: keep the three modes (the historical link is the byte-identical one); image-aware |
| `match UNIT` | `ninja <obj>` then `homm2 status` | adopt: selected-unit compile, compare and per-function report |
| `play` | `nix run`, `scripts/toolchain/create-wine-prefix.py` | adopt verb over the existing runner |
| `labels` | `homm2.build.source_symbols` (inside redelink) | merge: verb over the existing claim extraction |
| `model` | `model-drift` | merge: `model` reports the claimed inventory; drift check kept |
| `delink` | `redelink [--force]` | merge: `delink` is the name; `redelink` stays an alias while other branches use it |
| `compare` | objdiff run inside `status` | merge: verb over the existing report step |
| `verify status\|check\|bank\|readme` | `status`, `status update`, `status --write-readme` | merge: HoMM1 verbs over `match/status.py`; old spellings stay aliases |
| `verify <gate>` | `relocs`, `constants`, `od-frames`, `strict-allocations`, `data-relocs`, `data-topology census`, the `assert_*` modules, `link-diff` (ninja) | merge: every gate reachable as `homm2 verify <gate>`; old top-level verbs stay aliases |
| `sema` | `sema` (rva, xref, disasm, strings, match, symbol, def, refs, rename, decomp) | keep the implementations; add `-` batch, `vtable`, `gaps`, `map`, `frame` when needed (defer) |
| `walls` | none (`match/residual_queue.py`) | defer: the game has no residuals; adopt for editor residuals |
| `permute candidates\|campaign\|state\|variants` | `permute` (variants), `tu_state_noise` module | merge: `state` and `variants` verbs; keep `emission-order` and the recovery drivers; `campaign` deferred |
| `lsp` | `sema symbol/def/refs/rename`, `clangd` | merge: `lsp` verb over `clangd_query`; `clangd` becomes `lsp index` |
| `ghidra` | `ghidra` | keep |
| `workflow setup\|format-staged` | `format [--check]` | merge: `workflow format` keeps the header/enum formatter |
| `clean` | `clean` (`--verify`, publish) | keep the implementation; editor export added with the editor |
| `localization` | `homm2.build.localization` (run by build) | adopt verb |
| `tool <name>` | `core.wine`, `cc_wrap`, `ml_wrap`, `native_link` | adopt thin drivers: `wine cl ml link rc objdiff delinker` |
| `audit usage\|census\|dna-bands\|placements` | `audit` (16 campaign audits) | merge: add the four HoMM1 audits; keep every existing audit |
| (none) | `selftest` | removed (see "Tests" below); `verify behaviour` replaces it |

## Packages

| HoMM1 package | This repository today | Decision |
| --- | --- | --- |
| `core` (paths, inputs, image, pe, tsv, coff, usage, msvc_names, od_slots) | `core` (paths, retail, coff, manifest, od_slots, wine) | merge: image-aware `paths`, `inputs` (pins), `image`, `pe`, `tsv`, `usage`; `retail.py` replaced by the pins |
| `manifest` | `core/manifest.py` | merge: `homm2.manifest` with `images` and `[images.<key>]` |
| `graph` (ninja emitter, verbs, link, play, localization) | `build/configure`, `cc_wrap`, `ml_wrap`, `link_exe`, `native_link`, import libraries, `rc_res`, `localization`, `catalog`, `ordinary` | merge: move into `homm2.graph`; keep this repository's link pipeline (it produces the byte-identical image) |
| `retail_labels` | `build/source_symbols`, `annotated_*`, `symbol_providers` | merge: move |
| `delink` | `redelink.py`, `build/synth_pdb`, `reviewed_data`, `candidate_data_manifest`, `data_manifest_adapter`, `reloc_owners` | merge: move; keep the stripped-image relocation manifest and data-manifest pipeline |
| `compare` | `build/canonicalize_*`, `normalized_freshness`, objdiff run | merge: move |
| `verify` | `build/assert_*`, `link_diff`, `od_frame_audit`, `strict_allocations`, `coff_reloc_topology`, `symbol_model_drift`, `constants_audit`, `match/status`, `match/source_hashes` | merge: move; gate registry and tiers adopted |
| `sema` | `analysis` | merge: rename |
| `lsp` | `analysis/clangd_query`, `init/clangd` | merge: move |
| `walls` | `match/residual_queue` | defer |
| `permute` | `permute` | keep (same lineage) |
| `audit` | `audit` | merge (see commands) |
| `tool` | `core/wine` and wrappers | adopt drivers |
| `toolchain` | `init/toolchain` | merge: move |
| `workflow` | `format` | merge |
| `clean` | `clean` | keep |
| `ghidra` | `ghidra` | keep |
| `model` | none | inapplicable: `build/gen/symbol_names.csv` is the claimed model |

## Configuration

| HoMM1 file | This repository today | Decision |
| --- | --- | --- |
| `config/retail/targets.json` | `core/retail.py` hash | adopt (game and editor pins) |
| `config/retail/functions.tsv` | `config/retail_functions.csv` | merge: move to `config/retail/functions.csv` (kind name; the CSV schema is kept because 13 readers and the delinker share it) |
| `config/retail/absolute_relocations.tsv` | `config/delink_relocs.tsv` | merge: move to `config/retail/absolute_relocations.tsv` |
| `config/retail/functions_static_libs.tsv` | `config/crt_functions.csv` | merge: move to `config/retail/functions_static_libs.csv` |
| `config/retail/data_compgen.tsv` | `config/compiler_generated_data.tsv` | merge: move |
| (census kinds) | `compgen_functions.csv`, `eh_funclets.csv`, `import_thunks.csv` | merge: move under `config/retail/` with their kind names |
| `config/retail/reloc_referents.tsv` | `reloc_data_owners.tsv`, `reviewed_rel32_aliases.tsv`, `delink_reloc_{aliases,exclusions,inclusions}.tsv`, `delink_text_exclusions.csv` | merge: move under `config/retail/` (kind names kept; they are reviewed delinker channels) |
| `config/retail/link_order.tsv` | `units.toml` order, `retail_crt_order.txt` | merge: move `retail_crt_order.txt` to `config/retail/link_order_crt.txt`; units.toml order stays the object order |
| `config/retail/dna_bands.tsv` | none | adopt (`audit dna-bands`) |
| `config/retail/<image>/placements.tsv` | none | adopt (`audit placements`) |
| `config/link_diff.tsv`, `config/retail/editor/link_diff.tsv` | `config/link_diff_ceiling.tsv` | merge: rename; per-image ceilings |
| `config/match_baseline.tsv`, `match_baseline.<image>.tsv` | `config/match_baseline.tsv` | keep the schema (hash-scoped MAX); per-image file adopted |
| `config/units.toml` `images`, `[images.<key>]` | `config/units.toml` | merge |
| `config/reviews/` | `retail_bool_exceptions.tsv`, `retail_cast_exceptions.tsv` | merge: move to `config/reviews/` |
| `config/retail/` remaining | `required_initialized_storage.tsv` | merge: move under `config/retail/` |
| `config/README.md` | none | adopt |
| `config/compare.toml` (`data_matching`) | strict data always on | inapplicable: comparison is always strict here |

## Documentation and repository shape

| HoMM1 | This repository today | Decision |
| --- | --- | --- |
| `AGENTS.md` (short contract), `CLAUDE.md -> AGENTS.md` | `AGENTS.md` (matching guide), `CLAUDE.md` (project guide) | merge: one contract-shaped `AGENTS.md` keeping this repository's rules; the matching loop moves to `docs/workflow.md`; `CLAUDE.md` becomes the link |
| `.agents/skills/`, `.claude/skills ->` | `.claude/agents/`, `.claude/skills/permute` | merge: canonical `.agents/skills/`, `.claude/skills` links there |
| README: headline, generated block per image, branches, quickstart, license | README with one generated block, long build/play/link prose | merge: HoMM1 shape; the long prose moves to `docs/` |
| `docs/tooling.md`, `tooling-map.md`, `build-system.md`, `match-status.md`, `workflow.md`, `editor.md` | `docs/README.md`, `build-asserts.md`, `retail-exact-link.md`, others | merge: add the HoMM1 set; keep the evidence documents |
| `docs/patterns/` | `docs/patterns/` | keep |

## Gates

| HoMM1 gate | This repository today | Decision |
| --- | --- | --- |
| MAX regression (`verify check`) | `status` (observation only) | merge: `verify check` fails on a live loss |
| `link-diff` per image | `link_diff.py` ceiling (ninja default) | merge: per-image ceiling, `verify link-diff [--update]` |
| `strict-view` | `strict_allocations` | keep (same role: strict data identity) |
| `constants` | `constants_audit` | merge verb |
| `enum-reuse` | `audit enums` | keep as audit (no review ledger here yet) |
| `localization` | `build.localization` (byte-preserving check in build) | merge verb |
| clean exports | `clean --verify` | keep |
| play runner | `run-rebuilt-game.py` | merge verb |
| `assert_*` hard gates | `build/assert_*` | keep; reachable as `verify <gate>` |
| usage coverage | none | adopt (`audit usage`) |
| (none) | tool test suite (`selftest`) | removed; the game-behaviour tests are the `verify behaviour` gate |

## Tests

The tool test suite (`homm2 selftest`, 81 `test_*.py` modules, 986 cases) is
removed with its runner. It tested matching and tooling internals; the gates
above, which check the reconstruction itself, are the guarantees. Removed:

- `analysis`: disasm, od_frame_names
- `audit`: bool_fields, casts, cross_version, cross_version_bodies, data_claims,
  enums, gotos, historical_exact_losses, ledger, object_equivalence, od_oracle,
  pattern_catalog, readability, reconstruction, reloc_donation, reloc_sweep,
  scan_bitfield_residuals, strict_allocation_diff, unmatched_census
- `build`: annotated_compgen_data, annotated_data, annotated_functions,
  annotated_vtables, assert_fixed_width_ints, assert_relocs,
  candidate_data_manifest, canonicalize_data_symbols, canonicalize_relocs,
  clang_cxx11, coff_reloc_topology, configure_link_graph,
  data_manifest_adapter, data_topology_census, extract_resources,
  gen_vendor_imports, import_lib, legacy_import_lib, link_diff, link_exe,
  localization (tool mechanics), ml_wrap, native_link, normalized_freshness,
  od_frame_audit, ordinary, rc_res, regular_import_lib,
  regular_vendor_import_lib, reloc_owners, reviewed_data, source_symbols,
  strict_allocations, symbol_model_drift, symbol_providers, synth_pdb
- `clean`: clean_source; `core`: coff, manifest, retail, wine; `format`: enums,
  headers; `init`: clangd, init, toolchain; `match`: residual_queue,
  source_hashes, status
- `permute`: batch_source_variants, emission_order, generate_ast_variants,
  match_variants, recover_historical_exact, recover_residual_functions,
  tu_state_noise (and `permute/testdata/`)
- top level: cli, constants_audit, redelink, wine_launch_contract

Kept as game-behaviour tests under `scripts/homm2/verify/behaviour/`, run by
`homm2 verify behaviour`:

- `test_game_contracts` (was `homm2.audit.readability_contracts` and its
  fixture, now `game_contracts.cpp`): compiled against the game headers with
  the SOURCE/KB profile, linked with VC6 and run under wine. It checks enum
  protocol values, CP1251 case folding, creature classification, the map-cell
  sprite predicate, the barrier-tent mask, combat-hex and mage-guild formulas,
  Manhattan and integer vector lengths, and the file-value record round trip.
- `test_localization_text` (four cases from the former localization tests):
  original English wording, fragments that render complete combat, trading
  and requester sentences, and no text outside the catalog.

Open items:

- More behaviour coverage: save and map record round trips, the creature,
  spell and artifact tables, damage, cost and experience formulas, and
  pathing and AI decisions on fixtures.
- HoMM1 has no behaviour gate. The same `verify behaviour` gate could be
  mirrored there. That repository is not changed by this plan.

## Migration order

Each step is one or more commits; every commit holds the invariants above.

1. **Usage logging.** `homm2.core.usage` with `logged`; decorate every entry
   point (`cli.main`, every `python3 -m` module main, ninja-invoked wrappers);
   `homm2 audit usage`. Replaces the ad hoc `build/homm2_sema.log`.
2. **Image keying.** `config/retail/targets.json` (game and editor pins),
   `homm2.core.inputs`, image-aware `homm2.core.paths` (`IMAGE_ENV`,
   `image_key`, `retail_dir`, `image_build`, `retail_exe`), `homm2 --image`,
   `init --exe --editor-exe`, `inspect`. `core/retail.py` reads the pin.
3. **Retail facts by kind.** Move the retail tables into `config/retail/`
   and the reviews into `config/reviews/`; rename the link ceiling to
   `config/link_diff.tsv`; `config/README.md`.
4. **Command surface.** HoMM1 verbs over the existing implementations
   (`verify`, `match`, `delink`, `labels`, `compare`, `lsp`, `workflow`,
   `toolchain`, `localization`, `tool`, `play`), old spellings as aliases.
   `homm2 build verify`.
5. **README and per-image status.** HoMM1 README shape; `verify readme`
   renders one block per image; the per-image ledger file.
6. **Package layout.** Mechanical moves (`analysis -> sema`, `build ->
   graph/retail_labels/delink/compare/verify`, `init -> toolchain/lsp`,
   `format -> workflow`, `core/manifest -> manifest`), updating ninja rules,
   tests and docs in the same commit per package.
7. **AGENTS.md, skills and docs.** Contract-shaped `AGENTS.md`,
   `CLAUDE.md` link, `.agents/skills/`, `docs/tooling.md`,
   `docs/tooling-map.md`, `docs/build-system.md`, `docs/workflow.md`.
8. **Editor image** (separate phase): the census, DNA and placement audits;
   `config/retail/editor/`; the image-aware delink, compare and link; the
   editor's compiler profiles; the editor units.

Steps 1-3 are prerequisites of the editor work; steps 4-7 can interleave with
it. A step that cannot hold an invariant stops and is recorded here before
anything else changes.

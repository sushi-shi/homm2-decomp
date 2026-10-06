# Command map

All commands are `homm2 [--image {game,editor}] <command>`, implemented in
`scripts/homm2` and recorded by the shared usage logger. `--image` selects the
retail image (exported as `$HOMM2_IMAGE`); a command not yet keyed by image
refuses another image rather than answering for the game.

| Command | Implementation | Purpose |
| --- | --- | --- |
| `init [--exe] [--editor-exe]` | `init` | Stage and verify the retail images, fetch the toolchain, delink, write the clangd database |
| `inspect [--target] [--json]` | `cli`, `core.image` | Headers of a pinned retail image |
| `toolchain install\|check` | `toolchain` | The pinned VC6 SP5 release in `build/toolchain` |
| `configure`, `build`, `build verify`, `link` | `graph`, `verify` | Graph emission, the matching build, the gate tier, the native link (`--rsrc`, `--historical`) |
| `build --no-match [--ru\|--en]` | `graph.ordinary` | A locale build without retail inputs |
| `match UNIT...` | `cli`, `sema` | Selected-unit compile, comparison and per-function report |
| `play [--game DIR]` | `scripts/toolchain/run-rebuilt-game.py` | Build, link with resources and run the game in its own Wine prefix |
| `labels`, `model`, `delink`, `compare` | `retail_labels.source`, `verify.model_drift`, `delink.run`, `verify.status` | Claimed inventory, model drift, the delinked target, the objdiff report |
| `verify status\|check\|bank\|readme` | `verify`, `verify.status` | Metrics, the exactness gate, retained maxima, the README block |
| `verify <gate>`, `verify all` | `verify` | One gate, or the tier `build verify` runs ([build-asserts](build-asserts.md)) |
| `verify behaviour` | `verify.behaviour` | Game-behaviour tests of the reconstructed code |
| `sema` | `sema` | Retail and source navigation: rva, xref, disasm, strings, match, frames |
| `lsp` | `lsp` | clangd database (`compdb`) and queries (symbol, def, refs, hover, rename) |
| `permute` | `permute` | Source variants, TU-state censuses, emission order, queue drivers |
| `audit <tool>` | `audit` | On-demand campaign audits, usage-logging coverage |
| `ghidra` | `ghidra` | The optional Ghidra project |
| `workflow format [--check]` | `workflow` | Header and enum formatting |
| `clean` | `clean` | The generated source trees ([clean source](clean-source.md)) |
| `localization` | `graph.localization` | The text-catalog source gate ([localization](localization.md)) |
| `tool <name>` | `cli`, `tool.wine` | One era tool: `wine cl ml link rc objdiff delinker` |

Earlier spellings stay as aliases: `redelink` (`delink`), `status`
(`verify status`), `relocs` (`verify relocs`), `constants`,
`strict-allocations`, `od-frames`, `data-relocs`, `data-topology` (`verify
<gate>`), `model-drift` (`model`), `clangd` (`lsp compdb`), `format`
(`workflow format`).

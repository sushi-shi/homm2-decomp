# Builds and the native link

## Build without matching

Russian is the default locale. `homm2 build` (or `homm2 build --ru`) is the
Russian retail-comparison workflow; it compiles objects and refreshes matching
reports, but linking is a separate command.

For an ordinary executable, with no retail executable or delinked targets needed:

```sh
nix develop .#build
homm2 toolchain install          # one-time compiler setup; no matching setup
homm2 build --no-match --ru        # compile and link Russian (default)
homm2 build --no-match --en        # compile and link English
```

Outputs are `build/ordinary/ru/HMM2PL.exe` and `build/ordinary/en/HMM2PL.exe`.
Objects and Ninja state are separate for each locale. These commands do not
update matching objects, scores, reports, or the README. `--en` requires
`--no-match`: English is not the Buka matching target. Both modes accept `-j N`
and `-v`.

Like the generic link, these builds omit Windows resources and the retail icon;
they select catalog text, not game assets. You still need a compatible installed
game and its middleware DLLs. This does not translate text embedded in external
maps, graphics, videos, or other assets. The existing `nix run` play workflow
below still uses the Russian resource build, not these locale outputs.

Generated `source-gold-2.1-buka` preserves the same IDs and catalogs, with its own
modern build of the game and the scenario editor (`./build.py --ru` /
`./build.py --en`, `--target game|editor|all`). Generated
`classic-gold-2.1-buka` instead resolves IDs into readable Russian UTF-8 literals
and remains a reading-only view. See [localization](localization.md).


## Native linking

All supported modes drive the pinned `LINK.EXE` with raw compiler/assembler
outputs and import libraries generated from the reviewed `imports/*.def` ABI
manifests. LINK writes the final executable directly.

- `homm2 link` produces `build/link/generic/HMM2PL.exe` without resources or
  access to the retail executable.
- `homm2 link --rsrc` produces `build/link/rsrc/HMM2PL.exe`, adding resources
  compiled from `res/HMM2PL.rc` and the retail-extracted program icon. The
  resource compiler's output is checked against retail.
- `homm2 link --historical` adds the observed PDB path and four-link clock
  history, producing `build/link/historical/HMM2PL.exe` directly through LINK.

The COFF/PE layout correction machinery and `--transform` mode have been removed.
Successful native linking does **not** establish an exact retail executable:
object matching and final linked placement are separate checks, and native
layout residuals remain under investigation. Exact `.bss` ownership/order and
whole-executable matching remain the objective.

`ninja link-audit` compares the historical native image with retail and
writes `build/link/historical/HMM2PL.link.json`; it fails on differences without changing
the executable. The command and evidence boundaries are documented in
[`docs/retail-exact-link.md`](retail-exact-link.md). Earlier compiler
experiments remain in [`docs/compiler-re-allocation-order.md`](compiler-re-allocation-order.md)
as historical measurements, not proofs that source/build recovery is impossible.


## Toolchain

`homm2 init` fetches the pinned `toolchain-vc6-sp5` release (SHA-256 gated, then
re-verified file by file). Rebuilding the tarball from preserved media stays
supported via `scripts/toolchain/create-toolchain-release.nix`; the builder's
docstring records the media quirks. `clang`/`clangd` is editor tooling only —
the wine VC6 build is the sole verdict on a match.

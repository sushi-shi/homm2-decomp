# Workflow

The repository layout, the matching commands and how to navigate the
source and target.

## Layout

```
src/      {BASE,SOURCE,EDITOR}   reconstructed C++ (carcass: RVA-annotated stubs -> real bodies)
include/  {BASE,SOURCE,EDITOR}   recovered class headers (vtables, OVERRIDE) + va.h / Ints.h
build/orig/   your HEROES2W.EXE (gitignored; copy it here before `homm2 init`)
config/   units.toml             per-TU build manifest
scripts/homm2/    the CLI package - one role per subpackage, mirroring the commands:
          core/ analysis/ permute/ audit/ match/ build/ clean/ format/ init/ ghidra/
          tests live beside what they test; `homm2 selftest` runs them
scripts/toolchain/  VC 4.2 + LINK 3.00 provisioning from preserved media (run once)
scripts/archive/    retired tooling, kept only to reproduce old audit-ledger commands
build/    (gitignored)           toolchain, synth PDB, delinked targets, base objs, objdiff report
flake.nix two dev shells: default (analysis+diff+clang), build (+wine+MSVC 4.2)
```

The documentation map and retention policy are in the [documentation index](README.md).

## Commands

```sh
nix develop .#build            # MSVC 4.2 under wine + the tools
homm2 init                     # ONE-TIME: CodeView -> manifest -> ??_C@ names -> PDB -> delink -> configure
homm2 redelink                 # EXPLICIT: refresh all symbol models and atomically rebuild delinked targets
homm2 build                    # compile src (wine cl) -> comparisons + hard gates -> refresh status
homm2 link                     # strict final link + section/RVA audit in build/link/
homm2 status                   # per-unit + overall match %
homm2 format --check           # verify header and enum formatting
```

The final link is opt-in, so object matching stays fast. Its Ninja graph exposes `link-order`
(NB09 `sstModule` object order), `link-imports` (exact middleware import archives), a direct
VC 4.2 `LIB.EXE` BASE-archive rule, a direct pinned `LINK.EXE` response rule, and `link-map`
(PE section, entry-point, unresolved-symbol, and per-unit RVA diagnostics). The preparation
boundary and measured link-order experiments are documented in
[`docs/native-link-pipeline.md`](native-link-pipeline.md).

`homm2 build` never runs Vostok. After adding or changing a `VA`, `VA_COMPGEN`,
`DATA`, `DATA_COMPGEN`, `DATA_COMPGEN_GUARD`, `VTBL`, or `VTBL2` identity, run
`homm2 redelink` when you want to replace the fixed target, then run `homm2 build`.
The build performs only a fast warning-only model-drift census, so a half-built TU
does not force an immediate redelink.

The matching toolchain lives in `build/toolchain/` and has two components: the VC 4.2
compiler/header tree and the older VC 4.0 final-link tree. **`homm2 init` fetches both**
from the pinned `toolchain-vc42-link300` release, checks the archive against its
recorded SHA-256, and validates the unpacked tree against the pinned per-artifact
hashes. Nothing to do by hand:

```sh
homm2 init                                    # fetches the toolchain, then redelinks
python3 -m homm2.init.toolchain --check       # re-validate an existing tree
python3 -m homm2.init.toolchain --force       # refetch over it
```

Provisioning from your own media stays supported, and is the only option for an
edition the release does not pin. Both provisioners validate the compiler, linker,
headers, import libraries, and both CRT archives before publishing a tree atomically:

```sh
scripts/toolchain/make_toolchain.py /path/to/en_vc42ent_disc1.exe
scripts/toolchain/make_linker.py /path/to/MSVC40.iso
scripts/toolchain/make_toolchain.py --check build/toolchain/msvc
scripts/toolchain/make_linker.py --check build/toolchain/link300
```

Object compilation always uses VC 4.2. When the separately pinned `link300`
component is present, `homm2 link` uses VC 4.0 LINK 3.00.5270 and its sibling
CVPACK/CVTRES tools and `LIBCMT.LIB`. The VC 4.0 runtime archive supplies the
retail CRT members; in particular, its `testfdiv.obj` carries private literal
identities absent from the VC 4.2 archive. The deterministic Gruntz-style release
builder is `scripts/toolchain/create-toolchain-release.nix`; provenance, the release hash,
and recovery history are in
[`docs/toolchain-vc42.md`](toolchain-vc42.md). `clang`/`clangd` is editor tooling only;
the Wine MSVC 4.2 build is the sole verdict on a match.

ninja **tracks header dependencies** (via `cc_wrap.py`, since MSVC 4.2 has no `/showIncludes`),
so editing a shared header recompiles exactly its includers — no stale objects. `homm2 build`
then runs **hard gates** (a red gate fails the build): no TU declares types/enums/externs/
forward-decls locally (all come from headers), no object emits a function symbol absent from
CodeView, every global carries a unique `DATA(<its VA>)`, every free function is declared in its
owner header, and every extern global has a definition in its owner TU (link-completeness), and every class
vtable is claimed by a `VTBL()` census marker in its owner TU. Full catalog: `docs/build-asserts.md`.

## Navigate (`homm2 sema`)

Semantic questions about the source/target — grep is lexical only. `homm2 sema -h` lists all;
addresses are RVAs (a full `VA(0x..)` also works for `rva`):

```sh
homm2 sema xref   0x0004a3c0        # who calls this fn (--callees | --tree)
homm2 sema disasm 0x0004a3c0 --diff # our (compiled) vs retail asm, side by side
homm2 sema strings 0x0004a3c0       # a fn's string set (--find TEXT = reverse lookup)
homm2 sema match  SOURCE/KB         # per-fn match % of a unit (or an 0x RVA)
homm2 sema rva    0x0004a3c0        # dossier: claim / src loc / ghidra / match %
homm2 sema symbol combatManager     # fuzzy workspace-symbol search (clangd)
homm2 sema def|refs|hover src/… L C # clangd LSP at a point
```

xref/disasm/strings/match/rva/clangd need no Ghidra; xref library boundaries need
a one-time `homm2 ghidra` project (imports the EXE, applies our CodeView names).

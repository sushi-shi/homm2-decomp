# Visual C++ 4.2 matching toolchain

The project compiles retail-matching objects with the English Visual C++ 4.2
Enterprise toolchain preserved as archive.org item `en_vc42ent`. Disc 1 is a RAR
self-extracting executable containing a complete installed
`MSDEV/{BIN,INCLUDE,LIB}` tree.

## Historical gap

The initial provisioning session downloaded `en_vc42ent_disc1.exe`, extracted
those three directories with `unrar`, and moved them directly into the gitignored
`build/toolchain/msvc` directory. Commit `33fe2e4` recorded that provisioning in
its message, but committed only unrelated generated-manifest path changes. The
`scripts/make-toolchain.sh` reference existed from the initial scaffold and no
provisioning or release-builder script was ever added to Git. Release `v0.1.0`
therefore contains neither script, and its GitHub release has no binary assets.

The recovered workflow has two explicit entry points:

- `scripts/make-toolchain.sh` provisions or verifies a local tree. It supports
  the original RAR SFX, other archive/disc inputs, and installed `MSDEV` trees.
- `scripts/create-toolchain-release.nix` reproduces a normalized `.tar.xz` from
  the preserved disc1 media, following the Gruntz release-builder pattern.

Both paths require the pinned SHA-256 identities of the code-generating compiler
phases, linker, CVTRES/PDB support, core headers, and CRT libraries. An explicitly
unpinned tree is allowed only through `--allow-unpinned` for isolated A/B tests.

## Local provisioning

```sh
scripts/make-toolchain.sh /path/to/en_vc42ent_disc1.exe
scripts/make-toolchain.sh --check build/toolchain/msvc
```

The RAR SFX path requires `unrar` on `PATH`; an already installed `MSDEV` tree
needs only Python. The dedicated release Nix expression supplies `unrar` without
adding this one-time proprietary extractor to the normal build environment.

Provisioning publishes `build/toolchain/msvc` atomically and writes a manifest of
required artifact hashes. The media's historical nonzero `unrar` result after
extraction is not trusted or rejected by itself; the complete tree and pinned
artifact hashes are the gate.

## Release reproduction

```sh
nix-shell scripts/create-toolchain-release.nix
```

The Nix entrypoint pins the archive.org original by its published SHA-1. The
builder then applies the stronger extracted-file SHA-256 gate and creates
`build/homm2-toolchain-vc42.tar.xz` with sorted entries, fixed ownership, a fixed
timestamp, and normalized GNU tar metadata. Two clean reproductions produced the
same release SHA-256:

```text
892a47ee106b26de139a6a0a2ac044884ac899bf644b89a4e63491c4d6e5faf5
```

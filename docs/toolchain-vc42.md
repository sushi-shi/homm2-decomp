# Visual C++ 4.2 compiler and LINK 3.00 toolchain

The project compiles retail-matching objects with the English Visual C++ 4.2
Enterprise toolchain preserved as archive.org item `en_vc42ent`. Disc 1 is a RAR
self-extracting executable containing a complete installed
`MSDEV/{BIN,INCLUDE,LIB}` tree.

The retail executable's PE header was produced by linker version 3.00, not the
VC 4.2 tree's LINK 4.20.6164. The final-link component therefore comes from
archive.org item `msvc4x`, file `MSVC40.iso`. The verified VC 4.0 tools are LINK
3.00.5270, CVPACK, CVTRES, and MSPDB40; object compilation and libraries remain
VC 4.2.

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
- `scripts/make-linker.sh` provisions the separate pinned LINK 3.00 component
  from VC 4.0 media without replacing the VC 4.2 compiler or libraries.
- `scripts/create-toolchain-release.nix` reproduces a normalized `.tar.xz` from
  the preserved disc1 media, following the Gruntz release-builder pattern.

Both paths require the pinned SHA-256 identities of the code-generating compiler
phases, linker, CVTRES/PDB support, core headers, and CRT libraries. An explicitly
unpinned tree is allowed only through `--allow-unpinned` for isolated A/B tests.

## Local provisioning

```sh
scripts/make-toolchain.sh /path/to/en_vc42ent_disc1.exe
scripts/make-linker.sh /path/to/MSVC40.iso
scripts/make-toolchain.sh --check build/toolchain/msvc
scripts/make-linker.sh --check build/toolchain/link300
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

The Nix entrypoint pins both archive.org originals by their published SHA-1.
Because archive.org mirrors can terminate the 555 MB VC 4.0 ISO response early,
the single fixed-output download resumes the same file until its published full
size and SHA-1 both match.
The builder then applies stronger extracted-file SHA-256 gates and creates
`build/homm2-toolchain-vc42-link300.tar.xz` with sorted entries, fixed ownership,
a fixed timestamp, and normalized GNU tar metadata. Two independent packaging
runs produced the same release SHA-256:

```text
db9f74f75ed4325deebd57e21a733e1e0aeefc745bfaa3991b8d1dc5d961e219
```

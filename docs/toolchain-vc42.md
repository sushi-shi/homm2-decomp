# Visual C++ 4.2 compiler and VC 4.0 final-link toolchain

The project compiles retail-matching objects with the English Visual C++ 4.2
Enterprise toolchain preserved as archive.org item `en_vc42ent`. Disc 1 is a RAR
self-extracting executable containing a complete installed
`MSDEV/{BIN,INCLUDE,LIB}` tree.

The retail executable's PE header was produced by linker version 3.00, not the
VC 4.2 tree's LINK 4.20.6164. The final-link component therefore comes from
archive.org item `msvc4x`, file `MSVC40.iso`. The verified VC 4.0 component is
LINK 3.00.5270, CVPACK, CVTRES, MSPDB40, **and its sibling `LIBCMT.LIB`**. Object
compilation, headers, and non-CRT SDK/import libraries remain VC 4.2, but the
final link resolves the static Microsoft C runtime from the VC 4.0 archive.

The library choice is retail evidence, not merely installation convenience. The
VC 4.0 `testfdiv.obj` contains the exact 17 private runtime-literal identities
seen in retail; the VC 4.2 `LIBCMT.LIB` does not. Putting the verified VC 4.0
`LIB` directory first drops those 17 missing public-data identities while
preserving the final-link result and hard gates.

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
  and its pinned `LIBCMT.LIB` from VC 4.0 media without replacing the VC 4.2
  compiler, headers, or non-CRT libraries.
- `scripts/create-toolchain-release.nix` reproduces a normalized `.tar.xz` from
  the preserved disc1 media, following the Gruntz release-builder pattern.

Both paths require the pinned SHA-256 identities of the code-generating compiler
phases, linker, CVTRES/PDB support, core headers, and their respective CRT
archives. An explicitly unpinned tree is allowed only through `--allow-unpinned`
for isolated A/B tests.

The link audit still derives a `/Gf` writable-literal variant from the pinned VC
4.2 `LIBCMT.LIB` as a reproducible fallback and A/B oracle. With the complete
verified `link300` component, however, the VC 4.0 runtime archive precedes and
supersedes that derived archive. The byte-constrained VC 4.2 experiment and its
official-source proof are documented in
[`vc42-libcmt-gf.md`](vc42-libcmt-gf.md).

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

Provisioning publishes `build/toolchain/msvc` and `build/toolchain/link300`
atomically and writes a manifest of required artifact hashes for each component.
The `link300` manifest includes `lib/LIBCMT.LIB`, not only executable tools. The
media's historical nonzero `unrar` result after extraction is not trusted or
rejected by itself; the complete tree and pinned artifact hashes are the gate.

## Release reproduction

```sh
nix-shell scripts/create-toolchain-release.nix
```

The Nix entrypoint pins both archive.org originals by their published SHA-1.
The builder then applies stronger extracted-file SHA-256 gates, including the
VC 4.0 runtime archive, and creates
`build/homm2-toolchain-vc42-link300.tar.xz` with sorted entries, fixed ownership,
a fixed timestamp, and normalized GNU tar metadata. Two independent packaging
runs produced the same release SHA-256:

```text
37f04afbb8cf8005878e010608620c51f16f1a945e7cb76145fd0f52d09617a3
```

The archive is published on GitHub under the dedicated
[`toolchain-vc42-link300`](https://github.com/homm2-decomp/homm2-decomp/releases/tag/toolchain-vc42-link300)
release. Its `link300/lib/LIBCMT.LIB` has SHA-256
`d7bdb49c0a3bc77dee026b9aa9a994c5a78963c8aefc6512dbb298b4d41c4907`.

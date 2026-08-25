# Heroes of Might and Magic II Gold reconstructed source

This project recovers and modernizes the source code for Heroes of Might and
Magic II Gold 2.1 as published by Buka. The Price of Loyalty 2.0 reconstruction
remains available as an independent source reference and contributes verified
cross-version work.

This branch adds Project Ironfist as separately distributed, unofficial runtime
content on top of that reconstructed cross-platform engine.

The repository does not contain the original game resources. An installed copy
of the game is required to play it.

## Quick start on Linux

You need Git, [Nix](https://nixos.org/download/) with flakes enabled, internet
access for the first build, and a writable Heroes II: The Price of Loyalty game
directory. That directory must contain `DATA/HEROES2.AGG` and
`DATA/HEROES2X.AGG`.

1. Clone the Ironfist branch and enter it:

   ```sh
   git clone --branch ironfist --single-branch \
     https://github.com/sushi-shi/homm2-decomp.git homm2-ironfist
   cd homm2-ironfist
   ```

2. Point the game at your writable PoL installation:

   ```sh
   export HOMM2_DATA=/absolute/path/to/heroes2
   find "$HOMM2_DATA" -maxdepth 2 \( -type f -o -type l \) \
     \( -iname HEROES2.AGG -o -iname HEROES2X.AGG \)
   ```

3. Fetch the pinned original Ironfist source, build its resources, and install
   them into that game directory:

   ```sh
   nix run .#ironfist-resources -- "$HOMM2_DATA"
   ```

4. Build and run the native Linux game:

   ```sh
   nix run
   ```

Later runs only need `HOMM2_DATA=/absolute/path/to/heroes2 nix run`; rebuild the
resources only when this branch changes its pinned Ironfist source revision.
The longer build, Windows/Wine, Web, locale, and runtime-option instructions are
below.

## Repository branch structure

```text
decomp-pol-2.0 -------------------> decomp-gold-2.1-buka
    |                                   |
    +------------------+                +------------------------+
    |                  |                |                        |
    v                  v                v                        v
source-pol-2.0     classic-pol-2.0   source-gold-2.1-buka    classic-gold-2.1-buka
    |                                   |
    +-----------------+-----------------+
                      |
                      v
                    master --------> ironfist
```
- `decomp-pol-2.0` is the original Price of Loyalty 2.0 reconstruction.
- `source-pol-2.0` is its generated, matching-machinery-free source tree.
- `classic-pol-2.0` is the same generated tree with the original integer-enum
  and name-mangling model preserved.
- `decomp-gold-2.1-buka` is the Gold 2.1/Buka reconstruction and preferred
  non-incrementally-linked retail target.
- `source-gold-2.1-buka` is its generated clean source tree and the primary
  source base of `master`.
- `classic-gold-2.1-buka` is the corresponding legacy-mangling source view,
  stored as Windows-1251 with readable Russian literals rather than UTF-8.
- `master` is the cross-platform Linux, Windows, and Web port.
- `ironfist` applies Project Ironfist to the reconstructed cross-platform source.

The classic branches are terminal views of their corresponding reconstruction;
they do not feed Gold or `master`.

The cross-version behavioral ledger lives on the evidence-owning Buka
reconstruction branch; see [Retail version differences](docs/version-differences.md).

## Reconstruction evidence

The Gold decompilation branch matches all 1,727 reconstructed retail functions
and all 291,995 reviewed data bytes. Its audited exact-link path also reproduces
the supported retail executable byte for byte, including resources and import
layout.

Matching is evidence for a reconstruction, not a license to put compiler or
linker tricks in the source. The decompilation branches retain credible
developer-shaped C++; unavoidable private-data placement accommodations live in
disposable comparison/link tooling. The generated source branches remove RVA
annotations, delinking metadata, reconstruction comments, and other matching
machinery.

## Build

The supported builds use Nix:

```sh
nix build .#homm2-linux
nix build .#homm2-windows
nix build .#homm2-web
```

### Building without Nix

Requirements:

1. CMake 3.20+, Ninja, and pkg-config.
2. C and C++20 compilers with 32-bit support.
3. 32-bit SDL3 and FFmpeg libraries (`libavcodec`, `libavformat`, `libavutil`,
   and `libswresample`).

```sh
cmake -S . -B build -G Ninja
cmake --build build
HOMM2_DATA=/path/to/heroes2 ./build/homm2
```

## Installation

### Game data

To run Ironfist you need an installed copy of Heroes II Gold or The Price of Loyalty,
including both `DATA/HEROES2.AGG` and `DATA/HEROES2X.AGG`. Build and install the
Ironfist resources from the pinned original repository into that directory:

```sh
nix run .#ironfist-resources -- /path/to/heroes2
```

The installer makes a sparse clone of `jkoppel/project-ironfist` at the pinned
commit, verifies that commit, runs upstream's resource packers under an isolated
Wine prefix, and merges `DATA/`, `MAPS/`, `CAMPAIGNS/`, `MUSIC/`, and
`SCRIPTS/` into the game directory. See
[Building the Ironfist resources](docs/ironfist-resources.md).

`HOMM2_DATA` should then point to that combined game directory (paths are
resolved case-insensitively).

```sh
export HOMM2_DATA=/path/to/heroes2
```

The game detects the `western` or `buka-cyrillic` resource profile from
`FONT.ICN`, independently of `HOMM2_LANGUAGE`. To use Russian UI with English
primary data and a Buka resource overlay:

```sh
HOMM2_DATA=/path/to/heroes2-english \
HOMM2_LOCALE_DATA=/path/to/heroes2-buka \
HOMM2_LANGUAGE=ru nix run
```

`HOMM2_RESOURCE_PROFILE=western|buka-cyrillic` overrides automatic detection
for diagnostics. See [Localization architecture](docs/localization.md).

Without `HOMM2_DATA`, the engine searches these locations in order:

1. The executable directory.
2. The current directory.
3. `$XDG_DATA_HOME/homm2`.
4. `$XDG_DATA_HOME/homm2/data`.
5. `~/games/homm2`.

On Linux and Web, installed game data may be read-only. Preferences, saves,
high scores, and network exchange files are stored under the user data root.
Windows retains the original writable game-directory behavior.

### Linux

After setting `HOMM2_DATA`, run:

```sh
nix run
```

The saved PoL configuration uses a 480x360 window and Gallop movement. This
reproduces those two choices while retaining the native renderer:

```sh
HOMM2_WINDOW_SIZE=480x360 HOMM2_WALK_SPEED=gallop nix run
```

`HOMM2_WINDOW_SIZE=<width>x<height>` overrides the initial window size.
`HOMM2_WALK_SPEED` accepts `walk`, `trot`, `canter`, `gallop`, `jump`, or the
corresponding value from 0 through 4. Without an override the setting is read
from `HEROES2.CFG` in the user data directory and can be changed normally in
System Options.

### Windows

First run the source resource installer against the writable retail game
directory. The Windows package is statically linked; copy `HMM2PL.exe` there
and run it. For Wine:

```sh
nix build .#homm2-windows
cp result/bin/HMM2PL.exe /path/to/heroes2/
cp result/run-game.sh /path/to/heroes2/
/path/to/heroes2/run-game.sh
```

The helper creates a Wine prefix at `<game dir>/.wineprefix` on first run.

### Web

```sh
HOMM2_DATA=/path/to/heroes2 nix run .#web
```

The path must contain the retail data and the installed Ironfist resource pack.
The launcher packages the installed data (including custom campaigns), serves
the bundle on port 8080 (`HOMM2_WEB_PORT`), and caches it under
`~/.cache/homm2-web` (`HOMM2_WEB_OUTPUT`). The first build also cross-compiles
SDL3 and a minimal FFmpeg.

## License

The only Project Ironfist copyright notice located for this branch is reproduced
in [LICENSE](LICENSE): `(c) 2016 Ironfist, all rights reserved.`

Project references: [ironfi.st](http://ironfi.st/) and
[jkoppel/project-ironfist](https://github.com/jkoppel/project-ironfist).

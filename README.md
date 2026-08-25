# Heroes of Might and Magic II Gold reconstructed source

This project recovers and modernizes the source code for Heroes of Might and
Magic II Gold 2.1 as published by Buka. The Price of Loyalty 2.0 reconstruction
remains available as an independent source reference and contributes verified
cross-version work.

The repository does not contain the original game resources. An installed copy
of the game is required to play it.

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
2. A C++20 compiler with 32-bit support.
3. 32-bit SDL3 and FFmpeg libraries (`libavcodec`, `libavformat`, `libavutil`,
   and `libswresample`).

```sh
cmake -S . -B build -G Ninja
cmake --build build
HOMM2_DATA=/path/to/heroes2 ./build/homm2
```

## Installation

### Game data

`HOMM2_DATA` should point to an installed game directory containing
`DATA/HEROES2.AGG` (case-insensitive):

```sh
export HOMM2_DATA=/path/to/heroes2
```

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

Native SDL presentation can be varied independently without changing game
timing. Scaling defaults to nearest-neighbour and vsync defaults to the SDL
renderer policy. Set both variables explicitly for reproducible comparisons:

```sh
HOMM2_SCALE_MODE=nearest HOMM2_VSYNC=0 nix run
HOMM2_SCALE_MODE=linear  HOMM2_VSYNC=0 nix run
HOMM2_SCALE_MODE=nearest HOMM2_VSYNC=1 nix run
HOMM2_SCALE_MODE=linear  HOMM2_VSYNC=1 nix run
```

`HOMM2_SCALE_MODE` accepts `nearest` or `linear`; `HOMM2_VSYNC` accepts `0`
or `1`. The selected presentation mode is printed at startup.

### Windows

The Windows package is statically linked. Copy `HMM2PL.exe` into the writable
game directory and run it. For Wine:

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

The launcher packages the installed data, serves the bundle on port 8080
(`HOMM2_WEB_PORT`), and caches it under `~/.cache/homm2-web`
(`HOMM2_WEB_OUTPUT`). The first build also cross-compiles SDL3 and a minimal
FFmpeg.

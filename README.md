# Heroes of Might and Magic II Gold reconstructed source

This project recovers and modernizes the source code for Heroes of Might and
Magic II Gold 2.1 as published by Buka. The Price of Loyalty 2.0 reconstruction
remains available as an independent source reference and contributes verified
cross-version work.

The repository does not contain the original game resources. An installed copy
of the game is required to play it.

## Quick start on Linux

You need Git, [Nix](https://nixos.org/download/) with flakes enabled, and an
installed Heroes II game directory. The file `DATA/HEROES2.AGG` must exist;
directory and file names are matched case-insensitively.

1. Clone the native port and enter it:

   ```sh
   git clone --branch master --single-branch \
     https://github.com/sushi-shi/homm2-decomp.git homm2
   cd homm2
   ```

2. Point the engine at your installed game data:

   ```sh
   export HOMM2_DATA=/absolute/path/to/heroes2
   find "$HOMM2_DATA" -maxdepth 2 \( -type f -o -type l \) \
     -iname HEROES2.AGG
   ```

3. Build and run the native Linux game:

   ```sh
   nix run
   ```

Later runs only need `HOMM2_DATA=/absolute/path/to/heroes2 nix run`. Saves and
configuration are written under the user data directory, so the retail game
directory may remain read-only. Windows/Wine, Web, localization, manual-build,
and runtime-option instructions are below.

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

## License

Project-authored reconstruction source and tooling are dedicated to the public
domain under [CC0 1.0](LICENSE), to the extent the contributors can do so.
Files carrying separate copyright or license notices retain those terms. No
binary game assets are stored in this repository; retail inputs and build
outputs incorporating them are not covered by this dedication.

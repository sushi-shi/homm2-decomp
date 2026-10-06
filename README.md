# Heroes of Might and Magic II — Project Ironfist source

Project Ironfist as separately distributed, unofficial runtime content on top
of the reconstructed cross-platform engine of Heroes of Might and Magic II Gold
2.1 (Buka). Supply your own complete installation (Heroes II Gold, or the base
game upgraded with The Price of Loyalty); game data is not bundled.

## Branches

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
                    port --------> source-ironfist (you are here) --------> port-ironfist
```

- [`decomp-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/decomp-pol-2.0) — Price of Loyalty 2.0 `HEROES2W.EXE` (1997), VC4.2
- [`source-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/source-pol-2.0) — Clean source, PoL 2.0
- [`classic-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/classic-pol-2.0) — Reading view, PoL 2.0
- [`decomp-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/decomp-gold-2.1-buka) — Gold 2.1 (Buka) game, byte-identical; editor in progress
- [`source-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/source-gold-2.1-buka) — Clean source, Gold 2.1 (ru/en)
- [`classic-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/classic-gold-2.1-buka) — Reading view, UTF-8 Russian
- [`port`](https://github.com/sushi-shi/homm2-decomp/tree/port) — Native port: Linux, Windows, browser
- [`source-ironfist`](https://github.com/sushi-shi/homm2-decomp/tree/source-ironfist) — Project Ironfist on the source
- [`port-ironfist`](https://github.com/sushi-shi/homm2-decomp/tree/port-ironfist) — Project Ironfist on the port

## Play on Linux

You need Git, [Nix](https://nixos.org/download/) with flakes enabled, internet
access for the first build, and a writable complete Heroes II installation:
either Heroes II Gold, or the base game upgraded with The Price of Loyalty. The
directory must contain `DATA/HEROES2.AGG` and `DATA/HEROES2X.AGG`.

1. Clone this branch and enter it:

   ```sh
   git clone --branch source-ironfist --single-branch \
     https://github.com/sushi-shi/homm2-decomp.git homm2-ironfist
   cd homm2-ironfist
   ```

2. Point the game at that writable complete installation:

   ```sh
   export HOMM2_DATA=/absolute/path/to/heroes2
   find "$HOMM2_DATA" -maxdepth 2 \( -type f -o -type l \) \
     \( -iname HEROES2.AGG -o -iname HEROES2X.AGG \)
   ```

3. Let Nix fetch the pinned original Ironfist source, build its resource
   payload, and install it into that game directory:

   ```sh
   nix run .#ironfist-resources -- "$HOMM2_DATA"
   ```

4. Build and run the native Linux game:

   ```sh
   nix run
   ```

Later runs only need `HOMM2_DATA=/absolute/path/to/heroes2 nix run`; rebuild the
resources only when this branch changes its pinned Ironfist source revision.

### Game data

To run Ironfist you need a complete Heroes II installation: either Heroes II
Gold, or the base game upgraded with The Price of Loyalty. It must include both
`DATA/HEROES2.AGG` and `DATA/HEROES2X.AGG`. Build and install the Ironfist
resources from the pinned original repository into that directory:

```sh
nix run .#ironfist-resources -- /path/to/heroes2
```

The flake fetches only the required paths from `jkoppel/project-ironfist`, pins
them by commit and Nix content hash, and builds an immutable resource payload in
the local Nix store. The installer merges its `DATA/`, `MAPS/`, `CAMPAIGNS/`,
`MUSIC/`, and `SCRIPTS/` into the game directory. Runtime Git, Wine, and the
upstream Windows packers are not required. See
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

## Build from source

The supported builds use Nix:

```sh
nix build .#homm2-linux
nix build .#homm2-windows
nix build .#homm2-web
```

Without Nix:

Requirements:

1. CMake 3.20+, Ninja, and pkg-config.
2. C and C++20 compilers with 32-bit support.
3. 32-bit SDL3, libbz2, and FFmpeg libraries (`libavcodec`, `libavformat`,
   `libavutil`, and `libswresample`).

```sh
cmake -S . -B build -G Ninja
cmake --build build
HOMM2_DATA=/path/to/heroes2 ./build/homm2
```

## Browser

```sh
HOMM2_DATA=/path/to/heroes2 nix run .#web
```

The path must contain the retail data and the installed Ironfist resource pack.
The launcher packages the installed data (including custom campaigns), serves
the bundle on port 8080 (`HOMM2_WEB_PORT`), and caches it under
`~/.cache/homm2-web` (`HOMM2_WEB_OUTPUT`). The first build also cross-compiles
SDL3 and a minimal FFmpeg.

## Windows (native)

First run the source resource installer against the writable retail game
directory. The Windows package is statically linked and does not require the
retail Audiere, Miles, Smacker, or Wing DLLs; copy `HMM2PL.exe` there and run
it. For Wine:

```sh
nix build .#homm2-windows
cp result/bin/HMM2PL.exe /path/to/heroes2/
cd /path/to/heroes2
wine HMM2PL.exe
```

The SDL3 port does not need retail registry keys, CD-drive mappings, or a
special Wine prefix.

## Edition changes

- [Building the Ironfist resources](docs/ironfist-resources.md)
- [Ironfist on the port](docs/ironfist-port.md) and its
  [equivalence record](docs/ironfist-equivalence.md)
- [Localization architecture](docs/localization.md), the
  [port guide](docs/porting.md) and
  [intentional retail divergences](docs/retail-divergences.md)

## License

The only Project Ironfist copyright notice located for this branch is reproduced
in [LICENSE](LICENSE): `(c) 2016 Ironfist, all rights reserved.`

Project references: [ironfi.st](http://ironfi.st/) and
[jkoppel/project-ironfist](https://github.com/jkoppel/project-ironfist).

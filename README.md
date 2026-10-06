# Heroes of Might and Magic II Gold — Linux / Windows / WebAssembly source port

A native port of Heroes of Might and Magic II Gold 2.1 (Buka), rebuilt from
reconstructed source. Supply your own installed copy of the game; game data is
not bundled.

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
                    master (you are here) --------> ironfist --------> ironfist-master
```

| Branch | Purpose |
| --- | --- |
| `decomp-pol-2.0` | Price of Loyalty 2.0 reconstruction |
| `source-pol-2.0` | Its generated source tree, without matching machinery |
| `classic-pol-2.0` | The same tree with the original integer-enum and name-mangling model |
| `decomp-gold-2.1-buka` | Gold 2.1 (Buka) reconstruction, the preferred retail target |
| `source-gold-2.1-buka` | Its generated clean source tree, the source base of `master` |
| `classic-gold-2.1-buka` | Legacy-mangling view, with Windows-1251 strings rendered as UTF-8 Russian |
| `master` | Cross-platform Linux, Windows and browser port |
| `ironfist` | Project Ironfist applied to the cross-platform source |
| `ironfist-master` | Maintained Ironfist integration |

The classic branches are terminal views of their reconstruction; they do not
feed Gold or `master`. The Gold decompilation branch matches all 1,727
reconstructed retail functions and all 291,995 reviewed data bytes, and its
audited exact-link path reproduces the retail executable byte for byte. The
generated source branches remove RVA annotations, delinking metadata and other
matching machinery. Cross-version behavior is recorded in
[Retail version differences](docs/version-differences.md); changes made only by
this port are in [Intentional retail divergences](docs/retail-divergences.md).

## Play on Linux

On x86_64 Linux with Nix flakes enabled, point `HOMM2_DATA` at an installed game
directory, the one that contains `DATA`:

```sh
HOMM2_DATA=/path/to/heroes2 nix run github:sushi-shi/homm2-decomp/port
```

A Gold or Price of Loyalty installation works: `DATA/HEROES2.AGG` and
`DATA/HEROES2X.AGG` are required, and names are matched case-insensitively.
Tested archives, SHA-256:

```text
7a11c86db8ec8fbf19d810c1f7ebf9d8520f63fbdc42d6f51f7538654004315d  HEROES2.AGG   (GOG, English)
1f3edad1bb88052da50b3fae5d00e8f7e73dd977f356463022aab132b54f4d41  HEROES2X.AGG  (GOG, English)
da08a14cc545f6708bd2b746edd7ea7fa0eb7d0ed26f6e54ab8e6ccb2e735e19  heroes2.agg   (Buka, Russian)
68f12a2ca2dd1a000e1136ad70f38b19116686c2a7a0e9a0860528d998b52afd  heroes2x.agg  (Buka, Russian)
```

The Buka disc is on archive.org as the Heroes anthology
[homm-antologiya-platinum-buka](https://archive.org/details/homm-antologiya-platinum-buka),
a `.rar` of its CD image; the flake install below takes it as it is.

The first launch builds the game. The installation is only read: preferences,
saves and high scores go to `~/.local/share/homm2/homm2` (under
`$XDG_DATA_HOME`), so the game directory may stay read-only. Music is read from
`MUSIC` (GOG's `TrackNN.ogg`) or `TRACKS2` (the Buka disc) and movies from
`HEROES2/ANIM`; without them the game is silent or skips the movie.

The language follows your locale. The Russian translation needs the Cyrillic
font of a Buka installation, either as `HOMM2_DATA` or as an overlay over
English data:

```sh
HOMM2_DATA=/path/to/heroes2-english \
HOMM2_LOCALE_DATA=/path/to/heroes2-buka \
HOMM2_LANGUAGE=ru nix run github:sushi-shi/homm2-decomp/port
```

With `nix run`, pass game options after `--`. For example:

```sh
HOMM2_DATA=/path/to/heroes2 nix run github:sushi-shi/homm2-decomp/port -- /I0
```

| Option | Purpose |
| --- | --- |
| `/I0` | Skip the intro |
| `--language=en\|ru` | Choose the language (also `HOMM2_LANGUAGE`) |
| `--resource-profile=western\|buka-cyrillic` | Override the font and archive profile detected from `FONT.ICN` (also `HOMM2_RESOURCE_PROFILE`) |

Without `HOMM2_DATA`, the game looks for `DATA/HEROES2.AGG` next to the
executable, in the current directory, then in `$XDG_DATA_HOME/homm2`,
`$XDG_DATA_HOME/homm2/data` and `~/games/homm2`. In a local checkout, use
`HOMM2_DATA=/path/to/heroes2 nix run .`. See
[Localization architecture](docs/localization.md) and the
[port guide](docs/porting.md) for the remaining variables, including the
screenshot and input-replay hooks.

## Install with a NixOS flake

Add the port and a local folder holding your copy of the game to your flake
inputs:

```nix
inputs.homm2.url = "github:sushi-shi/homm2-decomp/port";
inputs.homm2-game = {
  url = "path:/path/to/heroes2";
  flake = false;
};
```

The folder is the installed game, the one that contains `DATA`, or the Buka
disc's files, or a folder holding a `.zip`, `.7z`, `.iso` or `.rar` of either.
Import the module and name your copy:

```nix
outputs = { nixpkgs, homm2, homm2-game, ... }: {
  nixosConfigurations."<host>" = nixpkgs.lib.nixosSystem {
    modules = [
      ./configuration.nix
      homm2.nixosModules.default
      {
        programs.homm2 = {
          enable = true;
          game = "${homm2-game}";
        };
      }
    ];
  };
};
```

Nix checks the copy and lays its data out in its store when the
configuration is built. Rebuild, replacing `<host>` with your host's name,
then launch:

```sh
sudo nixos-rebuild switch --flake '.#<host>'
heroes2
```

Instead of a local copy, a module of the configuration can fetch the Buka
anthology from archive.org:

```nix
{ pkgs, ... }:
let
  heroes-buka = pkgs.fetchurl {
    name = "heroes-platinum-buka.rar";   # the importer goes by the extension
    url = "https://archive.org/download/homm-antologiya-platinum-buka/%D0%93%D0%B5%D1%80%D0%BE%D0%B8.%20%D0%9F%D0%BB%D0%B0%D1%82%D0%B8%D0%BD%D0%BE%D0%B2%D0%B0%D1%8F%20%D0%B2%D0%B5%D1%80%D1%81%D0%B8%D1%8F%20%5B%D0%91%D1%83%D0%BA%D0%B0%5D.rar";
    hash = "sha256-YWAmitzQ5TozJxS8Jph6ATp7KB0BRjJFS7+JOjnQBPk=";
  };
in {
  programs.homm2 = { enable = true; game = heroes-buka; };
  system.extraDependencies = [ heroes-buka ];
}
```

The 1 GB archive is downloaded into the store once. `system.extraDependencies`
(`home.extraDependencies` with home-manager) keeps it through garbage
collection, so a rebuild that imports the game again does not download it
again. The same archive holds Heroes I for the
[HoMM1 port](https://github.com/sushi-shi/homm1-decomp/tree/port).

With home-manager, the same options install the game for one user:

```nix
homeConfigurations."<user>" = home-manager.lib.homeManagerConfiguration {
  pkgs = nixpkgs.legacyPackages.x86_64-linux;
  modules = [
    homm2.homeManagerModules.default
    { programs.homm2 = { enable = true; game = "${homm2-game}"; }; }
  ];
};
```

```sh
home-manager switch --flake '.#<user>'
```

`heroes2` keeps saves, high scores and settings in
`~/.local/share/homm2/homm2`, as the program does. `programs.homm2.locale =
"ru"` starts the game in Russian whatever your locale (it needs a Buka copy's
Cyrillic font). Without `game`, `heroes2` reads `HOMM2_DATA`. There is no
native scenario editor yet, so only the game is installed;
[more about the install](docs/porting.md#install-with-nix).

## Controls

The game is played with the mouse, as the original was.

| Action | Keyboard / mouse |
| --- | --- |
| Select, move, confirm | Left mouse button |
| Information about anything | Hold the right mouse button |
| Move the selected hero one step | Arrow keys or keypad |
| Scroll the map | Ctrl + arrow keys |
| Next hero / next town | H / T |
| Open the selected hero or town | Enter |
| Cast an adventure spell | C |
| Dig for the artifact / view the puzzle | D / P |
| View the world | V |
| Scenario or campaign information | I |
| Save / load / new game / quit | S / L / N / Q |
| Fullscreen / scaling / VSync | F4 / Shift+F4 / Ctrl+F4 |

Display settings persist between runs; see
[display controls](docs/porting.md#display-controls).

## Build from source

From the `master` branch:

```sh
nix develop
cmake --preset linux
cmake --build --preset linux
HOMM2_DATA=/path/to/heroes2 build/linux/homm2
```

`ctest --test-dir build/linux` runs the tests; they use synthetic fixtures and
need no game data. Without Nix, the game needs CMake
3.21+, Ninja, pkg-config, a C++20 compiler that can target 32-bit x86 (retail
packed layouts are load-bearing), and 32-bit SDL3, libbz2 and FFmpeg
(`libavcodec`, `libavformat`, `libavutil`, `libswresample`), plus Python 3.10+
and the GNU gettext tools (`msgfmt`, `msgcat`, `xgettext`) for the translations.
CMake names the distribution packages when the 32-bit ones are missing.
Single-configuration generators default to `RelWithDebInfo`; pass
`-DCMAKE_BUILD_TYPE=Debug` or `Release` to choose another.

The packaged builds are flake outputs:

```sh
nix build .#homm2-linux
nix build .#homm2-windows
nix build .#homm2-web
nix flake check
```

## Windows (native)

The Windows build is cross-compiled with Nix, on Linux or in WSL. There is no
native Windows toolchain build yet.

```sh
nix build .#homm2-windows
ls result/bin        # HMM2PL.exe  lang/
```

`HMM2PL.exe` is statically linked and needs no other DLL; the build fails if it
would import one that is not part of Windows. Copy the contents of
`result/bin` into the game folder, next to `DATA`:

```text
C:\Games\Heroes of Might and Magic II\
    HMM2PL.exe
    lang\            (Russian translation; optional)
    DATA\HEROES2.AGG
    DATA\HEROES2X.AGG
    MAPS\ ...
```

Double-click `HMM2PL.exe`. It finds `DATA` next to itself, whatever the working
directory and whatever the case of the file names. To keep the program
elsewhere, point `HOMM2_DATA` at the game folder; quotes are accepted:

```bat
set HOMM2_DATA=C:\Games\Heroes of Might and Magic II
C:\Tools\HMM2PL.exe
```

On Windows, saves and preferences stay in the game folder, as they did in
1996, so it must be writable (not under `Program Files`). Under Wine, run
`wine HMM2PL.exe` from the game folder. With an installed game,
`HOMM2_DATA=/path/to/heroes2 nix run .#windows-smoke` checks the Windows build
under Wine from a folder laid out like the one above.

## Browser

### Linux

Inside `nix develop .#web`:

```sh
emcmake cmake --preset wasm
cmake --build --preset wasm
HOMM2_DATA=/path/to/heroes2 homm2-web-package build/wasm
python3 -m http.server --directory build/wasm
```

`homm2-web-package` packs the installation into `build/wasm/homm2.data`. The
same steps, served with `emrun`, are one command:
`HOMM2_DATA=/path/to/heroes2 nix run .#web` (port `HOMM2_WEB_PORT`, default
8080; bundle in `~/.cache/homm2-web` or `HOMM2_WEB_OUTPUT`).

### Windows

Not supported yet: the browser build needs Emscripten builds of FFmpeg and
libbz2, which are made with Nix. Use the Linux steps in WSL.

### Play

Open [the game](http://localhost:8000/homm2.html) and press **Start Heroes II**.
The game data is part of the page's `homm2.data`. Saves and preferences stay in
browser storage; clearing it removes them.

## License

Project-authored reconstruction source and tooling are dedicated to the public
domain under [CC0 1.0](LICENSE), to the extent the contributors can do so.
Files carrying separate copyright or license notices retain those terms. No
binary game assets are stored in this repository; retail inputs and build
outputs incorporating them are not covered by this dedication.

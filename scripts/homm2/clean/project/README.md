# Heroes of Might and Magic II — Gold 2.1 (Buka) source

C++ source for the Buka release of Heroes of Might and Magic II Gold 2.1
(`HMM2PL.exe`, Windows), generated from the byte-identical reconstruction with
its matching machinery removed. It builds a 32-bit Windows program with Clang
and MinGW. The text lives in a catalog: `locales/messages.def` (English) and
`locales/ru.po` (the retail Russian). Building selects one of them.

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
                    port --------> source-ironfist --------> port-ironfist
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

## Build and play

On x86-64 Linux with Nix flakes enabled, from this directory, with your Buka
installation (its original `DATA`, `MAPS`, music and video files):

```sh
nix build                  # Russian (default)
nix build .#game-en        # English instead
cp result/HMM2PL.exe result/run-game.sh /path/to/buka-installation/
cd /path/to/buka-installation
./run-game.sh
```

The runner uses Wine and creates `.wineprefix` beside the game. Set
`HOMM2_WINEPREFIX` to choose another prefix. No retail assets are stored in
this repository.

## Build

In the supplied development shell:

```sh
nix develop
./build.py --ru             # Russian (default if omitted)
./build.py --en             # English
```

The executables are written to `build/ru/HMM2PL.exe` and
`build/en/HMM2PL.exe`; each locale has its own objects and generated compiler
inputs. `-j N` and `-v` select parallel jobs and verbose commands. Ninja directly
also works: `ninja` builds Russian and `ninja -f build-en.ninja` builds English.
A non-Nix environment needs Python 3, Ninja, Clang, LLD, LLVM dlltool and a
32-bit MinGW toolchain.

The source keeps every piece of game text as `localization::Tr("semantic.id")`,
with the English registry in `locales/messages.def` and UTF-8 Russian in
`locales/ru.po`. Building resolves the selected catalog into literal
Windows-1251 bytes under `build/<locale>/localized/`, without changing the
authored source or adding runtime lookups. Edit the IDs and catalogs, not the
generated compiler files. English selects source text only: external game
assets are not translated, and these builds do not include Windows resources or
the retail icon.

What Gold 2.1 and Buka changed from Price of Loyalty 2.0 is in the
[version ledger](https://github.com/sushi-shi/homm2-decomp/blob/decomp-gold-2.1-buka/docs/version-changes.md).

## Regeneration

`decomp-gold-2.1-buka` generates this branch with `homm2 clean`. Make source
changes there and regenerate; do not edit this branch by hand.

## License

Project-authored reconstruction source and tooling are dedicated to the public
domain under [CC0 1.0](LICENSE), to the extent the contributors can do so.
Files carrying separate copyright or license notices retain those terms. No
binary game assets are stored in this repository; retail inputs and build
outputs incorporating them are not covered by this dedication.

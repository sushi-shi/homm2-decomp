# Heroes of Might and Magic II — Price of Loyalty 2.0 source

C++ source for Heroes of Might and Magic II — The Price of Loyalty 2.0
(`HEROES2W.EXE`, Windows, New World Computing, 1997), generated from the
reconstruction with its matching machinery removed. It builds a 32-bit Windows
program with Clang and MinGW.

## Branches

```text
decomp-pol-2.0 -------------------> decomp-gold-2.1-buka
    |                                   |
    +------------------+                +------------------------+
    |                  |                |                        |
    v                  v                v                        v
source-pol-2.0 (you are here)     classic-pol-2.0   source-gold-2.1-buka    classic-gold-2.1-buka
    |                                   |
    +-----------------+-----------------+
                      |
                      v
                    port --------> source-ironfist --------> port-ironfist
```

- [`decomp-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/decomp-pol-2.0#branches) — Price of Loyalty 2.0 `HEROES2W.EXE` (1997), VC4.2
- [`source-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/source-pol-2.0#branches) — Clean source, PoL 2.0
- [`classic-pol-2.0`](https://github.com/sushi-shi/homm2-decomp/tree/classic-pol-2.0#branches) — Reading view, PoL 2.0
- [`decomp-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/decomp-gold-2.1-buka#branches) — Gold 2.1 (Buka) game and editor, byte-identical
- [`source-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/source-gold-2.1-buka#branches) — Clean source, Gold 2.1 (ru/en)
- [`classic-gold-2.1-buka`](https://github.com/sushi-shi/homm2-decomp/tree/classic-gold-2.1-buka#branches) — Reading view, UTF-8 Russian
- [`port`](https://github.com/sushi-shi/homm2-decomp/tree/port#branches) — Native port: Linux, Windows, browser
- [`source-ironfist`](https://github.com/sushi-shi/homm2-decomp/tree/source-ironfist#branches) — Project Ironfist on the source
- [`port-ironfist`](https://github.com/sushi-shi/homm2-decomp/tree/port-ironfist#branches) — Project Ironfist on the port

## Build and play

On x86-64 Linux with Nix flakes enabled, from this directory, with your PoL
installation (its original `DATA`, `MAPS`, music and video files):

```sh
nix build
cp result/HEROES2W.EXE result/run-game.sh /path/to/pol-installation/
cd /path/to/pol-installation
./run-game.sh
```

The runner uses Wine and creates `.wineprefix` beside the game. Set
`HOMM2_WINEPREFIX` to choose another prefix. No retail assets are stored in
this repository.

## Build

In the supplied development shell:

```sh
nix develop
ninja game
```

The executable is written to `build/HEROES2W.EXE`. A non-Nix environment needs
Ninja, Clang, LLD, LLVM dlltool and a 32-bit MinGW toolchain.

What Gold 2.1 and Buka changed is in the
[version ledger](https://github.com/sushi-shi/homm2-decomp/blob/decomp-gold-2.1-buka/docs/version-changes.md).

## Regeneration

`decomp-pol-2.0` generates this branch with `homm2 clean`. Make source changes
there and regenerate; do not edit this branch by hand.

## License

Project-authored reconstruction source and tooling are dedicated to the public
domain under [CC0 1.0](LICENSE), to the extent the contributors can do so.
Files carrying separate copyright or license notices retain those terms. No
binary game assets are stored in this repository; retail inputs and build
outputs incorporating them are not covered by this dedication.

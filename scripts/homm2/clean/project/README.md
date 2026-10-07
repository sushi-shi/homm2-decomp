# Heroes of Might and Magic II — Gold 2.1 (Buka) source

C++ source for the Buka release of Heroes of Might and Magic II Gold 2.1
(`HMM2PL.exe`, Windows), built as a 32-bit Windows program with Clang and
MinGW. The text lives in a catalog: `locales/messages.def` (English) and
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

On x86-64 Linux with Nix flakes enabled, from this directory, with your copy of
the Buka game (an installed game folder with its original `DATA`, `MAPS`, music
and video files):

```sh
nix build                  # Russian (default)
nix build .#game-en        # English instead
cp result/HMM2PL.exe result/run-game.sh /path/to/buka-installation/
cd /path/to/buka-installation
./run-game.sh
```

`nix build` builds `result/HMM2PL.exe`; `run-game.sh` runs it under Wine with a
prefix in `.wineprefix` beside the game (`HOMM2_WINEPREFIX` chooses another).
No retail assets are stored in this repository.

## Build

On x86-64 Linux with Nix flakes enabled, from this directory:

```sh
nix develop -c python3 build.py --ru   # Russian (default)
nix develop -c python3 build.py --en   # English
```

This writes `build/ru/HMM2PL.exe` or `build/en/HMM2PL.exe`, each locale with its
own objects. The flake supplies Clang, LLD, LLVM's dlltool, Ninja and a 32-bit
MinGW toolchain. `-j N` and `-v` select parallel jobs and verbose commands;
Ninja directly also works (`ninja`, or `ninja -f build-en.ninja` for English).
These builds do not include Windows resources or the retail icon.

The source keeps every piece of game text as `localization::Tr("semantic.id")`.
The build resolves each ID to the selected language as literal Windows-1251
bytes in a copy of the sources under `build/<locale>/localized/`; nothing is
looked up at run time. Edit the IDs and catalogs, not the copies. English
selects source text only; the game's data files stay as installed.

`nix build` and `run-game.sh` run the results with your game data; see
[Build and play](#build-and-play).

## Regeneration

`decomp-gold-2.1-buka` generates this branch with `homm2 clean`. Make source
changes there and regenerate; do not edit this branch by hand.

## License

Original project contributions are dedicated to the public domain under
[CC0 1.0](LICENSE), to the extent of the contributors' rights. This does not
grant rights to New World Computing's or Buka's game or to the Microsoft, RAD
Game Tools or other third-party material it uses. Game assets are not included.

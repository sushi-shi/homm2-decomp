# Heroes of Might and Magic II: `source-ironfist`

This branch carries Project Ironfist on the cross-platform engine of Heroes of
Might and Magic II Gold 2.1 (Buka), reimplemented on the recovered types for
Linux, Windows and the browser. It is built from `port`; `port-ironfist` follows.

## Build, run and test

```sh
nix develop                     # the native toolchain
cmake -S . -B build -G Ninja    # no presets on this branch
cmake --build build
HOMM2_DATA=DIR build/homm2
ctest --test-dir build          # synthetic fixtures, no game data
nix flake check                 # native, windows, web
nix run .#ironfist-resources -- DIR   # install Ironfist's resources into the game
HOMM2_DATA=DIR nix run          # the game; .#web packs and serves the page
```

## Layout

| Path | Contents |
| --- | --- |
| `src/SOURCE`, `src/BASE`, `src/IRONFIST` | the game, its engine library, Ironfist (headers: `include/`) |
| `src/PLATFORM`, `src/EXECUTABLE` | SDL3 platform layer; entry points per system |
| `tools/`, `scripts/` | ctest programs and checks; Ironfist resource build |
| `web/`, `vendor/`, `locales/` | browser page; Lua and tinyxml2; translations |

## Headless testing hooks

- `HOMM2_SCREENSHOT=PREFIX` writes presented frames as `PREFIX.NNNNNN.ppm`: the
  first only, or every Nth with `HOMM2_SCREENSHOT_EVERY=N`.
- `HOMM2_INPUT_REPLAY=FILE` replays input, one `MS ACTION ARGS` a line: `move`,
  `left-down`, `left-up`, `right-down`, `right-up` take `X Y`; `key-down`,
  `key-up` an SDL key name; `text` committed UTF-8 (`1200 text Привет`).
- `HOMM2_RESOURCE_PROFILE=western|buka-cyrillic` overrides the detected fonts.
- The `input_replay` ctest checks the replay parser and its clock.

## Branch rules

- `port` and the edition sources (`source-gold-2.1-buka`, `source-pol-2.0`) stay
  faithful to retail behaviour; `docs/retail-divergences.md` lists what must differ.
- `source-ironfist` and `port-ironfist` fix bugs as normal code.
- Windows builds stay fully static: `HMM2PL.exe` needs no DLL beside it.
- Merge only changes that add no new complexity.
- Don't commit game data or retail binaries.

## Documentation

`README.md` (playing, installing) and `docs/` (`ironfist-port.md`, `porting.md`).

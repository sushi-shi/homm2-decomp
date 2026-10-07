# Heroes of Might and Magic II: `port`

This branch ports Heroes of Might and Magic II Gold 2.1 (Buka) to Linux, Windows
and the browser. It is built from `source-gold-2.1-buka` and `source-pol-2.0`,
the editions' clean C++ sources, with SDL3 in place of Windows.

## Build, run and test

```sh
nix develop                     # .#web: the browser toolchain
cmake --preset linux            # presets: linux, wasm (emcmake, in .#web)
cmake --build --preset linux
HOMM2_DATA=DIR build/linux/homm2
ctest --test-dir build/linux    # synthetic fixtures, no game data
nix flake check                 # native, sanitized, windows, windows-tests, web, ...
nix build .#homm2-windows       # also .#homm2-linux, .#homm2-web, .#homm2-sanitized
HOMM2_DATA=DIR nix run          # the launcher; .#web packs and serves the page
```

## Layout

| Path | Contents |
| --- | --- |
| `src/SOURCE`, `src/BASE`, `src/EDITOR` | the game, its engine library, map cells (headers: `include/`) |
| `src/PLATFORM`, `src/EXECUTABLE` | SDL3 platform layer; entry points per system |
| `tools/` | ctest programs (`*_test.cpp`), Python checks, input replays |
| `nix/`, `web/`, `locales/` | launcher; browser page; translations |

## Headless testing hooks

- `HOMM2_SCREENSHOT=PREFIX` writes presented frames as `PREFIX.NNNNNN.ppm`: the
  first only, or every Nth with `HOMM2_SCREENSHOT_EVERY=N`.
- `HOMM2_INPUT_REPLAY=FILE` replays input, one `MS ACTION ARGS` a line: `move`,
  `left-down`, `left-up`, `right-down`, `right-up` take `X Y`; `key-down`,
  `key-up` an SDL key name; `text` committed UTF-8 (`1200 text Привет`).
- `HOMM2_USER_DATA=DIR` puts saves, settings and crash reports in `DIR`.
- `tools/gameplay_roundtrip.py` replays a save round trip on SDL's dummy drivers.

## Branch rules

- `port` and the edition sources (`source-gold-2.1-buka`, `source-pol-2.0`) stay
  faithful to retail behaviour; `docs/retail-divergences.md` lists what must differ.
- `source-ironfist` and `port-ironfist` fix bugs as normal code.
- Windows builds stay fully static: `HMM2PL.exe` imports only Windows' own DLLs.
- Merge only changes that add no new complexity.
- Don't commit game data or retail binaries.

## Documentation

`README.md` (playing, installing) and `docs/` (`porting.md`: builds, hooks).

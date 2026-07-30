# Native port

The branch chain is:

```text
decomp-pol-2.0
  -> source-pol-2.0
  -> master-pol-2.0
  -> port-pol-2.0
```

Core game fixes belong upstream. `port-pol-2.0` owns only `PLATFORM` and its
native build.

```text
include/PLATFORM/          interfaces
include/PLATFORM/WIN32/    Win32 declarations used by the game
src/PLATFORM/WIN32/        Win32 translation
src/PLATFORM/SDL3/         native SDL3 implementation
```

All 95 game translation units compile directly. There are no override units.

## Run

The native build needs an installed copy of the game data:

```sh
HOMM2_DATA=/path/to/heroes2 nix run .
```

Other targets:

```sh
nix build .#homm2-unwrapped
nix build .#homm2-unwrapped-debug
nix build .#homm2-windows
```

The Windows package remains byte-identical to the build from
`master-pol-2.0`.

## Current platform

Linux uses a 32-bit SDL3 build. The 32-bit target preserves retail pointer
width and packed layouts. Additional systems belong under `PLATFORM`.

Video and input run natively. Audio is silent, cinematics are skipped, and
network transports fail cleanly.

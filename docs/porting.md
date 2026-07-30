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
nix build .#homm2-linux
nix build .#homm2-windows
nix build .#homm2-web
```

The Windows package cross-compiles the SDL3 platform and minimal FFmpeg. It
includes the required DLLs and a Wine launcher.

## Web

The WebAssembly build cross-compiles SDL3 and a minimal FFmpeg. Game data is
kept outside the Nix store:

```sh
HOMM2_DATA=/path/to/heroes2 nix run .#web
```

The launcher packages the installed data, opens a local server, and stores the
web bundle under `~/.cache/homm2-web`. Set `HOMM2_WEB_OUTPUT` to change it.

## Current platform

Linux uses a 32-bit SDL3 build. The 32-bit target preserves retail pointer
width and packed layouts. Additional systems belong under `PLATFORM`.

Video, input, audio, and cinematics use the platform layer. Network transports
are not supported yet.

# Native port

The maintained branch chain is:

```text
decomp-gold-2.1-buka -> source-gold-2.1-buka -> master -> ironfist -> ironfist-master
```

`master` is the portable Gold 2.1 line. Matching-only changes stay on the
decomp branches; portable platform, localization, and runtime changes belong
here and flow onward through `ironfist` to `ironfist-master`.

Proven retail defects and deliberately replaced subsystems are tracked in
[Intentional retail divergences](retail-divergences.md).

```text
include/PLATFORM/          interfaces
include/PLATFORM/WIN32/    Win32 declarations used by the game
src/PLATFORM/WIN32/        Win32 translation
src/PLATFORM/SDL3/         native SDL3 implementation
```

All 97 game translation units compile directly. There are no override units.

## Run

The native build needs an installed copy of the game data:

```sh
HOMM2_DATA=/path/to/heroes2 nix run .
```

On the `ironfist` or `ironfist-master` branch, install the pinned Ironfist
resource payload into the same writable game directory first:

```sh
nix run .#ironfist-resources -- /path/to/heroes2
```

`HOMM2_DATA` wins when it is set. Otherwise the engine looks for a directory
holding `DATA/HEROES2.AGG`, in order: next to the executable, the current
directory, `$XDG_DATA_HOME/homm2`, `~/.local/share/homm2/data`, `~/games/homm2`.

Other targets:

```sh
nix build .#homm2
nix build .#homm2-debug
nix build .#homm2-linux
nix build .#homm2-windows
nix build .#homm2-web
```

The Windows package cross-compiles and statically links the SDL3 platform,
libbz2, and minimal FFmpeg. It includes a Wine launcher; no retail Audiere,
Miles, Smacker, or Wing DLLs are required.

## Web

The WebAssembly build cross-compiles SDL3, libbz2, and a minimal FFmpeg. Game
data is kept outside the Nix store:

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

# Native port

The maintained branch chain is:

```text
decomp-gold-2.1-buka -> source-gold-2.1-buka -> master -> ironfist
```

`master` is the portable Gold 2.1 line. Matching-only changes stay on the
decomp branches; portable platform, localization, and runtime changes belong
here and flow onward to `ironfist`.

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

The legacy renderer can perform several low-level blits while temporarily
drawing and restoring the software cursor. Those blits update the platform
framebuffer, but the completed result is presented only once by the owning
high-level screen blit. This keeps intermediate cursor and sprite composition
out of the visible SDL frame while preserving the original scrolling source
rectangle.

## Display controls

The game keeps its 640×480 logical surface. Use these shortcuts while playing
or in a game dialog; the window title shows the current settings:

| Shortcut | Setting |
| --- | --- |
| F4 | Toggle borderless desktop fullscreen |
| Shift+F4 | Cycle nearest, linear, and integer scaling |
| Ctrl+F4 | Toggle VSync, when the render driver supports it |

Nearest and linear modes preserve the aspect ratio with borders. Integer mode
uses whole-number scaling; windows smaller than the logical surface may crop
it, so resize the window or select another mode to see the whole game.
Mouse input follows the current presentation transform. Changing modes preserves
the screen buffer, palette, and composed software cursor. Holding F4 performs
one change per press. Shift takes precedence if both Shift and Ctrl are held.

Settings are saved in `HEROES2.DISPLAY` beside the user's `HEROES2.CFG`; the
retail binary configuration format is unchanged. Delete that text file to reset
scaling/VSync and fall back to the legacy fullscreen preference. Malformed files
are ignored. A valid file has this format:

```text
H2DISPLAY 1
fullscreen 0
scaling nearest
vsync 0
```

Native startup restores fullscreen when the window system accepts it. Browsers
start windowed because fullscreen requires a user gesture; use F4 after starting
the game. A denied fullscreen/VSync request is logged and keeps the available
mode. Web preferences use the existing browser storage mechanism; saving this
small preference file does not itself acknowledge durable IndexedDB persistence.
The browser can restore its canvas backing store after SDL's fullscreen resize
callback. Presentation checks the final canvas dimensions against SDL's pixel
size and reconciles a mismatch before drawing, preserving the viewport and
mouse-coordinate mapping when CSS scales the canvas to fit the page.

The `display_settings` CTest exercises the production SDL backend with its dummy
render driver: repeated fullscreen transitions, all three scaling policies,
resizing above/below the logical size, outside-border mouse mapping, device
reset, cursor/palette frame preservation, F4 repeat suppression, preference
round trips, and malformed preference files. Driver-specific display behavior
still needs testing on the intended desktop/browser.

## Deterministic input replay

The SDL3 host can replay timestamped framebuffer input without coupling the
parser or clock to SDL. Set `HOMM2_INPUT_REPLAY` to a UTF-8 text file. Each
non-comment line begins with a millisecond offset and one of these actions:

```text
0 move 320 240
20 left-down 320 240
40 left-up 320 240
60 key-down Return
61 key-up Return
80 text Привет
```

Mouse actions are `move`, `left-down`, `left-up`, `right-down`, and
`right-up`; their coordinates are in the 640x480 logical framebuffer. Key
names use SDL's key-name vocabulary. Quote names containing spaces, such as
`key-down "Left Shift"`, and send the matching `key-up` event after the chord. Blank lines and lines beginning with `#`
are ignored. Timestamps must be nondecreasing, and events sharing a timestamp
retain file order. A malformed line rejects the complete new replay and is
reported with its line number instead of being silently skipped.

The parser and wrapping-millisecond scheduler are covered by the
`input_replay` CTest. A headless gameplay test can therefore run the ordinary
binary under Xvfb with a checked-in replay and compare an independently chosen
observable result such as a save, log record, or screenshot.

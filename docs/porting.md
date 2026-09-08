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
nix build .#homm2-linux64
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

Linux defaults to the existing 32-bit SDL3 build. `homm2-linux64` provides an
opt-in x86-64 Linux executable using 64-bit dependencies. Its development shell
is `nix develop .#linux64`; configure a manual build there with
`-DHOMM2_32BIT=OFF`. The ordinary development shell and default package continue
to build the 32-bit game. Additional systems belong under `PLATFORM`.

Hero, army, town, player, campaign, setup, mine, and boat saves use fixed-size
little-endian byte records in `SaveRecords.h`. Their runtime classes use natural
alignment; changing runtime padding cannot change these records. Text conversion
stays at the file boundary. The codec checks exact input lengths before changing
state and preserves the retail base/expansion formats, reserved bytes, enum
storage widths, and overlapping town spell/count fields. The obsolete four-byte
expansion-campaign window-pointer slot is written as zero and ignored on read.
Unsaved runtime fields, including live pointers and AI state, remain untouched.

`save_records` tests each record's size, known scalar bytes, all truncation
lengths, and round trips without retail assets. The codecs are also suitable for
64-bit builds. The native flake checks build and test both Linux widths.
Packed map, resource, and preference records still require little-endian targets,
which CMake checks explicitly. Networking remains unsupported and its internal
runtime-object messages are not a retail wire-compatibility contract.

`i32l`/`u32l` retain the retail `long` type where it is four bytes and use fixed
32-bit types on LP64 hosts. Debug address formatting uses pointers directly.
Dialog results are transferred explicitly between message variants, whose
pointer-bearing fields need not have the same offsets on different hosts.
Adventure-panel widgets occupy naturally aligned pointer arrays; integer
overlays and fixed-byte padding no longer determine their initialization or
cleanup slots.

The ARM evaluation is reproducible with `nix build .#homm2-arm64-check`. It
compiles every core game translation unit for AArch64, then runs the save-record
and dialog-result tests under QEMU user-mode emulation. It does not link the ARM
SDL application or validate graphics/audio on ARM hardware. That remaining work
is distinct from the tested x86-64 Linux executable.

Video, input, audio, and cinematics use the platform layer. Network transports
are not supported yet.

The legacy renderer can perform several low-level blits while temporarily
drawing and restoring the software cursor. Those blits update the platform
framebuffer, but the completed result is presented only once by the owning
high-level screen blit. This keeps intermediate cursor and sprite composition
out of the visible SDL frame while preserving the original scrolling source
rectangle.

## Instrumented native checks

`nix build .#homm2-sanitized` builds the native game and runs CTest with
AddressSanitizer and UndefinedBehaviorSanitizer. This check is also included in
`nix flake check`. Sanitizer findings fail the run; address, alignment, and
integer-undefined-behavior diagnostics are not suppressed.

For a local build, configure a separate build directory:

```sh
cmake -S . -B build/sanitized -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo -DHOMM2_SANITIZERS=ON
cmake --build build/sanitized
ctest --test-dir build/sanitized --output-on-failure
```

Run the instrumented game with installed resources to check gameplay paths;
the asset-free CTests do not exercise a complete game. Windows and Web retain
their own build checks; this option supports native GCC and Clang only.

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

Keypad movement preserves the original physical keypad scan codes with Num Lock
on or off. Replay accepts SDL names such as `Keypad 1`, `Keypad 8`, and
`Keypad +`, independently of the dedicated navigation keys.

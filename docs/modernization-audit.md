# Master modernization audit — 2026-09-08

Audited revision: `16df440a` (`master` and local `origin/master` at audit start).
The worktree was clean. The remote tracking reference was not refreshed.

Implementation follow-up: [feature PRs, review order and validation
evidence](modernization-review.md). The findings below describe the audited
revision; the follow-up changes remain open for review.

**Yes, master needs further updates.** Its build and platform foundations are
already modern: C++20, SDL3, FFmpeg, libbz2, UTF-8 localization, a platform
interface, and reproducible Linux/Windows/Web builds. The strongest reasons for
further work are unchecked legacy data boundaries, incomplete regression
coverage, save durability, and dependency maintenance. A newer C++ language
standard alone would address none of the reproduced failures below.

This is an investigation and a proposed work order. No game implementation,
dependency pin, or existing test was changed. The accompanying probes preserve
the diagnostic inputs; they deliberately fail on the audited revision.

## Verification performed

`nix flake check --offline --no-update-lock-file -L` completed successfully.

| Check | Observed result | Scope |
| --- | --- | --- |
| Native | Game built; 21/21 CTests passed | Includes SDL backend/lifecycle, filesystem, text, timing and binary primitives; see the assertion caveat below |
| Windows | Static MinGW executable built | Compilation/linking, not a Windows or Wine gameplay run |
| Web | Emscripten bundle built | Compilation/linking/packaging, not a browser gameplay run |
| Rust icon tool | 31 integration tests and 2 doctests passed; Clippy and formatting passed | Includes four C++ parity tests; robustness coverage primarily exercises the separate Rust model |
| Malformed map description | AddressSanitizer heap-buffer-overflow; terminated control passed | Production text-detection functions with a synthetic header |
| Missing ICN stream | AddressSanitizer heap-buffer-overflow; terminated control passed | Production C++ blitter through the existing parity shim |
| Long extensionless save name | AddressSanitizer stack-buffer-overflow; short-name control passed | Production filename helper; ordinary UI reachability not established |
| Layout-test mutation | Deliberately wrong expected size passed with `NDEBUG`; failed with assertions enabled | Demonstrates an ineffective check in the native optimized test build |
| Web lowercase preflight | Launcher exited 1 for `data/heroes2.agg` | Synthetic file used only to test path discovery; not a game archive |

The observed native toolchain was GCC 15.2.0 and CMake 4.1.2, with SDL 3.4.8,
FFmpeg 8.1 and bzip2 1.0.8. Windows used GCC 15.2.0; Web used Emscripten.
Native configuration completed despite pkg-config diagnostics about missing
`alsa.pc` in the development shell. The flake emitted advisory app-metadata
warnings. These did not prevent successful builds.

No full campaign, combat playthrough, retail save round trip, or browser
persistence session was run. This audit establishes specific defects and build
health; it does not certify all gameplay paths or classify exploitability.

## Prioritized findings

### 1. High: fixed-width map text is consumed as an unbounded C string

[`GetMapHeader`](../src/SOURCE/REQUEST.cpp) checks that all 420 header bytes were
read, but does not validate or terminate `name[60]` and `description[300]` from
[`SMapHeader`](../include/SOURCE/REQUEST.h). `GetMapHeaderTextEncoding` passes
both directly to `DetectTextEncoding`, which calls `utf8::IsValid` and scans
until a null byte. `game::LoadMap` uses this path too.

The probe sets the description and following two count bytes to nonzero ASCII.
ASan reports a one-byte read immediately after the 420-byte allocation in
`src/BASE/Utf8.cpp:135`, through `DetectTextEncoding` and
`GetMapHeaderTextEncoding`. Adding a terminator inside the description makes
the control pass. A complete read therefore does not establish a valid record.

**Proposed update:** introduce bounded decoding for serialized text, validate
header fields before using them, and apply the same discipline to fixed-width
hero, town, rumour and player fields in save loading. Reject malformed records
with a useful error. Exercise exact-capacity strings, missing terminators,
truncated multibyte encodings, invalid counts and enum values under ASan/UBSan.

### 2. High: the live resource decoder trusts archive content

[`icon::icon`](../src/BASE/ICON.cpp) allocates the ICN-declared length, reads it,
and retains no payload length. [`IconToBitmap`](../src/BASE/Icon2b.cpp) indexes
the requested frame, trusts `srcOffset`, and advances its stream pointer
without an end pointer. A frame table with no stream byte produces an ASan
heap-buffer-overflow at `src/BASE/Icon2b.cpp:63`, even when clipping is requested.

[`resourceManager`](../src/BASE/RESMGR.cpp) checks that the AGG directory fits
the file, but does not validate every member's offset/length or constrain reads
to the selected member. `ReadBlock` logs and zeroes short payloads, allowing
callers to continue with invalid resource content. The
[`tileset`](../src/BASE/TILESET.cpp) and [`bitmap`](../src/BASE/BITMAP.cpp)
constructors also use file-provided dimensions in allocation calculations.
Those additional paths were inspected, not exhaustively fuzzed.

The safer [`homm2-icon` Rust crate](../tools/homm2-icon-rs/README.md) is explicitly
a separate model and testbed. Its passing robustness tests do not protect the
game's renderer.

**Proposed update:** validate AGG member spans, preserve payload sizes, validate
frame tables/indices/offsets/dimensions, bound RLE reads and destination writes,
and stop resource loading on failure. Consolidate repeated blitters only with
retail pixel/parity evidence. Test malformed inputs against the code that the
game actually calls, and compare valid retail rendering after each change.

### 3. High for validation: one layout test silently loses every assertion

[`tools/mapcell_layout_test.cpp`](../tools/mapcell_layout_test.cpp) uses runtime
`assert` for all ten expectations. The native flake check explicitly builds
`RelWithDebInfo`; its command includes `-O2 -g -DNDEBUG`. The resulting `main`
is simply `xor %eax,%eax; ret`.

To distinguish disabled checking from valid constant folding, a temporary copy
changed `assert(sizeof(cell) == 12)` to the false expectation `== 13`. It still
exited 0 with `-DNDEBUG`, and aborted with `-UNDEBUG`. The original test passed
with assertions enabled. This is a coverage defect, not evidence that today's
map layout is wrong. Other inspected tests mostly use `static_assert` or
explicit failure returns and do not share this problem.

**Proposed update:** use compile-time assertions where possible and explicit
runtime failure reporting for byte-layout checks. Require a deliberately wrong
expectation to fail in Release as well as Debug. Add an instrumented native
check and gameplay replays with observable save/frame results; the existing
`input_replay` test covers parsing/scheduling, not a played game. No checked-in
GitHub/GitLab/Jenkins CI configuration was found; external CI was not inspected.

### 4. Medium: saves overwrite the previous file before completion

[`game::SaveGame`](../src/SOURCE/GAME.cpp), around line 1194, opens the final
destination in `FileMode::Write`. The SDL backend maps that to `"wb"`, then
many separate writes build the save. A failed write calls shutdown. The
[`filesystem interface`](../include/PLATFORM/FileSystem.h) returns no close
status and has no replacement/commit operation; SDL close failures are logged.

This is a source-confirmed durability gap. Disk-full and forced-interruption
scenarios were not injected. A partial write or interruption can leave the old
save replaced by an incomplete file, including the repeatedly reused autosave.

**Proposed update:** write a sibling temporary file, check writes and close,
then replace the destination through an explicit platform operation. Preserve
the previous save on failure. Define native flush/durability expectations and
browser persistence semantics separately. Validate with fault injection at
multiple write positions and a successful save/load round trip.

### 5. Medium: update dependency pins on a tested maintenance schedule

[`flake.nix`](../flake.nix) pins nixpkgs directly to
`64c08a7ca051951c8eae34e3e3cb1e202fe36786`; the lock's source date is May 2026.
The observed SDL 3.4.8 and FFmpeg 8.1 are behind available maintenance releases:

| Dependency | Audited build | Available upstream on 2026-09-08 |
| --- | --- | --- |
| SDL | 3.4.8 | [3.4.16, a stable bugfix release](https://github.com/libsdl-org/SDL/releases/tag/release-3.4.16) |
| FFmpeg | 8.1 | [8.1.2 on the same branch; 9.0.1 on the newer major branch](https://ffmpeg.org/download.html) |

FFmpeg's [security ledger](https://www.ffmpeg.org/security.html) lists fixes
under 8.1.2. Applicability to this project's enabled codecs and any Nix patches
was not established, so this is not a claim of a demonstrated FFmpeg exploit.

**Proposed update:** refresh the explicit nixpkgs revision and lock together,
or apply reviewed dependency overrides, then rebuild all three targets and
exercise Smacker playback, Ogg music, input and display behavior. Assess the
8.1 maintenance update separately from a 9.x API migration. A reproducible old
pin still needs scheduled review; changing only `flake.lock` will not advance
an input whose URL names an exact revision.

### 6. Medium: Web packaging disagrees with case-insensitive data discovery

The native resolver walks path components case-insensitively, as documented.
The [`web launcher`](../flake.nix), lines 310–330, checks only
`DATA/HEROES2.AGG` and `DATA/heroes2.agg`, then preloads uppercase directory and
campaign/config names. Lowercase `data` is rejected; lowercase `maps` or
`music` can be omitted even when the archive check passes.

The actual `nix run .#web` launcher was run with a synthetic file at
`/tmp/homm2-audit-lowercase/data/heroes2.agg`, `HOMM2_WEB_PACK_ONLY=1`, and an
isolated output directory. It exited 1 with the request to set `HOMM2_DATA`,
before inspecting archive contents. This confirms the lowercase-directory
preflight failure; omitted map/music directories were identified from source.

**Proposed update:** resolve the supported data names consistently before
packaging, preserving the native resolver's ambiguity handling. Add synthetic
packaging tests for uppercase, lowercase, mixed case and ambiguous names. Keep
these separate from browser gameplay tests and retail assets.

### 7. Medium source debt: legacy save-name conversion has no capacity contract

[`GenerateStandardFileName`](../src/SOURCE/GAME.cpp), line 1084, uses `strcpy`
when the input has no dot. Its `SaveGame` caller provides a 100-byte scratch
buffer. Passing a 101-character extensionless name produces a 102-byte write
and an ASan stack-buffer-overflow at line 1087; an eight-character control
passes. An oversized extension is also copied without a bound.

The ordinary save dialog appends a short extension in
[`fileRequester::GetFilename`](../src/SOURCE/REQUEST.cpp), and its caller in
[`ADVMGR.cpp`](../src/SOURCE/ADVMGR.cpp) uses that result. The audit therefore
does **not** establish that simply typing a long ordinary save name reaches the
demonstrated failure. It establishes a helper with an unsafe input contract.

**Proposed update:** express the serialized filename's capacity explicitly,
normalize both extensionless and extended inputs into that bound, avoid
mutating the input string, and preserve the established retail wire field.
Cover empty, long, extensionless and Unicode names with boundary tests.

### 8. Planned architecture work: native 64-bit portability needs serialization work

The 32-bit target is deliberate and currently working. It limits future host
support and requires 32-bit dependencies. The flake exports only an
`x86_64-linux` build host, with i686 Linux/Windows and wasm32 game targets.
[`Ints.h`](../include/Ints.h) still defines `i32l/u32l` using `long`,
[`Misc.cpp`](../src/BASE/Misc.cpp) contains a pointer-to-`i32` cast, and game
objects remain packed with raw `sizeof`-based serialization.

**Proposed update:** separate fixed-width wire records from runtime classes,
make endianness explicit, replace pointer narrowing, and add record-size and
round-trip tests before introducing a 64-bit target. Keep the proven 32-bit
build available during that work. Disabling `HOMM2_32BIT` alone is not a port.
GNU-style compiler flags are also applied unconditionally to the game target;
support for MSVC would need a separate build-policy change and validation.

### 9. Smaller build and player-facing gaps

The [manual-build prerequisites](../README.md) omit Python 3 and gettext's
`msgfmt`, `msgcat`, and `xgettext`, which CMake requires. A fresh manual
configuration also left `CMAKE_BUILD_TYPE` empty: the attempted cached default
is set after `project()` has created that cache entry. Nix avoids that issue by
passing its build type explicitly. Document the test command and make manual
configuration select an intentional build type.

The game still presents a fixed 640×480 logical surface with nearest-neighbor
scaling. [`SetFullScreenStatus`](../src/SOURCE/wingraph.cpp), line 83, is a no-op;
startup uses the default windowed mode and forces the saved fullscreen flag
false, although the SDL backend can open a fullscreen window. Expose a working
in-place fullscreen toggle, scaling and presentation settings before considering
a larger UI/resolution redesign. Verify mouse-coordinate mapping and cursor
composition across each display mode.

Legacy DirectPlay/serial/NetBIOS entry points in
[`Transports.cpp`](../src/PLATFORM/Transports.cpp) remain stubs. Unsupported
networking is already documented; it is a separate feature project requiring
protocol and input validation, not a missing-library build fix.

## Recommended implementation order

1. Repair the ineffective layout test; add ASan/UBSan coverage for production
   parsers and turn the confirmed malformed-input probes into rejection tests.
2. Bound map/save text and AGG/ICN/TIL payloads, then make save replacement
   transactional. Check valid retail input and update the retail-divergence
   ledger for intentional behavior changes.
3. Refresh dependency pins, fix Web case handling, and complete manual-build
   instructions. Run the full flake checks plus media and persistence smoke tests.
4. Add repeatable gameplay/save/load checks and useful display settings.
5. Separate wire layouts from runtime state, then evaluate 64-bit/ARM builds
   and broader renderer or network changes.

## Reproducing the diagnostic probes

The three sources under [`audit-probes/`](audit-probes/) are intentionally
outside CTest and the Nix build input filter. They link production functions
with minimal startup substitutes. They establish function-level failures,
not complete game sessions. No retail assets are required.

From the repository root, enter `nix develop`, then build:

```sh
g++ -std=c++20 -O1 -g -fsanitize=address -fno-omit-frame-pointer \
  -ffunction-sections -fdata-sections -D__cdecl= -D__stdcall= -D__fastcall= \
  -Iinclude -I. docs/audit-probes/map-text.cpp src/SOURCE/REQUEST.cpp \
  src/SOURCE/LegacyText.cpp src/BASE/Utf8.cpp -Wl,--gc-sections \
  -o /tmp/homm2-audit-map-text

g++ -std=c++20 -O1 -g -fsanitize=address -fno-omit-frame-pointer \
  -ffunction-sections -fdata-sections -Iinclude docs/audit-probes/icon.cpp \
  tools/homm2-icon-rs/cxx/shim.cpp src/BASE/Icon2b.cpp -Wl,--gc-sections \
  -o /tmp/homm2-audit-icon

g++ -std=c++20 -O1 -g -fsanitize=address -fno-omit-frame-pointer \
  -ffunction-sections -fdata-sections -D__cdecl= -D__stdcall= -D__fastcall= \
  -Iinclude -I. docs/audit-probes/save-name.cpp src/SOURCE/GAME.cpp \
  -Wl,--gc-sections -o /tmp/homm2-audit-save-name
```

For each binary, `ASAN_OPTIONS=detect_leaks=0 <binary>` reports the described
buffer overflow and exits 1 on the audited revision. Adding `valid` supplies
the control input and exits 0. Leak checking is disabled only to focus these
small standalone diagnostics; it does not disable address checks.

For the layout mutation, copy `tools/mapcell_layout_test.cpp` to `/tmp`, change
only `assert(sizeof(cell) == 12)` to `assert(sizeof(cell) == 13)`, and compile
with `g++ -std=c++20 -O2 -DNDEBUG -Iinclude`. It incorrectly exits 0. Recompile
with `-UNDEBUG` following `-DNDEBUG`; the false expectation aborts. The
unmodified source passes with assertions enabled.

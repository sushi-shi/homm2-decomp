# Dependency maintenance

The Nix flake pins both the toolchain package set and explicit media maintenance
sources. Linux, Windows and Web use SDL 3.4.16 and FFmpeg 8.1.2; the source hashes
are shared through the `mediaMaintenance` overlay and the cross-build recipes.

The September 2026 review selected the
[SDL 3.4.16 bugfix release](https://github.com/libsdl-org/SDL/releases/tag/release-3.4.16)
and the [FFmpeg 8.1.2 maintenance release](https://ffmpeg.org/download.html).
FFmpeg 9.x is a separate API migration; the engine continues to use the 8.1
library interface here.

SDL's complete upstream test suite stays enabled. Its supported
`SDLTEST_TIMEOUT_MULTIPLIER=3` gives contended tests more scheduling time during
concurrent native/cross builds. The original 20-second outer limit for
`testrwlock` timed out in the initial i686 build; this changes the test deadline,
not the lock implementation, workload, or assertions.

Review upstream releases and relevant advisories monthly, and before shipping:

1. Check SDL and FFmpeg release notes and the
   [FFmpeg security ledger](https://www.ffmpeg.org/security.html). Determine which
   changes affect the enabled decoders, demuxers and platform backends.
2. Update the source versions and hashes in `flake.nix`. For a package-set
   refresh, update the exact nixpkgs revision in the input URL and regenerate
   `flake.lock` together. Review inherited patches when changing sources.
3. Run `nix flake check` for native tests, Windows/Web builds and the icon
   differential tests. Keep all targets on the same media versions.
4. Exercise Smacker playback, Ogg music, keyboard/mouse input and video output
   with installed resources. Check browser persistence and native saves when
   related dependencies change. Document what ran in the update PR.

The native `homm2_media_smoke_test` target decodes Ogg track 2, starts dummy SDL
audio, and decodes/presents eight frames of `DATA/IVYDES.SMK` using the production
movie and display backends. Run it with an installed game directory after a
native build:

```sh
./build/homm2_media_smoke_test /path/to/heroes2
```

It uses temporary user state and dummy SDL drivers, needs no screen or sound
device, and leaves the installed data untouched. It is an explicit smoke test,
not an asset-free CTest or an audible/visual quality assessment.

Source pins make builds repeatable; release/advisory review establishes when
those pins need an update. A version change alone does not prove that a
particular advisory applies to the engine's enabled features.

{
  description = "Heroes of Might and Magic II - Gold 2.1 (Buka) reconstruction";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";
  inputs.rust-overlay = {
    url = "github:oxalica/rust-overlay";
    inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, rust-overlay }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        overlays = [ rust-overlay.overlays.default ];
      };
      p32 = pkgs.pkgsi686Linux;
      mingw = pkgs.pkgsCross.mingw32;
      iconRust = pkgs.rust-bin.fromRustupToolchainFile
        ./tools/homm2-icon-rs/rust-toolchain.toml;
      # Only what the build reads. Everything else - docs, the README, the
      # flake itself - would otherwise rebuild all three targets when touched.
      source = builtins.path {
        path = ./.;
        name = "homm2-port-source";
        filter = path: type:
          let relative = pkgs.lib.removePrefix (toString ./. + "/") (toString path); in
          relative == "CMakeLists.txt"
          || pkgs.lib.any (kept: pkgs.lib.hasPrefix kept relative) [ "src" "include" "web" "locales" "tools" ];
      };

      # Emscripten writes its cache next to $HOME, which a build does not have.
      emscriptenHome = ''
        export HOME="$TMPDIR"
        export EM_CACHE="$TMPDIR/emscripten-cache"
      '';

      # The game needs Smacker video and Ogg music out of FFmpeg and nothing
      # else. Both cross builds have to decode the same things, so they share
      # one flag set.
      mkFfmpeg = { pname, stdenv, tools, targetOs, arch, nativeBuildInputs, preConfigure ? "" }:
        stdenv.mkDerivation {
          inherit pname nativeBuildInputs;
          inherit (pkgs.ffmpeg-headless) version src;
          configurePhase = ''
            runHook preConfigure
            ${preConfigure}./configure \
              --prefix="$out" \
              ${pkgs.lib.concatStringsSep " \\\n              " tools} \
              --target-os=${targetOs} \
              --arch=${arch} \
              --enable-cross-compile \
              --disable-everything \
              --disable-programs \
              --disable-doc \
              --disable-debug \
              --disable-network \
              --disable-autodetect \
              --disable-asm \
              --disable-avdevice \
              --disable-avfilter \
              --disable-swscale \
              --disable-pthreads \
              --disable-w32threads \
              --disable-os2threads \
              --disable-iconv \
              --disable-runtime-cpudetect \
              --enable-static \
              --disable-shared \
              --enable-avcodec \
              --enable-avformat \
              --enable-avutil \
              --enable-swresample \
              --enable-demuxer=smacker,ogg \
              --enable-decoder=smacker,smackaud,vorbis \
              --enable-protocol=file \
              --extra-cflags=-O2
            runHook postConfigure
          '';
          enableParallelBuilding = true;
        };

      ffmpeg-windows = mkFfmpeg {
        pname = "ffmpeg-windows";
        stdenv = mingw.stdenv;
        nativeBuildInputs = [ pkgs.gnumake pkgs.perl pkgs.pkg-config ];
        targetOs = "mingw32";
        arch = "x86_32";
        tools = [
          ''--cc="$CC"''
          ''--cxx="$CXX"''
          ''--ar="$AR"''
          ''--ranlib="$RANLIB"''
          ''--nm="$NM"''
          "--host-cc=${pkgs.stdenv.cc}/bin/cc"
        ];
      };

      # Statically, so that the Windows build is the one file it was in 1996.
      sdl3-windows = mingw.sdl3.overrideAttrs (previous: {
        cmakeFlags = previous.cmakeFlags ++ [
          "-DSDL_SHARED:BOOL=OFF"
          "-DSDL_STATIC:BOOL=ON"
        ];
      });

      # nixpkgs builds it shared only, and -static cannot link a DLL in.
      bzip2-windows = mingw.bzip2.override {
        enableStatic = true;
        enableShared = false;
      };

      # Windows' own DLLs. Anything else the programs import has to be in the
      # package, and nothing is: the build is one HMM2PL.exe plus lang/.
      windowsSystemDlls = [
        "advapi32.dll" "bcrypt.dll" "cfgmgr32.dll" "gdi32.dll" "hid.dll"
        "imm32.dll" "kernel32.dll" "msvcrt.dll" "ntdll.dll" "ole32.dll"
        "oleaut32.dll" "setupapi.dll" "shell32.dll" "user32.dll"
        "version.dll" "winmm.dll" "ws2_32.dll"
      ];

      windows = mingw.stdenv.mkDerivation {
        pname = "homm2-gold-buka";
        version = "2.1";
        src = source;
        nativeBuildInputs = [
          pkgs.cmake
          pkgs.gettext
          pkgs.ninja
          pkgs.pkg-config
          pkgs.python3
        ];
        buildInputs = [
          ffmpeg-windows
          sdl3-windows
          bzip2-windows
          mingw.windows.mcfgthreads
        ];
        cmakeFlags = [
          "-DHOMM2_32BIT=OFF"
          "-DHOMM2_PLATFORM=SDL3"
          "-DHOMM2_PLATFORM_WARNINGS_AS_ERRORS=ON"
          "-DCMAKE_BUILD_TYPE=Release"
          "-DCMAKE_EXE_LINKER_FLAGS=-static"
          # The path tests below; nothing runs them during a cross build.
          "-DBUILD_TESTING=ON"
        ];
        outputs = [ "out" "tests" ];
        # bin/ is the folder a player copies into the game directory.
        installPhase = ''
          runHook preInstall
          mkdir -p "$out/bin"
          cp homm2.exe "$out/bin/HMM2PL.exe"
          mkdir -p "$out/bin/lang"
          cp lang/*.mo "$out/bin/lang/"
          ln -s bin/HMM2PL.exe "$out/HMM2PL.exe"
          install -Dm755 homm2_filesystem_test.exe "$tests/bin/homm2_filesystem_test.exe"
          install -Dm755 data-root-test/homm2_data_root_test.exe "$tests/bin/homm2_data_root_test.exe"
          runHook postInstall
        '';
        # A DLL that is neither Windows' own nor shipped beside the program
        # is a "missing DLL" error on the player's machine. (An install check
        # would be skipped: the build machine cannot run the program.)
        postFixup = ''
          for program in "$out"/bin/*.exe "$tests"/bin/*.exe; do
            for dll in $(${mingw.stdenv.cc.targetPrefix}objdump -p "$program" | sed -n 's/^[[:space:]]*DLL Name: //p' | tr 'A-Z' 'a-z'); do
              case " ${pkgs.lib.concatStringsSep " " windowsSystemDlls} " in
                *" $dll "*) ;;
                *)
                  if [ ! -e "$(dirname "$program")/$dll" ]; then
                    echo "$program imports $dll, which is neither a Windows DLL nor shipped" >&2
                    exit 1
                  fi
                  ;;
              esac
            done
          done
          # (stdenv globs with nullglob: no match is an empty list.)
          for dll in "$out"/bin/*.dll; do
            echo "the Windows package is meant to be static, but ships $dll" >&2
            exit 1
          done
        '';
      };

      # The path tests of the Windows build, run by Wine. Neither needs a
      # display or the game data.
      windows-tests = pkgs.runCommand "homm2-windows-tests" {
        nativeBuildInputs = [ pkgs.wineWow64Packages.minimal ];
      } ''
        export HOME="$TMPDIR" WINEPREFIX="$TMPDIR/wine" WINEDEBUG=-all
        export WINEDLLOVERRIDES="mscoree,mshtml=" DISPLAY=
        # Wine names host files in the locale's charset; the tests use
        # non-ASCII folder names, as a Windows user name often is.
        export LC_ALL=C.UTF-8
        wineboot -i >/dev/null 2>&1
        # The data-root test creates a game folder beside itself.
        mkdir "$TMPDIR/tests"
        cp ${windows.tests}/bin/*.exe "$TMPDIR/tests/"
        chmod u+w "$TMPDIR"/tests/*.exe
        status=0
        for test in homm2_filesystem_test homm2_data_root_test; do
          echo "== $test"
          wine "$TMPDIR/tests/$test.exe" || status=1
        done
        wineserver -k || true
        [ "$status" = 0 ]
        touch "$out"
      '';

      # Plays the part of a Windows player under Wine: the package's bin/
      # copied into a game folder that holds only DATA\HEROES2.AGG and
      # HEROES2X.AGG, started from that folder, from another one, and with a
      # quoted HOMM2_DATA from cmd. Each start has to reach the main menu.
      # Game data cannot be in a flake check, so this is an app:
      #   HOMM2_DATA=/path/to/heroes2 nix run .#windows-smoke
      homm2-windows-smoke = pkgs.writeShellApplication {
        name = "homm2-windows-smoke";
        runtimeInputs = [
          pkgs.coreutils
          pkgs.findutils
          pkgs.wineWow64Packages.stable
          pkgs.xdotool
          pkgs.xorg-server
        ];
        text = ''
          data="''${HOMM2_DATA:-}"
          if [ -z "$data" ]; then
            echo "homm2-windows-smoke: set HOMM2_DATA to the installed game directory" >&2
            exit 1
          fi
          unset HOMM2_DATA HOMM2_LOCALE_DATA

          work=$(mktemp -d)
          xvfb=
          cleanup() {
            wineserver -k 2>/dev/null || true
            if [ -n "$xvfb" ]; then kill "$xvfb" 2>/dev/null || true; fi
            rm -rf "$work"
          }
          trap cleanup EXIT

          game="$work/Heroes of Might and Magic II"
          mkdir -p "$game/DATA" "$work/elsewhere" "$work/program"
          for name in HEROES2.AGG HEROES2X.AGG; do
            found=$(find -L "$data" -maxdepth 2 -ipath "*/DATA/$name" -print -quit)
            if [ -z "$found" ]; then
              echo "homm2-windows-smoke: $data has no DATA/$name" >&2
              exit 1
            fi
            ln -s "$(realpath "$found")" "$game/DATA/$name"
          done
          cp -R ${windows}/bin/. "$game/"
          cp -R ${windows}/bin/. "$work/program/"
          chmod -R u+w "$game" "$work/program"

          export WINEPREFIX="$work/prefix" WINEDEBUG=-all
          export WINEDLLOVERRIDES="mscoree,mshtml=" LC_ALL=C.UTF-8
          unset WAYLAND_DISPLAY
          Xvfb -displayfd 4 -screen 0 1024x768x24 -nolisten tcp 4>"$work/display" 2>/dev/null &
          xvfb=$!
          for _ in $(seq 50); do [ -s "$work/display" ] && break; sleep 0.1; done
          DISPLAY=":$(cat "$work/display")"
          export DISPLAY
          wineboot -i >/dev/null 2>&1

          failures=0
          # start NAME DIRECTORY PROGRAM [VARIABLE=VALUE...]
          start() {
            local name=$1 directory=$2 program=$3
            shift 3
            local shot
            shot="$(winepath -w "$work/$name")"
            (cd "$directory" && env HOMM2_SCREENSHOT="$shot" HOMM2_SCREENSHOT_EVERY=25 "$@" \
              timeout 60 wine "$program" >"$work/$name.log" 2>&1) &
            local game=$!
            local windows=
            for _ in $(seq 20); do
              sleep 1
              windows=$(xdotool search --name . getwindowname %@ 2>/dev/null | sort -u || true)
              case $windows in *"Startup Error"*|*"Unexpected Program Termination"*) break ;; esac
            done
            wineserver -k 2>/dev/null || true
            wait "$game" || true
            local frames
            frames=$(find "$work" -maxdepth 1 -name "$name.*.ppm" | wc -l)
            if printf '%s\n' "$windows" | grep -q -e "Startup Error" -e "Unexpected Program Termination" \
              || [ "$frames" = 0 ]; then
              echo "FAIL $name"
              printf '  window: %s\n' "$windows"
              grep '^\[homm2\]' "$work/$name.log" | sed 's/^/  /' || true
              failures=$((failures + 1))
            else
              echo "ok   $name ($frames frames presented)"
            fi
          }

          start game-folder "$game" "$game/HMM2PL.exe"
          start other-folder "$work/elsewhere" "$game/HMM2PL.exe"
          start quoted-HOMM2_DATA "$work/elsewhere" "$work/program/HMM2PL.exe" \
            HOMM2_DATA="\"$(winepath -w "$game")\""
          [ "$failures" = 0 ]
        '';
      };

      mkNative = debug: p32.stdenv.mkDerivation {
        pname = "homm2${if debug then "-debug" else ""}";
        version = "2.1";
        src = source;
        nativeBuildInputs = [ pkgs.cmake pkgs.gettext pkgs.ninja pkgs.pkg-config pkgs.python3 ];
        buildInputs = [ p32.bzip2 p32.ffmpeg-headless p32.sdl3 ];
        cmakeFlags = [
          "-DHOMM2_PLATFORM=SDL3"
          "-DHOMM2_PLATFORM_WARNINGS_AS_ERRORS=ON"
          "-DCMAKE_BUILD_TYPE=${if debug then "Debug" else "RelWithDebInfo"}"
        ];
        dontStrip = true;
        separateDebugInfo = false;
        hardeningDisable = pkgs.lib.optionals debug [ "fortify" ];
        installPhase = ''
          runHook preInstall
          install -Dm755 homm2 "$out/bin/homm2"
          install -d "$out/share/homm2/lang"
          cp lang/*.mo "$out/share/homm2/lang/"
          runHook postInstall
        '';
        meta.mainProgram = "homm2";
      };

      homm2 = mkNative false;
      homm2-debug = mkNative true;
      homm2-check = homm2.overrideAttrs (_previous: {
        doCheck = true;
        checkPhase = ''
          runHook preCheck
          ctest --output-on-failure
          runHook postCheck
        '';
      });

      homm2-sanitized = homm2-check.overrideAttrs (previous: {
        pname = "homm2-sanitized";
        cmakeFlags = previous.cmakeFlags ++ [ "-DHOMM2_SANITIZERS=ON" ];
        # LeakSanitizer stops the world through ptrace, which hosts with Yama
        # ptrace_scope >= 2 forbid. The check is for memory and undefined
        # behaviour errors, so leak detection stays off for reproducibility.
        preCheck = (previous.preCheck or "") + ''
          export ASAN_OPTIONS=detect_leaks=0
          export UBSAN_OPTIONS=print_stacktrace=1
        '';
      });

      icon-check = pkgs.stdenv.mkDerivation {
        pname = "homm2-icon-check";
        version = "0.1.0";
        src = source;
        nativeBuildInputs = [ iconRust pkgs.clang ];
        buildPhase = ''
          runHook preBuild
          export CARGO_HOME="$TMPDIR/cargo-home"
          cd tools/homm2-icon-rs
          cargo test --all-targets --locked
          cargo test --doc --locked
          cargo clippy --all-targets --locked -- -D warnings
          cargo fmt --check
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          touch "$out"
          runHook postInstall
        '';
      };

      sdl3-web = pkgs.stdenvNoCC.mkDerivation {
        pname = "sdl3-web";
        inherit (pkgs.sdl3) version src;
        nativeBuildInputs = [ pkgs.cmake pkgs.emscripten pkgs.ninja ];
        configurePhase = ''
          runHook preConfigure
          ${emscriptenHome}emcmake cmake -S . -B build -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="$out" \
            -DSDL_SHARED=OFF \
            -DSDL_STATIC=ON \
            -DSDL_INSTALL=ON \
            -DSDL_TEST_LIBRARY=OFF \
            -DSDL_TESTS=OFF \
            -DSDL_EXAMPLES=OFF \
            -DSDL_EMSCRIPTEN_PERSISTENT_PATH=/storage
          runHook postConfigure
        '';
        buildPhase = ''
          runHook preBuild
          cmake --build build -j"$NIX_BUILD_CORES"
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          cmake --install build
          runHook postInstall
        '';
      };

      # Build the pinned nixpkgs bzip2 source for Emscripten. Using the
      # Emscripten port directly would download it from GitHub during the
      # build, which is not reproducible in the Nix sandbox.
      bzip2-web = pkgs.stdenvNoCC.mkDerivation {
        pname = "bzip2-web";
        inherit (pkgs.bzip2) version src;
        nativeBuildInputs = [ pkgs.emscripten pkgs.gnumake ];
        buildPhase = ''
          runHook preBuild
          ${emscriptenHome}emmake make -j"$NIX_BUILD_CORES" libbz2.a \
            CC=emcc AR=emar RANLIB=emranlib CFLAGS="-O2"
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          install -Dm644 bzlib.h "$out/include/bzlib.h"
          install -Dm644 libbz2.a "$out/lib/libbz2.a"
          runHook postInstall
        '';
      };

      ffmpeg-web = (mkFfmpeg {
        pname = "ffmpeg-web";
        stdenv = pkgs.stdenv;
        nativeBuildInputs = [ pkgs.emscripten pkgs.gnumake pkgs.perl pkgs.pkg-config ];
        targetOs = "none";
        arch = "wasm32";
        preConfigure = emscriptenHome;
        tools = [
          "--cc=emcc"
          "--cxx=em++"
          "--ar=emar"
          "--ranlib=emranlib"
          "--nm=${pkgs.emscripten.llvmEnv}/bin/llvm-nm"
          ''--host-cc="$CC"''
        ];
      }).overrideAttrs { dontStrip = true; };

      homm2-web = pkgs.stdenvNoCC.mkDerivation {
        pname = "homm2-web";
        version = "2.1";
        src = source;
        nativeBuildInputs = [ pkgs.cmake pkgs.emscripten pkgs.gettext pkgs.ninja pkgs.python3 ];
        configurePhase = ''
          runHook preConfigure
          ${emscriptenHome}emcmake cmake -S . -B build -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DSDL3_DIR=${sdl3-web}/lib/cmake/SDL3 \
            -DHOMM2_BZIP2_ROOT=${bzip2-web} \
            -DHOMM2_FFMPEG_ROOT=${ffmpeg-web} \
            -DHOMM2_PLATFORM=SDL3 \
            -DHOMM2_PLATFORM_WARNINGS_AS_ERRORS=ON
          runHook postConfigure
        '';
        buildPhase = ''
          runHook preBuild
          cmake --build build -j"$NIX_BUILD_CORES"
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          mkdir -p "$out/share/homm2-web"
          cp build/homm2.html build/homm2.js build/homm2.wasm "$out/share/homm2-web/"
          mkdir -p "$out/share/homm2-web/lang"
          cp build/lang/*.mo "$out/share/homm2-web/lang/"
          mkdir -p empty/game
          touch empty/game/.keep
          ${pkgs.emscripten}/share/emscripten/tools/file_packager.py \
            "$out/share/homm2-web/homm2.data" \
            --preload "$out/share/homm2-web/lang@/lang" \
            --preload "$PWD/empty/game@/game" \
            --js-output="$out/share/homm2-web/homm2.data.js"
          runHook postInstall
        '';
      };

      homm2-web-run = pkgs.writeShellApplication {
        name = "homm2-web";
        runtimeInputs = [ pkgs.coreutils pkgs.emscripten pkgs.python3 ];
        text = ''
          data="''${HOMM2_DATA:-}"
          python3 ${source}/tools/package_web_data.py --game-data "$data" --check

          destination="''${HOMM2_WEB_OUTPUT:-''${XDG_CACHE_HOME:-$HOME/.cache}/homm2-web}"
          mkdir -p "$destination"
          chmod -R u+w "$destination"
          cp -R ${homm2-web}/share/homm2-web/. "$destination/"
          chmod -R u+w "$destination"

          python3 ${source}/tools/package_web_data.py \
            --game-data "$data" \
            --packager ${pkgs.emscripten}/share/emscripten/tools/file_packager.py \
            --lang "${homm2-web}/share/homm2-web/lang" \
            --output "$destination"

          if [ "''${HOMM2_WEB_PACK_ONLY:-0}" = 1 ]; then
            echo "$destination/homm2.html"
            exit
          fi

          exec emrun \
            --no-emrun-detect \
            --port "''${HOMM2_WEB_PORT:-8080}" \
            "$destination/homm2.html"
        '';
      };
    in {
      packages.${system} = {
        inherit
          homm2
          homm2-debug
          homm2-sanitized
          homm2-web
          homm2-web-run
          homm2-windows-smoke;
        homm2-linux = homm2;
        homm2-windows = windows;
        default = homm2;
      };

      checks.${system} = {
        native = homm2-check;
        sanitized = homm2-sanitized;
        windows = windows;
        windows-tests = windows-tests;
        web = homm2-web;
        icon = icon-check;
      };

      apps.${system} = {
        default = {
          type = "app";
          program = "${homm2}/bin/homm2";
        };
        web = {
          type = "app";
          program = "${homm2-web-run}/bin/homm2-web";
        };
        windows-smoke = {
          type = "app";
          program = "${homm2-windows-smoke}/bin/homm2-windows-smoke";
        };
      };

      devShells.${system} = {
        default = p32.mkShell {
          nativeBuildInputs = [ pkgs.cmake pkgs.gettext pkgs.ninja pkgs.pkg-config pkgs.python3 ];
          buildInputs = [ p32.bzip2 p32.ffmpeg-headless p32.sdl3 ];
        };
        icon = pkgs.mkShell {
          packages = [ iconRust pkgs.clang ];
        };
      };
    };
}

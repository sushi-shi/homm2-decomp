{
  description = "Heroes of Might and Magic II: The Price of Loyalty";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      p32 = pkgs.pkgsi686Linux;
      mingw = pkgs.pkgsCross.mingw32;
      # Only what the build reads. Everything else - docs, the README, the
      # flake itself - would otherwise rebuild all three targets when touched.
      source = builtins.path {
        path = ./.;
        name = "homm2-port-source";
        filter = path: type:
          let relative = pkgs.lib.removePrefix (toString ./. + "/") (toString path); in
          relative == "CMakeLists.txt"
          || pkgs.lib.any (kept: pkgs.lib.hasPrefix kept relative) [ "src" "include" "web" ];
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

      windows = mingw.stdenv.mkDerivation {
        pname = "homm2-pol";
        version = "2.0";
        src = source;
        nativeBuildInputs = [
          pkgs.cmake
          pkgs.ninja
          pkgs.pkg-config
        ];
        buildInputs = [
          ffmpeg-windows
          mingw.sdl3
          mingw.windows.mcfgthreads
        ];
        cmakeFlags = [
          "-DHOMM2_32BIT=OFF"
          "-DHOMM2_PLATFORM=SDL3"
          "-DCMAKE_BUILD_TYPE=Release"
        ];
        installPhase = ''
          runHook preInstall
          mkdir -p "$out/bin"
          cp homm2.exe "$out/bin/HEROES2W.EXE"
          cp ${mingw.sdl3.out}/bin/SDL3.dll "$out/bin/"
          cp ${mingw.stdenv.cc.cc.lib}/i686-w64-mingw32/lib/libgcc_s_sjlj-1.dll "$out/bin/"
          cp ${mingw.stdenv.cc.cc.lib}/i686-w64-mingw32/lib/libstdc++-6.dll "$out/bin/"
          cp ${mingw.windows.mcfgthreads.out}/bin/libmcfgthread-2.dll "$out/bin/"
          ln -s bin/HEROES2W.EXE "$out/HEROES2W.EXE"
          cp ${./run-game.sh} "$out/run-game.sh"
          chmod +x "$out/run-game.sh"
          runHook postInstall
        '';
      };

      mkNative = debug: p32.stdenv.mkDerivation {
        pname = "homm2${if debug then "-debug" else ""}";
        version = "2.0";
        src = source;
        nativeBuildInputs = [ pkgs.cmake pkgs.ninja pkgs.pkg-config ];
        buildInputs = [ p32.ffmpeg-headless p32.sdl3 ];
        cmakeFlags = [
          "-DHOMM2_PLATFORM=SDL3"
          "-DCMAKE_BUILD_TYPE=${if debug then "Debug" else "RelWithDebInfo"}"
        ];
        dontStrip = true;
        separateDebugInfo = false;
        hardeningDisable = pkgs.lib.optionals debug [ "fortify" ];
        installPhase = ''
          runHook preInstall
          install -Dm755 homm2 "$out/bin/homm2"
          runHook postInstall
        '';
        meta.mainProgram = "homm2";
      };

      homm2 = mkNative false;
      homm2-debug = mkNative true;

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
        version = "2.0";
        src = source;
        nativeBuildInputs = [ pkgs.cmake pkgs.emscripten pkgs.ninja ];
        configurePhase = ''
          runHook preConfigure
          ${emscriptenHome}emcmake cmake -S . -B build -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DSDL3_DIR=${sdl3-web}/lib/cmake/SDL3 \
            -DHOMM2_FFMPEG_ROOT=${ffmpeg-web} \
            -DHOMM2_PLATFORM=SDL3
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
          mkdir -p empty/game
          touch empty/game/.keep
          ${pkgs.emscripten}/share/emscripten/tools/file_packager.py \
            "$out/share/homm2-web/homm2.data" \
            --preload "$PWD/empty/game@/game" \
            --js-output="$out/share/homm2-web/homm2.data.js"
          runHook postInstall
        '';
      };

      homm2-web-run = pkgs.writeShellApplication {
        name = "homm2-web";
        runtimeInputs = [ pkgs.coreutils pkgs.emscripten ];
        text = ''
          data="''${HOMM2_DATA:-}"
          if [ -z "$data" ] || [ ! -e "$data/DATA/HEROES2.AGG" ]; then
            echo "homm2-web: set HOMM2_DATA to the installed game directory" >&2
            exit 1
          fi

          destination="''${HOMM2_WEB_OUTPUT:-''${XDG_CACHE_HOME:-$HOME/.cache}/homm2-web}"
          mkdir -p "$destination"
          chmod -R u+w "$destination"
          cp -R ${homm2-web}/share/homm2-web/. "$destination/"
          chmod -R u+w "$destination"

          preload=()
          for name in DATA GAMES HELP HEROES2 MAPS MUSIC; do
            if [ -e "$data/$name" ]; then
              preload+=(--preload "$data/$name@/game/$name")
            fi
          done
          for name in H2CAMP.TXT POLCAMP.TXT HEROES2.CFG; do
            if [ -e "$data/$name" ]; then
              preload+=(--preload "$data/$name@/game/$name")
            fi
          done

          ${pkgs.emscripten}/share/emscripten/tools/file_packager.py \
            "$destination/homm2.data" \
            "''${preload[@]}" \
            --js-output="$destination/homm2.data.js"

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
          homm2-web
          homm2-web-run;
        homm2-linux = homm2;
        homm2-windows = windows;
        default = homm2;
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
      };

      devShells.${system}.default = p32.mkShell {
        nativeBuildInputs = [ pkgs.cmake pkgs.ninja pkgs.pkg-config ];
        buildInputs = [ p32.ffmpeg-headless p32.sdl3 ];
      };
    };
}

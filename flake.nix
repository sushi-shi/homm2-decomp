{
  description = "Heroes of Might and Magic II: The Price of Loyalty";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      p32 = pkgs.pkgsi686Linux;
      mingw = pkgs.pkgsCross.mingw32;

      windows = mingw.clangStdenv.mkDerivation {
        pname = "homm2-pol";
        version = "2.0";
        src = ./.;
        nativeBuildInputs = [
          pkgs.ninja
          pkgs.llvmPackages.llvm
          pkgs.llvmPackages.lld
        ];
        buildInputs = [
          mingw.gccStdenv.cc.cc
          mingw.windows.mcfgthreads
        ];
        NIX_LDFLAGS =
          "-L${mingw.gccStdenv.cc.cc}/i686-w64-mingw32/lib "
          + "-L${mingw.gccStdenv.cc.cc}/lib/gcc/i686-w64-mingw32/"
          + mingw.gccStdenv.cc.cc.version;
        dontConfigure = true;
        buildPhase = ''
          runHook preBuild
          sed -i "s|^cxx = clang++$|cxx = $CXX|" build.ninja
          sed -i "s|--target=i686-w64-windows-gnu ||g" build.ninja
          ninja
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          mkdir -p "$out"
          cp build/HEROES2W.EXE run-game.sh "$out/"
          chmod +x "$out/run-game.sh"
          runHook postInstall
        '';
      };

      mkNative = debug: p32.stdenv.mkDerivation {
        pname = "homm2-native${if debug then "-debug" else ""}";
        version = "2.0";
        src = builtins.path {
          path = ./.;
          name = "homm2-port-source";
          filter = path: type:
            let base = baseNameOf path; in
            !(base == "build" || base == ".git" || base == "result");
        };
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
          install -Dm755 homm2 "$out/bin/homm2-unwrapped"
          runHook postInstall
        '';
      };

      homm2-unwrapped = mkNative false;
      homm2-unwrapped-debug = mkNative true;

      wrap = native: pkgs.writeShellApplication {
        name = "homm2";
        runtimeInputs = [ native ];
        text = ''
          if [ -n "''${HOMM2_DATA:-}" ]; then
            data="$HOMM2_DATA"
          else
            data=""
            for candidate in \
              "$PWD" \
              "''${XDG_DATA_HOME:-$HOME/.local/share}/homm2" \
              "$HOME/.local/share/homm2/data" \
              "$HOME/games/homm2"; do
              if [ -e "$candidate/DATA/HEROES2.AGG" ] || [ -e "$candidate/data/heroes2.agg" ]; then
                data="$candidate"
                break
              fi
            done
          fi

          if [ -z "$data" ]; then
            echo "homm2: set HOMM2_DATA to the directory containing DATA/HEROES2.AGG" >&2
            exit 1
          fi

          export HOMM2_DATA="$data"
          exec homm2-unwrapped "$@"
        '';
      };

      homm2 = wrap homm2-unwrapped;
      homm2-debug = wrap homm2-unwrapped-debug;
    in {
      packages.${system} = {
        inherit
          homm2
          homm2-debug
          homm2-unwrapped
          homm2-unwrapped-debug;
        homm2-windows = windows;
        game = windows;
        default = homm2;
      };

      apps.${system}.default = {
        type = "app";
        program = "${homm2}/bin/homm2";
      };

      devShells.${system}.default = p32.mkShell {
        nativeBuildInputs = [ pkgs.cmake pkgs.ninja pkgs.pkg-config ];
        buildInputs = [ p32.ffmpeg-headless p32.sdl3 ];
      };
    };
}

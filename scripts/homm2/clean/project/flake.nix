{
  description = "Heroes of Might and Magic II - Gold 2.1 (Buka) reconstruction";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      mingw = pkgs.pkgsCross.mingw32;
      # The Ninja targets: the game, the scenario editor, or both.
      programs = {
        game = [ "HMM2PL.exe" ];
        editor = [ "EDT2PL.exe" ];
        all = [ "HMM2PL.exe" "EDT2PL.exe" ];
      };
      programFor = target: locale: mingw.clangStdenv.mkDerivation {
        pname = "homm2-gold-buka-${target}";
        version = "2.1";
        src = ./.;
        nativeBuildInputs = [
          pkgs.ninja
          pkgs.python3
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
          for graph in build.ninja build-en.ninja; do
            test -f "$graph" || continue
            sed -i "s|^cxx = clang++$|cxx = $CXX|" "$graph"
            sed -i "s|--target=i686-w64-windows-gnu ||g" "$graph"
          done
          if test -f build.py; then
            python3 build.py --${locale} --target ${target}
          else
            ninja -k 0 ${target}
          fi
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          mkdir -p "$out"
          for program in ${pkgs.lib.concatStringsSep " " programs.${target}}; do
            if test -f build.py; then
              cp "build/${locale}/$program" "$out/"
            else
              cp "build/$program" "$out/"
            fi
          done
          cp run-game.sh "$out/"
          chmod +x "$out/run-game.sh"
          runHook postInstall
        '';
      };
    in {
      packages.${system} = {
        game = programFor "game" "ru";
        game-en = programFor "game" "en";
        editor = programFor "editor" "ru";
        editor-en = programFor "editor" "en";
        all = programFor "all" "ru";
        all-en = programFor "all" "en";
        default = programFor "game" "ru";
      };
      devShells.${system}.default = programFor "all" "ru";
    };
}

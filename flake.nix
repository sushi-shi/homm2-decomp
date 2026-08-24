{
  description = "Heroes of Might and Magic II - Gold 2.1 (Buka) reconstruction";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      mingw = pkgs.pkgsCross.mingw32;
      game = mingw.gccStdenv.mkDerivation {
        pname = "homm2-gold-buka";
        version = "2.1";
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
          sed -i "s|^cxx = i686-w64-mingw32-g++$|cxx = $CXX|" build.ninja
          sed -i "s|--target=i686-w64-windows-gnu ||g" build.ninja
          sed -i "s|-fuse-ld=lld ||g" build.ninja
          ninja
          runHook postBuild
        '';
        installPhase = ''
          runHook preInstall
          mkdir -p "$out"
          cp build/HMM2PL.exe run-game.sh "$out/"
          chmod +x "$out/run-game.sh"
          runHook postInstall
        '';
      };
    in {
      packages.${system} = {
        inherit game;
        default = game;
      };
    };
}

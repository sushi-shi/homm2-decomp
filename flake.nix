{
  description = "Heroes II matching-decompilation environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";
    rust-overlay = {
      url = "github:oxalica/rust-overlay/6cddd512fa2bf7231f098d3a2f92f6e4cff71e0a";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    vostok-delinker-src = {
      url = "github:srp-survarium/vostok-delinker/1393e24b4804cb357fdac147c68013f0aa5a9d95";
      flake = false;
    };
    objdiff-src = {
      url = "github:encounter/objdiff/v3.7.3";
      flake = false;
    };
  };

  outputs = { nixpkgs, rust-overlay, vostok-delinker-src, objdiff-src, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        overlays = [ rust-overlay.overlays.default ];
      };

      rust = pkgs.rust-bin.nightly.latest.default.override {
        extensions = [ "rust-src" "rustfmt" "clippy" ];
      };
      nightly-rustPlatform = pkgs.makeRustPlatform {
        cargo = rust;
        rustc = rust;
      };

      vostok-delinker = nightly-rustPlatform.buildRustPackage {
        pname = "vostok-delinker";
        version = "0.1.0";
        src = vostok-delinker-src;
        cargoHash = "sha256-ZwFdbqUyh4b0S+fUYKGMN1fWaxRu1zU2ozKpe7CbcYs=";
        patches = [
          ./nix/patches/vostok-data-comdat-sections.patch
          ./nix/patches/vostok-common-symbols.patch
          ./nix/patches/vostok-canonical-data-sinks.patch
          ./nix/patches/vostok-data-manifest-folded-comdat.patch
          ./nix/patches/vostok-reviewed-static-reuse.patch
        ];
      };

      # The CLI is built from the pinned source so its machine-readable diff
      # schema exposes the allocation evidence the strict data audits read.
      objdiffVersion = "3.7.3";
      objdiffUrl = name:
        "https://github.com/encounter/objdiff/releases/download/v${objdiffVersion}/${name}";
      objdiffGuiLibs = with pkgs; [
        libGL
        libxkbcommon
        wayland
        fontconfig
        freetype
        libx11
        libxcursor
        libxi
        libxrandr
        libxcb
      ];

      objdiff-cli = nightly-rustPlatform.buildRustPackage {
        pname = "objdiff-cli";
        version = objdiffVersion;
        src = objdiff-src;
        patches = [
          ./nix/patches/objdiff-data-symbol-details.patch
          ./nix/patches/objdiff-complete-data-sections.patch
          ./nix/patches/objdiff-score-reloc-addend.patch
        ];
        cargoHash = "sha256-Z9vyUj35nrHuUoOYM54RLCn7CzcQ6k3A6FsDYKCVqVM=";
        cargoBuildFlags = [ "-p" "objdiff-cli" ];
        cargoTestFlags = [ "-p" "objdiff-core" "-p" "objdiff-cli" ];
        cargoInstallFlags = [ "-p" "objdiff-cli" ];
        nativeBuildInputs = [ pkgs.protobuf ];
        OBJDIFF_REGENERATE_PROTO = "1";
      };

      objdiff = pkgs.stdenv.mkDerivation {
        pname = "objdiff";
        version = objdiffVersion;
        src = pkgs.fetchurl {
          url = objdiffUrl "objdiff-linux-x86_64";
          hash = "sha256-1pzhzJUl/BJQP2XS333KIfkx1YYi8ZyRdPMv5MnJGyA=";
        };
        dontUnpack = true;
        nativeBuildInputs = [ pkgs.autoPatchelfHook pkgs.makeWrapper ];
        buildInputs = [ pkgs.stdenv.cc.cc.lib ] ++ objdiffGuiLibs;
        installPhase = ''
          install -Dm755 $src $out/bin/objdiff
          wrapProgram $out/bin/objdiff \
            --prefix LD_LIBRARY_PATH : "${pkgs.lib.makeLibraryPath objdiffGuiLibs}"
        '';
      };

      # PyGhidra boots Ghidra's JVM in-process for `homm2 sema` xref and the
      # whole-.text function-boundary map; libclang parses source DATA claims;
      # capstone backs the image census.
      python = pkgs.python3.withPackages (ps: [ ps.pyghidra ps.libclang ps.capstone ]);
      homm2-cli = pkgs.writeShellScriptBin "homm2" ''
        project_dir="''${HOMM2_DIR:-}"
        if [ -z "$project_dir" ]; then
          project_dir="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
        fi
        export PYTHONPATH="$project_dir/scripts''${PYTHONPATH:+:$PYTHONPATH}"
        exec ${python}/bin/python3 -m homm2 "$@"
      '';
      commonTools = with pkgs; [
        homm2-cli python git ninja binutils llvm llvmPackages.clang-unwrapped
        ripgrep file xxd jq p7zip vostok-delinker objdiff objdiff-cli
        rust ghidra jdk21
        gh # `homm2 init` fetches the pinned toolchain release
      ];

      # Nix does not tell shellHook which local flake was entered, so the
      # checkout is found through Git from anywhere inside a worktree, and a
      # caller outside it names it with HOMM2_DIR.
      projectRootHook = ''
        _homm2_root="$(git -C "$PWD" rev-parse --show-toplevel 2>/dev/null || true)"
        if [ ! -f "$_homm2_root/config/units.toml" ]; then
          _homm2_root="''${HOMM2_DIR:-}"
        fi
        if [ ! -f "$_homm2_root/config/units.toml" ]; then
          echo "[homm2] checkout   : NOT FOUND" >&2
          echo "[homm2] enter with : HOMM2_DIR=/path/to/homm2 nix develop /path/to/homm2#build" >&2
          return 1 2>/dev/null || exit 1
        fi
        export HOMM2_DIR="$(cd "$_homm2_root" && pwd -P)"
        unset _homm2_root
      '';

      # `objdiff` opens this checkout's generated project unless the caller
      # selects another project directory.
      objdiffShimHook = ''
        if command -v objdiff >/dev/null 2>&1 \
            && [ -f "$HOMM2_DIR/build/objdiff/objdiff.json" ]; then
          if [ -z "''${HOMM2_OBJDIFF_REAL:-}" ] || [ ! -x "$HOMM2_OBJDIFF_REAL" ]; then
            export HOMM2_OBJDIFF_REAL="$(command -v objdiff)"
          fi
          export HOMM2_OBJDIFF_PROJECT="$HOMM2_DIR/build/objdiff"
          _homm2_objdiff_bin="$HOMM2_DIR/build/objdiff-shim"
          if mkdir -p "$_homm2_objdiff_bin" \
              && printf '%s\n' '#!/bin/sh' \
                'for arg in "$@"; do' \
                '  case "$arg" in' \
                '    -p|--project-dir|--project-dir=*) exec "$HOMM2_OBJDIFF_REAL" "$@" ;;' \
                '  esac' \
                'done' \
                'exec "$HOMM2_OBJDIFF_REAL" --project-dir "$HOMM2_OBJDIFF_PROJECT" "$@"' \
                > "$_homm2_objdiff_bin/objdiff" \
              && chmod +x "$_homm2_objdiff_bin/objdiff"; then
            export PATH="$_homm2_objdiff_bin:$PATH"
            export HOMM2_OBJDIFF_WRAPPED="$HOMM2_DIR"
          else
            echo "[homm2] objdiff    : wrapper setup failed" >&2
          fi
          unset _homm2_objdiff_bin
        fi
      '';

      commonHook = projectRootHook + ''
        export PYTHONPATH="$HOMM2_DIR/scripts''${PYTHONPATH:+:$PYTHONPATH}"
        export HOMM2_EXE="$HOMM2_DIR/build/orig/HMM2PL.exe"
        [ -f "$HOMM2_EXE" ] || echo "[homm2] target EXE : MISSING - copy your HMM2PL.exe into build/orig/ (gitignored, never committed)" >&2
        export HOMM2_CLANG="${pkgs.llvmPackages.clang-unwrapped}/bin/clang"
        # `homm2 audit reloc-sweep` reads find_relocs.py from the delinker's
        # source tree; the package installs only the binary.
        export VOSTOK_DELINKER="''${VOSTOK_DELINKER:-${vostok-delinker-src}}"
        export HOMM2_TOOLCHAIN="''${HOMM2_TOOLCHAIN:-$HOMM2_DIR/build/toolchain}"
        export MSVC_DIR="$HOMM2_TOOLCHAIN/msvc"
        export GHIDRA_INSTALL_DIR="${pkgs.ghidra}/lib/ghidra"
        export JAVA_HOME="${pkgs.jdk21}/lib/openjdk"
        export PYTHONDONTWRITEBYTECODE=1
      '' + objdiffShimHook;

      # `nix run`: the rebuilt game under Wine, from this checkout.
      run-game = pkgs.writeShellApplication {
        name = "homm2-run";
        runtimeInputs = commonTools ++ [
          pkgs.wineWow64Packages.staging
          pkgs.libfaketime
          pkgs.glibcLocales
        ];
        text = ''
          root="$(git -C "$PWD" rev-parse --show-toplevel 2>/dev/null || true)"
          if [ ! -f "$root/config/units.toml" ]; then
            root="''${HOMM2_DIR:-}"
          fi
          if [ ! -f "$root/config/units.toml" ]; then
            echo "homm2-run: run this inside the decomp checkout or set HOMM2_DIR" >&2
            exit 1
          fi
          HOMM2_DIR="$(cd "$root" && pwd -P)"
          export HOMM2_DIR
          if [ -n "''${HOMM2_DATA:-}" ]; then
            HOMM2_DATA="$(realpath "$HOMM2_DATA")"
            export HOMM2_DATA
          fi
          export HOMM2_EXE="$HOMM2_DIR/build/orig/HMM2PL.exe"
          export HOMM2_CLANG="${pkgs.llvmPackages.clang-unwrapped}/bin/clang"
          export PYTHONPATH="$HOMM2_DIR/scripts''${PYTHONPATH:+:$PYTHONPATH}"
          export VOSTOK_DELINKER="''${VOSTOK_DELINKER:-${vostok-delinker-src}}"
          export HOMM2_TOOLCHAIN="''${HOMM2_TOOLCHAIN:-$HOMM2_DIR/build/toolchain}"
          export MSVC_DIR="$HOMM2_TOOLCHAIN/msvc"
          export WINEPREFIX="$HOMM2_DIR/build/wineprefix"
          export WINEDEBUG="''${WINEDEBUG:-fixme-all,err-kerberos}"
          export WINEDLLOVERRIDES="mscoree,mshtml="
          export LOCALE_ARCHIVE="${pkgs.glibcLocales}/lib/locale/locale-archive"
          cd "$HOMM2_DIR"
          exec python3 scripts/toolchain/run-rebuilt-game.py "$@"
        '';
      };
    in {
      packages.${system} = {
        inherit vostok-delinker objdiff objdiff-cli run-game;
        default = vostok-delinker;
      };
      apps.${system} = {
        default = {
          type = "app";
          program = "${run-game}/bin/homm2-run";
        };
      };
      devShells.${system} = {
        default = pkgs.mkShell {
          packages = commonTools;
          shellHook = commonHook;
        };
        build = pkgs.mkShell {
          # VC6 SP5 under Wine. `homm2 init` provisions build/toolchain from
          # the pinned release; $HOMM2_TOOLCHAIN points elsewhere.
          packages = commonTools ++ [ pkgs.wineWow64Packages.staging pkgs.libfaketime ];
          shellHook = commonHook + ''
            export WINEPREFIX="$HOMM2_DIR/build/wineprefix"
            export WINEDLLOVERRIDES="mscoree,mshtml="
            export WINEDEBUG="fixme-all,err-kerberos"
            case "$-" in *i*) trap 'wineserver -k >/dev/null 2>&1 || true' EXIT ;; esac
            if [ ! -f "$MSVC_DIR/bin/CL.EXE" ] && [ ! -f "$MSVC_DIR/bin/cl.exe" ]; then
              echo "[homm2] VC6 SP5    : NOT PROVISIONED - run \`homm2 init\` to fetch the pinned release" >&2
              echo "[homm2]              (or rebuild it from media: nix-shell scripts/toolchain/create-toolchain-release.nix)" >&2
            fi
          '';
        };
      };
    };
}

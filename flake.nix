{
  description = "HoMM2 'Price of Loyalty' HEROES2W.EXE (NWC, 1997, MSVC 4.2) - matching decompilation environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";
    rust-overlay = {
      url = "github:oxalica/rust-overlay/6cddd512fa2bf7231f098d3a2f92f6e4cff71e0a";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    # Vostok branch with reviewed data topology and real folded-symbol identities.
    vostok-delinker-src = {
      url = "github:srp-survarium/vostok-delinker/fe8e3c50be307e7412adacf755af1051e6c98d04";
      flake = false;
    };
    objdiff-src = {
      url = "github:encounter/objdiff/v3.7.1";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, rust-overlay, vostok-delinker-src, objdiff-src }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; overlays = [ rust-overlay.overlays.default ]; };

      rust = pkgs.rust-bin.nightly.latest.default.override { extensions = [ "rust-src" "rustfmt" "clippy" ]; };
      nightly-rustPlatform = pkgs.makeRustPlatform { cargo = rust; rustc = rust; };

      # vostok-delinker - slices the EXE into per-symbol COFF "target" objects (needs a PDB).
      vostok-delinker = nightly-rustPlatform.buildRustPackage {
        pname = "vostok-delinker"; version = "0.1.0";
        src = vostok-delinker-src;
        cargoHash = "sha256-ry3TH1fz7Aj/JdbmlgQFFn29m8E7EQHyGaVXnZTEcXo=";
      };

      # Build the CLI from the pinned upstream source so its machine-readable diff
      # schema can expose the allocation evidence used by strict data audits.
      objdiffVersion = "3.7.1";
      objdiffUrl = name: "https://github.com/encounter/objdiff/releases/download/v${objdiffVersion}/${name}";
      objdiffGuiLibs = with pkgs; [ libGL libxkbcommon wayland fontconfig freetype libx11 libxcursor libxi libxrandr libxcb ];
      objdiff-cli = nightly-rustPlatform.buildRustPackage {
        pname = "objdiff-cli"; version = objdiffVersion;
        src = objdiff-src;
        patches = [ ./patches/objdiff-data-symbol-details.patch ];
        cargoHash = "sha256-KlNA9JleBd5TwpeVZrAhAL2nKyp28hKiQ59qyZq7nKg=";
        cargoBuildFlags = [ "-p" "objdiff-cli" ];
        cargoTestFlags = [ "-p" "objdiff-core" "-p" "objdiff-cli" ];
        cargoInstallFlags = [ "-p" "objdiff-cli" ];
        nativeBuildInputs = [ pkgs.protobuf ];
        OBJDIFF_REGENERATE_PROTO = "1";
      };
      objdiff = pkgs.stdenv.mkDerivation {
        pname = "objdiff"; version = objdiffVersion;
        src = pkgs.fetchurl { url = objdiffUrl "objdiff-linux-x86_64"; hash = "sha256-LpBPYyWPzuX5jm02WUovzqJQyqz+l8SbRURHDWgFqq8="; };
        dontUnpack = true; nativeBuildInputs = [ pkgs.autoPatchelfHook pkgs.makeWrapper ]; buildInputs = [ pkgs.stdenv.cc.cc.lib ] ++ objdiffGuiLibs;
        installPhase = ''
          install -Dm755 $src $out/bin/objdiff
          wrapProgram $out/bin/objdiff --prefix LD_LIBRARY_PATH : "${pkgs.lib.makeLibraryPath objdiffGuiLibs}"
        '';
      };

      # `homm2` CLI on PATH (survives `nix develop --command fish`): python -m homm2.
      homm2-cli = pkgs.writeShellScriptBin "homm2" ''
        d="''${HOMM2_DIR:-$PWD}"
        export PYTHONPATH="$d/scripts''${PYTHONPATH:+:$PYTHONPATH}"
        exec python3 -m homm2 "$@"
      '';

      # nvim shim: auto-load editor/nvim (:Homm2) without touching the user's config.
      nvimShimHook = ''
        if [ -z "''${HOMM2_NVIM_WRAPPED:-}" ] && command -v nvim >/dev/null 2>&1 && [ -d "$HOMM2_DIR/editor/nvim" ]; then
          _real="$(command -v nvim)"; _bin="$HOMM2_DIR/build/nvim-shim"; mkdir -p "$_bin"
          printf '#!/bin/sh\nexec "%s" --cmd "set rtp^=%s/editor/nvim" "$@"\n' "$_real" "$HOMM2_DIR" > "$_bin/nvim"
          chmod +x "$_bin/nvim"; export PATH="$_bin:$PATH"; export HOMM2_NVIM_WRAPPED=1
          echo "[homm2] nvim       : WRAPPED -> auto-loads editor/nvim (:Homm2)." >&2
        fi
      '';

      # Analysis + diffing tools. Ghidra (headless, via PyGhidra) backs `homm2 sema`
      # xref: it supplies the WHOLE-.text function-boundary map (incl. the
      # library/runtime funcs CodeView omits). Our CodeView symbols
      # stay authoritative for names; Ghidra never discovers - it's fed the EXE (+ our
      # known names). Ghidra 12.0.4 + pyghidra + jdk21 pin-match gruntz (same nixpkgs
      # rev) so they're store cache hits, not a rebuild.
      commonTools = [ homm2-cli rust objdiff objdiff-cli vostok-delinker ] ++ (with pkgs; [
        (python3.withPackages (ps: [ ps.pyghidra ps.libclang ]))  # pyghidra (headless Ghidra) + libclang (clang.cindex, for scripts/permute_ast.py)
        ghidra jdk21                      # Ghidra 12.0.4 headless + JRE (homm2 sema xref)
        ninja
        llvm                              # llvm-pdbutil (synth_pdb yaml2pdb)
        llvmPackages.clang-unwrapped      # clangd + clang-format + clang driver (UNWRAPPED: no host gcc/glibc include shadowing)
        ripgrep file xxd jq binutils p7zip
      ]);

      # PyGhidra env, shared by both shells: pyghidra.start() reads GHIDRA_INSTALL_DIR
      # to boot the Ghidra JVM via jpype (JAVA_HOME picks the JRE).
      ghidraEnvHook = ''
        export GHIDRA_INSTALL_DIR="${pkgs.ghidra}/lib/ghidra"
        export JAVA_HOME="${pkgs.jdk21}/lib/openjdk"
      '';
    in {
      packages.${system} = { inherit vostok-delinker objdiff objdiff-cli; default = vostok-delinker; };

      devShells.${system} = {
        # Default - analysis, target-side delink, objdiff, clangd. No MSVC.
        default = pkgs.mkShell {
          name = "homm2-decomp";
          packages = commonTools;
          shellHook = ''
            export HOMM2_DIR="$PWD"
            export HOMM2_EXE="$HOMM2_DIR/build/orig/HEROES2W.EXE"
            [ -f "$HOMM2_EXE" ] || echo "[homm2] target EXE : MISSING - copy your HEROES2W.EXE into build/orig/ (gitignored, never committed)" >&2
            export HOMM2_CLANG="${pkgs.llvmPackages.clang-unwrapped}/bin/clang"
            export PYTHONPATH="$HOMM2_DIR/scripts''${PYTHONPATH:+:$PYTHONPATH}"
            if [ "$(git -C "$HOMM2_DIR" config --local core.hooksPath 2>/dev/null)" != ".githooks" ]; then
              git -C "$HOMM2_DIR" config --local core.hooksPath .githooks 2>/dev/null \
                && echo "[homm2] hooks      : pre-commit auto-format on (.githooks)" >&2
            fi
            ${ghidraEnvHook}
            echo "[homm2] target EXE : $HOMM2_EXE (minimal CodeView NB09 - public names/RVAs only)" >&2
            echo "[homm2] tools      : vostok-delinker, objdiff(-cli), llvm-pdbutil, clang(d), ghidra" >&2
            echo "[homm2] cli        : 'homm2 <cmd>' (status/clangd/sema/ghidra/format/...)" >&2
            echo "[homm2] build/MSVC : 'nix develop .#build' for 'homm2 build' (VC4.2 + wine)" >&2
            ${nvimShimHook}
          '';
        };

        # Build - MSVC 4.2 under wine. Toolchain defaults to build/toolchain (populate
        # from the en_vc42ent discs via scripts/make-toolchain.sh); override $HOMM2_TOOLCHAIN
        # to a hosted tarball once pinned.
        build = pkgs.mkShell {
          name = "homm2-build";
          packages = commonTools ++ [ pkgs.wineWow64Packages.staging ];
          shellHook = ''
            export HOMM2_DIR="$PWD"
            export HOMM2_EXE="$HOMM2_DIR/build/orig/HEROES2W.EXE"
            [ -f "$HOMM2_EXE" ] || echo "[homm2] target EXE : MISSING - copy your HEROES2W.EXE into build/orig/ (gitignored, never committed)" >&2
            export HOMM2_CLANG="${pkgs.llvmPackages.clang-unwrapped}/bin/clang"
            export PYTHONPATH="$HOMM2_DIR/scripts''${PYTHONPATH:+:$PYTHONPATH}"
            export HOMM2_TOOLCHAIN="''${HOMM2_TOOLCHAIN:-$HOMM2_DIR/build/toolchain}"
            export MSVC_DIR="$HOMM2_TOOLCHAIN/msvc"
            if [ -f "$HOMM2_TOOLCHAIN/link300/bin/LINK.EXE" ]; then
              export HOMM2_LINK_EXE="$HOMM2_TOOLCHAIN/link300/bin/LINK.EXE"
            fi
            export WINEPREFIX="$HOMM2_DIR/build/wineprefix"
            export WINEDEBUG="fixme-all,err-kerberos"
            export WINEDLLOVERRIDES="mscoree,mshtml="
            case "$-" in *i*) trap 'wineserver -k >/dev/null 2>&1 || true' EXIT ;; esac
            git -C "$HOMM2_DIR" config --local core.hooksPath .githooks 2>/dev/null || true
            echo "[homm2] MSVC 4.2   : $MSVC_DIR/bin/CL.EXE (under wine)" >&2
            echo "[homm2] final LINK : ''${HOMM2_LINK_EXE:-$MSVC_DIR/bin/LINK.EXE}" >&2
            if [ ! -f "$MSVC_DIR/bin/CL.EXE" ] && [ ! -f "$MSVC_DIR/bin/cl.exe" ]; then
              echo "[homm2] MSVC 4.2   : NOT PROVISIONED - run scripts/make-toolchain.sh on the en_vc42ent discs" >&2
            fi
            ${ghidraEnvHook}
            echo "[homm2] target EXE : $HOMM2_EXE" >&2
            echo "[homm2] cli        : 'homm2 <cmd>' (build/configure/status/sema/ghidra/...)" >&2
            ${nvimShimHook}
          '';
        };
      };
    };
}

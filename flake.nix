{
  description = "HoMM2 'Price of Loyalty' HEROES2W.EXE (NWC, 1997, MSVC 4.2) - matching decompilation environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/64c08a7ca051951c8eae34e3e3cb1e202fe36786";
    rust-overlay = {
      url = "github:oxalica/rust-overlay/6cddd512fa2bf7231f098d3a2f92f6e4cff71e0a";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    # vostok-delinker (the delinker) (fix/absolute-data-relocs: DIR32 for abs refs, REL32 for branches).
    vostok-delinker-src = {
      url = "github:srp-survarium/vostok-delinker/8a42a0ba6f6b90651d62d1911eb97b80a5faa149";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, rust-overlay, vostok-delinker-src }:
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

      # objdiff - upstream prebuilt Linux binaries (foreign ELF patched), x86 + COFF.
      objdiffVersion = "3.7.1";
      objdiffUrl = name: "https://github.com/encounter/objdiff/releases/download/v${objdiffVersion}/${name}";
      objdiffGuiLibs = with pkgs; [ libGL libxkbcommon wayland fontconfig freetype libx11 libxcursor libxi libxrandr libxcb ];
      objdiff-cli = pkgs.stdenv.mkDerivation {
        pname = "objdiff-cli"; version = objdiffVersion;
        src = pkgs.fetchurl { url = objdiffUrl "objdiff-cli-linux-x86_64"; hash = "sha256-QNhW2gHgpnbA8zr1NOVi8JjNUORey2Tzs0ZBjHsmSuY="; };
        dontUnpack = true; nativeBuildInputs = [ pkgs.autoPatchelfHook ]; buildInputs = [ pkgs.stdenv.cc.cc.lib ];
        installPhase = "install -Dm755 $src $out/bin/objdiff-cli";
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
          printf '#!/bin/sh\nexec "%s/nvim" -c Ranger "$@"\n' "$_bin" > "$_bin/ranger"
          chmod +x "$_bin/nvim" "$_bin/ranger"; export PATH="$_bin:$PATH"; export HOMM2_NVIM_WRAPPED=1
          echo "[homm2] nvim       : WRAPPED -> editor/nvim (:Homm2); 'ranger' starts nvim :Ranger." >&2
        fi
      '';

      # Analysis + diffing tools (no Ghidra, no JDK - we have CodeView symbols).
      commonTools = [ homm2-cli rust objdiff objdiff-cli vostok-delinker ] ++ (with pkgs; [
        python3
        ninja
        llvm                              # llvm-pdbutil (synth_pdb yaml2pdb)
        llvmPackages.clang-unwrapped      # clangd + clang-format + clang driver (UNWRAPPED: no host gcc/glibc include shadowing)
        ripgrep file xxd jq binutils
      ]);
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
            echo "[homm2] target EXE : $HOMM2_EXE (CodeView NB09 - symbols are authoritative)" >&2
            echo "[homm2] tools      : vostok-delinker, objdiff(-cli), llvm-pdbutil, clang(d)" >&2
            echo "[homm2] cli        : 'homm2 <cmd>' (status/clangd/format/...)" >&2
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
            export WINEPREFIX="$HOMM2_DIR/build/wineprefix"
            export WINEDEBUG="fixme-all,err-kerberos"
            export WINEDLLOVERRIDES="mscoree,mshtml="
            case "$-" in *i*) trap 'wineserver -k >/dev/null 2>&1 || true' EXIT ;; esac
            git -C "$HOMM2_DIR" config --local core.hooksPath .githooks 2>/dev/null || true
            echo "[homm2] MSVC 4.2   : $MSVC_DIR/bin/CL.EXE (under wine)" >&2
            if [ ! -f "$MSVC_DIR/bin/CL.EXE" ] && [ ! -f "$MSVC_DIR/bin/cl.exe" ]; then
              echo "[homm2] MSVC 4.2   : NOT PROVISIONED - run scripts/make-toolchain.sh on the en_vc42ent discs" >&2
            fi
            echo "[homm2] target EXE : $HOMM2_EXE" >&2
            echo "[homm2] cli        : 'homm2 <cmd>' (build/configure/status/...)" >&2
            ${nvimShimHook}
          '';
        };
      };
    };
}

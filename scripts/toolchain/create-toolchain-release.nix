# Reproduce the Buka VC6 SP5 toolchain tarball from preserved Microsoft media.
#
# Usage from the repository root:
#   nix-shell scripts/toolchain/create-toolchain-release.nix

{ pkgs ? import <nixpkgs> { config.allowUnfree = true; } }:

let
  # archive.org item 1998-10-01-visual-studio-6.0-enterprise-edition-disc-1,
  # original file VSE600EUN1.ISO:
  #   size 642,975,744 bytes
  #   sha1 bd742f7cceaf2e3032982ba2b4582358f79b3b04
  # Supplies the base VC98 tree: the rest of BIN, all of INCLUDE and LIB, and
  # the MSPDB60/MSOBJ10 pair that CL.EXE loads from COMMON\MSDEV98\BIN. The
  # image carries one ISO 9660 primary volume descriptor and no supplementary
  # one, so it can only spell 8.3 names; the assembly restores the six longer
  # C++ standard header names the way setup would.
  vc6-disc1 = pkgs.fetchurl {
    name = "VSE600EUN1.ISO";
    url = "https://archive.org/download/1998-10-01-visual-studio-6.0-enterprise-edition-disc-1/VSE600EUN1.ISO";
    hash = "sha1-vXQvfM6vLjAymCuitFgjWPebOwQ=";
  };
  # archive.org item vs6sp5, original file vs6sp5.exe:
  #   size 136,410,832 bytes
  #   sha1 6af12fb3e2c7c271be67c9c9833c63b82bb7b9bf
  # Supplies every updated component, including the back end - which it stores
  # as os\system\msvcep.dll and renames to c2.dll on install. SHA-1 is the
  # strongest digest archive.org publishes for an original file; the extracted
  # artifacts have pinned SHA-256s in create-toolchain-release.py and are a
  # mandatory second verification layer.
  vc6-sp5 = pkgs.fetchurl {
    name = "vs6sp5.exe";
    url = "https://archive.org/download/vs6sp5/vs6sp5.exe";
    hash = "sha1-avEvs+LHwnG+Z8nJgzxjuCu3ub8=";
  };
in
pkgs.mkShell {
  packages = [
    pkgs.python3
    pkgs.p7zip
    pkgs.cabextract          # multi-volume cabinets; 7z drops spanning entries
    pkgs.gnutar
    pkgs.xz
  ];

  shellHook = ''
    export VC6_DISC1="${vc6-disc1}"
    export VC6_SP5="${vc6-sp5}"
    export HOMM2_DIR="$PWD"
    export PYTHONPATH="${./.}''${PYTHONPATH:+:$PYTHONPATH}"
    exec python3 ${./create-toolchain-release.py}
  '';
}

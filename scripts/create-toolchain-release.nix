# Reproduce the HoMM2 VC 4.2 compiler + VC 4.0 LINK 3.00 toolchain tarball
# from preserved Microsoft media.
#
# Usage from the repository root:
#   nix-shell scripts/create-toolchain-release.nix

{ pkgs ? import <nixpkgs> { config.allowUnfree = true; } }:

let
  # archive.org item en_vc42ent, original file:
  #   size 202,798,728 bytes
  #   md5  e51b003f4e2bf08d4a1ed35d02d1b5b2
  #   sha1 108384972bf8d7b045044e3de380e2df8f5b2866
  # It contains the complete installed MSDEV/{BIN,INCLUDE,LIB} tree used by the
  # first project build. SHA-1 is the strongest digest archive.org publishes for
  # this original file; the extracted matching artifacts have pinned SHA-256s in
  # make_toolchain.py and are a mandatory second verification layer.
  vc42-disc1 = pkgs.fetchurl {
    name = "en_vc42ent_disc1.exe";
    url = "https://archive.org/download/en_vc42ent/en_vc42ent_disc1.exe";
    hash = "sha1-EIOElyv417BFBE4944Di349bKGY=";
  };
  # archive.org item msvc4x, original MSVC40.iso:
  #   size 555,663,360 bytes
  #   md5  772b1bbd7d7ff95399145f02d719587b
  #   sha1 81e139ac41d76740a6ba6d474355b37ed2e46c66
  vc40-iso = pkgs.fetchurl {
    name = "MSVC40.iso";
    url = "https://archive.org/download/msvc4x/MSVC40.iso";
    hash = "sha1-geE5rEHXZ0Cmum1HQ1WzftLkbGY=";
  };
in
pkgs.mkShell {
  packages = [
    pkgs.python3
    pkgs.unrar
    pkgs.p7zip
    pkgs.gnutar
    pkgs.xz
  ];

  shellHook = ''
    export VC42_DISC1="${vc42-disc1}"
    export VC40_ISO="${vc40-iso}"
    export HOMM2_DIR="$PWD"
    export PYTHONPATH="${./.}''${PYTHONPATH:+:$PYTHONPATH}"
    exec python3 ${./create-toolchain-release.py}
  '';
}

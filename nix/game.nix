# The installable game (README, "Install with a NixOS flake"): the `heroes2`
# launcher on the native program, with a desktop entry.
#
#   game       the player's copy: an installed game folder (GOG's, a Windows
#              or DOS installation), the Buka disc's files, or a .zip/.7z/.iso
#              of one (tools/game_data.py). Checked and laid out in the store at
#              install time, locally, never substituted from a cache; the icon
#              comes from its Windows program. Without it the launcher runs on
#              HOMM2_DATA, or on the folders the game searches by itself.
#   locale     the language the game starts in (en, or a locales/<LANG>.po);
#              null follows the desktop's locale, as the program does.
#   stateName  the per-user folder below $XDG_DATA_HOME/homm2 that holds the
#              saved games, high scores and preferences. "homm2" is the
#              program's own, so the launcher and the bare program share it.
#
# `edition` (from the flake) names the programs: another build, such as
# Project Ironfist, is another launcher and its own `stateName`.
{ pkgs, programs, importer, languages, edition }:
{ game ? null, locale ? null, stateName ? edition.stateName }:
assert pkgs.lib.assertMsg (locale == null || builtins.elem locale languages)
  "homm2: locale must be null or one of ${builtins.concatStringsSep ", " languages}";
let
  inherit (pkgs) lib;
  inherit (edition) name title;
  data = pkgs.runCommand "${name}-game-data" {
    preferLocalBuild = true;
    allowSubstitutes = false;
    nativeBuildInputs = [ importer pkgs.icoutils pkgs.imagemagick ];
  } ''
    homm2-import --game ${lib.escapeShellArg "${game}"} --out "$out" --program program
    ${edition.prepareData or ""}
    # The program's 32x32 icon, and larger copies scaled by whole pixels.
    for executable in program/*; do
      [ -f "$executable" ] || continue
      wrestool -x -t 14 -n HEROES -o icon.ico "$executable" 2>/dev/null || continue
      icotool -x -w 32 -h 32 -o icon.png icon.ico || continue
      for size in 32 64 128 256; do
        mkdir -p "$out/share/icons/hicolor/''${size}x$size/apps"
        magick icon.png -filter point -resize "''${size}x$size" \
          "$out/share/icons/hicolor/''${size}x$size/apps/${name}.png"
      done
      rm icon.ico icon.png
    done
  '';
  launcher = pkgs.writeShellApplication {
    inherit name;
    runtimeInputs = [ pkgs.coreutils ];
    text = ''
      program=${programs}/bin/homm2
      title=${lib.escapeShellArg title}
      store_data=${if game == null then "" else data}
      state_name=${lib.escapeShellArg stateName}
      language=${if locale == null then "" else lib.escapeShellArg locale}
      ${builtins.readFile ./launch.sh}
    '';
  };
  desktop = pkgs.makeDesktopItem ({
    inherit name;
    desktopName = title;
    comment = edition.comment;
    exec = name;
    categories = [ "Game" "StrategyGame" ];
  } // lib.optionalAttrs (game != null) { icon = name; });
in
pkgs.symlinkJoin {
  name = "${name}${lib.optionalString (locale != null) "-${locale}"}";
  paths = [ launcher desktop ];
  postBuild = lib.optionalString (game != null) ''
    if [ -d ${data}/share/icons ]; then
      mkdir -p "$out/share"
      cp -r ${data}/share/icons "$out/share/icons"
    fi
  '';
  passthru = { inherit data programs; };
  meta = {
    description = if game == null
      then "${title} (native port); set HOMM2_DATA to your installed game"
      else "${title} (native port) with your game data";
    mainProgram = name;
    platforms = [ "x86_64-linux" ];
  };
}

# The `heroes2` launcher (nix/game.nix). Embedded by writeShellApplication,
# which sets program (the native executable), title, store_data (the game data
# laid out in the store at install time, or empty), state_name (the per-user
# folder's name) and language (empty to follow the desktop's locale).
#
# The game only reads its data, so the launcher points it at the store's copy
# and keeps everything the game writes - saved games in GAMES, high scores,
# HEROES2.CFG and the display and language settings - in
# $XDG_DATA_HOME/homm2/$state_name, which the store never touches. HOMM2_DATA,
# HOMM2_USER_DATA and HOMM2_LANGUAGE set by the player win.

for argument in "$@"; do
  case "$argument" in
    --help|-h)
      printf '%s\n' \
        "$title (native port)." \
        "Game data: ${store_data:+$store_data/game, or }HOMM2_DATA=DIR (the installed game folder, which holds DATA)." \
        "Saved games, high scores and settings: \$XDG_DATA_HOME/homm2/$state_name (or HOMM2_USER_DATA)." \
        "Options: /I0 skips the intro; --language=en|ru; --resource-profile=western|buka-cyrillic."
      exit 0
      ;;
  esac
done

if [[ -z "${HOMM2_DATA:-}" && -n "$store_data" ]]; then
  export HOMM2_DATA="$store_data/game"
fi
if [[ -z "${HOMM2_USER_DATA:-}" ]]; then
  case "${XDG_DATA_HOME:-}" in
    /*) data_home="$XDG_DATA_HOME" ;;
    *) data_home="${HOME:?HOME or an absolute XDG_DATA_HOME is required}/.local/share" ;;
  esac
  export HOMM2_USER_DATA="$data_home/homm2/$state_name"
fi
if [[ -z "${HOMM2_LANGUAGE:-}" && -n "$language" ]]; then
  export HOMM2_LANGUAGE="$language"
fi
exec "$program" "$@"

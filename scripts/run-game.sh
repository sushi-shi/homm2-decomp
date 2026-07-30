#!/bin/sh
set -eu

game_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
export WINEPREFIX="${HOMM2_WINEPREFIX:-$game_dir/.wineprefix}"
export WINEDEBUG="${WINEDEBUG:--all}"

if ! command -v wine >/dev/null 2>&1; then
    echo "wine is required" >&2
    exit 1
fi

if [ ! -d "$WINEPREFIX/drive_c" ]; then
    mkdir -p "$WINEPREFIX"
    wineboot -u
    wineserver -w
fi

if [ -n "${NIRI_SOCKET:-}" ] && command -v niri >/dev/null 2>&1 \
    && command -v jq >/dev/null 2>&1; then
    before_ids=" $(niri msg -j windows 2>/dev/null | jq -r '.[].id' | tr '\n' ' ') "
else
    before_ids=
fi

cd "$game_dir"
wine HEROES2W.EXE "$@" &
game_pid=$!

if [ -n "$before_ids" ]; then
    attempts=0
    while [ "$attempts" -lt 200 ]; do
        for window_id in $(niri msg -j windows 2>/dev/null | jq -r '.[].id'); do
            case "$before_ids" in
                *" $window_id "*) ;;
                *)
                    niri msg action move-window-to-floating --id "$window_id" >/dev/null 2>&1
                    niri msg action set-window-width --id "$window_id" 640 >/dev/null 2>&1
                    niri msg action set-window-height --id "$window_id" 480 >/dev/null 2>&1
                    niri msg action center-window --id "$window_id" >/dev/null 2>&1
                    attempts=200
                    break
                    ;;
            esac
        done
        attempts=$((attempts + 1))
        sleep 0.05
    done
fi

wait "$game_pid"

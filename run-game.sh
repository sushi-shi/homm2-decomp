#!/bin/sh
set -eu

game_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
wine_prefix=${HOMM2_WINEPREFIX:-$game_dir/.wineprefix}
game_exe=${HOMM2_EXE:-HMM2PL.exe}
game_locale=${HOMM2_LANG:-ru_RU.UTF-8}
fullscreen=${HOMM2_FULLSCREEN:-0}

case ${1:-} in
    --fullscreen)
        fullscreen=1
        shift
        ;;
    --windowed)
        fullscreen=0
        shift
        ;;
    --setup-only)
        HOMM2_SETUP_ONLY=1
        shift
        ;;
    --help)
        echo "usage: $0 [--windowed|--fullscreen|--setup-only] [game arguments...]"
        exit 0
        ;;
esac

export WINEPREFIX=$wine_prefix
export WINEDEBUG=${WINEDEBUG:--all}
export LANG=$game_locale
export LIBGL_ALWAYS_SOFTWARE=${HOMM2_LIBGL_ALWAYS_SOFTWARE:-1}
if [ -n "${HOMM2_LOCALE_ARCHIVE:-}" ]; then
    export LOCALE_ARCHIVE=$HOMM2_LOCALE_ARCHIVE
fi

if ! command -v wine >/dev/null 2>&1; then
    echo "wine is required" >&2
    exit 1
fi
if ! command -v wineboot >/dev/null 2>&1; then
    echo "wineboot is required" >&2
    exit 1
fi
if ! command -v wineserver >/dev/null 2>&1; then
    echo "wineserver is required" >&2
    exit 1
fi
if [ ! -f "$game_dir/$game_exe" ]; then
    echo "game executable not found: $game_dir/$game_exe" >&2
    exit 1
fi

for middleware in audiere.dll MSS32.DLL SMACKW32.DLL WING32.DLL; do
    if [ ! -f "$game_dir/$middleware" ]; then
        echo "required middleware not found: $game_dir/$middleware" >&2
        exit 1
    fi
done

if [ ! -d "$WINEPREFIX/drive_c" ]; then
    mkdir -p "$WINEPREFIX"
    wineboot -u
    wineserver -w
fi

ensure_drive() {
    drive=$1
    target=$2
    link=$WINEPREFIX/dosdevices/$drive:

    if [ ! -e "$link" ] && [ ! -L "$link" ]; then
        ln -s "$target" "$link"
        return
    fi
    if [ "$(readlink -f "$link")" != "$(readlink -f "$target")" ]; then
        echo "Wine drive $drive: already points somewhere else: $link" >&2
        exit 1
    fi
}

if [ -n "${HOMM2_CD_DIR:-}" ]; then
    cd_dir=$HOMM2_CD_DIR
elif [ -d "$game_dir/.wine-cd" ]; then
    cd_dir=$game_dir/.wine-cd
else
    cd_dir=$game_dir
fi
if [ ! -d "$cd_dir" ]; then
    echo "CD data directory not found: $cd_dir" >&2
    exit 1
fi

ensure_drive g "$game_dir"
ensure_drive d "$cd_dir"

game_key='HKLM\Software\Wow6432Node\Buka\3DO\Heroes of Might and Magic Platinum\1.000'

reg_add_string() {
    wine reg add "$1" /v "$2" /t REG_SZ /d "$3" /f >/dev/null
}

reg_add_dword() {
    wine reg add "$1" /v "$2" /t REG_DWORD /d "$3" /f >/dev/null
}

reg_has_value() {
    wine reg query "$1" /v "$2" >/dev/null 2>&1
}

# GetDriveTypeA only reports DRIVE_CDROM when Wine's DOS-device mapping is
# paired with the machine-wide drive-type entry.  This is the same measured
# setup used by the Gruntz play prefix.
reg_add_string 'HKLM\Software\Wine\Drives' 'D:' cdrom
reg_add_string 'HKCU\Software\Wine\Direct3D' renderer "${HOMM2_WINE_RENDERER:-gl}"
reg_add_string "$game_key" PathPL2 'G:\'
reg_add_string "$game_key" 'HMM2POL CDDrive' 'D:'

if ! reg_has_value "$game_key" 'HMM2POL MusicVolume'; then
    reg_add_dword "$game_key" 'HMM2POL MusicVolume' 1
fi
if ! reg_has_value "$game_key" 'HMM2POL SoundQuality'; then
    reg_add_dword "$game_key" 'HMM2POL SoundQuality' 1
fi
if ! reg_has_value "$game_key" 'HMM2POL UseOpera'; then
    reg_add_dword "$game_key" 'HMM2POL UseOpera' 1
fi
if ! reg_has_value "$game_key" 'HMM2POL SlowVideo'; then
    reg_add_dword "$game_key" 'HMM2POL SlowVideo' 0
fi
if ! reg_has_value "$game_key" 'HMM2POL GameFullScreen'; then
    reg_add_dword "$game_key" 'HMM2POL GameFullScreen' 1
fi

# Miles 6 crashes in Wine while closing a Smacker audio stream. Keep effects
# muted by default; set HOMM2_FX_VOLUME explicitly to test another Miles setup.
reg_add_dword "$game_key" 'HMM2POL FXVolume' "${HOMM2_FX_VOLUME:-0}"

# The Buka executable contains Windows-1251 strings. Keep the dedicated prefix
# on the matching ANSI code page even when the host lacks a Russian locale.
reg_add_string 'HKLM\System\CurrentControlSet\Control\Nls\CodePage' ACP 1251
reg_add_string 'HKLM\System\CurrentControlSet\Control\Nls\CodePage' OEMCP 866
reg_add_string 'HKLM\System\CurrentControlSet\Control\Nls\CodePage' MACCP 10007

if [ "${HOMM2_SETUP_ONLY:-0}" = 1 ]; then
    echo "Wine prefix ready: $WINEPREFIX"
    exit 0
fi

cd "$game_dir"
if [ "$fullscreen" = 1 ]; then
    if ! command -v gamescope >/dev/null 2>&1; then
        echo "gamescope is required for fullscreen mode" >&2
        exit 1
    fi
    if [ -n "${WAYLAND_DISPLAY:-}" ]; then
        exec gamescope --backend wayland -f -w 640 -h 480 \
            -S stretch -F nearest --force-windows-fullscreen -- \
            wine explorer /desktop=HOMM2,640x480 "$game_exe" "$@"
    fi
    exec gamescope -f -w 640 -h 480 -S stretch -F nearest \
        --force-windows-fullscreen -- \
        wine explorer /desktop=HOMM2,640x480 "$game_exe" "$@"
fi

# Wine's virtual desktop is the reliable windowed path for this DirectDraw game.
# Niri tiles new windows by default, so remember the existing windows and float
# the newly-created Wine desktop at its native size when Niri is available.
if [ -n "${NIRI_SOCKET:-}" ] && command -v niri >/dev/null 2>&1 \
    && command -v jq >/dev/null 2>&1 \
    && niri_windows=$(niri msg -j windows 2>/dev/null) \
    && before_ids=$(printf '%s\n' "$niri_windows" | jq -r '.[].id' | tr '\n' ' '); then
    before_ids=" $before_ids "
else
    before_ids=
fi

wine explorer /desktop=HOMM2,640x480 "$game_exe" "$@" &
game_pid=$!

if [ -n "$before_ids" ]; then
    attempts=0
    while [ "$attempts" -lt 200 ]; do
        niri_windows=$(niri msg -j windows 2>/dev/null) || break
        window_ids=$(printf '%s\n' "$niri_windows" | jq -r '.[].id') || break
        for window_id in $window_ids; do
            case "$before_ids" in
                *" $window_id "*) ;;
                *)
                    if ! niri msg action move-window-to-floating \
                            --id "$window_id" >/dev/null 2>&1 \
                        || ! niri msg action set-window-width \
                            --id "$window_id" 640 >/dev/null 2>&1 \
                        || ! niri msg action set-window-height \
                            --id "$window_id" 480 >/dev/null 2>&1 \
                        || ! niri msg action center-window \
                            --id "$window_id" >/dev/null 2>&1; then
                        attempts=200
                        break
                    fi
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

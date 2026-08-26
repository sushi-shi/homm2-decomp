#!/usr/bin/env bash

set -euo pipefail

aggregate_sha256="2952e91a5d6c38216d8c805346f4941e7527ff31ba05c0d7a1161e90f56a5599"

usage() {
    echo "usage: $0 /path/to/heroes2" >&2
    echo "       HOMM2_DATA=/path/to/heroes2 $0" >&2
    echo "       HOMM2_IRONFIST_RESOURCE_PAYLOAD=/path/to/payload $0 /path/to/heroes2" >&2
}

find_game_directory() {
    local name="$1"
    find "$game_root" -mindepth 1 -maxdepth 1 \
        \( -type d -o -type l \) -iname "$name" -print -quit
}

ensure_game_directory() {
    local name="$1"
    local directory
    directory=$(find_game_directory "$name")
    if [[ -z "$directory" ]]; then
        directory="$game_root/$name"
        mkdir -p "$directory"
    fi
    printf '%s\n' "$directory"
}

has_file() {
    local directory="$1"
    local name="$2"
    [[ -n "$(find "$directory" -mindepth 1 -maxdepth 1 \
        \( -type f -o -type l \) -iname "$name" -print -quit)" ]]
}

game_root="${1:-${HOMM2_DATA:-}}"
if [[ -z "$game_root" ]]; then
    usage
    exit 2
fi
if [[ ! -d "$game_root" ]]; then
    echo "ironfist-resources: game directory does not exist: $game_root" >&2
    exit 1
fi
game_root=$(realpath "$game_root")

data_directory=$(find_game_directory DATA)
if [[ -z "$data_directory" ]]; then
    echo "ironfist-resources: $game_root is not a Heroes II installation (DATA/ is missing)" >&2
    exit 1
fi
if ! has_file "$data_directory" HEROES2.AGG || ! has_file "$data_directory" HEROES2X.AGG; then
    echo "ironfist-resources: Price of Loyalty requires DATA/HEROES2.AGG and DATA/HEROES2X.AGG" >&2
    exit 1
fi

payload_root="${HOMM2_IRONFIST_RESOURCE_PAYLOAD:-}"
if [[ -z "$payload_root" || ! -d "$payload_root" ]]; then
    echo "ironfist-resources: HOMM2_IRONFIST_RESOURCE_PAYLOAD is not a payload directory" >&2
    echo "ironfist-resources: use 'nix run .#ironfist-resources -- $game_root' or build a payload first" >&2
    exit 1
fi
payload_root=$(realpath "$payload_root")

required_files=(
    DATA/artifacts.xml
    DATA/creatures.xml
    DATA/overlays.xml
    DATA/ironfist.agg
    CAMPAIGNS/cyborg.cmp
    MAPS/CAMP5_01.HXC
    MUSIC/Track44.ogg
    SCRIPTS/MODULES/binding.lua
)

for filename in "${required_files[@]}"; do
    if [[ ! -f "$payload_root/$filename" ]]; then
        echo "ironfist-resources: payload is missing $filename" >&2
        exit 1
    fi
done

actual_aggregate_sha256=$(sha256sum "$payload_root/DATA/ironfist.agg" | cut -d ' ' -f 1)
if [[ "$actual_aggregate_sha256" != "$aggregate_sha256" ]]; then
    echo "ironfist-resources: aggregate hash is $actual_aggregate_sha256, expected $aggregate_sha256" >&2
    exit 1
fi

maps_directory=$(ensure_game_directory MAPS)
campaigns_directory=$(ensure_game_directory CAMPAIGNS)
music_directory=$(ensure_game_directory MUSIC)
scripts_directory=$(ensure_game_directory SCRIPTS)

cp -R "$payload_root/DATA/." "$data_directory/"
cp -R "$payload_root/MAPS/." "$maps_directory/"
cp -R "$payload_root/CAMPAIGNS/." "$campaigns_directory/"
cp -R "$payload_root/MUSIC/." "$music_directory/"
cp -R "$payload_root/SCRIPTS/." "$scripts_directory/"

for filename in "${required_files[@]}"; do
    directory=${filename%%/*}
    name=${filename#*/}
    installed_directory=$(find_game_directory "$directory")
    if [[ ! -f "$installed_directory/$name" ]]; then
        echo "ironfist-resources: failed to install $filename" >&2
        exit 1
    fi
done

echo "Installed Project Ironfist resources into $game_root"

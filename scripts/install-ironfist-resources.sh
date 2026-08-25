#!/usr/bin/env bash

set -euo pipefail

source_url_default="https://github.com/jkoppel/project-ironfist.git"
source_commit="314932011ed5308efb9f35cecc62e8ca638a7375"

usage() {
    echo "usage: $0 /path/to/heroes2" >&2
    echo "       HOMM2_DATA=/path/to/heroes2 $0" >&2
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

resource_tmp=$(mktemp -d "${TMPDIR:-/tmp}/homm2-ironfist-resources.XXXXXX")
cleanup() {
    rm -rf -- "$resource_tmp"
}
trap cleanup EXIT

source_url="${HOMM2_IRONFIST_SOURCE_URL:-$source_url_default}"
source_root="$resource_tmp/project-ironfist"
stage_root="$resource_tmp/stage"

echo "Fetching Project Ironfist $source_commit..."
git init -q "$source_root"
git -C "$source_root" sparse-checkout init --cone
git -C "$source_root" sparse-checkout set \
    assets/agg \
    assets/music \
    cmp \
    data \
    maps \
    tools/dist/agg \
    tools/dist/frm \
    tools/dist/icn
git -C "$source_root" fetch -q --depth=1 --filter=blob:none "$source_url" "$source_commit"
git -C "$source_root" checkout -q --detach FETCH_HEAD

actual_commit=$(git -C "$source_root" rev-parse HEAD)
if [[ "$actual_commit" != "$source_commit" ]]; then
    echo "ironfist-resources: fetched $actual_commit, expected $source_commit" >&2
    exit 1
fi

echo "Building ironfist.agg with the upstream packers..."
mkdir -p "$source_root/build"
export WINEPREFIX="$resource_tmp/wineprefix"
export WINEDEBUG=-all
(
    cd "$source_root/assets"
    wine cmd /d /c pack.bat
)
wineserver -w

aggregate="$source_root/build/ironfist.agg"
if [[ ! -s "$aggregate" ]]; then
    echo "ironfist-resources: upstream pack.bat did not produce build/ironfist.agg" >&2
    exit 1
fi

mkdir -p \
    "$stage_root/DATA" \
    "$stage_root/MAPS" \
    "$stage_root/CAMPAIGNS" \
    "$stage_root/MUSIC" \
    "$stage_root/SCRIPTS/MODULES" \
    "$stage_root/SCRIPTS/GENERIC"

cp "$aggregate" "$stage_root/DATA/ironfist.agg"
cp "$source_root"/data/*.xml "$stage_root/DATA/"
cp "$source_root"/cmp/*.cmp "$stage_root/CAMPAIGNS/"
cp "$source_root/assets/music/homm2_43.ogg" "$stage_root/MUSIC/Track44.ogg"

while IFS= read -r -d '' filename; do
    cp "$filename" "$stage_root/MAPS/"
done < <(find "$source_root/maps" -mindepth 1 -maxdepth 1 -type f ! -iname '*.lua' -print0)

while IFS= read -r -d '' filename; do
    cp "$filename" "$stage_root/SCRIPTS/"
done < <(find "$source_root/maps" -mindepth 1 -maxdepth 1 -type f -iname '*.lua' -print0)

cp -R "$source_root/maps/MODULES/." "$stage_root/SCRIPTS/MODULES/"
cp -R "$source_root/maps/GENERIC/." "$stage_root/SCRIPTS/GENERIC/"

maps_directory=$(ensure_game_directory MAPS)
campaigns_directory=$(ensure_game_directory CAMPAIGNS)
music_directory=$(ensure_game_directory MUSIC)
scripts_directory=$(ensure_game_directory SCRIPTS)

cp -R "$stage_root/DATA/." "$data_directory/"
cp -R "$stage_root/MAPS/." "$maps_directory/"
cp -R "$stage_root/CAMPAIGNS/." "$campaigns_directory/"
cp -R "$stage_root/MUSIC/." "$music_directory/"
cp -R "$stage_root/SCRIPTS/." "$scripts_directory/"

required_files=(
    "$data_directory/artifacts.xml"
    "$data_directory/creatures.xml"
    "$data_directory/overlays.xml"
    "$data_directory/ironfist.agg"
    "$campaigns_directory/cyborg.cmp"
    "$maps_directory/CAMP5_01.HXC"
    "$music_directory/Track44.ogg"
    "$scripts_directory/MODULES/binding.lua"
)

for filename in "${required_files[@]}"; do
    if [[ ! -f "$filename" ]]; then
        echo "ironfist-resources: source build did not install $filename" >&2
        exit 1
    fi
done

echo "Installed Project Ironfist $source_commit into $game_root"

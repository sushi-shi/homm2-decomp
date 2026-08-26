#!/usr/bin/env bash

set -euo pipefail

aggregate_sha256="2952e91a5d6c38216d8c805346f4941e7527ff31ba05c0d7a1161e90f56a5599"

usage() {
    echo "usage: $0 /path/to/project-ironfist /path/to/empty-output" >&2
}

source_root="${1:-}"
output_root="${2:-}"
if [[ -z "$source_root" || -z "$output_root" ]]; then
    usage
    exit 2
fi
if [[ ! -d "$source_root/assets/agg" \
    || ! -d "$source_root/assets/music" \
    || ! -d "$source_root/cmp" \
    || ! -d "$source_root/data" \
    || ! -d "$source_root/maps" ]]; then
    echo "ironfist-payload: incomplete Project Ironfist source tree: $source_root" >&2
    exit 1
fi
if [[ -e "$output_root" \
    && -n "$(find "$output_root" -mindepth 1 -maxdepth 1 -print -quit)" ]]; then
    echo "ironfist-payload: output directory is not empty: $output_root" >&2
    exit 1
fi

source_root=$(realpath "$source_root")
script_directory=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
resource_builder="${HOMM2_IRONFIST_RESOURCE_BUILDER:-$script_directory/build-ironfist-resources.py}"

mkdir -p \
    "$output_root/DATA" \
    "$output_root/MAPS" \
    "$output_root/CAMPAIGNS" \
    "$output_root/MUSIC" \
    "$output_root/SCRIPTS/MODULES" \
    "$output_root/SCRIPTS/GENERIC"

aggregate="$output_root/DATA/ironfist.agg"
python3 "$resource_builder" "$source_root/assets/agg" "$aggregate"

actual_aggregate_sha256=$(sha256sum "$aggregate" | cut -d ' ' -f 1)
if [[ "$actual_aggregate_sha256" != "$aggregate_sha256" ]]; then
    echo "ironfist-payload: aggregate hash is $actual_aggregate_sha256, expected $aggregate_sha256" >&2
    exit 1
fi

cp "$source_root"/data/*.xml "$output_root/DATA/"
cp "$source_root"/cmp/*.cmp "$output_root/CAMPAIGNS/"
cp "$source_root/assets/music/homm2_43.ogg" "$output_root/MUSIC/Track44.ogg"

while IFS= read -r -d '' filename; do
    cp "$filename" "$output_root/MAPS/"
done < <(find "$source_root/maps" -mindepth 1 -maxdepth 1 -type f ! -iname '*.lua' -print0)

while IFS= read -r -d '' filename; do
    cp "$filename" "$output_root/SCRIPTS/"
done < <(find "$source_root/maps" -mindepth 1 -maxdepth 1 -type f -iname '*.lua' -print0)

cp -R "$source_root/maps/MODULES/." "$output_root/SCRIPTS/MODULES/"
cp -R "$source_root/maps/GENERIC/." "$output_root/SCRIPTS/GENERIC/"

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
    if [[ ! -f "$output_root/$filename" ]]; then
        echo "ironfist-payload: source build did not produce $filename" >&2
        exit 1
    fi
done

echo "Built Project Ironfist resource payload in $output_root"

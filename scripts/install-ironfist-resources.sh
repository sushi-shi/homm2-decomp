#!/usr/bin/env bash

set -euo pipefail

resource_version="ironfist-pol-2.0-resources-v1"
resource_archive="${resource_version}.zip"
resource_sha256="37194d329c43a32615e2f1926a4ab33d7fdeda4be3add99acb6b59c21175876a"
resource_url_default="https://github.com/sushi-shi/homm2-decomp/releases/download/${resource_version}/${resource_archive}"

usage() {
    echo "usage: $0 /path/to/heroes2" >&2
    echo "       HOMM2_DATA=/path/to/heroes2 $0" >&2
}

game_root="${1:-${HOMM2_DATA:-}}"
if [[ -z "$game_root" ]]; then
    usage
    exit 2
fi

if [[ ! -d "$game_root/DATA" ]]; then
    echo "ironfist-resources: $game_root is not a Heroes II installation (DATA/ is missing)" >&2
    exit 1
fi

has_retail_aggregate() {
    local filename="$1"
    [[ -n "$(find "$game_root/DATA" -maxdepth 1 -type f -iname "$filename" -print -quit)" ]]
}

if ! has_retail_aggregate HEROES2.AGG || ! has_retail_aggregate HEROES2X.AGG; then
    echo "ironfist-resources: Price of Loyalty requires DATA/HEROES2.AGG and DATA/HEROES2X.AGG" >&2
    exit 1
fi

resource_tmp=$(mktemp -d "${TMPDIR:-/tmp}/homm2-ironfist-resources.XXXXXX")
trap 'rm -rf -- "$resource_tmp"' EXIT

resource_url="${HOMM2_IRONFIST_RESOURCES_URL:-$resource_url_default}"
echo "Downloading $resource_version..."
if ! curl --fail --location --retry 3 --output "$resource_tmp/$resource_archive" "$resource_url"; then
    rm -f -- "$resource_tmp/$resource_archive"
    if command -v gh >/dev/null && gh auth status --hostname github.com >/dev/null 2>&1; then
        echo "Anonymous download unavailable; retrying with the authenticated GitHub CLI..."
        gh release download "$resource_version" \
            --repo sushi-shi/homm2-decomp \
            --pattern "$resource_archive" \
            --dir "$resource_tmp"
    else
        echo "ironfist-resources: download failed; authenticate with 'gh auth login' and retry" >&2
        exit 1
    fi
fi

(
    cd "$resource_tmp"
    printf '%s  %s\n' "$resource_sha256" "$resource_archive" | sha256sum --check --status
)

unzip -q -o "$resource_tmp/$resource_archive" -d "$game_root"

required_files=(
    DATA/artifacts.xml
    DATA/creatures.xml
    DATA/ironfist.agg
    CAMPAIGNS/cyborg.cmp
    MAPS/CAMP5_01.HXC
    MUSIC/Track44.ogg
    SCRIPTS/MODULES/binding.lua
)

for filename in "${required_files[@]}"; do
    if [[ ! -f "$game_root/$filename" ]]; then
        echo "ironfist-resources: archive did not install $filename" >&2
        exit 1
    fi
done

echo "Installed $resource_version into $game_root"

#!/usr/bin/env bash
set -euo pipefail

# Run from the repository root inside `nix develop`, after a native build.
probe_dir=tools/review/ironfist
touch /tmp/ironfist-empty.lua
mkdir -p /tmp/ironfist-review-data/SCRIPTS/MODULES
cp /tmp/ironfist-empty.lua /tmp/ironfist-review-data/SCRIPTS/MODULES/binding.lua

common=(-std=c++20 -Iinclude -I. -Ivendor/lua-5.3.3 -Ivendor/tinyxml2-7.1.0
  -D__cdecl= -D__stdcall= -D__fastcall= -ffunction-sections -fdata-sections)
g++ "${common[@]}" -Wno-invalid-offsetof -fsanitize=address -g \
  "$probe_dir/script_probe.cpp" src/IRONFIST/scripting.cpp \
  src/IRONFIST/lua_utils.cpp src/IRONFIST/callback.cpp \
  src/IRONFIST/deepbinding.cpp src/IRONFIST/hooks.cpp \
  build/libhomm2_lua.a -Wl,--gc-sections -o /tmp/ironfist-script-probe
ASAN_OPTIONS=detect_leaks=0 /tmp/ironfist-script-probe
if ASAN_OPTIONS=detect_leaks=0 /tmp/ironfist-script-probe overflow \
    > /tmp/ironfist-review-overflow.log 2>&1; then
  echo 'Overflow probe did not reproduce; inspect whether the defect was fixed.'
else
  head -16 /tmp/ironfist-review-overflow.log
fi

g++ "${common[@]}" -Wno-invalid-offsetof -fno-access-control \
  "$probe_dir/xml_probe.cpp" src/IRONFIST/save_xml.cpp \
  src/IRONFIST/xml_utils.cpp src/IRONFIST/scripting.cpp \
  build/libhomm2_tinyxml2.a -Wl,--gc-sections -o /tmp/ironfist-xml-probe
timeout 5 /tmp/ironfist-xml-probe

python3 "$probe_dir/link_engine_probe.py"
HOMM2_DATA=/tmp/ironfist-review-data XDG_DATA_HOME=/tmp/ironfist-review-user \
  SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  timeout 10 /tmp/ironfist-engine-probe

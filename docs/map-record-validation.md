# Map extra record validation

Map and save loading retain each extra record's declared byte length. The
production validators in `src/SOURCE/MapRecords.cpp` inspect bounded spans before
typed access. Hero/town records require their fixed prefix; signs, rumours,
sphinx riddles and events require a terminator inside their variable tail.
Sphinx answers and custom hero/town names use their individual field capacities.
Creature, hero, artifact, skill, faction and mage-guild values used as array
indices are checked before setup consumes them.

Event lists must fit their runtime arrays and reference real records. Map-cell
extra chains must terminate inside the allocated table without cycles or free
markers. Unreachable editor slots, shared chain suffixes, extra record padding,
decorative object tiles and custom portrait indices remain supported. Saved
hero/town extras have already been consumed, so their repurposed cell metadata
is not interpreted as a new-game record reference. Runtime event access also
validates the selected record, covering objects beneath a hero in a loaded save.

Malformed data is reported through the game's existing fatal file-error path.
This does not make loading transactional or validate every gameplay number in
a save; resource arithmetic and the broader runtime save schema are separate
concerns.

The asset-free `map_records` CTest exercises every truncated record prefix,
unterminated tails and fixed fields, invalid index domains, table/chain bounds,
cycles, event capacity, empty signs and consumed save extras. Build and run it:

```sh
nix develop -c cmake -S . -B build
nix develop -c cmake --build build --target homm2_map_records_test
ctest --test-dir build -R '^map_records$' --output-on-failure
```

The same test executable optionally reads retail map files and passes their
records and cell chains to the production validators. Assets are neither needed
by CTest nor included in the repository:

```sh
python3 - "$HOMM2_DATA/MAPS" ./build/homm2_map_records_test <<'PY'
from pathlib import Path
import subprocess
import sys

maps = [p for p in Path(sys.argv[1]).iterdir()
        if p.suffix.lower() in (".mp2", ".mx2", ".h2c", ".hxc")]
if not maps:
    raise SystemExit("No retail map files found")
raise SystemExit(subprocess.run([sys.argv[2], *map(str, maps)]).returncode)
PY
```

Development validation accepted all 281 installed map files across the two local
distributions, including duplicate scenarios. Both the rejection cases and the
retail survey also passed with ASan/UBSan and recovery disabled.

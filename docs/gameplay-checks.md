# Retail gameplay round trips

`tools/gameplay_roundtrip.py` runs the ordinary Linux executable twice with
dummy SDL video/audio, isolated user state, and the checked-in
`tools/replays/save-roundtrip.replay`. It loads a normal scenario save, opens
and closes the selected hero/town view, saves over the original slot, returns
to the menu and quits. It requires installed retail data and a playable `.GM1`
seed; those private files are deliberately outside CTest and the source tree.

Create a seed by starting Broken Alliance in the original-game scenario list
and saving as `NEWGAME`. Copy that save to a private fixture directory. Use a
save at the adventure screen with a selected hero or town and no pending
event/dialog. Keep the original seed and its hash to compare future builds.

```sh
python3 tools/gameplay_roundtrip.py \
  --exe build/homm2 --data /path/to/heroes2 \
  --seed /path/to/private/NEWGAME.GM1 --output /tmp/homm2-roundtrip
```

The output directory must be new. Only resource paths are linked from the
installation; the runner copies the seed into an isolated save directory.
Logs, framebuffer captures, copies of each resulting save and `report.json`
remain in the output directory for inspection. **These artifacts contain
retail data; keep them private.** Older builds that still require the `Tracks2`
audio marker must have it in the installed data directory too.

Each process must exit successfully, report loading the save, produce at least
two distinct complete 640×480 frames, and actually replace the save's inode and
advance its modification time. The first save normalizes legacy text padding
and the embedded filename; every subsequent save must be byte-identical.
`--expect-seed-unchanged` additionally compares every output to an already
normalized `NEWGAME.GM1` seed. Sanitizer builds run with fatal ASan/UBSan checks
and leak detection enabled.

Repeat `--exe` to exchange a save between builds, for example native 64-bit and
32-bit sanitizer executables. A single executable runs twice; multiple ones
run once each in the specified order. The default timeout is 65 seconds per
process. The replay's fixed timestamps assume the menus and game load within
the allotted time; a slow machine fails rather than claiming success from a
replay that missed its controls.

The frame hashes are recorded evidence, not fixed visual golden images:
animation and scheduling can change the capture times. Inspect the captures
when changing rendering or the replay. This check exercises scenario loading,
the adventure UI and save replacement. Combat, campaign progression, network
play and browser storage need their own coverage.

This regression check expects transactional save replacement and the stable
legacy save serialization from the modernization changes. The shared resource
path resolver comes from the Web packaging tools. Use the combined feature
stack when validating those changes together; a build that still truncates the
destination in place intentionally fails the replacement check.

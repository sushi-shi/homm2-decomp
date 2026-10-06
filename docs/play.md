# Playing the rebuilt game

From nothing to a playable rebuilt executable:

```sh
# 0. prerequisites: nix (with flakes), a legally obtained Buka HMM2PL.exe,
#    and a legally obtained game install (DATA/, MAPS/, the middleware DLLs)
git clone <this repo> && cd homm2-decomp
cp /path/to/HMM2PL.exe build/orig/          # the retail control image
nix develop .#build
homm2 init                                  # fetch pinned toolchain, delink
homm2 build                                 # compile + verify 1727/1727 exact
homm2 link --rsrc                           # playable build; extracts the retail icon

# one-time play environment (game data + dedicated wineprefix + D: cd drive)
python3 scripts/toolchain/create-wine-prefix.py /path/to/installed/game
build/game-wine/play.sh                     # runs the rebuilt HMM2PL.exe
```

The same verified build, resource link, dedicated Wine-prefix setup, and launch
are also available as one command:

```sh
HOMM2_DATA=/path/to/installed/game nix run
```

From the build shell, `homm2 play --game /path/to/installed/game` runs the
same workflow (`--prepare-only` stops after setup).

On its first run, the app copies the legally obtained `HMM2PL.exe` from that
installation into ignored build state, initializes the matching environment,
and provisions the dedicated game prefix. Every run performs incremental
`homm2 build` and `homm2 link --rsrc` steps before refreshing the staged
executable. The prefix uses the same registry, code-page, virtual-desktop and
CD-drive setup as the cleaned Buka runner. Pass `--prepare-only` to stop after
setup without starting the game.

The staged `game/HMM2PL.exe` is always the ordinary `homm2 link --rsrc`
output, and Wine launches that file directly. The play workflow does not patch
or transform the candidate.

The play environment lives in `build/game-wine/` (gitignored): `game/` holds
your install's data plus the rebuilt `HMM2PL.exe`, `cd/` holds the staged
`Anim2` movies and `Tracks2` music, and `prefix/` is a wineprefix separate from
the build one, with a virtual desktop so the game's mode switches never touch
the host. Re-running the provisioner refreshes the rebuilt executable and
never touches saves or configuration.

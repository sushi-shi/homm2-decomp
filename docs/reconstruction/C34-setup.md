# C34: natural initialization assignments

Removed seven redundant written casts in game::SetupOrigData. The declared
destination type already performs the required assignment conversion:
player color, hero ID, hero portrait, town ID, boat ID, current-player mask
and watched-player mask. The two actual FactionType conversions remain.

The fixed loops give player indices 0..5, hero indices 0..53, town indices
0..71 and boat indices 0..47. These fit the original signed/unsigned byte
destinations. The current player is initialized to zero. The watched-player
scan still requires a local human among the four initial active players;
for a terminating valid scan its shift count and one-bit mask fit u8.
No validation, loop behavior or destination type changes.

The complete seven-axis Cartesian product contains 128 variants. All compiled
in 138.562 seconds, restored the parent source and completed without truncation.
Every arm has the same 2513-byte text hash `17aba7a1cfda9b7e`, normalized
relocation hash `89f70eba2c384768` and diagnostic 59/59 exact block graph.
Trial 127 removes all seven casts and is the retained source.

The exact source family and measured disposition are archived in
`docs/matching/game-SetupOrigData/c34-natural-assignments.cpp`. Full results
are under `build/source-variant-batch/c34-setup/`. No unchanged-source state
census is claimed: this tests cast removal from an already compatible parent,
not a search for a residual island. No generated declarations are retained.

The batch's strict identity test returns exact=false even for its baseline.
It is not used as a retail closure claim. The retained arm independently
passes all 2513 retail bytes, exact size, complete DIR32 sites and every
ordered target (`build/u12-0x4df08-final-retail.log`). The combined U12/U13
and cast change preserves all 98 native objects and 1826 emitted functions,
including allocated bytes, attributes and ordered relocation graphs
(`build/u12-c34-final-native.log`). The final build passes 1727/1727 function
checks and 291987/291987 data bytes (`build/build-u12-c34-final.log`).
The actual VC6 layout probe and all eight consumer TUs in both dialects also
pass after retaining these assignments (`build/u12-c34-final-layout.log`).

This completes C34's seven SetupOrigData candidates, not the other functions'
C34 candidates. Existing loaded-data, name-capacity and player-lifecycle
findings remain explicit in the per-function report.

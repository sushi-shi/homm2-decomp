# C34/G47: map-state conversions and artifact-hint exit

Removed 16 written casts across four functions. Three color-lookup argument
casts and the artifact-hint retry-limit goto remain after measured alternatives
changed retail code.

| Function | Cast combinations tested | Casts removed | Retained retail bytes |
| --- | ---: | ---: | ---: |
| ComputeUALoc | 8 | 3 | 891 |
| game::CreateBoat | 64 | 6 | 281 |
| game::ClaimTown | 64 | 4 | 775 |
| game::ClaimMine | 16 | 3 | 1044 |

## Conversion ownership and bounds

The removed assignment casts duplicate conversions already performed by their
declared destinations. Hint chance/coordinates, boat slot/ID/coordinates/owner,
town record/table/list entries and mine record/table entries remain signed
bytes. The boat's saved metadata remains an unsigned byte. No declared type,
signedness, truncation rule or serialization width changes.

The town and mine wire-ID casts likewise duplicate SendMapChange's existing
i8 parameter conversion. In particular, mine IDs 128..143 still narrow to
the same signed-byte wire representation under VC6; removing the explicit
cast does not assert that every mine ID is representable as a positive i8.
The boat metadata still takes the same low eight bits of the 13-bit field.
Coordinate and sentinel conversions preserve existing behavior, not new
validation or a claim that every coordinate is below 128.

The three retained casts narrow the argument of GetPlayerColor, whose current
recovered parameter is i32. Removing either town argument cast reduces the
function by one byte; removing both reduces 775 to 773. Removing the mine
argument cast reduces 1044 to 1043. The normal player domain makes these
conversions semantically redundant, but the tested current source requires
their emitted argument preparation. A future evidence-backed helper-type
recovery is a separate question.

All four complete products restore source and finish without wall-time or
generator truncation: 9.168, 65.573, 67.278 and 18.817 seconds respectively.
Every cast-only arm for hint and boat preserves code. Town's 16 arms keeping
both color casts preserve code. Mine's eight arms keeping its color cast have
identical text, but the batch's relocation hash changes with temporary labels;
that distinction was audited rather than ignored.

## Mine relocation identity check

The final mine function has 20 relocation sites. Four switch-dispatch labels
and ten switch-table labels acquire different compiler-generated names after
the combined cast removals. All 14 remain in the same function at the same
owner-relative offsets; their sites and types also match. The other six
relocations are unchanged. The independent retail checker proves the actual
destination of every site, including those labels. No new normalization or
checker exception was introduced. The batch's differing hash alone is neither
a rejection nor a proof of correctness.

## G47: retain the retry-limit goto

A structured loop-break alternative was tested before cast changes. It
reduces the function from 891 bytes/31 diagnostic blocks to 887 bytes/29
blocks. After selecting the cast removals, a second complete product crosses
both control-flow families with all eight hint-cast combinations. All eight
goto arms remain 891 bytes; all eight loop-break arms remain 887 bytes.
The latter product completes 16/16 arms in 17.620 seconds, restores source
and is not truncated. Retain the retry-limit goto and its saveLocation label.
No generated declarations, filler statements or TU-state probes are retained.

The first small generator invocation reported a conservative truncation flag.
It was superseded by an untruncated two-arm run with a larger generation cap,
then the full 16-arm CFG/cast cross above. Only those complete runs support
the disposition; no broader AST or compiler-state coverage is claimed.

## Final verification and scope

All four retained functions independently pass ordinary retail bytes, exact
size, the complete DIR32-site list and every ordered relocation target
(`build/c34-map-<RVA>-final-retail.log`). The full build passes 1727/1727
function checks and 291987/291987 data bytes (`build/build-c34-map-final.log`).
Strict native comparison against clean9e33e4659 passes all 98 objects and
1826 emitted functions, allocated bytes, attributes and ordered relocation
graphs (`build/c34-map-final-native.log`). The game layout and all eight
consumer TUs pass both supported dialect checks again
(`build/c34-map-final-dialects.log`).

Every exact source family and measured disposition is archived under
`docs/matching/game-map-casts/`; full products are under
`build/source-variant-batch/c34-{hint,boat,town,mine}/`, `g47-retry-full/`
and `g47-cast-cross/`.

This covers these four C34 consumers and G47, not C34's remaining functions.
Existing player/town/mine/boat domains, map-edge accesses, live record/list
consistency, hint-source validity and protocol contracts remain B55/B51.

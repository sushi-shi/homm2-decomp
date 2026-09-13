# C34/S37: SaveGame and LoadGame

The cast products start at clean `4fd5bdbcb`. Both complete bodies, their
destination types and all local-buffer consumers were reread. This scope is
only SaveGame (`0x4d3ae`) and LoadGame (`0x4e8d9`), not the separate map-loader
or network-transfer buffers.

## Scalar conversions

Each function has a complete four-arm product of its two written casts.
All arms compile; the source is restored, with no generator or wall-time
truncation. No generated AST transformations or TU-state census is claimed.

| Function | Casts removed | Time | Emitted size |
|---|---|---|---|
| SaveGame | current-player and human-flag assignments to char storage | 5.516435480676591 s | 2906 |
| LoadGame | current/watched player mask assignments to u8 globals | 5.334152310155332 s | 2681 |

SaveGame's `bchar` is plain char, not bool. The human flags are copied with
the same narrowing conversion; this does not introduce truth-value
normalization. The current-player byte is likewise unchanged. LoadGame's
shift still happens as an integer before conversion to the same unsigned
byte. The ordinary live-player domain is 0..5; removing the written cast
does not validate a malformed shift count or make the watched-player loop
terminate when no local human exists.

Every SaveGame arm has text hash `042334ab331accb4` and normalized relocation
hash `c16d188278d43bc0`. Every LoadGame arm has text hash `cd864eec504ad031`;
its private-label relocation hashes vary. Equal text hashes alone are not
the ordered-target proof. The matrix block views have 60/60 and 57/57 exact
blocks, respectively; block metrics are diagnostic, not a substitute for
bytes and relocation semantics. Trial3 is selected in both products.

The retained before/after raw LoadGame comparison changes only eight `$SG`
string-symbol names, at offsets0x10/0x30/0x60/0x79/0x7e/0x9d/0x2c5/0xa5b.
Their sites and types remain identical; the ordinary retail checker proves
the same C-string contents at each destination. The full native comparison
also proves all allocated string bytes and ordered relationships. No
private-name-only hash is used to accept a changed target.

## Storage and filename contracts

The storage products start with all four selected cast removals. Each
function has six independent two-arm local-owner choices, including
deleting its unread declarations and reducing arrays to observed direct
accesses. SaveGame's lastTag arm deletes both declaration and unread store.
LoadGame's four-byte chunkTag is excluded: its reads really consume four
bytes. All path/name buffers and the heap empty-payload allocation remain
outside this storage product.

The SaveGame workBuf100-to40 arm is only a conditional size experiment:
40 bytes cover the clear, 36-byte reserved-field write and 14-byte filename
write, but they do not bound GenerateStandardFileName's preceding output.
The helper copies the entire input when there is no dot, and appends an
uncapped extension when there is one. No universal smaller-owner proof is
claimed, regardless of compiler results. The existing 100-byte owner also
requires the B56 caller capacity contract.

The actual LoadGame workData accesses are reads of 36 and 14 bytes. Both
human arrays consume six entries; current-player arrays consume one byte.
Unused declarations do not prove removable native stack reservations.
Selected source arms and complete measured dispositions accompany this
report under `docs/matching/game-save-load/`.

Both storage products completed all64 compiled arms, restored their source
and had no generator or wall-time truncation: SaveGame took
67.81681750435382 seconds; LoadGame took66.24839116912335 seconds.
In each, only trials0,8,16,24 preserve baseline text and relocation hashes.
Trial24 is retained: the human array shrinks8-to6 and current-player
array4-to1. Removing any of the three unused declarations or reducing the
work buffer changes bytes. Those alternatives remain measured residuals
under this parent, not claims of universal impossibility. LoadGame's
workData therefore stays40 bytes despite its largest direct read being36.
The corresponding private-label relocation hash is `e78d04bd45323af3` in
this storage parent; all four text-identical arms share it.

Overall, all136 arms across four products completed. No timeout or
partially generated product is counted as coverage. The early LoadGame
axes were rejected before compilation because their text also occurred
in other functions; the measured product uses the unique post-close(fd)
context instead. No source replacement was allowed to affect those siblings.

## Verification

The combined retained source passes `homm2 build`:1727/1727 function gates
and291987/291987 data bytes. Strict before/after comparison of all98 native
objects preserves all1826 emitted function instances, allocated bytes,
section attributes and ordered relocation graphs with no exceptions
(`build/c34-save-load-final-native.log`). The VC6 game-layout probe and all
16 existing consumer/dialect checks also pass
(`build/c34-save-load-final-dialects.log`).

Both independent retail checks retain exact ordinary bytes, sizes and DIR32
sites. Their existing CRT-name gaps remain exactly the U12-functions.tsv
set: SaveGame offsets0x198/0x1f4/0x20e and LoadGame offsets0x69/0x2ce use
candidate `_strnicmp` with retail destination0x4d8f60. These are not new
differences and are not silently normalized into a fully passing general
ordered-target check. The reviewed CRT manifest names that address
`__strnicmp`; no manifest or checker exception was changed here.
Logs are `build/c34-save-load-<RVA>-{before,final}-retail.log`; final scoped
relocation and block reports use the same prefix. Build output is
`build/build-c34-save-load-final.log`.

## Remaining contracts

B57 file-supplied counts, map dimensions, signed lengths, unchecked short
reads/writes, cross-member serialized prefixes, duplicated count headers
and allocation/lifecycle contracts are unchanged. No file-format hardening,
extra padding, alternate owner or gameplay change is introduced. These
experiments establish compatible reconstructions, not original spelling.

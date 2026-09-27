# Audiere constructor ownership and remaining native placement

The resource-parameterized node stores the existing `Resource*`, stream reference,
and next-node pointer. Its concrete `sample` constructor specialization is defined
before the public functions; its primary inline destructor remains at EOF. No
public body, compile flag, archive boundary, or compiler-input count changes.

The complete source products and rejected descendants are recorded in
[specialized-constructor-boundaries.cpp](specialized-constructor-boundaries.cpp)
and [concrete-destructor-ownership.cpp](concrete-destructor-ownership.cpp).
The reviewed integration payload was compiled against the isolated
`matcher/native-link-recovery` headers. Its shared-header consumers
(AudiereMusic, MilesSound, soundmgr) preserve all section bytes and ordered
semantic relocations; only anonymous compiler-counter spellings change.

## Native measurement

On the verified 204-byte Misc/DIMMER checkpoint, this ordinary unsplit object
produces a 1,208,393-byte executable with SHA-256
`29064b1aa592911b31345d3f71535f802876f9e974cf35a360530da958dffde2`.
It differs from retail in 115 bytes: 114 `.text` bytes and one `.data` byte.
The headers, section geometry, `.rdata`, resources and overlay are exact.
Against the 204-byte checkpoint, 94 differing offsets are repaired and five
previously equal offsets differ; the net reduction is 89 bytes. The five new
offsets remain within the same Audiere ordering issue.

The residual is completely localized:

| Location | Differing bytes | Cause |
| --- | ---: | --- |
| VA `0x4cc7a5`, `0x4cc854` | 2 | Two Purge call displacements target the moved node destructor |
| VA `0x4cd003` through `0x4cd07b` | 112 | Node destructor and ctype initialization helpers have the wrong order |
| VA `0x4ef140` | 1 | CRT initializer pointer follows the moved helper |

The stream-reference destructor and assignment now precede the node destructor.
The remaining tail is node / guarded ctype initialization / atexit registration;
retail requires guarded initialization / registration / node. All three actual
helper bodies and ordered relocations match their retail counterparts.

The CRT cell holds `0x4cd030` instead of `0x4cd000`. Both candidate objects and
the retail comparison object have `.CRT$XCU+0 DIR32 -> _$E21`, with zero addend.
The guarded helper itself is byte-identical at its displaced address. Recovering
its ordinary placement therefore fixes the data byte without a storage rewrite.
The two call operands and this pointer are consequences of the same order issue.

Artifacts in matcher-4:
`build/link/audiere-stable-unsplit/{control,specialized-constructor}/`,
`build/link/native-image-audit.json`, and
`build/link/ownership-combinations/audiere115-raw-audit.json`.
The independent CRT proof lives in matcher-1's
`build/audiere-crt-pointer-audit/{audit.py,results.json,audit.log}`.

## Ordinary pipeline verification

The source change was then integrated in matcher-4 and passed canonical
`homm2 redelink`, `homm2 build`, and `homm2 link --historical`. All four
header consumers rebuilt. The native output reproduced the exact SHA-256 above.
The full comparison reports 1,727/1,727 source functions and
292,010/292,010 data bytes exact. Focused relocation checks pass for all twelve
AudiereEffects public functions. The raw production object reproduces the
reviewed candidate's complete section bytes, layout, and ordered relocation
records, allowing only anonymous labels identified by section and offset.

Logs: `build/link/ownership-combinations/audiere115-{redelink,build,native,relocs}.log`;
whole-image offsets: `audiere115-image-audit.json` in that directory.
These object-level results do not close the 115-byte native placement residual.

The independent `link_exe --audit-existing --strict` audit exits 1, as expected
for this residual. It reports all 1,509 source function addresses exact, but
only 1,724/1,727 project function addresses exact: the three helpers above are
displaced. Import ABI, IAT order, all 6,374 semantic import bytes, all 244 import
thunks, and resources match. The audit does not confuse exact comparison
objects with an exact linked image. Its log and exit status are preserved as
`audiere115-strict-audit.{log,status}` in the same artifact directory.


## Separate diagnostic branch

Three semantic owners can reproduce every byte after the PE headers, but add
two C++ compiler producers. That image has 263 differing header bytes and no
nonheader differences. It is diagnostic evidence, not executable closure.
Its archive-extraction root was explicitly forced for the diagnostic and is
not retained in production.
The [direct-input control](../LINK6/direct-project-import-boundary.md) removes
that root but disrupts the earlier import-thunk boundary. A separate
[Music provider matrix](music-provider-split-parent.cpp) can emit the exact node
body before Music's real globals, preserving existing sections, but also emits
an earlier ANY copy in Purge. That duplicate prevents the desired selection.
Neither diagnostic is a retained source/build fix. The discarded-contribution probes in
[producer-selection.md](../LINK6/producer-selection.md) show that loading and
then discarding an object does not remove its producer record.

## Clean-source propagation checks

From isolated checkpoint `1af9ffa09`, `homm2 clean --out
build/native-recovery-clean --verify` generated 235 files and successfully
built the independent modern Win32 executable at
`/nix/store/vr2p96b5hbxgd1z5fdnc3vic5xwhyrcf-homm2-gold-buka-i686-w64-mingw32-2.1/HMM2PL.exe`.
This verifies the accepted Misc, DIMMER and Audiere structures after matching
annotations and retail-only inline hints are removed.

An isolated Git snapshot of that generated source also feeds the classic
Russian transformation. It emits 231 files and passes UTF-8, scaffold and
punctuation checks. The classic output is a terminal reading view: the generic
`--verify` build attempt fails because it has no `build.ninja`. This is not a
successful classic executable build. Logs are `build/link/native-recovery-clean.log`,
`native-recovery-classic.log`, and `native-recovery-classic-verify.log`.
No shared branch or checkout was published or changed by these checks.

## Current-parent build alternatives

The [frontend replay](current-template-frontends.cpp) measures two distinct
preserved C1XX binaries with the pinned driver/backend. Both preserve all section
bytes, ordered semantic relocations, producer identity, and helper order. The
old RTM/SP3 path labels are duplicate binary inputs, not independent revisions.

The [non-/Gy comparison](../Audiere-emission-queues/non-gy-current-parent.md)
preserves all nineteen function bodies, ordered relocations, EH and data while
moving N after G/R. Its order is G R S A N, however, and the ordinary public
functions are packed in one contribution without retail's gaps. This lower
build state is preserved; it supplies neither the required S/A position nor an
independent alignment mechanism. Neither experiment changes the production
115-byte executable or its recorded SHA-256.

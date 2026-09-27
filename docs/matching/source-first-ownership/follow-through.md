# Whole-module controls and storage cleanup

The [complete module/storage controls](whole-module-follow-through.cpp) cover
12 audio arrangements and a 32-image REQUEST/DIMMER product. All completed.
They broaden the previous helper experiments, but remain experiments organized
around known mismatches. They do not replace writing a coherent developer-style
implementation first and measuring that frozen source afterward.

Two changes are retained at this checkpoint: REQUEST uses ordinary empty string
literals instead of five named one-byte arrays, and DIMMER includes the normal
`KB.h` umbrella. The global `cFRDummy` remains. The measured alternatives and the
actual 2.0 CodeView owner records are preserved in the dossier.

Fresh `homm2 redelink`, `homm2 build`, historical native linking, and status
regeneration pass. Normalized object comparison reports **1,727/1,727 functions
at 100% and 291,990/291,990 data bytes**. This does not establish final executable
equality or complete ordered relocation closure.

The 1,208,393-byte historical image has SHA-256
`2cbf8bb213067d1030546a8e829f99bbea0c5dc647782de95d2f2f6aada227c7`.
It differs from retail at **10,099 file offsets**: headers 264, `.text` 7,054,
`.rdata` 3 and `.data` 2,778. Resources match. The preceding checkpoint differed
at 290,183 offsets. The large reduction comes from restoring DIMMER's missing
initializer contribution and consequent address alignment, without bringing
back the artificial string arrays.

The strict native audit correctly exits 1. Ordinary source starts agree at
1,494/1,509 addresses, with 15 displaced. Project starts agree at 1,706/1,727,
with 21 displaced and none unavailable. Import ABI, IAT order, all 6,374 semantic
import bytes and all 244 semantic thunk bytes agree; raw thunk layout also agrees.

DIMMER's seven ordinary bodies preserve raw bytes and complete ordered
relocation sites/types/identities/addends relative to the preceding raw object.
REQUEST preserves its ordinary masked bodies and relocation counts, but its
empty-string owner-relative offsets rotate back to the unresolved natural-literal
arrangement. Focused relocation-owner checks complete without missing owners;
they are not a claim that this rotation is correct.

The clean export and standalone Win32 build pass. No runtime/play-test result
is claimed. Logs, raw review and full image comparison are under
`build/holistic-storage-widgets/`: `final-{redelink,build,link,status,audit}.log`,
`final-image.json`, `retained-raw-review.log`, `0x*-relocs.log`, and `clean.log`.
All work remains isolated on the draft decomp PR; no master/Ironfist propagation
is claimed.

# Readonly contribution audit

This audit separates section placement, allocation topology, and relocation
destination identity. Retail `.rdata` starts at RVA `0xeb000`; the current pinned
LINK 3.00 candidate starts at `0xea000` because its preceding `.text` is one page
shorter. Both sections have raw size `0xe00` and virtual size `0xc9d`.

All 31 retained public `.rdata` symbols now have the exact retail
section-relative offset. This includes the TOWNMGR vtable at `0x140`, the
swapManager vtable at `0x650`, the advManager vtable, FONT, and RESMGR. The old
claim that the first relative drift was `+0x8` at swapManager is obsolete: it
described a PHILAI candidate constant-pool excess which has been removed.

Raw `.rdata` is not byte-exact because vtables contain displaced candidate
function RVAs and the export directory contains link-time fields. Those bytes do
not imply a readonly allocation-order defect. The final-link report records the
raw comparison separately from public section-relative topology and direct code
relocation targets. The current direct-operand pass compared 435 aligned
`.rdata` sites and found neither a section-offset nor an identity divergence.

`philAI::DetermineTargetPosition` multiplies the selected human target by the
retained `AI_EVENT_HUMAN_VALUE_FACTOR` owner (`1.5f`). At the same
function-relative DIR32 site, the retail instruction operand reads RVA
`0xeb280`. The anonymous `1.1f` spelling instead relocated to a different
candidate allocation which payload pairing had associated with equal `1.1f`
bytes elsewhere in the retail pool. That proved a possible allocation mapping;
it did not prove that this code site selected it. The retail PE operand does,
and it selects `1.5f`.

The former `0x368` delinked comparison object was synthesized from that reviewed
candidate-derived model; it was not an original retail COFF object. We therefore
cannot claim that retail compilation emitted a dead `1.1f` slot or that LINK
discarded one. Correcting the model first dropped the object-level data score, as
expected. Regenerating the canonical target then removed the invalid eight bytes
from both the target and denominator: current candidate and target PHILAI
`.rdata` are `0x360`, and the program data score is again 100% for the corrected
evidence set.

## Historical SOURCE recovery

The measurements below record contribution defects from older candidate epochs.
They are retained as recovery evidence, not current offsets or conclusions.

The first public, `??_7townManager@@6B@`, is at offset `0x140` in both images. The
candidate `TOWNMGR.obj` ordinary readonly contribution is `0xc0` bytes at offset `0x80`;
its SHA-1 is `478e9567157c...`, and the complete bytes occur once at retail offset `0x80`.
Neither contribution causes the first drift.

An older candidate readonly sequence was:

| Candidate offset | Owner | Raw/aligned bytes | Retail evidence |
| ---: | :--- | ---: | :--- |
| `0x150` | `SPELLS.obj` | `0xac` / `0xb0` | Retail span to exact `COMMAND` bytes is `0xb8`; active matching owner, not changed here. |
| `0x200` | `COMMAND.obj` | `0x64` | Complete bytes occur once at retail `0x208`. |
| `0x264` | `Viewwrld.obj` | `0x10` | Complete bytes occur at retail `0x26c`. |
| `0x278` | `PHILAI.obj` | `0x3c4` / `0x3c8` | Word alignment against retail places the next owner boundary at `0x5e0`, an inferred retail span of `0x360`. |
| `0x640` | `ARMY.obj` | `0x68` | Word alignment begins the retail ARMY pool at `0x5e0`; its span to the swap vtable is `0x70`. |
| `0x6a8` | `??_7swapManager@@6B@` | vtable | Retail offset `0x650`, then-current relative delta `+0x58`. |

The combined source-contribution accounting is therefore `SPELLS -0x8`, `PHILAI
+0x68`, and `ARMY -0x8`, producing the observed `+0x58`. This is an ownership and
constant-pool recovery queue, not permission to add padding or force section placement.
An experiment making PHILAI's typed floating constants preprocessor macros reduced its
pool, but folded retail x87 arithmetic and dropped `ValueOfBuyingHero` from its retained
99.94% to 88.04%; it was rejected.

## SWAPMGR through ADVMGR

Retail offsets `0x650` through `0x65b` contain the three relocated swapManager vtable
entries, followed by four alignment bytes. Retail `0x660` through `0x67f` is the exact
eight-element integer table:

```text
64, 57, 40, 21, 7, 5, 3, 0
```

Both `advManager::SetEnvironmentOrigin` and `advManager::InsertSound` relocate indexed
loads to this table; their candidate and retail relocation counts/offsets are respectively
18/18 and 9/9 with no candidate-only relocation. The prior reconstruction used a casted
six-integer string literal and omitted `3, 0`, placing the bytes in writable literal
storage.

Recovering the real `static const int[8]` produces one `0x68` ADVMGR ordinary `.rdata`
contribution: the `0x20` table followed by the existing `0x48` compiler constant pool.
LINK places that ordinary contribution before `??_7advManager@@6B@`, as retail does.
Consequently the advManager vtable changed from an independent `-0x10` relative delta to
the same then-current `+0x58` cumulative delta as swapManager. The swap-to-adv boundary was structurally
resolved even though earlier SPELLS/PHILAI/ARMY contributions still displace both.

## Linker identity

Current measurements use the archive.org-verified LINK 3.00.5270 component and
its sibling CVPACK/CVTRES/MSPDB40 tools and `LIBCMT.LIB`. The disposable
final-link objects remove source-only `dllexport` directives, and an explicit
module definition restores only retail's `AppAbout` and `AppWndProc` exports plus
the `Heroes of Might and Magic 2` description. The resulting `.edata` has the
retail `0x5d` span at `.rdata+0xc40`; its function RVAs remain displaced with
`.text`. See `compiler-detection.md` and
`toolchain-vc42.md` for hashes and reproduction.

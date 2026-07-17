# Readonly contribution audit

This audit separates whole-section placement from contribution drift. Retail `.rdata`
starts at RVA `0xeb000`; the pinned LINK 3.00 candidate starts at RVA `0xf0000`.
Offsets below are relative to each image's own `.rdata`, so that `0x5000` section-RVA
difference is intentionally removed.

The current linked result is exact through the TOWNMGR vtable at relative offset
`0x140`. The first later public anchor, the swapManager vtable, is at `0x650` in
retail and `0x658` in the candidate (`+0x8`). The advManager vtable retains that
`+0x8` delta. Alignment after it raises the inherited delta to `+0x10` at the
highScoreManager vtable. FONT (`0x9e4` retail, `0x9f4` candidate) and RESMGR
(`0x9f0` retail, `0xa00` candidate) retain the same `+0x10`; they do not prove a
new BASE contribution defect.

The underlying SOURCE payload difference is narrower than those anchor deltas.
SPELLS (`0xb4`), COMMAND (`0x64`), Viewwrld (`0x10`), and ARMY (`0x70`) reach
their next retail anchors with the expected sizes and alignment. PHILAI's
ordinary readonly contribution is `0x368`, while the retail owner span is
`0x360`. That `+0x8` payload appears directly at swapManager/advManager and rises
to `+0x10` after later alignment. It remains source/compiler constant-pool
recovery work, not a linker or BASE/FONT placement issue.

`philAI::DetermineTargetPosition` multiplies the selected human target by the
anonymous `1.1f` temporary. The retail pool orders that temporary between the
function's own `1.05` double and the battle `1.11` double, a position only a
literal at this site can produce; an earlier revision substituted the
`AI_EVENT_HUMAN_VALUE_FACTOR` owner (`1.5f`) on masked-relocation evidence and
has been reverted. The other former `0x8` excess came from a typed
hero-purchase divisor and an eight-byte attention identity; the divisor is a
double macro and the identity a float again, so the contribution matches the
reviewed 0x368 candidate layout (retail payload multiset plus two alignment
zero words).

## Historical SOURCE recovery

The measurements below record the contribution defects that led to the current
exact SOURCE prefix. They are retained as recovery evidence, not current offsets.

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
its sibling CVPACK/CVTRES/MSPDB40 tools. Its PE linker version matches retail.
The VC 4.2 LINK 4.20 result is no longer the final-link baseline. See
`compiler-detection.md` and `toolchain-vc42.md` for hashes and reproduction.

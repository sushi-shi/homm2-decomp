# Readonly contribution audit

This audit separates whole-section placement from contribution drift. Retail `.rdata`
starts at RVA `0xeb000`; the current LINK 4.20 candidate starts at RVA `0xf0000`.
Offsets below are relative to each image's own `.rdata`, so that `0x5000` section-RVA
difference is intentionally removed.

## TOWNMGR through SWAPMGR

The first public, `??_7townManager@@6B@`, is at offset `0x140` in both images. The
candidate `TOWNMGR.obj` ordinary readonly contribution is `0xc0` bytes at offset `0x80`;
its SHA-1 is `478e9567157c...`, and the complete bytes occur once at retail offset `0x80`.
Neither contribution causes the first drift.

The next candidate readonly sequence is:

| Candidate offset | Owner | Raw/aligned bytes | Retail evidence |
| ---: | :--- | ---: | :--- |
| `0x150` | `SPELLS.obj` | `0xac` / `0xb0` | Retail span to exact `COMMAND` bytes is `0xb8`; active matching owner, not changed here. |
| `0x200` | `COMMAND.obj` | `0x64` | Complete bytes occur once at retail `0x208`. |
| `0x264` | `Viewwrld.obj` | `0x10` | Complete bytes occur at retail `0x26c`. |
| `0x278` | `PHILAI.obj` | `0x3c4` / `0x3c8` | Word alignment against retail places the next owner boundary at `0x5e0`, an inferred retail span of `0x360`. |
| `0x640` | `ARMY.obj` | `0x68` | Word alignment begins the retail ARMY pool at `0x5e0`; its span to the swap vtable is `0x70`. |
| `0x6a8` | `??_7swapManager@@6B@` | vtable | Retail offset `0x650`, current relative delta `+0x58`. |

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
Consequently the advManager vtable changes from an independent `-0x10` relative delta to
the same `+0x58` cumulative delta as swapManager. The swap-to-adv boundary is structurally
resolved even though earlier SPELLS/PHILAI/ARMY contributions still displace both.

## Linker gate

These measurements use the locally available LINK 4.20.6164. Retail's PE optional header
records final-linker version 3.00, and no local LINK 3.00 binary is available. Do not tune
remaining contribution offsets around behavior that a provenance-correct final linker may
change. Use the isolated linker A/B path documented in `compiler-detection.md` when a
provenance-known LINK 3.00 artifact becomes available.

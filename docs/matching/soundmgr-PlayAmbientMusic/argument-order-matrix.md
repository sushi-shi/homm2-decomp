# `soundManager::PlayAmbientMusic` (RVA 0xb64d0) — the audiere-arm argument order

Reviewed attempts, in the order they were measured. Baseline 82.459770%, size 253
(retail 262), 20/20 blocks with `edges=0 pred=0 flow=0 shift=0` — a structurally
aligned residual whose only defect was three block SIZES.

## Attempt 1 — call-site spelling matrix (`homm2 permute`, exact-span axes)

`build/source-variant-batch/20260805-041019-BASE-soundmgr-0xb64d0`,
`--min-depth 0 --max-depth 0`, one axis over the whole call statement:

| option | result |
| :-- | :-- |
| `orig` (multiline, device third) | 82.459770% size 253 relocs 5/5 |
| `oneline` | identical |
| `this_qual` (`this->` on all three members) | identical |
| `explicit_temp` (`audiere::AudioDevicePtr(m_audiereDevice)`) | identical |
| `static_cast_temp` | identical |
| `paren_refs` (`(m_musicTrack), (m_musicFadeSteps)`) | identical |
| `paren_device` | identical |
| `deref_addr_device` (`*&m_audiereDevice`) | identical |

Every arm produced the same bytes. Disposition: the defect is not a call-site
expression shape.

## Attempt 2 — TU-state island census

`python3 -m homm2.permute.tu_state_noise --source src/BASE/soundmgr.cpp
--rva 0xb64d0 --trials 50 --families forest --retain-best`:

```
target states: 1 unique byte/relocation states
  state 01 a10c7bed75ffc8fb: baseline; occurrences=51 scores=82.459770% size=253
    blocks=20/20 edges=0 pred=0 flow=0 shift=0 retail-byte-delta=237
```

One state over 51 compiles. Disposition: inert to compiler state — the signature
of a declaration defect, not a codegen orbit.

## Attempt 3 — codegen probe (decisive)

A three-function probe compiled once with the unit's own flags settled which
declaration. Full listing and asm in
`docs/patterns/byvalue-class-arg-position-names-parameter-order.md`; the result is
that `Callee5(RefLike c, int& a, int& b, int d)` called as
`Callee5(dev, track, steps, t)` reproduces retail instruction for instruction,
while `Callee(int& a, int& b, RefLike c, int d)` reproduces ours.

## Closure

`PlayAudiereMusic`'s reconstructed parameter order was wrong. Retail's is

```cpp
void PlayAudiereMusic(audiere::AudioDevicePtr device, i32& currentTrack, i32& fadeSteps, i32 track);
```

`PlayAmbientMusic` 82.46% -> EXACT; `PlayAudiereMusic` (BASE/AudiereMusic) stayed
EXACT across the reorder, because `__fastcall` skips the class parameter when it
assigns `ecx`/`edx` and the two orders give the callee an identical frame.

# One frame slot serving two `case` arms is ONE shared local

**Trigger.** Two mutually exclusive `switch` arms compute the same *kind* of
value (a modifier, a running count, a scratch float) and retail writes both
through the **same** `-0xN(%ebp)` slot. Reconstructing them as two separate
per-arm locals adds one slot and shifts every deeper local by four bytes, so
the whole frame below the split reads wrong even though the instruction
stream matches.

## The measured divergence

`combatManager::RawEffectSpellInfluence` at RVA 0x0009716f. Retail uses
`-0xc` in the DRAGON_SLAYER arm:

```
     mov  dword ptr [ebp-0xc], 0x3f800000     ; adjacent -> 1.0f
     ...
     fild dword ptr [ebp-0x74]
     fstp dword ptr [ebp-0xc]
     fld  dword ptr [_COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER]
     fmul dword ptr [ebp-0xc]
```

and the *same* `-0xc` in the SHIELD arm:

```
     fild dword ptr [ebp-0x78]
     fstp dword ptr [ebp-0xc]
     fld  dword ptr [ebp-0xc]
     fadd qword ptr [__real@8@3ffd9999999999999800]
     fstp dword ptr [ebp-0xc]
     ...
     fld  dword ptr [_COMBAT_SPELL_AI_SHIELD_MODIFIER]
     fmul dword ptr [ebp-0xc]
```

Ours, from `float dragonMod;` plus `float shooterMod;`, used `-0x2c` and
`-0x24` — two slots, frame `0x70` where retail is `0x78`.

## What made it match

One local for both arms:

```cpp
    float factor;
    ...
    case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
        ...
        effect = static_cast<i32>(COMBAT_SPELL_AI_DRAGON_SLAYER_MODIFIER * factor);
        break;
    case ARMY_SPELL_INFLUENCE_SHIELD:
        ...
        effect = static_cast<i32>(COMBAT_SPELL_AI_SHIELD_MODIFIER * factor);
        break;
```

PoL 2.0's retail bytes for the same function share a slot the same way
(`-0x4` in both arms there), so the sharing is invariant across versions even
though the two frames are otherwise unrelated — useful corroboration when the
single-slot reading looks accidental.

## Companion evidence

Retail's frame for this function is `0x78` = 30 slots: 17 live named locals,
**3 declared-but-unused** (`-0x30`, `-0x38`, `-0x48` are never referenced),
the `this` spill and nine compiler temps. Merging the two floats and adding
the three dead locals is what makes 27 live slots land on retail's offsets;
see also [dead-local-frame-gap](dead-local-frame-gap.md) and
[od-frame-function-scope-locals](od-frame-function-scope-locals.md) — retail
declares all 20 named locals at function scope, interleaving what a
C++-styled reconstruction puts in per-case blocks.

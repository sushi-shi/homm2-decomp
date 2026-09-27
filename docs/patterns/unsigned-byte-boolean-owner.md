# Unsigned byte truth tests belong to unsigned byte fields

Measured in ARMY PowEffect, RVA0x1df4e, VC6 SP5 with the configured SOURCE/ARMY
profile. The former `bchar` fields m_animationState/m_animationCycle required
seven `static_cast<u8>` expressions to reproduce retail's zero extension.

The complete2^8 experiment removes each of those seven casts and the unrelated
i32 identity cast independently. Only identity removal is exact under the
old owner. At function+0x1e3, removing the first cast changes four load bytes
to three, while preserving the logical zero/nonzero result:

```text
retail / old explicit u8 cast / new u8 field     old char field, no cast
33 c0       xor eax,eax                         0f be 02  movsx eax,byte [edx]
8a 02       mov al,[edx]
85 c0       test eax,eax                        85 c0     test eax,eax
```

The full consumer pass finds only unsigned-byte reads and boolean writes,
with one external false store in combatManager::CastSpell. Recover the fields:

```cpp
u8 m_animationState;
u8 m_animationCycle;
// ...
if (current->m_animationState) { /* existing body */ }
```

All seven compensating casts disappear. The entire4567-byte PowEffect and
all128 ordered relocation sites/types/targets/addends agree with retail.
The header change is independently native-equivalent across98 objects and
1826 emitted function instances, including all allocated sections and ordered
relocations. The packed0x482 army size and other fields are unchanged.

This is an owner-type recovery, not a rule that every char boolean should be
unsigned. Review all field consumers and ABI/layout constraints first; do not
change the shared bchar alias or substitute VC6 bool.

Evidence: `build/c56-pow-batch/results.json` (256/256, restored, no truncation),
`army-owner-all-native.log`, `army-pow-owner-final-retail.log`, and the
[army review](../reconstruction/C55-C56-B73.md). The old-owner failed arms remain
diagnostic history; they do not contradict the new structural parent.

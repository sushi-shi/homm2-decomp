# Two-dimensional indexing preserves the real row owner

Measured with this branch's VC6 SP5 flags, not inferred from another decomp.
ClearEffects originally flattened `gArmyEffected[0]` across two20-byte rows.
The second side is outside that first row, despite fitting the enclosing
2x20 object. Recover the indexing of the actual declared owner:

```cpp
// Before
*(gArmyEffected[0] + IDX(side) * COMBAT_ARMY_SLOT_COUNT + idx) = 0;
// Recovered
gArmyEffected[IDX(side)][idx] = 0;
```

The complete2-arm family is byte-neutral, including every ordered relocation.
At function+0x39, the retail and both candidate forms emit:

| Retail bytes/instruction | Recovered candidate bytes/instruction |
| --- | --- |
| `8b 55 f8` mov edx,[ebp-8] | `8b 55 f8` mov edx,[ebp-8] |
| `6b d2 14` imul edx,edx,20 | `6b d2 14` imul edx,edx,20 |
| `8b 45 fc` mov eax,[ebp-4] | `8b 45 fc` mov eax,[ebp-4] |
| `c6 84 02 b0 65 52 00 00` mov byte [edx+eax+0x5265b0],0 | same bytes after proven relocation resolution |

The raw COFF address field is zero with one DIR32 relocation at function+0x45
to `gArmyEffected`, addend0. Retail's operand is VA0x005265b0. Full82-byte
function size, all raw non-relocation bytes, complete sites and ordered
destinations pass; there is no score-only closure.

Fireball and MeteorShower independently confirm both read and write forms
with4-arm products each. Five accesses are recovered overall, without a
global layout change. Complete SPELLS/SPELLAI native objects retain all54
emitted functions and allocated sections. Detailed matrices and private
numeric-operand proof: [O05/C58/S56](../reconstruction/O05-C58-S56-B75.md).

This establishes a sufficient clean spelling, not the original source text.
It does not justify flattening arbitrary neighboring scalar objects or
removing guards for invalid side/slot indices.

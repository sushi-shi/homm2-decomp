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

## A flat stack marker can also expose two real dimensions

AI's DoLichShot RVA0x172d6 originally owns a flat40-byte marker and manually
computes `side*20+index`. This is **not** the previous first-row overrun: the
original pointer stays inside its declared flat object for valid inputs.
Recovering its two semantic dimensions nevertheless removes three manual
pointer expressions without changing the owner size or native code:

```cpp
// Before
u8 marked[IDX(COMBAT_SIDE_COUNT) * COMBAT_AI_ARMY_SLOT_COUNT];
*(marked + IDX(targetArmy->m_side) * COMBAT_AI_ARMY_SLOT_COUNT + targetArmy->m_index) = 1;
// After
u8 marked[COMBAT_SIDE_COUNT][COMBAT_AI_ARMY_SLOT_COUNT];
marked[IDX(targetArmy->m_side)][targetArmy->m_index] = 1;
```

The complete three-arm A02 product tests original pointers, ordinary flat
indexing and two-dimensional rows. Original and rows have659 identical bytes,
all six ordered relocation identities and24 exact blocks. Flat indexing
instead produces656 bytes; semantic equivalence alone does not close that arm.

The first marker store has no relocation; retail and the retained source emit
these exact bytes, offsets relative to DoLichShot:

```text
offset  retail                                  recovered rows
+dc     8b 45 dc    mov eax,[ebp-0x24]           same
+df     8b 88 e6 00 00 00 mov ecx,[eax+0xe6]     same  (army side)
+e5     6b c9 14    imul ecx,ecx,20              same
+e8     8d 54 0d b4 lea edx,[ebp+ecx-0x4c]       same  (actual marker base)
+ec     8b 45 dc    mov eax,[ebp-0x24]           same
+ef     8b 88 ea 00 00 00 mov ecx,[eax+0xea]     same  (army index)
+f5     c6 04 0a 01 mov byte [edx+ecx],1         same
```

The adjacent-cell marker read/write retain the same row stride and stack
base. `check-ai-evidence.py` checks all three sequences against raw COFF and
retail bytes; complete native verification covers all98 objects,1826 emitted
function instances and allocated sections. Valid side0..1 and index0..19
remain caller/occupancy contracts. See [C65/A02/S59](../reconstruction/C65-A02-S59-B80.md).

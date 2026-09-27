# Common-interface pointer arrays replace overlapping views

Measured in Buka VC6 SP5 SOURCE/ADVMGR. When every use of a stored derived
pointer is a common base-interface operation, recover the array of base
pointers instead of retaining integer, derived-array and scalar overlays.
First prove the actual slot mappings and every consumer; this is not a
general license to erase derived types that consumers need.

Here the six widget-building/initializing functions and ClearBottomView
share two twelve-entry `widget*` arrays. Primary begins at manager+0x3a,
secondary at+0x6a. The old five-icon subview began at+0x42 and the five-text
subview at+0x6e. Using `primary[i+2]` and `secondary[i+1]` folds the constant
slot into the same displacement; construction uses implicit base conversion
without additional instructions.

In UpdBottomViewHero, RVA0xbd33:

| Relative site | Retail and both native versions | Recovered access |
| --- | --- | --- |
| +0x487 | `89 54 81 42` mov [ecx+eax*4+0x42],edx | primary[displayIndex+2] store |
| +0x494 | `83 7c 81 42 00` cmp [ecx+eax*4+0x42],0 | same slot null check |
| +0x564 | `89 54 81 6e` mov [ecx+eax*4+0x6e],edx | secondary[displayIndex+1] store |
| +0x571 | `83 7c 81 6e 00` cmp [ecx+eax*4+0x6e],0 | same slot null check |

The complete before/after census proves all98 TUs/1826 emitted functions and
all allocated sections unchanged, including ordered relocation semantics.
The scoped retail body's byte comparison establishes the excerpt above;
its separate exception-reference limitation is explicit in
[U08](../reconstruction/U08.md). Logs: `build/u08-hero-{base,target}.log`,
`build/u08-whole-native.log`, `build/u08-retail.log`.

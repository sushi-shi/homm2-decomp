# A parenthesized `float` cast divisor loads with `fild` + `fdivp`

**Trigger.** A float quotient of two integers where retail loads the divisor
with its own `fild` and divides with `fdivp`, while ours folds the divisor
into `fidiv`, and the multiply after it still takes a `dword` (float)
constant, so the division is not a `double` one
([fidiv-vs-fild-fdivp](fidiv-vs-fild-fdivp.md)).

Measured on `editManager::PlaceTowns` (EDT2PL.exe 0x1e709), the land share of
each region:

```
retail                                       ours
-------------------------------------------- --------------------------------------------
movsx edx, word ptr [ebp+2*ecx-0xd28]        movsx edx, word ptr [ebp+2*ecx-0xd28]
mov   [ebp-0xd6c], edx                       mov   [ebp-0xd6c], edx
fild  dword ptr [ebp-0xd6c]                  fild  dword ptr [ebp-0xd6c]
fild  dword ptr [gLandCellCount]             fidiv dword ptr [gLandCellCount]
fdivp st(1), st
fmul  dword ptr [100.0f]                     fmul  dword ptr [100.0f]
fstp  qword ptr [ebp+8*eax-0x880]            fstp  qword ptr [ebp+8*eax-0x880]
```

## The probe (VC6 SP5, the editor's `base` profile)

```cpp
void D(i32 i) { gd[i] = static_cast<float>(gs[i]) / static_cast<float>(gL) * 100.0f; }
void I(i32 i) { gd[i] = static_cast<float>(gs[i]) / (static_cast<float>(gL)) * 100.0f; }
```

`D` divides with `fidiv dword [gL]`; `I`, whose divisor cast is parenthesized,
loads it with `fild dword [gL]` and divides with `fdivp st(1), st`. Both
multiply by a `dword` constant. The `double`-typed spellings (`(double)a / b`,
`a / (double)b`, `(float)(a / (double)b)`) all still take `fidiv` for a plain
global divisor.

## What made it match

```cpp
shareValue[slot] = static_cast<float>(regionSizes[slot])
                   / (static_cast<float>(gLandCellCount)) * 100.0f;
```

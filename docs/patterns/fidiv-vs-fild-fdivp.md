# `fidiv` vs `fild`+`fdivp`: the divide's own type, not the cast spelling

**Trigger.** A float-valued expression `intA / intB`. VC6 `/Od` picks

- `fild A; fidiv B`      when the *division* is done in **float** precision, and
- `fild A; fild B; fdivp st(1),st` when the division is done in **double** precision.

The casts you write on the operands do not decide it; the type of the division does.

Measured on `advManager::DoVisions` (RVA 0x154ac).

## The divergence

```
retail                                        ours
--------------------------------------------- ---------------------------------------------
89 85 fc fe ff ff  mov  [ebp-0x104], eax      89 85 fc fe ff ff  mov  [ebp-0x104], eax
db 85 fc fe ff ff  fild dword [ebp-0x104]     db 85 fc fe ff ff  fild dword [ebp-0x104]
...                                           ...
89 8d f8 fe ff ff  mov  [ebp-0x108], ecx      89 8d f8 fe ff ff  mov  [ebp-0x108], ecx
db 85 f8 fe ff ff  fild dword [ebp-0x108]     da b5 f8 fe ff ff  fidiv dword [ebp-0x108]
de f9              fdivp st(1), st
d9 5d e8           fstp dword [ebp-0x18]      d9 9d 28 ff ff ff  fstp dword [ebp-0xd8]
```

Same operands, same temp slots, one instruction apart.

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

```cpp
float p1(void){ int a=gA(); return static_cast<float>(a)  / static_cast<float>(gCnt*gFv[1]); }
float q7(void){ int a=gA(); return static_cast<double>(a) / (gCnt*gFv[1]);                   }
```

```
p1:  db 45 fc  fild [ebp-4]   ...  da 75 f8  fidiv [ebp-8]          <- float divide
q7:  db 45 fc  fild [ebp-4]   ...  db 45 f8  fild  [ebp-8]
                                   de f9     fdivrp st, st(1)       <- double divide
```

Every `float`-typed arm (`(float)a/(float)b`, `(float)a/b`, `a/(float)b`) produced
`fidiv`; every `double`-typed arm (`(double)a/(double)b`, `(double)a/b`, `a/(double)b`,
`1.0*a/b`) produced `fild`+`fdivp`. Precision of the *divide*, nothing else.

## What made it match

```cpp
    fRatio = static_cast<float>(gpPhilAI->FightValueOfStack(...))     // ours, fidiv
             / static_cast<float>(gMonsterDatabase[IDX(type)].fightValue * count);
```
```cpp
    fRatio = static_cast<double>(gpPhilAI->FightValueOfStack(...))    // retail, fild+fdivp
             / static_cast<double>(count * gMonsterDatabase[IDX(type)].fightValue);
```

`fRatio` stays a `float`; only the division is evaluated in double, so the store is
still `fstp dword`. (The operand swap in the same line is the separate
`inline-call-operand-index-first` axis: `/Od` lowers `a * b` as `mov reg,a; imul reg,b`,
so the retail order of the two loads names the source order.)

# A literal-true switch-case guard survives under VC6 /Od

**Symptom.** A case body begins with `mov reg,1; test reg,reg; je <case end>`.
VC6 `/Od` did not fold the constant condition, so the dead edge is source
structure rather than padding.

Measured on `combatManager::CastSpell` (RVA `0x99653`), at the
`SPELL_MAGIC_ARROW` arm:

```text
unguarded spelling                         retail / guarded spelling
fld  dword ptr [combatSpeedConstant]       mov  eax, 1
...                                        test eax, eax
                                            je   <case end>
                                            fld  dword ptr [combatSpeedConstant]
                                            ...
```

Retail's exact bytes at the arm head are `b8 01 00 00 00 85 c0 0f 84 ...`.

**Close.** Preserve the literal source guard:

```cpp
case SPELL_MAGIC_ARROW:
    if (1) {
        // case body
    }
    break;
```

The paired VC4.2 matrix folded this wrapper in every tested state, so this is a
Buka compiler/source spelling and not transferable matching evidence for PoL.


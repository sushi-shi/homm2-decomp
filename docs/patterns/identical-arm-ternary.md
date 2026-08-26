# An identical-arm ternary still materializes its condition

**Symptom.** Retail emits `neg/sbb/and 0/add K` for a value that appears to be a
constant. The zero mask means the two ternary arms have the same value; the
condition remains observable even though the result does not vary.

Measured twice in `combatManager::ChainLightning` (RVA `0x9df96`) for the two
bolt-angle limits:

```text
plain constant                             retail / identical-arm ternary
push 50h                                   mov  edx, [firstBolt]
push 0Ah                                   neg  edx
                                            sbb  edx, edx
                                            and  edx, 0
                                            add  edx, 50h
                                            push edx
                                            mov  eax, [firstBolt]
                                            neg  eax
                                            sbb  eax, eax
                                            and  eax, 0
                                            add  eax, 0Ah
                                            push eax
```

**Close.** Keep the otherwise-dead read explicitly:

```cpp
firstBolt ? CHAIN_LIGHTNING_FIRST_MIN_ANGLE : CHAIN_LIGHTNING_MIN_ANGLE
firstBolt ? CHAIN_LIGHTNING_FIRST_MAX_ANGLE : CHAIN_LIGHTNING_MAX_ANGLE
```

For this Buka image each pair of named constants has equal values. Raw bytes
and complete relocations are exact; simplifying either expression loses the
retail instructions.


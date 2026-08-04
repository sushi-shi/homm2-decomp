# `xor reg,reg; test reg,reg; je $+7; jmp <label>` is a literal-FALSE `if` the front end never folded

**Trigger.** A `switch` arm (or any block) opens with a register zeroed, tested
against itself, and a conditional jump that ALWAYS skips a single near `jmp`.
VC6 `/Od` does not fold a constant condition away; it materialises the value,
tests it, and emits the dead arm. A named zero-valued enumerator in the condition
survives as `xor reg,reg`, so the arm is reachable in the source text and dead at
runtime.

Sibling of the true case (`mov eax,1; test; je`) — see the INDEX entry
`constant-true-case-guard`.

Measured on `combatManager::DoCompAI` (RVA 0x1628b), at the head of the
`COMBAT_AI_ATTACK_WALK` arm.

## The divergence

```
ours (no guard)                                retail
---------------------------------------------- ----------------------------------------------
0a21 e9 d1 03 00 00  jmp <switch end>          0a21 e9 d1 03 00 00  jmp <switch end>   (FLY break)
                                               0a26 33 c9           xorl %ecx, %ecx
                                               0a28 85 c9           testl %ecx, %ecx
                                               0a2a 74 05           je   0xa31
                                               0a2c e9 00 05 00 00  jmp  <goto-finish stub>
0a26 83 7d d4 00     cmpl $0x0, -0x2c(%ebp)    0a31 83 7d d4 00     cmpl $0x0, -0x2c(%ebp)
0a2a 0f 84 27 01..   je   ...                  0a35 0f 84 27 01..   je   ...
```

The `jmp` target is one of the parked `goto finish` stubs, so the dead arm is a
`goto`, and the stub island grows by one entry (31 stubs instead of 30) — worth
counting when the size delta does not add up.

## What made it match

```cpp
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
```

`COMBAT_AI_ATTACK_NONE` is 0, so the arm is dead; VC6 still emits it. Any
zero-valued enumerator produces the same three instructions — the bytes cannot
tell you WHICH constant the devs wrote, only that it folded to zero.

`combatManager::DoCompAI` 95.55% -> EXACT (this was 11 of the 28 missing bytes).

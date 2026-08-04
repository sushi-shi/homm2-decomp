# An empty then-branch inside a loop: `continue`, not `if (x) {} else {...}`

**Trigger.** Inside a `for` body, retail tests a guard and takes a **short backward
`jmp` to the loop increment**, while ours takes a **`jmp` forward over the rest of
the body**. Both spellings are semantically identical; only `continue` puts the
loop increment in the jump.

Measured on `combatManager::CycleCombatScreen` (RVA 0x315fa), which has the shape
twice (the hero-overlay loop and the hero-animation loop).

## The divergence

`for (i ...) { nextHeroAnimation[i] = -1; if (m_heroIcons[i] == NULL) {} else { ...60 lines... } }`

```
ours  (empty then-branch + else)              retail (continue)
--------------------------------------------- ---------------------------------------------
8b 55 ec  movl -0x14(%ebp), %edx              8b 55 ec  movl -0x14(%ebp), %edx
c7 44 95 f8 ff ff ff ff                       c7 44 95 f8 ff ff ff ff
          movl $-1, -0x8(%ebp,%edx,4)                   movl $-1, -0x8(%ebp,%edx,4)
8b 45 ec  movl -0x14(%ebp), %eax              8b 45 ec  movl -0x14(%ebp), %eax
8b 4d b0  movl -0x50(%ebp), %ecx              8b 4d b0  movl -0x50(%ebp), %ecx
83 bc 81 c5 33 00 00 00                       83 bc 81 c5 33 00 00 00
          cmpl $0x0, 0x33c5(%ecx,%eax,4)                cmpl $0x0, 0x33c5(%ecx,%eax,4)
75 05     jne  <body>                         75 02     jne  <body>
e9 ef 02 00 00  jmp <join after if/else>      eb d0     jmp  <loop increment>
```

The empty then-branch still needs a terminator. With `if/else` the terminator is a
5-byte near `jmp` to the join that MSVC places after the else body, and the join
then falls into the loop's own back edge; with `continue` the terminator is a
2-byte `jmp` straight to the increment. Three bytes and one basic block apart.

The same site reads `eb 52` (forward) vs `eb df` (backward) when the else body is
short enough for a short jump - the direction of the displacement, not its width,
is the reliable tell.

## What made it match

```cpp
    for (index_0 = COMBAT_ATTACKER_SIDE; index_0 < COMBAT_SIDE_COUNT; ++index_0) {
        nextHeroAnimation_0[index_0] = -1;
        if (m_heroIcons[index_0] == NULL)
            continue;

        if (m_heroAnimationState[index_0] == HERO_ANIMATION_DEATH_FIRST
        ...
    }
```

`combatManager::CycleCombatScreen` 93.62% -> EXACT (with the other axes on the same
function).

**Caveat.** When the `if` is the *last* statement of the loop body, the if/else join
IS the increment and both spellings emit the same bytes; the axis only separates when
something follows the `if`, or when the else body is long enough to force the near
form.

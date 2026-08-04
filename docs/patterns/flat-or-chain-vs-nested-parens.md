# A flat `a|b|c|d|e` chain accumulates into one register; parenthesised nesting spills into `esi`/`edi`

**Trigger.** A multi-term bitwise chain passed as a call argument. If the retail
body loads the FIRST term into a register and then issues one `or reg, mem` per
remaining term, the source is a **flat, unparenthesised** chain. A
right-nested `a | (b | (c | (d | e)))` evaluates the innermost pair first, needs
three live registers at once, and drags `push esi; push edi` into the prologue.

Measured on `combatManager::DoCompAI` (RVA 0x1628b), the `GetStrength` mask
argument.

## The divergence

```
ours   a | (b | (c | (d | e)))                    retail   a | b | c | d | e
--------------------------------------------      --------------------------------------------
0009 56              pushl %esi                   (no esi/edi in the prologue)
000a 57              pushl %edi

0255 8b 8d 60 ff..   movl -0xa0(%ebp), %ecx       0247 8b 8d 60 ff..  movl -0xa0(%ebp), %ecx
025b 8b 89 af f2..   movl 0xf2af(%ecx), %ecx      024d 8b 91 af f2..  movl 0xf2af(%ecx), %edx
0261 8b b5 60 ff..   movl -0xa0(%ebp), %esi       0253 8b 44 85 a0    movl -0x60(%ebp,%eax,4), %eax
0267 8b b6 af f2..   movl 0xf2af(%esi), %esi      0257 0b 44 95 f0    orl  -0x10(%ebp,%edx,4), %eax
026d 8b bd 60 ff..   movl -0xa0(%ebp), %edi       025b 8b 8d 60 ff..  movl -0xa0(%ebp), %ecx
0273 8b bf af f2..   movl 0xf2af(%edi), %edi      0261 8b 91 af f2..  movl 0xf2af(%ecx), %edx
0279 8b 74 b5 b8     movl -0x48(%ebp,%esi,4),%esi 0267 0b 44 95 f8    orl  -0x8(%ebp,%edx,4), %eax
027d 0b 74 bd e0     orl  -0x20(%ebp,%edi,4),%esi ...
0281 8b 4c 8d f8     movl -0x8(%ebp,%ecx,4), %ecx (two more `movl this / movl side / orl`)
0285 0b ce           orl  %esi, %ecx
0287 8b 54 95 f0     movl -0x10(%ebp,%edx,4),%edx
028b 0b d1           orl  %ecx, %edx
028d 8b 44 85 a0     movl -0x60(%ebp,%eax,4),%eax
0291 0b c2           orl  %edx, %eax
0293 50              pushl %eax                   0293 50             pushl %eax
```

The flat form reloads `this` and the side index before every `or`; the nested
form hoists all five index loads up front because it must keep three partial
results alive. The `push esi`/`push edi` in the prologue is the loudest tell:
a `/Od` body that never needs a callee-saved register does not save one.

## What made it match

```cpp
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
```

`combatManager::DoCompAI` 95.55% -> EXACT (this was 24 of the differing rows,
plus the two prologue pushes).

## Reading the chain back

The FIRST term of the source chain is the one loaded with `mov reg, mem`; every
later term appears in source order as `or reg, mem`. Nesting depth is visible
as the count of extra live registers, not as any reordering of the terms.

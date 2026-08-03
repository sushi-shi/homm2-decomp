# two-backedge-jmps-in-one-loop

**Symptom.** One `for` body ends with TWO distinct one-instruction blocks that both
`jmp` to the loop increment, and they are not adjacent: a middle condition fails into
the first, the tail conditions fail into the second.

`combatManager::FindResurrectArmyIndex` (0x98f2b) retail skeleton
(`homm2 sema disasm 0x98f2b --blocks --target --lite`):

    B8  @844  8i cmp sides[c], -1        jcc B17 | fall B9
    B9  @863  8i test frames[c]          jcc B11 | fall B10
    B10 @87d  7i cmp cells[hex+1], -1    jcc B13 | fall B11
    B11 @896  8i cmp frames[c], 1        jcc B14 | fall B12
    B12 @8b1  7i cmp cells[hex-1], -1    jcc B14 | fall B13
    B13 @8ca  1i jmp B6                  <-- the `continue`
    B14 @8cf  8i cmp sides[c], side      jcc B17 | fall B15
    B15 @8ea 26i ... SpellCastWorkChance jcc B17 | fall B16
    B16 @94a  7i ... jmp B19             <-- `return`
    B17 @962  1i jmp B6                  <-- natural end of the loop body

Our `&&`-chain with an inner `if (...) continue;` produced only ONE back-edge block
and put it after the side test, so B13/B14 swapped roles and their block sizes
differed (`!! ##` in `--blocks --diff --lite`):

    ours B12  7i mov..jne [jcc B17 | fall B13]      retail B12  7i mov..je  [jcc B14 | fall B13]
    ours B13  8i mov..je  [jcc B15 | fall B14]      retail B13  1i jmp..jmp [jmp B6^]
    ours B14  1i jmp..jmp [jmp B6^]                 retail B14  8i mov..jne [jcc B17 | fall B15]

**Reading rule.**

- A one-instruction `jmp <increment>` block whose predecessors are *conditions* is an
  explicit `continue;` statement. The condition that jumps INTO it is the true-branch
  of an `if (...) continue;`, so its `jcc` is the condition as written (not inverted):
  the peephole folded `jcc Lskip; jmp Lcont` into `jncc Lcont`.
- The LAST such block, reached by the tail conditions' false branches, is the natural
  end of the loop body; every enclosing `if` whose body ends there shares it.
- Therefore `B8 -> B17` and `B14 -> B17` are two *nested* `if`s, while `B10/B12 -> B13`
  is a separate `if (...) continue;` between them. An `&&` chain cannot produce two
  targets.
- Inside the `continue` guard, `jcc <next pair>` on the first operand and
  `jcc <continue>` on the second is the `(A && B) || (C && D)` short-circuit: A false
  jumps to C's evaluation, B true jumps straight to the statement, C false jumps past
  the whole guard, D true falls into it.

**Close.**

```cpp
for (corpse = m_hexCells[hex].m_deadOccupantCount - 1; corpse >= 0; --corpse) {
    if (m_hexCells[hex].m_deadOccupantSides[corpse] != COMBAT_SIDE_NONE) {
        if ((m_hexCells[hex].m_deadOccupantFrames[corpse] == ARMY_FACING_LEFT
             && m_hexCells[hex + 1].m_occupantSide != COMBAT_SIDE_NONE)
            || (m_hexCells[hex].m_deadOccupantFrames[corpse] == ARMY_FACING_RIGHT
                && m_hexCells[hex - 1].m_occupantSide != COMBAT_SIDE_NONE))
            continue;
        if (m_hexCells[hex].m_deadOccupantSides[corpse] == side) {
            target = &m_armies[IDX(...)][m_hexCells[hex].m_deadOccupantIndices[corpse]];
            if (target->SpellCastWorkChance(spell) > 0.0f)
                return m_hexCells[hex].m_deadOccupantIndices[corpse];
        }
    }
}
```

i.e. the De Morgan dual of the `&&` guard, with the guard's failure written as
`continue` and the rest nested. `combatManager::FindResurrectArmyIndex` 97.22% -> EXACT.

Complements [if-else-two-jmp-backedge](if-else-two-jmp-backedge.md), which reads the
opposite case (a forward `jmp` to a back-edge trampoline means if/else, not `continue`).

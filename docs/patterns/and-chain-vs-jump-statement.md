# and-chain-vs-jump-statement

**Negative result: there is no second lowering of `&&`.** A residual where retail
emits two jumps and we emit one is not a compiler shape we failed to reproduce —
it is a `break` / `continue` / `return` / `goto` **statement** that our
reconstruction folded into the condition as an extra `&&` term.

## Symptom

The final term of a condition:

```
ours                                retail
cmp edx, ecx                    |   cmp edx, ecx
jge <exit>                      |   jl  <next instruction after the jmp>
                                |   jmp <exit>
```

Retail's `jcc` hops over the `jmp`, and the `jmp` is the real target. Ours falls
through into the body.

## Rule (probe matrix, VC6 `/Od /MT /Gr /G5 /Ob1`)

21 shapes compiled through the real CL. **Every** plain `&&` condition emits one
conditional jump per term, all to the same skip label, body falling through —
regardless of guard form, what the final term compares, chain length, `if` vs
loop guard, or whether the body is a statement, a block, or has an `else`:

| varied | shapes | result |
| :-- | :-- | :-- |
| `if (A && B) stmt;` / `{ block }` / `else` | 3 | single jcc per term |
| final term vs local / member / call / bare int | 4 | single jcc per term |
| 2-term, 3-term, `\|\|`, `A && (B \|\| C)` | 4 | single jcc per term |
| `for` guard, `while` guard, `for` with block body | 3 | single jcc per term |
| negated early-return guard `if (!A \|\| !B) return;` | 2 | jcc per term **+ one jmp** |
| body is `continue` / `break` / `return 1` | 3 | jcc per term **+ one jmp** |

The trailing `jmp` appears exactly when the guarded body is a **jump statement**,
never from the condition itself:

```
P01  if (a > 0 && b > 0) Sink(1);            P16  ... { if (i > 0 && i < b) break; Sink(i); }
  cmp [a], 0 ; jle SKIP                        cmp [i], 0  ; jle AFTER
  cmp [b], 0 ; jle SKIP    <- falls through    cmp edx,[b] ; jge AFTER
                                               jmp EXIT    <- the break
                                             AFTER:
```

This is the discriminator already recorded in `docs/campaign-state.md` ("two
jumps = `if (...) return;`"); the matrix just proves nothing else produces it, so
when you see it, stop looking for a lowering.

## The three sightings, resolved

All three were the same reconstruction error, not one unexplained rule:

- **`combatManager::PushCombatPoint`** — our `if (speed <= 0 || distance <= speed) { body }`
  against retail's `jle A; jle A; jmp RET`. Retail is
  `if (speed > 0 && distance > speed) return;` followed by the body unindented.
  Applied; the guard shape now matches byte for byte.
- **`dropListWidget::DrawDropStuff`** — our
  `for (i = 1; i < m_visibleItemCount - 1 && m_topIndex + i < m_itemCount; i++)`
  against retail's `jl +5; jmp EXIT`. Retail keeps one term in the guard and
  makes the second a statement:

  ```c
  for (i = 1; i < m_visibleItemCount - 1; i++) {
      if (m_topIndex + i >= m_itemCount)
          break;
      ...
  ```

  Applied; the loop head matches.
- **`combatManager::FindResurrectArmyIndex`** — retail's `je BODY; jmp CONTINUE`
  on the last `||` pair is the same shape, but the obvious `continue` guard
  (`if (frames[corpse] == RIGHT && cells[hex-1].side != NONE) continue;` after a
  two-term `if`) measured WORSE, 3 differing bytes to 4, and was reverted. The
  rule says a jump statement is there; it does not say which term of the chain it
  replaces, and this site has two `||` pairs so the split point is not pinned.
  Read the `jmp` target and the surrounding block before choosing.

## How to read one

Take the `jcc`'s target. If it is the instruction immediately after the following
`jmp`, the source is `if (<negation of the tested condition>) <jump statement>;`
and the `jmp`'s target tells you which statement: the loop exit is `break`, the
loop increment is `continue`, the epilogue is `return`. If instead the `jcc`
targets the skip label directly, it is an ordinary `&&` term.

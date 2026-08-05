# Solving a long `/Od` slot chain: pair-linked stems, beam search, then bytes

**Trigger.** A function is byte-complete except for dozens of `-0xN(%ebp)`
displacements, `homm2 sema frames` shows the residual is a permutation of the
named-local layout, and `slot-permutation-suffix-solve` does not scale because
the chain is long (tens of locals, not fifteen). A greedy per-role name search
reports INFEASIBLE and it is tempting to write it off as "the retail names are
unrecoverable".

`army::SpecialAttack` (0x1a1d9, 0x165f, **50** function-scope locals) was parked
that way at 99.78% with a note saying no plausible vocabulary existed. It does.

## What makes a long chain different

`/Od` sorts locals by `(bucket(name), -declaration index)`, so retail's slot
order fixes a **non-decreasing bucket sequence**. With 50 roles over 16 buckets
the sequence is mostly ties — the constraint is weak — but two things break a
naive search:

1. **Coordinate PAIRS are linked and far apart.** `targetX` is slot 3 and
   `targetY` is slot 21; `sourceY` is slot 4 and `sourceX` is slot **50**. A
   shared stem fixes both buckets at once, so the search must commit at the
   first member and honour the second twenty positions later. A plain
   left-to-right DP over "current bucket floor" cannot express that.
2. **X/Y buckets are related by XOR, not by +1.** `bucket = (h & 0xF) ^ ((h >> 16) & 0xF)`,
   and appending `Y` instead of `X` adds 1 to `h`. If `h & 0xF` is even the two
   buckets differ by exactly one bit; if it is 7 they differ by `0xF`, i.e.
   `b2 = 15 - b1`. So a pair CAN span the frame — but only for particular stems,
   which is why `sourceY` (slot 4) and `sourceX` (slot 50) are reachable at all.

## The method that worked

`build/scratch-m4/sa_solve.py` + `sa_run.py`:

- **Compositional vocabulary.** Per role, cross qualifier words with spelling
  forms (`<stem>X`, `x<Stem>`, `<stem>PosX`, terse `sx`/`tx`/`bx`) and the
  prefixes this codebase actually uses (`i`/`n`/`b`/`p`/`f`, as in `giCurPlayer`,
  `gbTroopReload`, `gafAITurnCostResource`). Each candidate carries a
  naturalness penalty.
- **Collapse to the cheapest spelling per bucket cell** — the optimiser never
  wants two names in the same bucket, so thousands of spellings become ≤136 per
  pair and ≤16 per singleton. This alone turned a 10-minute run into 1.2 s.
- **Beam search with pending state**: a state is `(bucket floor, pending second-member
  buckets, chosen names)`. Prune any state whose pending bucket already sits below
  the floor — it can never be consumed.
- **Per-FLOOR quotas, not a global top-N.** This is the step that decides the
  run. A global cost-ordered beam fills up with cheap prefixes that have
  committed their pairs to narrow spreads, and the search dies at the first
  far-apart second member (`INFEASIBLE at position 21 (p2 B)`). Giving every
  floor value its own quota keeps the expensive-but-viable wide-spread
  commitments alive.

Feasibility is a separate, cheaper question than optimality: run the same walk
with **no cost at all**, filtered to `pen <= 2` candidates. That answers "does a
readable solution exist" in one second, and it is the answer that matters before
spending any time on weights.

## Numbers

| vocabulary | outcome |
| :-- | :-- |
| the obvious English role names | 23 bucket drops; longest non-decreasing subsequence 15/50 |
| ~12 curated candidates per role | INFEASIBLE (dies at `effectType`, position 12) |
| compositional, prefixes only | feasible but 6 stretched names, every X/Y pair broken |
| + terse coordinate pairs (`sx`/`tx`/`bx`) | **FEASIBLE at pen ≤ 2** — no stretched name needed |
| + targeted bare-word enrichment | 11 roles keep their natural name, 3 at pen 2, X/Y pairs intact |

## Then let the bytes finish the job

Fixing the frame exposed what the permutation had been hiding. With all 50 slots
finally identical, a raw byte compare (masking only bytes covered by a
relocation on either side) left **three** differing bytes, and both were real
source facts no name change could reach:

```
0x0961  target  e9 95 ..   jmp <body 0xefa>   |  ours  e9 f0 ..   jmp <body 0xf55>
0x10b4  target  8b 85 0c fc..  movl -0x3f4(%ebp),%eax  |  ours  8b 85 60 fc..  movl -0x3a0(%ebp),%eax
0x10ba  target  3b 85 60 fc..  cmpl -0x3a0(%ebp),%eax  |  ours  3b 85 0c fc..  cmpl -0x3f4(%ebp),%eax
```

- the mage/bolt arm's `jmp` lands **inside** the ammo-cart block, not past it:
  `m_monster.shots--` is common to both arms, not nested in the `else`;
- the splash-identity test loads the victim first: `splashTarget != pEnemy`.

Both landed the function EXACT. The lesson is ordering: **solve the frame first,
then re-diff**, because a slot permutation masks ordinary operand-order and
control-flow evidence across the whole body.

## Reproduce

```sh
python3 build/scratch-m4/sa_run.py 4 60000 0.0 1   # solve (MAXPEN quota lambda show)
python3 build/scratch-m4/sa_apply.py <map>         # rewrite + verify the frame model
```

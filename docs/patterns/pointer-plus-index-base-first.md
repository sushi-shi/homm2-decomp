# Base loaded FIRST in a flat map index -> `*(p + x + y*W)`, not `p[y*W + x]`

**Trigger.** Same five instructions, same registers modulo rotation, but the
pointer-global load sits on the *other* side of the index arithmetic. Retail
loads the base, folds one index term straight into it, and leaves the other
term for the addressing mode; ours computes the whole index first and loads
the base last.

`p[i + j]` parses as `*(p + (i + j))`: VC6 `/Od` evaluates the parenthesised
index subtree first, then loads `p`. `*(p + i + j)` parses as `*((p + i) + j)`:
the base becomes the accumulator of the first `+`, so it is loaded first and
`i` is added into it.

## The divergence

`game::SetupAdjacentMons` (RVA 0x5bc76), the `mapExtra` read-modify-write:

```
retail                                        ours (mapExtra[y * MAP_WIDTH + x])
--------------------------------------------  --------------------------------------------
8b 15 <mapExtra>   movl  mapExtra, %edx       8b 55 f8           movl  -0x8(%ebp), %edx   ; y
03 55 fc           addl  -0x4(%ebp), %edx     0f af 15 <W>       imull MAP_WIDTH, %edx
8b 45 f8           movl  -0x8(%ebp), %eax     03 55 fc           addl  -0x4(%ebp), %edx   ; + x
0f af 05 <W>       imull MAP_WIDTH, %eax      a1 <mapExtra>      movl  mapExtra, %eax
8a 0c 02           movb  (%edx,%eax), %cl     8a 0c 10           movb  (%eax,%edx), %cl
```

## What made it match

```cpp
*(mapExtra + x + y * MAP_WIDTH) |= IDX(MAP_EXTRA_ADJACENT_MONSTER);
```

`SetupAdjacentMons` 83.17% -> EXACT. Note the term order too: the operand the
base absorbs (`x` here) is the one written immediately after the pointer.

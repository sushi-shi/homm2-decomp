# Flat map index: `*(p + x + y*W)` vs `p[x + y*W]` (VC6 /Od)

**Trigger.** A byte map addressed with a computed 2-D index (`i8* gaiXxx` in
PHILAI). Two spellings that mean the same thing lower to two different
addressing shapes, and the difference is one instruction plus which term ends
up in the SIB index register.

- `p[A + B]` — the **whole index is computed first** (`B`, then `A`, then
  `add`), the base is loaded last and folded into the SIB.
- `*(p + A + B)` — **`p + A` is accumulated first**, then `B` is computed and
  folded into the SIB.

## The divergence (`philAI::GetTurnAIVars`, RVA 0x808a1)

```
ours  gaiEnemyHeroReachable[MAP_WIDTH * y4 + x8] = 1;
                                          retail
--------------------------------------    --------------------------------------
8b 55 b0  movl -0x50(%ebp), %edx  ; y      8b 15 ..  movl gai, %edx
0f af 15  imull MAP_WIDTH, %edx   ; y*W    03 55 d4  addl -0x2c(%ebp), %edx  ; +x
8b 45 bc  movl -0x44(%ebp), %eax  ; x      8b 45 cc  movl -0x34(%ebp), %eax  ; y
03 c2     addl %edx, %eax         ; x+y*W  0f af 05  imull MAP_WIDTH, %eax   ; y*W
8b 0d ..  movl gai, %ecx
c6 04 01 01  movb $0x1, (%ecx,%eax)        c6 04 02 01  movb $0x1, (%edx,%eax)
```

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

```cpp
extern signed char* gMap;  extern int gW;
void a1(int x, int y) { gMap[x + y * gW] = 1; }
void a2(int x, int y) { gMap[y * gW + x] = 1; }
void a3(int x, int y) { *(gMap + x + y * gW) = 1; }
void a4(int x, int y) { (gMap + x)[y * gW] = 1; }
void a5(int x, int y) { gMap[gW * y + x] = 1; }
void a6(int x, int y) { *(gMap + x + gW * y) = 1; }
```

| arm | shape |
| :-- | :-- |
| a1 `gMap[x + y*gW]` | `movl y; imull gW; movl x; addl; movl gMap; movb (%edx,%ecx)` |
| a2 `gMap[y*gW + x]` | same, one `addl` fused with the memory x |
| **a3 `*(gMap + x + y*gW)`** | **`movl gMap; addl x; movl y; imull gW; movb (%eax,%ecx)`** — retail |
| **a4 `(gMap + x)[y*gW]`** | byte-identical to a3 |
| a5 `gMap[gW*y + x]` | `movl gW; imull y; addl x; movl gMap; movb (%ecx,%eax)` |
| a6 `*(gMap + x + gW*y)` | a3 shape but `movl gMap; imull y` (product operands swapped) |

So the *pointer* form is what puts `base + firstTerm` in the accumulator; the
*subscript* form always resolves the index completely before touching the base.
Which factor leads the product is a separate axis: `y * W` gives `movl y; imull
W`, `W * y` gives `movl W; imull y`.

## What made it match

```cpp
    *(gaiEnemyHeroReachable + x3 + y4 * MAP_WIDTH) = 1;
    if (mineValue17 < *(gaiTurnValueOfMine + x3 + y4 * MAP_WIDTH))
        *(gaiTurnValueOfMine + x3 + y4 * MAP_WIDTH) = static_cast<i8>(mineValue17);
```

and in `philAI::EvaluateMineEvent` (0x8b4c3)

```cpp
        * *(gaiTurnValueOfMine + x + y * MAP_WIDTH)
```

All three sites closed with the pointer form; the subscript form is one
instruction longer and puts the wrong term in the SIB index.

**Related.** When the pointer side is an *inlined accessor call* the rule
flips again and the SUBSCRIPT is evaluated first — see
[inline-call-operand-index-first](inline-call-operand-index-first.md), which
`gpSearchArray->GetRow(x, 1)[MAP_WIDTH * y]` needs in the same two functions.

# /Od 2D cell access form + the block-jmp-to-next wall

**tags:** `topic:od` `cpp:array` `topic:wall` `toolchain:vc42`

## cell-access form (steerable)
Retail accesses a row-major grid element `cells[width*y + x]` as a **base pointer
+ scaled index in the addressing mode**, NOT a combined index:
```
movl 0x8(this),eax ; imull y,eax ; leal(eax,eax,2),eax ; shll $2,eax  ; width*y*12
addl (this),eax                                                       ; + cells (base)
movl x,ecx ; leal(ecx,ecx,2),ecx                                      ; x*3
movw 0xa(eax,ecx,4),dx                                                ; + x*12 + field
```
The natural `cells[width*y + x].extra` instead **combines** the index
(`imul; add x; lea*3`) then scales once — a mismatch. Spell it as a base pointer:
```cpp
(cells + width * y)[x].extra        // width*y folds into the base, x into addressing
(cells + width * y) + x             // for &cells[width*y+x]
```
This made the four cell reads in GetNewCellExtra* match.

## the block-jmp wall (topic:wall — not yet steered)
Retail GetNewCellExtraOverlay/Object carry /Od **jmp-to-next-instruction** filler
that a literal translation does not emit: a *leading* `jmp` straight after the
prologue (target+0x11), plus paired `jmp;jmp` at block ends and after returning
`if` blocks. `if`, `if/else`, and `while((...)==0){...return;}` were all tried;
none reproduced the leading jmp, and the count stays ~10 instructions short
(~86% vs 100%). Likely a specific source construct (an inline accessor that VC4.2
/Od does *not* inline but still brackets, or a loop/scope shape) drives these.
Combined with the 4-local hash-slot order (cur/idx/ni/cp), these two functions
are parked at a plateau. Revisit once the MSVC 4.2 identifier hash is reversed
and the jmp-emitting construct is identified.

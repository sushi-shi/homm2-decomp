# /Od 2D cell access and inline-continuation placement

**tags:** `topic:od` `cpp:array` `toolchain:vc42` `cpp:inline`

## cell-access form (steerable)
Retail accesses a row-major grid element `cells[width*y + x]` as a **base pointer
+ scaled index in the addressing mode**, NOT a combined index:
```
movl 0x8(this),eax ; imull y,eax ; leal(eax,eax,2),eax ; shll $2,eax  ; width*y*12
addl (this),eax                                                       ; + cells (base)
movl x,ecx ; leal(ecx,ecx,2),ecx                                      ; x*3
movw 0xa(eax,ecx,4),dx                                                ; + x*12 + field
```
Getting this exact addressing needs the row base to materialize as a **pointer
value** before `[x]`. A *raw* `(cells + width*y)[x].extra` REASSOCIATES to
`width*y*12 + x*12` then `+cells` -> wrong `0xa(%eax,%ecx)` (no scale). An **inline
row accessor** keeps the boundary and defers `[x]`:
```cpp
mapCell *Row(int y) { return cells + width * y; }   // then Row(y)[x].extra
```
gives `0xa(%eax,%ecx,4)` — the retail form. See `inline-accessors.md`.

## Inline-continuation cause recovered
The `jmp $+0` filler was **inline expansion (`/Ob1`)**, not a mysterious /Od quirk:
each inlined accessor call emits a per-site continuation `jmp $+0`. Reconstructing
the `Row`/`Extra` inline accessors + `/Ob1` reproduces both the addressing and the
jumps, taking GetNewCellExtra* from the ~86–91% plateau to ~97%. Full writeup:
**`inline-accessors.md`**.

## Residual continuation placement
Retail GetNewCellExtraOverlay/Object carry /Od **jmp-to-next-instruction** filler
that a literal translation does not emit: a *leading* `jmp` straight after the
prologue (target+0x11), plus paired `jmp;jmp` at block ends and after returning
`if` blocks. `if`, `if/else`, and `while((...)==0){...return;}` were all tried;
none reproduced the leading jmp, and the count stays ~10 instructions short
(~86% vs 100%). Likely a specific source construct (an inline accessor that VC4.2
/Od does *not* inline but still brackets, or a loop/scope shape) drives these.
The remaining residual is the exact `jmp $+0` placement (which accessor decomposition
the original used) plus the 4-local hash-slot order (cur/idx/ni/cp) — ordinary
matcher polish, no longer a wall.

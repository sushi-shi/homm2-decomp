# A list walk's `return NULL` position names the null test's owner

## Trigger

A singly-linked-list search: walk `head`, compare one field, return the node or
`NULL`. Two spellings produce the **same instruction multiset and the same
byte count**, so size, frame, slot displacements and relocations all agree and
only the *block order* differs:

```c
// A: the null test is the loop CONDITION
for (T* p = head; p != NULL; p = p->next)
    if (p->key == want) return p;
return NULL;

// B: the null test is a GUARD at the top of an infinite loop's body
T* p = head;
for (;;) {
    if (p == NULL) return NULL;
    if (p->key == want) return p;
    p = p->next;
}
```

The discriminator is **where `xor %eax,%eax` sits**:

- **A** parks the loop-exhausted `return NULL` as the LAST block before the
  epilogue; the loop test's `je` reaches it, and it *falls through* into
  `mov %ebp,%esp` with no `jmp` of its own.
- **B** puts `xor %eax,%eax` at the TOP of the loop body, followed by a real
  `jmp <epilogue>`, because the guard is an ordinary early return.

The step block moves with it: **A** advances the cursor in a separate block
reached by the entry `jmp` (see `for-increment-block-vs-while-tail.md`), **B**
advances it at the bottom of the body where the source wrote it.

## Retail vs ours

`?FindAudiereSample@@YIPAUAudiereSampleNode@@PAVsample@@@Z` (RVA 0xcc8b0,
0x3a bytes). Both sides are 0x3a bytes with identical frames (`node` at
`-0x4`, the `__fastcall` spill at `-0x8`) and one identical DIR32.

```
retail (A)                                     ours (B: for(;;) + guard)
---------------------------------------------  ---------------------------------------------
55              pushl  %ebp                    55              pushl  %ebp
8b ec           movl   %esp, %ebp              8b ec           movl   %esp, %ebp
83 ec 08        subl   $0x8, %esp              83 ec 08        subl   $0x8, %esp
89 4d f8        movl   %ecx, -0x8(%ebp)        89 4d f8        movl   %ecx, -0x8(%ebp)
a1 <gAudiereSampleList>  movl 0x0, %eax        a1 <gAudiereSampleList>  movl 0x0, %eax
89 45 fc        movl   %eax, -0x4(%ebp)        89 45 fc        movl   %eax, -0x4(%ebp)
eb 09           jmp    <test>          <---- entry jmp over the step
8b 4d fc        movl   -0x4(%ebp), %ecx  <step>
8b 51 08        movl   0x8(%ecx), %edx     ; p = p->next
89 55 fc        movl   %edx, -0x4(%ebp)
83 7d fc 00     cmpl   $0x0, -0x4(%ebp)  <test>   83 7d fc 00  cmpl  $0x0, -0x4(%ebp)  <top>
74 12           je     <return NULL>              75 04        jne   <compare>
                                                  33 c0        xorl  %eax, %eax     ; return NULL
                                                  eb 1b        jmp   <epilogue>        INSIDE
8b 45 fc        movl   -0x4(%ebp), %eax           8b 4d fc     movl  -0x4(%ebp), %ecx
8b 48 04        movl   0x4(%eax), %ecx            8b 51 04     movl  0x4(%ecx), %edx
3b 4d f8        cmpl   -0x8(%ebp), %ecx           3b 55 f8     cmpl  -0x8(%ebp), %edx
75 05           jne    <body end>                 75 05        jne   <step>
8b 45 fc        movl   -0x4(%ebp), %eax           8b 45 fc     movl  -0x4(%ebp), %eax
eb 04           jmp    <epilogue>                 eb 0b        jmp   <epilogue>
eb df           jmp    <step>          <body end> 8b 45 fc     movl  -0x4(%ebp), %eax  <step>
33 c0           xorl   %eax, %eax      <return NULL, falls through>
                                                  8b 48 08     movl  0x8(%eax), %ecx
                                                  89 4d fc     movl  %ecx, -0x4(%ebp)
                                                  eb db        jmp   <top>
8b e5           movl   %ebp, %esp                 8b e5        movl  %ebp, %esp
5d              popl   %ebp                       5d           popl  %ebp
c3              retl                              c3           retl
```

Note the load registers also swap (`0x4(%eax),%ecx` vs `0x4(%ecx),%edx`): the
cursor read that feeds the compare is the second `mov` in A and the first in B,
so `/Od`'s round-robin scratch allocation lands differently. That is a
consequence of the block order, not a separate lever.

## What made it match

```cpp
AudiereSampleNode* FindAudiereSample(class sample* sampleResource) {
    for (AudiereSampleNode* node = gAudiereSampleList; node != NULL; node = node->next) {
        if (node->sampleResource == sampleResource)
            return node;
    }
    return NULL;
}
```

`FindAudiereSample` -> EXACT (4.13% -> 100%), closing BASE/AudiereEffects at
17/17. The 4.13% is what a pure block permutation of an otherwise byte-correct
function scores: objdiff aligns almost nothing, so a near-zero on a short
function is a *structure* reading, never evidence that the body is wrong.

Related: `for-increment-block-vs-while-tail.md` (the entry `jmp` over the step
block), `while1-literal-test.md` (`while (1)` vs `for (;;)` head test).

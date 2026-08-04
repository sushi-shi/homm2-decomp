# `arr[i] = <byte>` — an `int`-typed RHS adds `and $0xff` AND evaluates before the index

**Trigger.** A one-byte store into an array whose retail form is
`mov r32,[src]; and r32,0xff; mov r32b,[index]; mov [arr+r32b], r8` while ours is
`mov r32,[index]; mov r8,[src]; mov [arr+r32], r8`. Same three values, opposite
order, and retail carries a zero-extension our side does not. The RHS type decides
both: a `u8` lvalue assigned to `char` is a plain byte copy and the *index* is
evaluated first; an `int`-typed RHS is materialised in a register first, so the
subexpression order flips.

Measured on `PopNetBox` (RVA 0x6c0a6), the chat input character store.

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

```cpp
union K { int keyCode; u8 keyByte; };
struct M { K k; };  M g_msg;  char g_buf[160];  int g_len;

void s_bytecast(void) { g_buf[g_len] = (char)g_msg.k.keyByte; }        // ours
void s_intmask(void)  { g_buf[g_len] = (char)(g_msg.k.keyCode & 0xff); } // retail
```

```
s_bytecast  (ours)                            s_intmask  (retail)
--------------------------------------------- ---------------------------------------------
a1 00 00 00 00  movl 0x0, %eax   ; g_len      a1 00 00 00 00  movl 0x0, %eax   ; keyCode
8a 0d 00 00 00 00  movb 0x0, %cl ; keyByte    25 ff 00 00 00  andl $0xff, %eax
88 88 00 00 00 00  movb %cl, (%eax)           8b 0d 00 00 00 00  movl 0x0, %ecx ; g_len
                                              88 81 00 00 00 00  movb %al, (%ecx)
```

Note the operand roles swap: in the `(char)u8` form `%eax` holds the *index*, in the
`(char)(int & 0xff)` form `%eax` holds the *value*. That is the whole tell — the
index/value register assignment is decided by which side is "complex".

## What made it match

```cpp
                                inputText_c[inputLength_a] =
                                    static_cast<char>(event_a.payload.keyboard.keyCode & 0xff);
```

(previously `static_cast<char>(event_a.payload.keyboard.keyCode)`, then
`static_cast<char>(...keyByte)`; neither emits the `and`.) Measured through one
`homm2.permute.batch_source_variants` axis of five spellings: only the explicit
`& 0xff` on the `int` union member reproduces retail's order and zero-extension —
98.84% -> 99.05% and 81/81 blocks with no divergence, then EXACT after the frame
solve.

The same union read in a *comparison* (`keyByte < 0x20`) lowers identically for both
spellings, so only the store site discriminates them.

# `field = (a == b);` on a byte field — `sete`, not two `movb` stores

**Trigger.** Retail writes a one-byte field with
`xor r32,r32 / cmp .. / sete r8 / mov [field], r8` (4 instructions, no branch) where
ours has `cmp .. / jne / movb $1 / jmp / movb $0` (5 instructions, two blocks). The
retail shape is a *value* assignment of the relational; the branching shape is an
`if/else`. This is the byte-field sibling of
[bool-expression-return-temp](bool-expression-return-temp.md), which materialises a
**dword** temp for a `return` — a store straight into a `char`/`i8` field skips the
temp entirely and uses `setcc`.

Measured on `ReceiveRemotePlayerExit` (RVA 0x6f115), `exitInfo.continueGame`.

## The probe (VC6 SP5, `/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX`)

```cpp
struct S { char cont; };  S g_s;  int g_dlg;

void b_ifelse(void) { if (g_dlg == 5) g_s.cont = 1; else g_s.cont = 0; }  // ours
void b_assign(void) { g_s.cont = g_dlg == 5; }                            // retail
```

```
b_ifelse  (ours)                              b_assign  (retail)
--------------------------------------------- ---------------------------------------------
83 3d .. 05  cmpl $0x5, 0x0                   33 c0        xorl %eax, %eax
75 09        jne  L1                          83 3d .. 05  cmpl $0x5, 0x0
c6 05 .. 01  movb $0x1, 0x0                   0f 94 c0     sete %al
eb 07        jmp  L2                          a2 00 00 00 00  movb %al, 0x0
c6 05 .. 00  movb $0x0, 0x0
```

The `xor` that clears the destination is emitted **before** the `cmp`, so a diff that
starts with an ours-only `cmp` one row early is the same finding.

## What made it match

```cpp
        NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
        exitInfo.continueGame = gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE;
```

(previously `if (... == NORMAL_DIALOG_BUTTON_FIVE) exitInfo.continueGame = 1; else
exitInfo.continueGame = 0;`.) The function went from 201 to 200 emitted instructions
and, with the frame solve, to EXACT.

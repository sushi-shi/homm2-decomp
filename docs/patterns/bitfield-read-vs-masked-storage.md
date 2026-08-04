# bitfield-read-vs-masked-storage

**Trigger.** A narrow field of a union'd storage word is read. Two spellings of
the same value lower differently, and the difference is *where the zero
extension happens*:

- `storage & MASK` (the union's whole-word member, masked by hand) →
  **zero-extend first**: `xor reg,reg; mov reg16,[mem]; and reg32,MASK`.
- `bitfieldMember` (the `u16 x : N` member) → **zero-extend last**:
  `mov reg16,[mem]; [shr reg16,SHIFT;] and reg16,MASK; and reg32,0xffff`.

The instruction count is the same, so the residual reads as a pure register
shuffle until the operand widths are compared.

Measured on `advManager::VWCompleteDraw` (RVA 0xae4ee), VC6 SP5 `/Od /Ob1`.

## Bytes - the one-bit field at `mapCell+4` bit 0

```
ours   cell0->m_objectData & 1              retail  cell0->m_objectLayerBit0
--------------------------------------      --------------------------------------
33 d2              xorl  %edx, %edx
66 8b 51 04        movw  0x4(%ecx), %dx     66 8b 48 04        movw  0x4(%eax), %cx
83 e2 01           andl  $0x1, %edx         66 83 e1 01        andw  $0x1, %cx
                                            81 e1 ff ff 00 00  andl  $0xffff, %ecx
85 d2              testl %edx, %edx         85 c9              testl %ecx, %ecx
```

## Bytes - the 13-bit field at the same word (`m_objectMetadata : 13`)

The shifted form is the same rule with a `shr` in front; the trailing
`and reg32,0xffff` is again the tell that the value came out of the bitfield
member and not out of a hand-masked `u16`:

```
66 8b 4a 04        movw  0x4(%edx), %cx
66 c1 e9 03        shrw  $0x3, %cx
66 81 e1 ff 1f     andw  $0x1fff, %cx
81 e1 ff ff 00 00  andl  $0xffff, %ecx
```

## What made it match

Use the declared bitfield member, not a mask of the union's storage member:

```cpp
if (cell->m_objectLayerBit0 && cell->m_objectIndex != WORLD_NO_SPRITE ...)
```

`advManager::VWCompleteDraw` 90.12% -> EXACT (two sites; the same TU also has
two `m_objectMetadata` reads that were already spelled as the bitfield and
already matched, which is what pinned the rule's direction).

## Reading it backwards

A retail `and reg32,0xffff` (or `and reg32,0xff`) *after* a narrow mask says
the source names a bitfield. A retail `xor reg,reg` *before* the load says the
source masks the storage member by hand. Both are members of the same union in
`mapCell`, so semantics do not decide it - only the bytes do.

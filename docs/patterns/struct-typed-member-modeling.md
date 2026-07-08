# Model a member's real struct/array type, not a raw buffer

**Tags:** o2, od, structs, layout, struct-return
**Symptom:** a function that copies a fixed-size record out of a member — or **returns a struct by
value** — plateaus, because the member is declared as a raw pad (`char _pad[..]`) or a bare `int`.
Accessing it then needs pointer-arithmetic + a hand-rolled dword copy loop that does NOT line up with
the retail struct-move, so displacements drift for the whole body.

## The tell

Ghidra shows a **7-dword (or N-dword) copy loop** — that is a struct `memcpy`/assignment the optimizer
open-coded — and/or `*(T *)(this + i*sizeof(T) + off)` indexing into a member:

```c
piVar6 = local_1c;                          // <- local struct
for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {   // 7 dwords = one tag_message (0x1c)
    *piVar6 = *(int *)piVar5;               // copy from   this + head*0x1c + 0x36
    piVar5 = piVar5 + 4; piVar6 = piVar6 + 1;
}
```

## Fix

Model the member as its real struct-typed array `T member[N]` (N from the offset span ÷ `sizeof(T)`),
not a byte pad. Then a plain struct copy + by-value return lowers to the exact retail move:

```cpp
// inputManager.h:  tag_message field_0x36[64];   // was: int field_0x36; char _pad_0x3a[0x6fc];
tag_message inputManager::GetEvent(void) {
    tag_message local_1c;
    local_1c = field_0x36[head];     // <- struct copy == the 7-dword loop, byte-exact
    ...
    return local_1c;                 // <- by-value return uses the hidden return ptr the retail expects
}
```

### Real asm (GetEvent event copy, before/after)

```
; before  (int field_0x36 + raw pad) — hand pointer math, wrong base
lea  esi, [ecx + eax*4]              ; ad-hoc address of this+head*..; scale wrong
...                                  ; dword loop over a mis-typed buffer
; after   (tag_message field_0x36[64]) — byte-exact
lea  eax, [eax + eax*2]              ; head * 3
lea  esi, [ecx + eax*4 + 36h]        ; &field_0x36[head]  (0x1c stride == 7*4)
mov  ecx, 7
rep  movsd                           ; struct copy -> return slot
```

## When it applies

Any member that is really an array of records or a struct: an **event/ring buffer**
(`tag_message[64]`), a **directory** (`aggEntry {id,offset,size}[]` — see [[struct-typed-member-modeling]]
sibling use in resourceManager), a **linked-list next/prev** (`widget *field_0xc`, not `int`). Modeling
the real type also removes the C-casts the raw form forced — which the decomp doctrine requires anyway.

## Result
inputManager `GetEvent` / `PeekEvent` / `Open` → **byte-exact (100%)**. Same family as
[[codeview-array-decayed-to-pointer]] (model the array, not the decayed pointer); the `aggEntry`
directory model took resourceManager `PointToFile`/`GetFileSize` to ~91%.

# `return (A && B) || (C && D);` — the dword 0/1 temp, false arm first

**Trigger.** A predicate function whose retail frame reserves one 4-byte slot
that only ever receives the literals `0` and `1`, and whose epilogue returns
`mov eax, <that slot>` instead of `mov eax, 1` / `xor eax, eax`. The `0` store
comes FIRST and jumps over the `1` store.

## The divergence (`IsCycleColor`, RVA 0xc0050)

```
ours  if ((c >= 0xd6 && c <= 0xdd) || (c >= 0xe7 && c <= 0xed)) return 1;
      return 0;
                                            retail
------------------------------------------  ------------------------------------------
55           push  ebp                       55           push  ebp
8b ec        mov   ebp, esp                  8b ec        mov   ebp, esp
51           push  ecx                       83 ec 08     sub   esp, 0x8
89 4d fc     mov   [ebp-4], ecx              89 4d fc     mov   [ebp-4], ecx
81 7d fc d6..cmp   [ebp-4], 0xd6             81 7d fc d6..cmp   [ebp-4], 0xd6
7c 09        jl    L1                        7c 09        jl    L1
81 7d fc dd..cmp   [ebp-4], 0xdd             81 7d fc dd..cmp   [ebp-4], 0xdd
7e 12        jle   TRUE                      7e 1b        jle   TRUE
L1: 81 7d fc e7.. cmp [ebp-4], 0xe7          L1: 81 7d fc e7.. cmp [ebp-4], 0xe7
7c 10        jl    FALSE                     7c 09        jl    FALSE
81 7d fc ed..cmp   [ebp-4], 0xed             81 7d fc ed..cmp   [ebp-4], 0xed
7f 07        jg    FALSE                     7e 09        jle   TRUE
TRUE: b8 01 00 00 00  mov eax, 1             FALSE: c7 45 f8 00 00 00 00  mov [ebp-8], 0
eb 02        jmp   END                       eb 07        jmp   END
FALSE: 33 c0 xor   eax, eax                  TRUE:  c7 45 f8 01 00 00 00  mov [ebp-8], 1
END: 8b e5    mov   esp, ebp                 END: 8b 45 f8  mov eax, [ebp-8]
```

Two tells beyond the temp: retail's frame is `sub esp,8` (two slots: the param
spill plus the temp) where the `return 1/0` form needs only `push ecx`, and the
last relational is `jle TRUE` (jump to the true arm) rather than `jg FALSE` —
the `||` chain's final term jumps to the TRUE block and the FALSE block is the
fall-through, so the `0` store is emitted first.

## What made it match

```cpp
VA(0x004c0050, 0x44)
i32 IsCycleColor(i32 color) {
    return (color >= CYCLE_RANGE_ONE_FIRST && color <= CYCLE_RANGE_ONE_LAST)
        || (color >= CYCLE_RANGE_TWO_FIRST && color <= CYCLE_RANGE_TWO_LAST);
}
```

EXACT. VC6 materialises the `bool` value of a short-circuit expression into a
**dword** frame temp even when the function returns `int`, so a named
`i32 result; if (...) result = 1; else result = 0; return result;` is NOT the
same shape — that form emits the `1` store first (the `then` block follows the
condition) and would need the arms swapped.

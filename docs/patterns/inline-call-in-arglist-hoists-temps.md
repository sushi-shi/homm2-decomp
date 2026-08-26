# An inline call in an argument list hoists sibling conversion temporaries

**Symptom.** A constructor call has the correct values but a narrowing or
post-increment temporary is materialized before the trailing argument pushes.
The local expression is not necessarily the cause: another argument in the
same call may have crossed an `/Ob1` inline boundary.

Measured in `NormalDialog` (RVA `0x6f5cb`) at the `"or"` `textWidget`
constructor. With `NormalDialogCenterOffset(windowWidth)` in the first
argument, VC6 hoisted the `textWidgetId++` conversion before evaluating that
inline call. Retail evaluates the ordinary division first:

```text
inline-call spelling                       retail / exact spelling
mov  ecx, [textWidgetId]                   mov  eax, [windowWidth]
add  ecx, 1                               cdq
mov  [textWidgetId], ecx                   sub  eax, edx
call NormalDialogCenterOffset              sar  eax, 1
push eax                                   sub  eax, 0Ah
                                            push eax
                                            mov  ecx, [textWidgetId]
                                            add  ecx, 1
```

**Close.** Use the source-level operation evidenced at that argument:

```cpp
windowWidth / NORMAL_DIALOG_CENTER_PART_COUNT
    - NORMAL_DIALOG_OR_TEXT_CENTER_X_OFFSET
```

Do not move the sibling temporary manually. Removing the unrelated inline call
restores retail evaluation order for the whole argument list.


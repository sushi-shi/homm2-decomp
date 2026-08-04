# hiword-macro-double-mask

**Symptom.** A high-word extraction from a Win32 `LPARAM`. Ours emits
`shr $0x10` and ONE `and $0xffff`; retail emits `shr $0x10` and **two**
consecutive `and $0xffff`, or `shr $0x10; and $0xffff` followed by `movswl`
where ours has `shr $0x10; movswl` with no mask at all.

    ours: static_cast<u16>(static_cast<u32l>(l) >> 16)   retail: HIWORD(l)
    ------------------------------------------------    ---------------------------------
    8b 55 0c        mov  edx, [ebp+0xc]                 8b 55 0c        mov  edx, [ebp+0xc]
    c1 ea 10        shr  edx, 0x10                      c1 ea 10        shr  edx, 0x10
    81 e2 ff ff 00 00  and edx, 0xffff                  81 e2 ff ff 00 00  and edx, 0xffff
    83 e2 ff        and  edx, 0xff                      81 e2 ff ff 00 00  and edx, 0xffff
                                                        83 e2 ff        and  edx, 0xff

    ours: static_cast<i16>(static_cast<u32l>(l) >> 16)  retail: (i16)HIWORD(l)
    ------------------------------------------------    ---------------------------------
    8b 55 0c        mov  edx, [ebp+0xc]                 8b 55 0c        mov  edx, [ebp+0xc]
    c1 ea 10        shr  edx, 0x10                      c1 ea 10        shr  edx, 0x10
    0f bf c2        movsx eax, dx                       81 e2 ff ff 00 00  and edx, 0xffff
                                                        0f bf c2        movsx eax, dx

(`KeyboardMessageHandler` 0xbbf40 and `MouseMessageHandler` 0xbc390.)

## Why

VC6's `WINDEF.H` spells the macro

    #define HIWORD(l)  ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))

so the `& 0xFFFF` is written in the source AND the `(WORD)` conversion is
materialised separately: at /Od the mask is emitted once for the `& 0xFFFF`
and once more when the `WORD` value is widened back to `int`. A hand-written
`static_cast<u16>(x >> 16)` produces only the single conversion mask, and
`static_cast<i16>(x >> 16)` produces none (the `movsx` reads `dx` anyway).

**A doubled `and $0xffff` after `shr $0x10` is the `HIWORD` macro, not a cast.**
`LOWORD(l)` is `((WORD)(l))` — no shift and no mask, so a bare
`movswl 0xc(%ebp), %eax` is `(i16)lParam`, not `(i16)LOWORD(lParam)`.

## Close

    event->payload.keyboard.keyCode = HIWORD(messageData) & IDX(SCAN_CODE_MASK);
    event->payload.mouse.y =
        (static_cast<i16>(HIWORD(messageData)) * MOUSE_SCREEN_HEIGHT)
        / iMainWinScreenHeight;

Measured: both handlers reached EXACT with the macro; every `static_cast`
spelling tried was 6 bytes short per site.

# `rep movsl` after a struct-returning call: assignment, not initialisation

**Trigger.** A struct-returning call whose result lands in a *named* local. If retail
copies the returned aggregate with `rep movsl` out of a hidden temp, the local was
declared first and assigned afterwards; if it writes the sret buffer straight into the
local's slot, the local was *initialised* by the call and VC6 elided the copy.

Measured on `advManager::DoHeroKnob` (RVA 0x0e6be) and its twin `DoTownKnob`
(0x0e8a5). `inputManager::GetEvent()` returns a 28-byte `tag_message` by value.

## The divergence

Retail (delinked `SOURCE/ADVMGR.c.obj`) vs ours before the fix:

```
retail                                     ours
------------------------------------------ ------------------------------------------
8d 55 a8   lea    edx, [ebp-0x58]          8d 55 dc   lea    edx, [ebp-0x24]
52         push   edx                      52         push   edx
8b 0d ..   mov    ecx, gpInputManager      8b 0d ..   mov    ecx, gpInputManager
e8 ..      call   GetEvent                 e8 ..      call   GetEvent
8b f0      mov    esi, eax                 (nothing)
b9 07..    mov    ecx, 7                   (nothing)
8d 7d cc   lea    edi, [ebp-0x34]          (nothing)
f3 a5      rep    movsl                    (nothing)
83 7d cc 10  cmp  [ebp-0x34], 0x10         83 7d dc 10  cmp  [ebp-0x24], 0x10
```

Retail passes a *hidden* sret buffer (`ebp-0x58`), then copies 7 dwords into the named
local at `ebp-0x34`. We passed the named local itself as the sret buffer, so the copy
disappeared. The frame shows it too: retail `sub esp,0x80`, ours `sub esp,0x64` -
exactly 0x1c (28) bytes less, one whole `tag_message` short.

## What made it match

Split the declaration from the call:

```cpp
    tag_message message = gpInputManager->GetEvent();   // ours: sret straight into `message`
```
```cpp
    tag_message message;                                // retail
    message = gpInputManager->GetEvent();
```

Both `DoHeroKnob` and `DoTownKnob` went 96.70% -> EXACT with that one edit plus the
`od_slots` rename it unblocked. The second `GetEvent()` in the same function (inside the
loop) is already an assignment and already emitted `rep movsl` on both sides - that is
the control that pins the reading.

## Reverse direction

A `rep movsl` that we emit and retail does not means the opposite: retail *initialised*
the local at its declaration. Same fingerprint, mirrored.

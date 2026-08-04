# switch-goto-tail-stub-island

**Symptom.** A `switch` whose cases all reach the same shared tail. Ours gives
every case one **short** jump straight to the tail; retail gives every case a
**near** jump into a run of 5-byte `jmp`s parked between the last `return` and
the epilogue, each of which jumps to the tail. The stubs appear in **reverse
source order** (case 1's stub is the LAST one), and the switch's range check
(`ja`) does NOT point at the tail.

    ours (case bodies end with `break;`)      retail (case bodies end with `goto tail;`)
    ---------------------------------------  ------------------------------------------
    ; case WM_MOUSEMOVE                      ; case WM_MOUSEMOVE
    8b 4d f8     mov  ecx, [ebp-8]           8b 4d f8     mov  ecx, [ebp-8]
    c7 01 04..   mov  dword [ecx], 4         c7 01 04..   mov  dword [ecx], 4
    eb 4b        jmp  <tail>          <-- 2  e9 79 02 00 00  jmp <stub6>       <-- 5
    ...                                      ...
    ; range check                            ; range check
    0f 87 ..     ja   <default stub>         0f 87 7c 01 00 00  ja <after tail>
    ...                                      ...
    ; function tail                          ; function tail
    8b e5        mov  esp, ebp               eb 1e        jmp <epilogue>
    5d           pop  ebp                    e9 39 fe ff ff  jmp <tail>   ; stub1
    c2 08 00     ret  8                      e9 34 fe ff ff  jmp <tail>   ; stub2
                                             e9 2f fe ff ff  jmp <tail>   ; stub3
                                             e9 2a fe ff ff  jmp <tail>   ; stub4
                                             e9 25 fe ff ff  jmp <tail>   ; stub5
                                             e9 20 fe ff ff  jmp <tail>   ; stub6
                                             8b e5        mov  esp, ebp
                                             5d           pop  ebp
                                             c2 08 00     ret  8

(`MouseMessageHandler` 0xbc390. Six stubs at +0x343..+0x35c, all targeting
+0x181 — the coordinate-scaling code that the last case falls into.)

## The rule

VC6 /Od gives a source `goto` its own label id and, when several `goto`s
target one label, emits **one `jmp` stub per goto** at the end of the function
body (before the epilogue), in LIFO order of encounter. A `break` reuses the
switch's own exit label and jumps direct — one jump, short when it fits.

So **count the stubs: that is the number of `goto`s.** And the `ja` of the
range check is the switch's real end, which here sits *after* the shared tail —
proof that the tail lives INSIDE the last case rather than after the switch.

## Close

    switch (message) {
        case WM_MOUSEMOVE:
            event->type = MESSAGE_MOUSE_MOVE;
            goto mouseCoordinates;          /* x6, one stub each */
        ...
        case WM_RBUTTONUP:
            event->type = MESSAGE_RIGHT_BUTTON_UP;
            captureReleased = ReleaseCapture();
            if (captureReleased == 0)
                LogStr(gInputManagerText.rightReleaseCaptureFailure);
        mouseCoordinates:
            <shared tail>
            break;
        default:
            break;                          /* `ja` lands here */
    }

Measured: `break;` in all seven cases 899/899 bytes but no island and a `ja`
into a `jmp` thunk; the `goto` form above reproduces the island and the `ja`
target byte-for-byte. `MouseMessageHandler` 83.24% -> EXACT.

See also [goto-label-jump-stub](goto-label-jump-stub.md), the single-goto form.

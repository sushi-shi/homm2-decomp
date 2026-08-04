# and-guard-join-is-loop-back-edge

**Symptom.** An `&&`-guard inside a loop. Ours sends both failure `jcc`s to the
next statement after the guard (the `do`/`while` condition test); retail sends
them to a lone 5-byte `jmp <loop top>` island parked at the very end of the
enclosing block. The `switch` inside the guard still jumps to the guard's own
end, so the island is NOT the switch's break target.

    ours: do { ... } while (!playerInfoOk || !mapHeaderOk);
    ---------------------------------------------------------------
      25f: 83 7d d8 00        cmpl  $0x0, -0x28(%ebp)   ; remoteBuffer
      263: 0f 84 9d 00 00 00  je    fn+0x306            ; -> the while-condition
      269: 8b 45 d8           movl  -0x28(%ebp), %eax
      26c: 0f be 48 05        movsbl 0x5(%eax), %ecx
      270: 83 f9 02           cmpl  $0x2, %ecx
      273: 0f 85 8d 00 00 00  jne   fn+0x306            ; -> the while-condition
      ...  switch ...
      297: eb 6d              jmp   fn+0x306            ; switch default
      306: 83 7d d0 00        cmpl  $0x0, -0x30(%ebp)
      30a: 0f 84 40 ff ff ff  je    fn+0x250            ; back edge, direct
      310: 83 7d d4 00        cmpl  $0x0, -0x2c(%ebp)
      314: 0f 84 36 ff ff ff  je    fn+0x250            ; back edge, direct
      31a: 6a 44              pushl $0x44               ; code AFTER the loop
      ...
      504: eb 07              jmp   fn+0x50d            ; if/else join stub
      506: c7 45 ec 01 ...    movl  $0x1, -0x14(%ebp)
      50d: e9 ca 04 00 00     jmp   fn+0x9dc            ; then-branch -> if/else end
      512: c6 45 f0 00        movb  $0x0, -0x10(%ebp)   ; else branch of the outer if

    retail (?NewGame@game@@QAEHXZ, 0x475b4b)
    ---------------------------------------------------------------
      25f: 83 7d d8 00        cmpl  $0x0, -0x28(%ebp)
      263: 0f 84 ac 02 00 00  je    fn+0x515            ; -> ISLAND
      269: 8b 45 d8           movl  -0x28(%ebp), %eax
      26c: 0f be 48 05        movsbl 0x5(%eax), %ecx
      270: 83 f9 02           cmpl  $0x2, %ecx
      273: 0f 85 9c 02 00 00  jne   fn+0x515            ; -> ISLAND
      ...  switch ...
      297: eb 6d              jmp   fn+0x306            ; switch default (unchanged!)
      306: 83 7d d4 00        cmpl  $0x0, -0x2c(%ebp)   ; playerInfoOk
      30a: 0f 84 05 02 00 00  je    fn+0x515            ; -> ISLAND
      310: 83 7d d0 00        cmpl  $0x0, -0x30(%ebp)   ; mapHeaderOk
      314: 0f 84 fb 01 00 00  je    fn+0x515            ; -> ISLAND
      31a: 6a 44              pushl $0x44               ; GUARDED body, still in the loop
      ...
      4fd: c7 45 e8 00 ...    movl  $0x0, -0x18(%ebp)   ; result = 0
      504: e9 d7 04 00 00     jmp   fn+0x9e0            ; goto cleanup
      509: c7 45 e8 01 ...    movl  $0x1, -0x18(%ebp)   ; result = 1
      510: e9 cb 04 00 00     jmp   fn+0x9e0            ; goto cleanup
      515: e9 36 fd ff ff     jmp   fn+0x250            ; ISLAND = for(;;) back edge
      51a: c6 45 ec 00        movb  $0x0, -0x14(%ebp)   ; else branch of the outer if

## The rule

A guard's failure `jcc` always targets the guard's own **end label**. So the
island IS that end label, and the island holds the loop's back edge — which
means **everything between the guard and the island is nested inside the
guard, and the guard is the last statement of the loop body**. A loop whose
body ends in a guard has no condition of its own: it is `for (;;)`, and the
only way out is a `goto`/`return` inside the guarded body (a `break` would
emit `jmp <after loop>` and the after-loop code would sit at the island+5,
which here is the outer `else` arm — impossible).

Corollary, byte-proven here: when the `if`'s then-arm ends in an infinite
`for (;;)`, VC6 /Od emits **no** `jmp <if-join>` after the back edge. The
else-arm's first instruction follows the island directly. Ours (a `do`/`while`
that falls out) needs both the if/else join stub and the branch-end jump.

## Close

    if (gbWaitForRemoteReceive) {
        mapHeaderOk = 0;
        playerInfoOk = 0;
        for (;;) {
            PollSound();
            remoteBuffer = (NewGameRemotePacket*)GetRemoteData(1);
            if (remoteBuffer != NULL && remoteBuffer->type == REMOTE_MESSAGE_RELIABLE) {
                switch (remoteBuffer->command) { ... }
                if (playerInfoOk && mapHeaderOk) {
                    ... window setup, DoDialog ...
                    if (gpWindowManager->m_dialogResult == GAME_DIALOG_CANCEL) {
                        result = 0;
                        goto cleanup;
                    }
                    result = 1;
                    goto cleanup;
                }
            }
        }
    } else {
        ...
    }
    cleanup:

`game::NewGame` 99.19% -> EXACT (with the slot solve and
[or-chain-join-past-loop-names-goto-loop](or-chain-join-past-loop-names-goto-loop.md)).

See also [goto-label-jump-stub](goto-label-jump-stub.md),
[or-chain-early-return-vs-and-chain-guard](or-chain-early-return-vs-and-chain-guard.md).

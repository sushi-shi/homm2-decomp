# or-chain-join-past-loop-names-goto-loop

**Symptom.** A retry loop. Ours spells it `for (;;) { ...; if (A||B||C) Retry();
else break; }` and emits three jumps at the bottom — the then-arm's join stub,
the `else break` stub, and the back edge. Retail emits **one**: the last term's
`jcc` goes STRAIGHT to the code after the loop, and the back edge sits inside
the then-arm.

    ours: for (;;) { ...; if (A || B || C) gpGame->GetMap(); else break; }
    ---------------------------------------------------------------------
      6c1: 39 0d <giNumHumanPlayers>  cmpl  %ecx, giNumHumanPlayers
      6c7: 7e 0d                      jle   fn+0x6d6      ; last term FALSE -> $else
      6c9: 8b 0d <gpGame>             movl  gpGame, %ecx
      6cf: e8 00 00 00 00             calll ?GetMap@game@@QAEXXZ
      6d4: eb 02                      jmp   fn+0x6d8      ; then-arm -> $end
      6d6: eb 05                      jmp   fn+0x6dd      ; $else: break
      6d8: e9 35 fe ff ff             jmp   fn+0x512      ; $end: back edge
      6dd: 83 3d <gbRemoteOn> 00      cmpl  $0x0, gbRemoteOn

    retail (?NewGame@game@@QAEHXZ, 0x475b4b)
    ---------------------------------------------------------------------
      6c9: 39 0d <giNumHumanPlayers>  cmpl  %ecx, giNumHumanPlayers
      6cf: 7e 10                      jle   fn+0x6e1      ; last term FALSE -> PAST THE LOOP
      6d1: 8b 0d <gpGame>             movl  gpGame, %ecx
      6d7: e8 00 00 00 00             calll ?GetMap@game@@QAEXXZ
      6dc: e9 39 fe ff ff             jmp   fn+0x51a      ; back edge INSIDE the then-arm
      6e1: 83 3d <gbRemoteOn> 00      cmpl  $0x0, gbRemoteOn

(The two leading terms are unchanged in both: `je fn+0x6d1` / `jl fn+0x6d1`
into the then-arm — the plain `||` chain shape.)

## The rule

For `if (A||B||C) S;` VC6 /Od sends terms 1..n-1 TRUE to the then-arm and the
LAST term FALSE to the if's end label. So **whatever the last `jcc` targets is
the if's end label.** When that label is the first instruction after the loop,
the loop has no body past the `if`, no `else`, and no `break` — the `jmp` at
the end of the then-arm is the *whole* loop:

    retry_label:
        <body>
        if (A || B || C) {
            Retry();
            goto retry_label;
        }
        <after loop>

An `else break;` cannot produce this: `break` needs its own stub at the `$else`
label and the loop needs a separate `$end` back edge — three jumps where retail
has one.

## Close

    pick_map:
        wrongExpansionType = 0;
        ...
        mapHeaderRead = GetMapHeader(m_mapFilename, &m_mapHeader);
        if (!mapHeaderRead || giNumHumanPlayers < m_mapHeader.minHumanPlayers
            || giNumHumanPlayers > m_mapHeader.maxHumanPlayers) {
            gpGame->GetMap();
            goto pick_map;
        }

`game::NewGame` — this hunk plus
[and-guard-join-is-loop-back-edge](and-guard-join-is-loop-back-edge.md) and the
frame solve took 99.19% -> EXACT.

See also [trailing-loop-return-not-break](trailing-loop-return-not-break.md),
[direct-jcc-vs-goto-stub](direct-jcc-vs-goto-stub.md).

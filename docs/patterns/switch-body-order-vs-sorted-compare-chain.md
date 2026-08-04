# switch-body-order-vs-sorted-compare-chain

**Symptom.** A small `switch` lowered as a compare chain. The chain is
identical in both objects — same constants, same order — yet one case's body
(here a one-instruction `goto` stub) sits at a different place in the run of
bodies, and every jump downstream is displaced by its length.

    ours   case order: MAP_OPTIONS_CONTROL, DIALOG_OK, DIALOG_CANCEL, default
    ------------------------------------------------------------------------
      60f: 83 bd 4c fc ff ff 36           cmpl $0x36, -0x3b4(%ebp)
      616: 74 1d                          je   fn+0x635    ; SHORT - stub is adjacent
      618: 81 bd 4c fc ff ff 01 78 00 00  cmpl $0x7801, -0x3b4(%ebp)
      622: 74 77                          je   fn+0x69b
      624: 81 bd 4c fc ff ff 02 78 00 00  cmpl $0x7802, -0x3b4(%ebp)
      62e: 74 0a                          je   fn+0x63a
      630: e9 cf 00 00 00                 jmp  fn+0x704    ; default
      635: e9 d7 06 00 00                 jmp  fn+0xd11    ; MAP_OPTIONS: goto chooseMap
      63a: <DIALOG_OK body>
      69b: <DIALOG_CANCEL body>
      704: e9 4f 05 00 00                 jmp  fn+0xc58    ; default: break

    retail (?NewGameHandler@@YIHAAUtag_message@@@Z, 0x47734a)
    ------------------------------------------------------------------------
      606: 83 bd 4c fc ff ff 36           cmpl $0x36, -0x3b4(%ebp)
      60d: 0f 84 e7 00 00 00              je   fn+0x6fa    ; NEAR - stub is far away
      613: 81 bd 4c fc ff ff 01 78 00 00  cmpl $0x7801, -0x3b4(%ebp)
      61d: 74 72                          je   fn+0x691
      61f: 81 bd 4c fc ff ff 02 78 00 00  cmpl $0x7802, -0x3b4(%ebp)
      629: 74 05                          je   fn+0x630
      62b: e9 cf 00 00 00                 jmp  fn+0x6ff    ; default
      630: <DIALOG_OK body>
      691: <DIALOG_CANCEL body>
      6fa: e9 0d 06 00 00                 jmp  fn+0xd0c    ; MAP_OPTIONS: goto chooseMap
      6ff: e9 4f 05 00 00                 jmp  fn+0xc53    ; default: break

## The rule

VC6 /Od emits a compare-chain `switch` in **two independently ordered passes**:

- the **comparisons** are sorted by case VALUE (`0x36`, `0x7801`, `0x7802` —
  which is neither side's source order here), so the chain tells you nothing
  about the source;
- the **bodies** follow in **source order**, and the `default` arm's `jmp`
  closes the run.

So a body/stub that is in the wrong position inside the body run — with a
matching short-vs-near `jcc` flip on its dispatch entry — is purely a
**source case-order** fact. It is never a `goto`/`break` question: the stub
itself is byte-identical, only its address moved.

Read the source order straight off the body run. Here retail's is
`GAME_DIALOG_OK`, `GAME_DIALOG_CANCEL`, `GAME_MAP_OPTIONS_CONTROL`, `default`.

## Close

Moving `case GAME_MAP_OPTIONS_CONTROL: goto chooseMap;` below
`case GAME_DIALOG_CANCEL:` (leaving the chain and every body untouched)
realigned the whole tail; `NewGameHandler` 99.67% -> EXACT.

See also
[jump-table-entry-swap-names-case-labels](jump-table-entry-swap-names-case-labels.md)
(the table-dispatch counterpart) and
[switch-goto-tail-stub-island](switch-goto-tail-stub-island.md).

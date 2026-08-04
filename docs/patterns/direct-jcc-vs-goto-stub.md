# A single near `jcc` to a far join is a structured `if`, never `if (c) goto L;`

**Trigger.** Retail reaches a far label with one 6-byte `0f 8x` conditional jump
where ours needs a 2-byte short `jcc` over a 5-byte `e9` near `jmp`. The two
sides agree on the condition, the polarity, and the target — only the shape of
the transfer differs, and ours is one byte longer at every such site.

`goto-label-jump-stub` reads the same evidence from the other side: a `goto`
*does* produce the short-jcc-plus-jmp stub. This entry is the inverse rule, and
it is what tells you the retail source used an ordinary braced block.

## The divergence

`oldmain` (RVA 0x66767), the `quit` guard at the head of the post-setup block:

```
retail                                        ours
--------------------------------------------- ---------------------------------------------
83 7d f0 00     cmpl $0, -0x10(%ebp)          83 7d e8 00     cmpl $0, -0x18(%ebp)
0f 85 a5 07 00 00  jne  <+0xfa2>              74 05           je   <+0x802>
                                              e9 2e 04 00 00  jmp  <+0xc2b>
```

Ours came from

```cpp
        if (quit)
            goto game_finished;
        LogStr("DWM 2");
        ...
```

## What made it match

```cpp
        LogStr("DWM 1");
        if (!quit) {
            LogStr("DWM 2");
            ...
            gpWindowManager->FadeScreen(FADE_OUT, OLD_MAIN_FADE_SPEED, gPalette);
        }
        if (gbGameOver) {
            RemoteCleanup();
            ...
        }
```

The `jne` target is then just the block's closing brace, and the second guard
(`if (!gbGameOver) goto game_finished;` in the reconstruction) is the same
rewrite: retail's `cmp gbGameOver,0 / je <far>` is `if (gbGameOver) { ... }`
wrapping everything down to the end of the loop body, not a `goto` out of it.

`oldmain` 99.37% -> EXACT (three sites: the `quit` guard, the `gbGameOver`
guard, and `NewGameHandler`'s `if (message.type == MESSAGE_WIDGET) { ... }`,
whose reconstruction was `if (message.type != MESSAGE_WIDGET) goto finish;`).

## The corollary that closes the last byte

A real `goto` still keeps the stub, so where retail *does* show
`short jcc + near jmp` the source really is `goto`, and then the **label's
position matters to the byte**: `oldmain`'s `goto process_menu_command`
targeted +0x410 in our build and +0x419 in retail, i.e. the retail label sits
*after* the `if (giMenuCommand != -1)` test it guards, inside the block:

```cpp
        } else {
            if (giMenuCommand != -1) {
            process_menu_command:
                switch (giMenuCommand) {
```

and two `goto`s into the middle of a later block (`game_over:` placed on
`RemoteCleanup()` inside `if (gbGameOver) {`) are what let the campaign-win
arms skip the `gbGameOver` test entirely.

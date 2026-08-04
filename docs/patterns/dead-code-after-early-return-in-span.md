# Dead code after an early `return` still occupies the function span

`/Od` does NOT delete the statements that follow an unconditional `return` at
function scope. It stops emitting at the `return` and **resumes at the first
internal label** the front end had already created — a `for` loop's increment
label, a `switch` case, a `goto` target. Everything from that label on is
emitted normally, unreachable but present.

The consequence for span work: Ghidra's inventory reports only the *reachable*
bytes, so such a function looks absurdly small and delinks at 0.00%.

## Retail evidence — `ShowThisMapGame` (0x0048e7b0)

`config/retail_functions.csv` claimed 26 bytes:

```
0x8e7b0,26,FUN_0048e7b0,0,2,92,92,118
                            ^^ ^^  ^^^ 92 unreachable, 118 total
```

The reachable 26 bytes are the whole prologue plus `return 1;`:

```
48e7b0: 55                    push   ebp
48e7b1: 8b ec                 mov    ebp,esp
48e7b3: 81 ec 34 01 00 00     sub    esp,0x134      <- the DEAD locals are still in the frame
48e7b9: 89 8d cc fe ff ff     mov    [ebp-0x134],ecx
48e7bf: b8 01 00 00 00        mov    eax,0x1
48e7c4: eb 5c                 jmp    0x48e822       <- return 1;
```

and then, with no `mov [ix],0` and no entry jump (both unreachable and
label-less, so dropped), emission resumes at the `for` loop's increment label:

```
48e7c6: 8b 55 fc              mov    edx,[ebp-0x4]  <- $Lincr: ++ix
48e7c9: 83 c2 01              add    edx,0x1
48e7cc: 89 55 fc              mov    [ebp-0x4],edx
48e7cf: 83 7d fc 08           cmp    DWORD PTR [ebp-0x4],0x8
...
48e7f2: 68 ec 6a 51 00        push   0x516aec       <- strcmpi(mapName, "BROKENA")
...
48e825: c3                    ret
```

## What made it match

Nothing in the body: the source already carried the dead code

```cpp
i32 ShowThisMapGame(char* filename) {
    return 1;

    char mapName[FILE_REQUESTER_PATH_SIZE];
    i32 ix;
    strcpy(mapName, filename);
    ...
}
```

Only the span marker was wrong. Widening it to the next claimed function
(`0x0048e826 - 0x0048e7b0 = 0x76`) took the function from 0.00% to EXACT:

```
-VA(0x0048e7b0, 0x1a)
+VA(0x0048e7b0, 0x76)
```

## How to spot it

- The inventory row's trailing columns disagree (`reachable != total`).
- The claimed span ends in the middle of a basic block, and the gap to the
  next claimed function is not alignment padding.
- The prologue's `sub esp,N` is far larger than the reachable code needs —
  the frame still contains every dead local.

# Two swapped jump-table entries = two case LABELS on the wrong bodies

When a switch's dispatch table differs from retail in exactly two 4-byte
entries and **every other byte of the function is identical**, the case bodies
are already in the right order and at the right addresses. What differs is
which `case` constant selects which body: the source has two case labels
attached to the wrong statements.

This is worth its own entry because the first instinct — "the case bodies are
in the wrong source order" — is the opposite conclusion, and acting on it
moves hundreds of bytes.

## Retail evidence — `fileRequester::Main` (0x0048f737)

The whole function matched except four bytes. Comparing the normalized objects
with the relocated fields left in place:

```
10aa base=b8 targ=88
10ab base=0c targ=0b
10ae base=88 targ=b8
10af base=0b targ=0c
diff bytes 4
```

i.e. two adjacent dword table entries swapped:

```
                       entry[0]      entry[1]
retail   @+0x10aa   88 0b 00 00   b8 0c 00 00     (fn+0xb88, fn+0xcb8)
ours     @+0x10aa   b8 0c 00 00   88 0b 00 00     (fn+0xcb8, fn+0xb88)
```

The dispatch and the byte index table are byte-identical on both sides:

```
16d5: 8b 8d ec fb ff ff     mov  ecx,[ebp-0x414]
16db: 83 e9 03              sub  ecx,0x3            <- selector = id - 3
16e4: 83 bd ec fb ff ff 65  cmp  DWORD PTR [ebp-0x414],0x65
16eb: 0f 87 eb 05 00 00     ja   0x1cdc             <- default
16f9: 8a 90 be 10 00 00     mov  dl, BYTE PTR [eax+0x10be]   <- byte table
16ff: ff 24 95 aa 10 00 00  jmp  DWORD PTR [edx*4+0x10aa]    <- entry table
```

byte table: `[0] = 0`, `[11] = 1`, `[12] = 2`, tail `= 3`, everything else `4`.
So `entry[0]` serves selector 0 = id 3 and `entry[1]` serves selector 11 = id 14.
`fn+0xb88` is the big "jump the list to the clicked position" body, `fn+0xcb8`
is the three-instruction `DoKnob(); break;`.

Retail therefore binds **id 3 -> the position body** and **id 14 -> DoKnob()**;
the reconstruction had them the other way round.

## What made it match

Swap the two `case` constants, leave both bodies exactly where they are:

```diff
-case FILE_REQUESTER_SCROLL_KNOB: {          // 14
+case FILE_REQUESTER_SCROLL_GUTTER: {        // 3
     positions = m_fileCount - (iMaxListSize - 1);
     ...
 }
-case FILE_REQUESTER_SCROLL_GUTTER:          // 3
+case FILE_REQUESTER_SCROLL_KNOB:            // 14
     DoKnob();
     break;
```

99.99% -> EXACT, and the semantics become the obvious ones: pressing the knob
starts a drag, pressing the gutter jumps the list.

## How to spot it

Mask the relocated fields and byte-compare. If the only differing bytes sit
inside the dispatch table and are a permutation of the same addends, do not
touch the bodies — re-derive `selector -> entry` from the byte table and check
which case constant each entry really serves.

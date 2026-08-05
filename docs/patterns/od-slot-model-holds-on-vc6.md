# od-slot-model-holds-on-vc6

The MSVC 4.2 `/Od` frame model in `homm2/core/od_slots.py` reproduces **VC6 SP5**
frames exactly, including arrays and structs. Measured on `advManager::DoEvent`
(45 function-scope locals, three `tag_message`/`char[]` aggregates, a 500-byte
buffer) — every one of the 45 offsets predicted by
`slot_order(decl) -> off -= sizeof` matched the `/Z7` `S_BPREL32` census:

    slot_order = sorted(names, key=(bucket(name), -declaration_index))
    off = -0x0c                       # after the 12-byte EH registration record
    for n in slot_order:  off -= sizeof(n);  offset_of(n) = off

    sphinxAnswer_a  b0   -0x40   (char[52])      thirdUpgrade   b1   -0x80
    eventHero2      b0   -0x44                   monsterType6   b2   -0x8c
    eventType_g     b0   -0x48                   oracleMessage1 b3   -0xa8  (0x1c)
    unusedEventMsg  b0   -0x64   (0x1c)          ...            b8/b9/b11/b13
    ...                                          eventText      b13  -0x310 (0x1f4)

Consequences that matter when solving a frame:

* **The base is `-0xc`, not `-0x4`, in a `/GX` function** — the three dwords of
  the EH registration record (`push -1; push scopetable; push fs:[0]`) sit at
  `-0x4 .. -0xc` and the first local starts at `-0x10`.
* **Aggregate size, not 4, is subtracted**, so one wrong array bound shifts the
  entire tail of the frame. `SPHINX_ANSWER_BUFFER_SIZE` was recoverable purely
  from the gap between the first local and the second (`0x34` -> 52).
* Because only the *name* picks the bucket, giving every local a bucket-0 name
  reduces the whole layout to **reverse declaration order**, which is the
  cheapest way to reproduce a retail permutation: read retail's slot order off
  the delinked object, then declare in the reverse of it.

## The layout has five regions, in this order

    [ function-scope named locals ]     bucket-sorted, as above
    [ inner-block named locals    ]     one group, in block source order
    [ front-end expression temps  ]     dword-granular, body order
    [ inline-expansion slots      ]     one set per /Ob1 call site, body order
    [ this / __fastcall spill     ]
    [ back-end temps              ]     byte-granular for `char`

Block-scope locals really do slot after **all** function-scope locals (VC6
confirms the 4.2 rule) — a `{ char c; ... }` inside a `case` lands right below
the largest function-scope array, before the temps.

`od_slots` covers only the first two regions. Nothing in the last three is
reachable by renaming: they are ordered by *when the front end created them*,
so the source lever there is structural (whether a value is an inline callee's
local or a caller's named local), never a name hash. See
`inline-expansion-slots-after-expression-temps.md`, which resolves what used to
be recorded here as an open residual: `DoEvent`'s three CP1251 fold variables
interleave with its seven expression/expansion slots because they are inlined
callees' locals, and the two front-end passes run in sequence. Spelling the
folds as one file-static `inline` closed `DoEvent` exactly (0/24521 bytes).

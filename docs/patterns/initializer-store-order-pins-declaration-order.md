# The `= constant` store sequence pins DECLARATION order, not just slots

**Trigger.** `homm2 sema frames` says every local sits on the retail slot, the
CFG is identical, objdiff reads 99.9%, and yet hundreds of raw bytes differ.
The prologue's run of `movl $N, -0xXX(%ebp)` stores is in a different ORDER
than retail's.

`od_slots` only constrains the slot *order* (bucket ascending, declaration
index descending within a bucket). Many declaration orders realise the same
layout — but a local declared **with an initializer** emits its store at its
declaration point, so retail's store sequence is direct evidence of the
relative declaration order of the initialised locals. Solve the buckets first,
then pick, among the declaration orders that realise them, the one whose
initialised members appear in retail's store sequence.

## The divergence

`game::RandomizeEvents` (RVA 0x513cf), the eight `= 1` counters. Frames were
already identical; only the emission order differed:

```
retail                                    ours (bucket-group declaration order)
----------------------------------------  ----------------------------------------
c7 45 f8 01 00 00 00  movl $1,-0x08(%ebp)  c7 45 f8 01 00 00 00  movl $1,-0x08(%ebp)
c7 45 b4 01 00 00 00  movl $1,-0x4c(%ebp)  c7 45 fc 01 00 00 00  movl $1,-0x04(%ebp)
c7 45 c8 01 00 00 00  movl $1,-0x38(%ebp)  c7 45 ec 01 00 00 00  movl $1,-0x14(%ebp)
c7 45 b8 01 00 00 00  movl $1,-0x48(%ebp)  c7 45 c8 01 00 00 00  movl $1,-0x38(%ebp)
...                                        ...
```

Both sides put `shrineId` on -0x08 and `bottleId` on -0x4c; retail simply
declares `bottleId` second, we declared it nineteenth.

## What made it match

Keep the solved names, then reorder the declaration block so the initialised
locals appear in retail's store order while every bucket group keeps its
required internal order (reverse of slot order):

```cpp
i32 valid;  u32 extraIndex27;  i32 row;  i32 xPos;  ArtifactType value;
i32 shrineId8 = 1;  i32 bottleId = 1;  i32 jailId8 = 1;  i32 sphinxId = 1;
i32 tentId0 = 1;    i32 hutId27 = 1;   i32 eyeId13 = 1;  i32 signId = 1;
i32 j4;  mapCell* cell2;  ...
```

`RandomizeEvents` went from 4181 differing raw bytes (objdiff 99.98%) to 0.
Never trust objdiff's percentage here: it masks the displacement operand, so a
whole-frame ordering error reads as "almost exact".

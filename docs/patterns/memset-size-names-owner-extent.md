# The `memset` size immediate is the cleared owner's extent

Measured with the pinned VC6 SP5 compiler (BASE flags), 2026-10-07.

## Mechanism

VC6 does not inline `memset` at `/Od`; `memset(&obj, 0, sizeof obj)` is a
plain call whose size is a pushed immediate:

```
struct Pool { int ready; int handles[16]; int count; } gPool;
void Clear() { memset(&gPool, 0, sizeof gPool); }

6a 48              push 0x48          ; sizeof(Pool)
6a 00              push 0
68 00 00 00 00     push offset gPool  ; DIR32
e8 00 00 00 00     call _memset
83 c4 0c           add esp, 0xc
```

## Use

Read the immediate as the extent of whatever the source cleared, like the
`operator new` push in [class-extent-from-new-size](class-extent-from-new-size.md).
`StartupMilesSamples` (VA 0x004cdb30) pushes 0x48 for the sample-handle owner at
0x005396d8 (`ready`, 16 handles, a count), where a reconstruction that cleared a
4-byte scalar was wrong. `nb_init` (RVA 0x73e34) clears 36 bytes: nine event
handles, not ten; the four bytes before the next global are alignment.

A size alone does not prove one aggregate. `InitEntireCampaign` clears 327
bytes starting at `game+2`, a range that crosses several named members, so
check the users of the cleared range before modelling it as one object.

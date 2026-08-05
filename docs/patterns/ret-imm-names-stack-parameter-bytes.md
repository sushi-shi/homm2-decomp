# `retl $N` names the stack-argument byte count - check it before anything else

**Symptom.** A large body is byte-identical to retail and still not exact. One
byte differs, in the epilogue.

`??0sample@@QAE@PADJJJ@Z` (RVA 0xce250, 528 bytes) - the whole span differed in
exactly these places, and every one but `0x1b5` is a masked relocation field:

```
offset  retail  ours   what
0x006   09      00     /GX EH scope-table addend (reloc, masked by objdiff)
0x0e9   d3 01   00 00  switch index-table addend (reloc)
0x0f0   b7 01   00 00  switch jump-table addend (reloc)
0x1b5   04      10     <-- retl $0x4  vs  retl $0x10
```

```
retail                     ours
-------------------------  -------------------------
8b e5        movl %ebp,%esp   8b e5        movl %ebp,%esp
5d           popl %ebp        5d           popl %ebp
c2 04 00     retl $0x4        c2 10 00     retl $0x10
```

## What it proves

`__thiscall` passes `this` in `ecx` and every other parameter on the stack, and
the callee pops them. `retl $0x4` is ONE 4-byte stack parameter; our
`sample::sample(char*, i32l, i32l, i32l)` popped four. The reconstruction had
invented three arguments the retail body never reads - and the body confirms it:
the only positive frame reference in the whole function is `0x8(%ebp)`.

The mangled name in `build/gen/symbol_names.csv` is a source-owned CLAIM, so a
wrong parameter list survives happily at 99.8% and quietly caps every caller
too. Here the single caller

```
?GetSample@resourceManager@@QAEPAVsample@@PAD@Z   (RVA 0xb84f0)
```

pushes exactly one argument in retail:

```
6a 34        pushl $0x34            ; sizeof(sample)
e8 ..        calll ??2@YAPAXI@Z
83 c4 04     addl  $0x4,%esp
89 45 e4     movl  %eax,-0x1c(%ebp)
...
8b 55 08     movl  0x8(%ebp),%edx   ; name  - the ONLY push
52           pushl %edx
8b 4d e4     movl  -0x1c(%ebp),%ecx ; this
e8 ..        calll ??0sample@@QAE@PAD@Z
```

and was stuck at 94.66% for the four-argument spelling.

## Close

`sample::sample(char* name)`; header, definition and the one call site
(`new sample(name)`). The mangled name changes
(`??0sample@@QAE@PADJJJ@Z` -> `??0sample@@QAE@PAD@Z`), so this needs a
`homm2 redelink`.

`sample::sample` 99.78% -> EXACT (closing `BASE/SAMPLE` at 8/8) and
`resourceManager::GetSample` 94.66% -> 99.83% for free.

## Rule

Read `retl $N` (and `ret` with no immediate, which is `__cdecl`) on the FIRST
pass over any function whose signature is a reconstruction guess. `N/4` is the
stack-argument count exactly; combined with the highest `[ebp+K]` the body
touches it pins the parameter list before a single byte of the body is written.

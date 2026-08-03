# struct-return-temp-vs-scalar-typedef

**Reverse pattern.** The trigger is not in the function you are matching: it is the
declaration of a type in a shared header.

**Symptom.** A call to a function that returns a one-member struct is followed, in
our object only, by a store/reload pair through an extra frame slot, and the whole
frame below the assignment is 4 bytes deeper than retail's:

```
ours                                            retail
----------------------------------------------  ----------------------------------
b9 00 00 00 00  mov ecx, gText                   b9 00 00 00 00  mov ecx, gText
e8 ..           call ?LoadPlaySample@@YI?AUSAM.  e8 ..           call ?LoadPlaySample@@..
89 45 f4        mov [ebp-0xc], eax   <-- $T      89 45 f8        mov [ebp-0x8], eax
8b 45 f4        mov eax, [ebp-0xc]   <-- copy
89 45 f8        mov [ebp-0x8], eax
```

and, at every call that takes the value onward,

```
ours                                            retail
----------------------------------------------  ----------------------------------
83 c9 ff        or  ecx, -1                      83 ca ff        or  edx, -1
8b 55 f8        mov edx, [ebp-0x8]               8d 4d f8        lea ecx, [ebp-0x8]
52              push edx
e8 ..           call ?WaitEndSample@@..          e8 ..           call ?WaitEndSample@@..
```

Measured on `combatManager::CheckApplyGoodMorale` (RVA 0x2848f) and
`combatManager::Open` (0x269fe); the same shape appears at every `SAMPLE2` site in
the image.

## What the retail bytes prove

VC6 `/Od` **always** materialises the return value of a struct-returning function
into a compiler temp before assigning or initialising, whatever the source spelling.
A probe with the exact shipped flags (`/Od /MT /Gr /G5 /Ob1`):

```cpp
struct SAMPLE2 { sample* pSample; };          typedef sample* SAMPLE2S;
SAMPLE2  LoadPlaySampleA(char*);              SAMPLE2S LoadPlaySampleB(char*);

void fA(void){ SAMPLE2  s = {0}; s = LoadPlaySampleA(0); WaitEndSampleP(&s, -1); }
void fB(void){ SAMPLE2S s = {0}; s = LoadPlaySampleB(0); WaitEndSampleP(&s, -1); }
```

```
fA  (1-member struct)                           fB  (scalar typedef)
c7 45 fc 00 00 00 00  movl $0,-0x4(%ebp)        c7 45 fc 00 00 00 00  movl $0,-0x4(%ebp)
33 c9                 xorl %ecx,%ecx            33 c9                 xorl %ecx,%ecx
e8 ..                 calll LoadPlaySampleA     e8 ..                 calll LoadPlaySampleB
89 45 f8              movl %eax,-0x8(%ebp)      89 45 fc              movl %eax,-0x4(%ebp)
8b 45 f8              movl -0x8(%ebp),%eax
89 45 fc              movl %eax,-0x4(%ebp)
83 ca ff              orl  $-1,%edx             83 ca ff              orl  $-1,%edx
8d 4d fc              leal -0x4(%ebp),%ecx      8d 4d fc              leal -0x4(%ebp),%ecx
```

`fB` is retail byte for byte. Copy-initialisation (`SAMPLE2 s = f();`) does **not**
elide the temp either - a second probe (`gA`/`gB`) gives the same store/reload - so
no spelling inside the matched function can reach retail's shape.

**Therefore the retail type is a scalar, not a one-member struct**, and the frame
difference is a header claim, not a body claim. Two further retail facts agree:

- `SAMPLE2 s = {NULL};` emits `c7 45 f8 00000000` in retail - a single-element
  brace initialiser, legal for a scalar in C and C++ alike, and consistent with
  `NULL_SAMPLE2` not existing in this image (PoL 2.0 copies an 8-byte global here).
- retail's `WaitEndSample` spills **both** arguments from registers
  (`mov [ebp-8],ecx` / `[ebp-0xc]` from edx) and dereferences the first
  (`mov eax,[ebp-8]; cmp dword ptr [eax],0`), which no by-value struct parameter
  can produce under `__fastcall`: the parameter is `SAMPLE2*`.

## The fix

Declare the type as the scalar it is, and the consumer as taking its address
(`include/SOURCE/KB_TYPES.h`, `include/SOURCE/KB.h`):

```c
typedef class sample* SAMPLE2;               /* not struct SAMPLE2 { sample* }; */
void WaitEndSample(SAMPLE2* s, i32 waitTime);
```

Call sites become `WaitEndSample(&sample, -1);`. Because the temp disappears, every
frame slot below the assignment moves up one dword, so this also unblocks the local
slot solve in each affected function - do not fit local names against a frame that
still carries the temp.

## Reading it in one line

An extra `mov [$T],eax; mov eax,[$T]` after a call whose mangled return type is
`?AU<name>@@` means the retail declaration of `<name>` was not a struct.

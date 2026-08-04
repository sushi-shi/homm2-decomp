# `mov reg,1 / test / jne` inside an `&&` chain — an inlined constant predicate

**Trigger.** A term of a source `&&` chain lowers to `mov r32,1 / test r32,r32 / jne <out>`.
A folded literal never produces that: `!1` and `!TRUE_ENUM` and `!kConst` all fold to
`xor r32,r32 / test r32,r32 / je <out>` (the *inverse* jump). Only an **inlined function
whose body returns a constant** materialises the `1` and keeps the `jne`. `/Ob1` is on,
so a `static inline bool P(void) { return true; }` is expanded at the call site and the
constant is still loaded into a register before the short-circuit test.

Measured on `SystemOptionsHandler` (RVA 0x14e82), the `SYSTEM_OPTION_MUSIC_VOLUME` arm.

## The probe (VC6 SP5, `/nologo /c /Od /MT /Gr /G5 /Ob1 /Gi- /GX /DNO_STRICT`)

```cpp
extern int gv;  void sink(int);
enum ProbeFlag { PROBE_AVAILABLE = 1 };
static const int kAvail = 1;
inline bool AvailB(void) { return true; }

void f1(void) { if (gv == 0 && !1               && !1              ) sink(1); sink(9); }
void f3(void) { if (gv == 0 && !PROBE_AVAILABLE && !PROBE_AVAILABLE) sink(3); sink(9); }
void f7(void) { if (gv == 0 && !kAvail          && !kAvail         ) sink(7); sink(9); }
void f5(void) { if (gv == 0 && !AvailB()        && !AvailB()       ) sink(5); sink(9); }
```

```
f1 / f3 / f7  (literal, enum, const int)      f5  (inlined constant predicate) == retail
--------------------------------------------- ---------------------------------------------
83 3d .. 00  cmpl $0x0, 0x0                   83 3d .. 00  cmpl $0x0, 0x0
75 16        jne  Lskip                       75 1c        jne  Lskip
33 c0        xorl %eax, %eax                  b8 01000000  movl $0x1, %eax
85 c0        testl %eax, %eax                 85 c0        testl %eax, %eax
74 10        je   Lskip                       75 13        jne  Lskip
33 c9        xorl %ecx, %ecx                  b9 01000000  movl $0x1, %ecx
85 c9        testl %ecx, %ecx                 85 c9        testl %ecx, %ecx
74 0a        je   Lskip                       75 0a        jne  Lskip
```

Retail's bytes at 0x14e82+0x186:

```
83 3d .. 00        cmpl $0x0, gConfig.musicVolume
75 36              jne  Lskip
b8 01 00 00 00     movl $0x1, %eax
85 c0              testl %eax, %eax
75 2d              jne  Lskip
b9 01 00 00 00     movl $0x1, %ecx
85 c9              testl %ecx, %ecx
75 24              jne  Lskip
```

`AvailB() == 0` spells the same bytes as `!AvailB()`; both work.

## What made it match

```cpp
static inline bool RedbookMusicPresent(void) { return true; }
static inline bool MidiMusicPresent(void)    { return true; }
...
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED
        && !RedbookMusicPresent() && !MidiMusicPresent()) {
```

(previously two `gpSoundManager->m_cdReady == 0` / `m_midiReady == 0` member reads that
retail does not have at all — see docs/version-changes.md.)

## Companion: `!(A || B)` materialises one more bool temp than `!A && !B`

The same arm's neighbour, `SYSTEM_OPTION_SOUND_VOLUME`, and `soundManager::Open`
(RVA 0xb5d20) differ only in this. Both inline `IsAudiereBackend`/`IsMilesBackend`,
each of which lands its result in its own `mov $1 / jmp / mov $0 / and $0xff / test`
frame temp. `Open` stops there:

```
Open (retail, this+0x36/0x42 then this+0x36/0x3e) -> `!IsAudiere(this) && !IsMiles(this)`
  movl $0x1, -0x10(%ebp) ... andl $0xff,%ecx; testl; jne  <- temp 1 tested, chain continues
  movl $0x1, -0x14(%ebp) ... andl $0xff,%ecx; testl; jne  <- temp 2 tested, done
```

`SystemOptionsHandler` adds a **third** temp that holds the `||` itself:

```
  movl $0x1, -0x24(%ebp) ... testl; jne L               <- IsAudiere
  movl $0x1, -0x28(%ebp) ... testl; jne L               <- IsMiles
L:movl $0x1, -0x2c(%ebp)  /  movl $0x0, -0x2c(%ebp)     <- the (A || B) value
  movl -0x2c(%ebp),%eax; andl $0xff,%eax; testl; jne    <- the outer `!`
```

so the source is

```cpp
    if (gConfig.soundVolume == CONFIG_VOLUME_MUTED
        && !(IsAudiereBackend(gpSoundManager) || IsMilesBackend(gpSoundManager))) {
```

and not the `&& !A && !B` form `Open` uses. Count the frame temps before choosing.

# An ours-only `and $0xff` after `setcc` means the other addend is an enum

**Trigger.** A relational used as a value and added to a constant. Ours emits
`xor r,r / setcc r8 / and r32,0xff / add r32,imm`; retail emits the same without the
`and`. The mask is not about the comparison at all - it appears when the *other*
operand of the `+` has **enum** type. A plain `int` constant produces no mask.

Measured on `UpdateSystemOptions` (RVA 0x14b62), three sites.

## The divergence

```
retail                                     ours
------------------------------------------ ------------------------------------------
33 d2              xorl  %edx, %edx        33 c9              xorl  %ecx, %ecx
83 3d .. 00        cmpl  $0x0, gConfig.x   83 3d .. 00        cmpl  $0x0, gConfig.x
0f 95 c2           setne %dl               0f 95 c1           setne %cl
                                           81 e1 ff 00 00 00  andl  $0xff, %ecx
83 c2 12           addl  $0x12, %edx       83 c1 12           addl  $0x12, %ecx
89 55 f8           movl  %edx, -0x8(%ebp)  89 4d fc           movl  %ecx, -0x4(%ebp)
```

## The probe

```cpp
extern int g; enum E { BASE = 0x12 }; extern int v;
void p1(void) { v = (g != 0) + 0x12; }          // setne al          ; add eax,0x12
void p2(void) { v = 0x12 + (g != 0); }          // setne al          ; add eax,0x12
void p3(void) { v = (g != 0) + BASE; }          // setne al; AND 0xff; add eax,0x12
void p4(void) { v = BASE + (g != 0); }          // setne al; AND 0xff; add eax,0x12
void q2(void) { v = (int)(g != 0) + BASE; }     // setne al          ; add eax,0x12
void q4(void) { v = (g != 0) + (int)BASE; }     // setne al          ; add eax,0x12
```

Operand ORDER is irrelevant (p1 == p2, p3 == p4). Only the enum-ness of the
non-bool addend decides, and either cast removes it.

## What made it match

```cpp
    msg.payload.widget.data.value =
        (gConfig.showRoute == 0) + ADVMGR_SYSTEM_OPTIONS_ROUTE_FRAME_BASE;   // ours
```
```cpp
    msg.payload.widget.data.value =
        static_cast<i32>(gConfig.showRoute == 0) + ADVMGR_SYSTEM_OPTIONS_ROUTE_FRAME_BASE;
```

`UpdateSystemOptions` 93.97% -> EXACT with that cast at the three `setcc`-plus-frame-base
sites (`SOUND`, `ROUTE`, `VIDEO` frame bases). Retail's constants at those sites were
plain integers, so the reconstruction's named enum members need the cast to stay
byte-faithful.

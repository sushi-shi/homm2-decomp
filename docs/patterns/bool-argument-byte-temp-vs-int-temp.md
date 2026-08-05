# A `bool` argument gets a BYTE temp; an ours-only `xor reg,reg` says ours is `int`

**Symptom.** One ours-only `xor reg,reg` immediately in front of the `cmp` that
feeds a `setcc` whose result is pushed as an argument. Two bytes, nothing else
in the function differs, and no operand respelling of the comparison moves it.

`?PlayAudiereMusic@@YIXAAH0V?$RefPtr@VAudioDevice@audiere@@@audiere@@H@Z`
(RVA 0xcd680, `stream->setRepeat(...)`), retail vs ours:

```
retail                                        ours
--------------------------------------------- ---------------------------------------------
89 95 88 fe ff ff  movl %edx,-0x178(%ebp)      89 95 88 fe ff ff  movl %edx,-0x178(%ebp)
                                               33 c0              xorl %eax,%eax
83 bd 88 fe ff ff 00 cmpl $0,-0x178(%ebp)      83 bd 88 fe ff ff 00 cmpl $0,-0x178(%ebp)
0f 95 c0           setne %al                   0f 95 c0           setne %al
50                 pushl %eax                  50                 pushl %eax
```

`?PlayAudiereSample@@YIX...` (RVA 0xcc8f0) has the identical two-byte residual
at its own `setRepeat` site (`33 c9` before `cmpl $0,0x28(%eax)`).

## What the two forms mean

`xor reg,reg; cmp; setcc reg8` materialises a **32-bit** 0/1. Bare
`cmp; setcc reg8` materialises an **8-bit** one and pushes the register with its
upper bits still holding whatever was there (retail pushes `ecx` still carrying
`gAudiereSampleList`). VC6 picks the width from the STATIC TYPE of the argument
expression, not from the callee's parameter type - `setRepeat(bool)` is `bool`
on both sides.

Measured on VC6 SP5 `/Od /Ob1 /GX` (probe `build/scratch-m3/probe13.cpp`), for a
`bool` parameter:

| argument expression | emitted |
| :-- | :-- |
| `x != 0` | `xor`, 32-bit temp |
| `(bool)x`, `static_cast<bool>(x)`, `!!x`, `!(x == 0)`, `x > 0` | `xor`, 32-bit temp |
| `bool b = x != 0; f(b)` | byte local, extra `movb`/`movb` pair |
| **`x != 0 ? true : false`** | **no `xor`, byte temp - retail's shape** |

A bare relational is not enough. Only a conditional whose two arms are the
`true`/`false` **bool literals** produces the byte temp.

## The trap: `#define true 1` turns the fix into a no-op

`include/Ints.h` carried

```c
#if !H2_STRICT_ENUMS
#define true 1
#define false 0
#endif
```

from the MSVC 4.2 line ("Boolean-int aliases for the pre-bool compiler"). VC6 has
a real `bool` with real `true`/`false` keywords, so under that shim
`x != 0 ? true : false` is `x != 0 ? 1 : 0` - an **int** conditional - and emits
the `xor` exactly like the bare relational. Every spelling above was tested
inside the real TU and all of them widened; the ternary only started working
once the macros were gone.

The bisect that found it (`build/scratch-m3/INC0..INC7.cpp`): the same
`stream->setRepeat(repeat != 0 ? true : false)` in a TU including only
`<audiere.h>` emits no `xor`; adding `#include <va.h>` (which pulls `Ints.h`)
re-introduces it. Nothing else in the include set matters.

## Close

Delete the shim (VC6 and the clang analysis build both have real `bool`) and
spell the site with the ternary:

```cpp
stream->setRepeat(repeatMusic != 0 ? true : false);
```

Removing the two `#define`s is byte-neutral across the whole tree except for one
other site that was already spelled this way -
`BASE/Midi ?MIDIStartup@@YI_NXZ` (`gMusicFlagA = AIL_midiOutOpen(...) == 0 ?
true : false;`) went 90.00% -> EXACT. Measured over all 2473 functions:
3 improved, 0 regressed.

`PlayAudiereSample` 99.64% -> EXACT, `PlayAudiereMusic` 99.57% -> EXACT
(closing `BASE/AudiereMusic` at 19/19).

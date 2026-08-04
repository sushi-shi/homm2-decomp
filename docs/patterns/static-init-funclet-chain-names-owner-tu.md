# static-init-funclet-chain-names-owner-tu

**Symptom.** A unit reports extra `_$E<n>` rows at `--` (never produced by our
object) beyond the usual `_$E19`/`_$E18` pair every TU carries. They are not an
inventory bug: the funclets are VC6's dynamic-initializer chain for a **global
object with a constructor**, and their `.text` address says which TU defined it.

Every TU in this image ends with two funclets — `_$E19` (39 bytes, the
`ctype<unsigned short>::id` once-guard) and `_$E18` (18 bytes, its
`atexit` registration). A TU that also defines a global class instance emits
FOUR more, in this fixed shape.

## The bytes (SOURCE/PATH, RVA 0x7e1b0 .. 0x7e1ee)

```
_$E17   (0x7e1b0, 15)  the .CRT$XCU entry
  55                 push ebp
  8b ec              mov  ebp, esp
  e8 <rel32 _$E1>    call _$E1                <-- run the constructor
  e8 <rel32 _$E18@>  call _$E18@0x7e1ce       <-- register the destructor
  5d c3              pop ebp; ret

_$E1    (0x7e1bf, 15)  the constructor thunk
  b9 <DIR32 ?SVSearchArray@@3VsearchArray@@A>  mov ecx, &SVSearchArray
  e8 <rel32 ??0searchArray@@QAE@XZ>            call searchArray::searchArray

_$E18@0x7e1ce (0x7e1ce, 18)  the atexit registration
  68 <DIR32 _$E1@0x7e1e0>  push &<dtor thunk>
  e8 <rel32 _atexit>       call atexit
  83 c4 04                 add esp, 4

_$E1@0x7e1e0  (0x7e1e0, 15)  the destructor thunk
  b9 <DIR32 ?SVSearchArray@@3VsearchArray@@A>  mov ecx, &SVSearchArray
  e8 <rel32 ??1searchArray@@QAE@XZ>            call searchArray::~searchArray
```

The `mov ecx, <DIR32>` operand of the `_$E1` pair is the whole answer: the
object being constructed is `?SVSearchArray@@3VsearchArray@@A`, and the chain
lives in `SOURCE/PATH`'s `.text`, between PATH's last claimed function
(`army::GetBestDirection`, ends 0x7e16a) and the next TU. Therefore **retail's
PATH translation unit contains `searchArray SVSearchArray;`**.

Our tree defined it in `src/SOURCE/PHILAI.cpp` instead, so our PATH.obj emitted
only the two standard funclets and the four rows stayed unmatched.

## Verifying the attribution

Three independent checks, all cheap:

1. The funclet's `.text` RVA falls inside the candidate unit's span (here
   between PATH's last function and the next unit's first).
2. `llvm-objdump -dr --disassemble-symbols=_\$E1 build/delink/<UNIT>.c.obj`
   names the constructed object and its `??0`/`??1` pair directly.
3. Moving the definition and rebuilding flips the rows.

Measured for this case: moving `searchArray SVSearchArray;` from
`src/SOURCE/PHILAI.cpp` to `src/SOURCE/PATH.cpp` took SOURCE/PATH from 14/18 to
16/18 (`_$E17` and `_$E1` -> EXACT) with SOURCE/PHILAI and the tree-wide count
otherwise unchanged (1464 -> 1466). The remaining two rows,
`_$E18@0x7e1ce` / `_$E1@0x7e1e0`, are name-collision artifacts of the claimed
inventory: one unit cannot carry two symbols literally called `_$E18`, so the
generator suffixes the second pair and objdiff has nothing to pair them with.

**Note.** `_$E<n>` numbering is a per-TU counter and does NOT agree between our
object and retail (ours emitted `_$E14/15/16/17/23/24` for the same six
bodies); pair on address and body, never on the number.

# Frame gaps are retail's dead locals — count them, don't compress the frame

**Trigger.** Retail's `sub esp, N` matches ours but the slot census does not: retail
uses a set of `-0xN(%ebp)` displacements with **holes** in it. Under `/Od` MSVC gives a
frame slot to every declared local, referenced or not, so a hole that is not an array
body and not alignment padding is a local the developers declared and never used. A
reconstruction that omits it shifts every deeper slot by four bytes.

Measured on `game::TransmitSaveGame` (RVA 0x5da83) and `game::ReceiveSaveGame`
(RVA 0x5e236); `game::SetupTowns` (RVA 0x5c623) needed one, `game::ProcessMapExtra`
(RVA 0x5c3d4) already had two.

## Reading the census

`homm2 sema disasm <rva> --blocks --target`, then bucket the operands. Retail
`TransmitSaveGame`:

```
-0x4(5)  -0x8(4)   [ 0x1c4-byte hole = char filename[452] ]  -0x1cc(4) -0x1d4(3) ...
                                                                        ^^^^^^ -0x1d0 missing
... -0x204(4)  -0x20c(14) ...                                    ^^^^^^ -0x208 missing
... -0x218(4)  -0x228(12)                              ^^^^^^ -0x21c,-0x220,-0x224 missing
```

Five 4-byte holes, five dead locals. Our source had four (`unused140`, `unused1c3`,
`unused3c12`, `unused484`), so one had to be added before the order would solve.

Array bases are distinguishable from holes: they are the target of a `lea`
(`lea ecx, [ebp - 0x1cc]` here), and the hole size then equals the declared array size
rounded up to 4. That is how `TRANSMIT_FILENAME_CAPACITY` was corrected from 456 to
452 (= the already-present `SAVE_PATH_CAPACITY`): `0x1cc - 0x8 = 0x1c4 = 452`.

## Naming OUR side of the frame

`/Z7` adds `S_BPREL32` debug records without changing `/Od` code generation, so the
compiler will name our own slots:

```sh
python3 -m homm2.build.cc_wrap --out /tmp/u.obj --src src/SOURCE/GAME.cpp -- \
    /nologo /c /Od /MT /Gr /G5 /Ob1 /Gi- /GX /Z7 /DNO_STRICT
# then read .debug$S: CV5 S_GPROC32 = 0x100b, S_LPROC32 = 0x100a, S_END = 0x0006,
# S_BPREL32 = 0x1006 with body = off(i32) typind(u32) namelen(u8) name
```

Pairing that list with the block-aligned retail/base operand correspondence turns a
25-local frame into a plain permutation to solve with `homm2/core/od_slots.py`, instead
of guessing which local is which from use counts.

## What made it match

`TransmitSaveGame`: `filename[452]` moved to slot 3, one extra dead local added, and
the eighteen live locals re-suffixed through the existing per-function
`#define name nameN` / `#undef` block. 98.91% -> EXACT.
`ReceiveSaveGame`: `filename[452]` moved to slot 1. 96.90% -> EXACT.

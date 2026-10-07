# The Rich header counts linked objects per compiler build

Pinned VC6 SP5 `LINK.EXE`, Ghidra reading, 2026-10-06; checked against both
retail images.

## Mechanism

Every VC6 object carries an absolute `@comp.id` symbol naming the tool and
build that produced it. LINK stores its value in the module record
(`0x00461b20`, module `+0x30`), and the Rich header lists each `@comp.id`
with the number of linked objects that carried it. The writer itself was not
traced; the game's C++ count equals its reconstructed C++ units.

## Use

The C++ compiler entry (build 8966 for VC6 SP5) is the exact number of C++
translation units in the link. HMM2PL.exe lists 96, the same as the
reconstruction's C++ units, so splitting a unit into two, or adding a
separate owner unit for one function, is ruled out unless another merge
frees an object. The editor's header limits its units the same way (one C++
object per `ctype` registration pair, 65 and 65; see
[ob2-places-initializer-literals-lexically](ob2-places-initializer-literals-lexically.md)).

Library members count too, under their own producer IDs, so read the C++
compiler row, not the total.

"""homm2.clean - `homm2 clean` - derive the shipped source tree from the matching one.

The matching tree carries scaffolding that exists only to prove the source
reproduces retail object code: VA/DATA/VTBL annotations, DATA_COMPGEN delinker
metadata, the frozen retail file/line operands, and the dual-build H2_ENUM_*
machinery. None of it belongs in anything else built from this source, so the
generator performs those expansions ahead of time and emits ordinary C++.

  clean_source.py   the generator, its rule table, and the publish/verify flow
  overrides/        files macro expansion CANNOT produce, supplied verbatim
  project/          the build system the generated tree needs, copied verbatim

overrides/ is small and load-bearing. src/BASE/BITS.cpp and src/BASE/TILE.cpp are
MSVC `__asm` in the matching tree - the bit primitives and the hand-asm tile
blitter - and nothing but the pinned MSVC can compile them, so the published
tree gets portable C++ reimplementations. include/Ints.h is the dual-build
switch itself, so it cannot select itself away; the override is single-branch
C++20.

project/ is deliberately NOT this repository's flake: that one provisions wine,
the pinned MSVC, Ghidra, objdiff and the delinker, and the published branch
must need none of it. Its flake knows only clang and ninja.

Output feeds this branch's published source trees.
"""

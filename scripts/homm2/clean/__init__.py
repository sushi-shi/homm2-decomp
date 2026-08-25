"""homm2.clean - `homm2 clean` - derive the shipped source tree from the matching one.

The matching tree carries scaffolding that exists only to prove the source
reproduces retail object code: VA/DATA/VTBL annotations, DATA_COMPGEN delinker
metadata, the frozen retail file/line operands, and the dual-build H2_ENUM_*
machinery. None of it belongs in anything else built from this source, so the
generator performs those expansions ahead of time and emits ordinary C++.

  clean_source.py   the generator, its rule table, and the publish/verify flow
  overrides/        files macro expansion CANNOT produce, supplied verbatim
  project/          the build system the generated tree needs, copied verbatim

overrides/ is small and load-bearing. include/Ints.h is the dual-build switch
itself, so it cannot select itself away; the override is single-branch C++20.
The bit primitives and tile blitter are ordinary reconstructed C++ in the
matching tree and pass through the normal generator.

project/ is deliberately NOT this repository's flake: that one provisions wine,
MSVC 4.2, Ghidra, objdiff and the delinker, and the published branch must need
none of it. Its flake knows only clang and ninja.

Output feeds source-pol-2.0 and classic-pol-2.0, and through the former,
master-pol-2.0 and ironfist-pol-2.0.
"""

/*
Function: game::NewGame
PoL RVA: 0x000b769e
Buka RVA: 0x00075b4b

Retained PoL source
-------------------
    strcmpi(gpGame->m_mapFilename, "brokena.mp2")
    strcmpi(gpGame->m_mapFilename, "arrax.mx2")

Complete clean VC4.2 product
----------------------------
Axes: build/newgame-crt-name-axes.json
Results: build/source-variant-batch/newgame-crt-name/results.json
Coverage: 3 x 3 = 9/9.

Both sites select the direct-target spelling:
    strcmpi / strcmpi       96.237760%, 2752 bytes, 156/156 relocs
    one direct spelling     96.229020%
    no direct spellings     96.220280%

Raw candidate COFF identities:
    `_strcmpi(...)` -> `__strcmpi`  (old PoL source)
    `strcmpi(...)`  -> `_strcmpi`   (PoL delinked retail identity)
    `stricmp(...)`  -> `_stricmp`   (exact Buka source)

Buka's `stricmp` is independently pinned by its VC6 OLDNAMES/Rich producer
census (`docs/matching/CRT-alias-link-order/source-spelling-matrix.cpp` in the
Buka tree). That link-order result is version/compiler-specific. PoL adopts the
same semantic source correction without the erroneous leading underscore, but
uses its own target-correct `strcmpi` dialect spelling. The function's larger
95/99-block structural residual is outside this matrix, so no TU-state census
is claimed here.

Disposition: matching decomp port with target-specific CRT spelling; no logic
change.
*/

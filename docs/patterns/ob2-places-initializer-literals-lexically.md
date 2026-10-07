# `/Ob2` places initializer literals at their lexical position

Measured with the pinned VC6 SP5 compiler, 2026-10-07 (probes under
`build/exp/acc`, `build/exp/ob2`).

## Signature

A file-scope pointer initialized with a string literal sits among the
object's globals, but its text sits among the function literals at the
pointer's lexical place (retail EDITMGR: `gMapCodeLetters` at 0x47d738, its
`"ABC...Z"` at `MakeMapCode`'s place, 0x47e018). The ordinary build puts the
text ahead of every function literal.

## Observation

`/Od /MT /Gr /G5 /Gi- /GX`, no `/Gf`:

```cpp
int gA = 1;
void f1(void) { puts("AAA1"); }
char* P = "ZZZ9";
inline char* L(void) { return P; }
char mk(int i) { char* p = L(); return p[i]; }
int gB = 2;
void f2(void) { puts("CCC3"); }
```

| Inline option | `.data` order |
| --- | --- |
| `/Ob0`, `/Ob1`, none | `gA P gB` `ZZZ9` `AAA1` `CCC3` |
| `/Ob2` | `gA P gB` `AAA1` `ZZZ9` `CCC3` |
| `/Ob2` + `#pragma auto_inline(off)` at the top | `gA P gB` `ZZZ9` `AAA1` `CCC3` |

`#pragma auto_inline(on)`, `inline_depth` and `inline_recursion` under `/Ob1`
change nothing. The `$SG` numbering is the same in every row; only the
emission point of the initializer's literal moves. Without auto-inlining the
front end emits file-scope initializer literals as a group after the
globals (`docs/compiler-re-allocation-order.md` section 5); with
auto-inlining active they are emitted in lexical order with the function
literals. The pointer object stays among the globals either way.

At `/Od`, `/Ob2` changes no code: every one of the editor's 66 C++ units
compiled with `/Ob2` in place of `/Ob1` keeps byte-identical `.text`,
`.bss`, `.rdata` and EH sections. Only `.data` moves, and only in EDITMGR
among the editor's own sources (no-`/Gf` `/Gy` BASE units also merge their
per-function literal sections into the main `.data`).

## Inference

- The editor project compiled its own sources with `/Ob2` ("any suitable"
  inline expansion): EDITMGR's `.data` then equals retail byte for byte, and
  every other editor-own object is unchanged. `homm2.manifest.unit_flags`
  applies `/Ob2` to the EDITOR tier for the editor image.
- BASE's Misc is one translation unit compiled with `/Ob2` (profile
  `base_nogf_ob2`). `gcCDTrackName = "\\Tracks2\\..."`, `giChangeThreshold`
  and `iLastSeed` are defined between IsCDDrive and DriveSupportsFreeSpaceQuery
  (their first users follow), so the pointer and the tables lead Misc's
  `.data` while the track text sits between the two halves' literals, in
  both programs. This replaces the earlier two-unit split
  (`docs/matching/Misc-track-name-data/owner-split.cpp` at `f0ae961d2`): the editor's Rich
  header counts one C++ object per `ctype` registration (65 and 65), which
  the split could not meet, and both linked images are byte-identical with
  the one-object Misc.
- Which other units shared the setting is not decided by this evidence: a
  unit without a file-scope initializer literal after a function literal is
  indifferent to it in its linked bytes.

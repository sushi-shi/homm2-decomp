# Producer records follow loaded objects, including discarded contributions

Measured on 2026-09-27 in `matcher-3` (`matcher/bss-dimmer`), with cwd,
branch, and `HOMM2_DIR` verified in the pinned Nix build shell. No reconstructed
source, compiler object, or image was modified. The probes use ordinary VC6 SP5
compilation, LIB, and LINK only.

The earlier [producer census](../link-input-ownership/rich-producer-census.cpp)
already established that empty direct objects and extracted data-only objects
increment the compiler producer count. This follow-up measures duplicate COMDAT
selection and archive extraction followed by removal of unused functions.

## Six native links

All source files use the production DIMMER compile flags. Every raw object has
`@comp.id = 0x000b2306`. Links use `/ENTRY:entry /NODEFAULTLIB
/SUBSYSTEM:CONSOLE /INCREMENTAL:NO`; the table gives the explicit `/OPT` choice.
Rich records are decoded directly from each generated PE by locating `Rich`,
reading its XOR key, and validating the decoded `DanS,0,0,0` prologue. No header
rewriting or producer suppression is involved.

| Link | Inputs and selection | `/OPT` | C++ producer count | `.text` virtual size |
|---|---|---|---:|---:|
| `template-only` | Entry and explicitly instantiated `Box<int>::value` in one object | `NOREF` | 1 | 26 |
| `direct-duplicate` | Same entry object plus a second object containing only the same instantiated method | `NOREF` | 2 | 26 |
| `unextracted-duplicate` | Same entry object plus an archive containing the duplicate object | `NOREF` | 1 | 26 |
| `extracted-kept` | Entry object also contains an unused caller of `extra`; archive supplies `extra` | `NOREF` | 2 | 42 |
| `extracted-discarded` | Same caller and provider archive, with unused functions discarded | `REF` | 2 | 10 |
| `unextracted-provider` | Entry without the unused caller, plus the same provider archive | `REF` | 1 | 10 |

All six links succeed. Complete emitted section records—name, virtual size, RVA,
and raw bytes—are identical across the first three links. They are also identical
between `extracted-discarded` and `unextracted-provider`.

Therefore a loaded object still contributes a producer record when its only
COMDAT loses duplicate selection, or when every one of its functions is later
removed. An unneeded archive member avoids the record precisely because it is
never loaded. These paths cannot make additional necessary source owners free of
their compiler-input-count effect.

The shared template source is:

```cpp
template<class T> struct Box { static int value(); };
template<class T> int Box<T>::value() { return sizeof(T); }
template struct Box<int>;
```

The entry object appends `extern "C" int __cdecl entry() { return Box<int>::value(); }`.
The duplicate object contains only the shared template source. The archive-removal
pair instead uses these ordinary definitions:

```cpp
// Caller object:
extern "C" int __cdecl extra();
extern "C" int __cdecl unused() { return extra(); }
extern "C" int __cdecl entry() { return 4; }

// Provider object:
extern "C" int __cdecl extra() { return 17; }
```

An initial explicit *free-function* template probe triggered VC6 C1001 in the
unreferenced duplicate TU. Its rejected source and compiler log are retained;
it supplied no link result and is not counted among the six successful links.
The class-template probe above avoids that compiler rejection.

## Existing module-ownership evidence

The current isolated integration manifest contains **98 units: 96 C++ and two
assembly inputs**. Reading the actual available PoL 2.0 NB09 module directory
finds 498 module records. Case-insensitive object basenames match **94 of the 98**
current unit names. The four without a matching predecessor module are:

- `BASE/AudiereEffects`
- `BASE/AudiereMusic`
- `BASE/MilesSound`
- `BASE/DIMMERDestructor`

This is a historical ownership prior, not proof that Buka 2.1 retained every
2.0 translation-unit boundary. It identifies no additional artificial fragment
with independent evidence for consolidation. The already reviewed
[MusicFlags/Midi consolidation](../Midi-owner-boundary/music-state-prefix.cpp)
and Misc ownership recovery are known cases, not new opportunities counted here.
No arbitrary merger is proposed merely to adjust Rich metadata.

The source image was
`/home/sheep/Projects/homm2/investigation/extracted/img-pol/HEROES2W.EXE`, SHA-256
`bc8f362dd49216c9fbcee1eb2e0429b467082a93330dd84ff95757c0182fc8a3`.
The manifest snapshot and its hash accompany the census, so later source changes
do not silently change what the 94/98 figure describes.

## Artifacts

All artifacts live under matcher-3's `build/link/producer-selection-audit/`:

- `run.py`, `run.log`, `results.json`, `section-comparisons.json`
- Ordinary probe sources, untouched objects, archives, six executables, MAPs,
  and compiler/LIB/LINK logs
- `rejected-function-template.cpp` and `.log`
- `module_audit.py`, `module-provenance.json`, `units.snapshot.toml`

The existing LINK decompilation at root's `build/link/link6-re/00461b20.c` is
consistent with the measurements: its COFF symbol walk recognizes absolute
static `@comp.id` and stores the value in the module record (`param_2[0xc]`).
That excerpt alone does not prove when the final Rich census is accumulated;
the native probe results above establish the observed discarded-object behavior.

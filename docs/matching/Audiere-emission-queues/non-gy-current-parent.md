# Non-/Gy layout on the current constructor-specialization parent

Measured2026-09-27 in matcher-2, `matcher/bss-audiere`, with cwd and
`HOMM2_DIR` verified in the persistent Nix shell. This is a bounded two-state
build comparison on the actual115 source/header parent, not a source mutation.
The earlier minimal-rebuild probe in `ownership-forms.cpp` used the plain Node
parent and examined a transition back to /Gy. It did not establish the current
resource-template/concrete-constructor layout with function packaging disabled.

## Compiler dialect and complete controls

`build/audiere-gy-parent/probe.py` first compiles unchanged production flags and
then appends `/Gy-`. The latter is **not** a valid disabling switch in this
pinned VC6 compiler: it reports D4002, `ignoring unknown option '/G-'`, and keeps
the packaged topology. That diagnostic state is preserved as `no-gy/` and must
not be cited as an actual disabled-function-packaging result.

`omit.py` then compiles the actual disabled state by omitting `/Gy` from the
otherwise identical production flags. This successful, warning-free state is
`omitted-gy/`. All three compilations completed without timeout. Source and
header bytes, include search, compiler, and other flags are unchanged; each
invocation has one C++ input. No source, original compiler, root worktree, or
production manifest edits were made.

## Individual body and relocation audit

`audit.py` reads the raw COFF symbol and relocation tables. For each defined
function it starts at the symbol's actual offset and ends at the next function
symbol in that section, or at the section end. It does **not** attribute an
entire shared text section to every function. All resulting spans exactly match
the corresponding ordinary /Gy function sizes; no inferred padding is trimmed.

All19 defined functions match in size, every relocation-masked byte, and
complete ordered relocation source offsets/types/target identities/raw addends.
That covers all12 public functions and the seven helpers. The Play handler is
located through its actual+6 relocation, and its actual xdata target follows;
both EH sections match in bytes and ordered semantic relocations. All data
sections match, including section flags, symbol storage classes/offsets, bytes
and ordered relocations. Anonymous EH labels are compared by their actual
handler/xdata owner and offset. Calls within the shared text section remain
ordinary COFF relocations, so no resolved-call reconstruction is needed here.

The ordinary `@comp.id` symbol remains one absolute STATIC symbol with value
729862 (`0x000b2306`) in each object. This flag change does not add a C++ input.
It does not by itself prove final executable headers or producer counts.

## Raw layout result

Without /Gy, ordinary public bodies and ctype G/R share section4 `.text`,
size2041 (`0x7f9`). Its raw per-function starts are:

| Body | Offset | Size |
| --- | --- | --- |
| Purge | 000 | 162 |
| Find | 162 | 03a |
| Play | 19c | 39c |
| Playing | 538 | 03c |
| Stop | 574 | 070 |
| SetVolume | 5e4 | 063 |
| Wait | 647 | 062 |
| StopAll | 6a9 | 077 |
| SetAllVolumes | 720 | 06b |
| BeginIteration | 78b | 012 |
| EndIteration | 79d | 012 |
| IterationActive | 7af | 011 |
| G, guarded initializer | 7c0 | 027 |
| R, registration | 7e7 | 012 |

Offsets and sizes above are hexadecimal. The remaining code contributions are:

| Section | Body | Size |
| --- | --- | --- |
| 7 | S, OutputStream RefPtr destructor | 02c |
| 8 | A, OutputStream RefPtr assignment | 051 |
| 9 | AudioDevice RefPtr destructor | 02c |
| 10 | N, Node destructor | 02c |
| 11 | ctype id cleanup | 005 |

These template/inline helpers remain separate COMDATs. The relevant projection
therefore changes from `S A N G R` to **`G R S A N`**, not retail `S A G R N`.
All raw section flags, symbols, contribution offsets and body records are kept
in `raw-topology.json`; `omitted.asm` is the complete disassembly/relocation
listing for the disabled state.

## Two remaining contradictions

N does move after G/R, but S/A also move after them. Independently, the ordinary
functions become packed without their retail inter-function gaps. Find begins
at Purge+0x162 instead of+0x170; Play begins at+0x19c instead of+0x1b0. Those
relative starts are inside one indivisible section contribution. Merely
relocating or reordering that contribution cannot restore its internal gaps.
No full native link or new source family was run: the measured build state
already exposes both mismatches, and the assignment requested review before
expanding into descendants. This is preserved as a lower build state, not an
exact executable or a universally excluded no-/Gy source family.

Artifacts: `build/audiere-gy-parent/{probe.py,omit.py,audit.py,results.json,
provenance.json,raw-topology.json,audit.json,audit.log,omitted.asm}` and each
control object/log. No flag is retained in the build configuration.

## Alignment-descendant coverage review

Before considering expansion, the following existing DIMMER dossiers were read
from the isolated root matcher-4 checkout. They concern the same pinned compiler
mechanisms, but are **not** counted as newly measured Audiere source states:

- [native-alignment-matrix.cpp](../dimmerWidget-destructor/native-alignment-matrix.cpp):
  complete32-arm `/G3`–`/G6` × `/Od`, `/Od /Og`, `/Od /Os`, `/Od /Ot` × two
  destructor ownership forms. `/G3`–`/G5` with `/Od` or added`/Ot` retain packed
  starts; `/G6` changes a constructor and still packs starts. `/Og` introduces
  16-byte starts but changes every measured body. `/Os` changes bodies without
  restoring the required boundaries.
- [phase-optimization.cpp](../dimmerWidget-destructor/phase-optimization.cpp):
  driver phase traces distinguish global`/Og` from C2-only controls. The latter
  preserve both bodies and the wrong packed starts. This does not isolate an
  independently useful function-alignment setting.
- [pragma-body-alignment.cpp](../dimmerWidget-destructor/pragma-body-alignment.cpp):
  complete12-arm ownership/global-optimization/pragma product. Pragma-off arms
  restore exact bodies **and** packed starts; optimizing controls align starts
  but change bodies. The dossier also independently records the rejected
  `/Gy-` spelling and implicit `/Gy` effect of literal`/O2`.
- [backend-alignment-options.cpp](../dimmerWidget-destructor/backend-alignment-options.cpp):
  bounded help/option-table inspection found a boolean `bzalign` control;
  six measured source/option arms leave function records and starts unchanged.
  Section alignment is distinct from padding between functions inside a shared
  section. No numeric public function-alignment option was found in that audit.
- [older-frontend-no-gy.cpp](../dimmerWidget-destructor/older-frontend-no-gy.cpp):
  the available RTM/SP5 frontend combinations preserve exact bodies but retain
  packed starts. Its corrected provenance identifies the purported SP3 package
  as a duplicate RTM binary; distinct SP3 remains unavailable and unmeasured.

The new Audiere state reproduces the same concrete mechanism: ordinary bodies
share one contribution with no internal gaps, while template COMDATs keep their
own contribution boundaries. No additional compiler-mode hypothesis follows
from this parent that the existing alignment work has not already addressed at
the mechanism level. Consequently no new broad flag matrix was run. This does
not claim byte-for-byte measurement of `/G6`, `/Og`, or `/Ot` on Audiere, nor
exclude every possible source/build descendant. Independently, repairing public
gaps would still leave S/A after G/R in this measured no-/Gy owner arrangement.

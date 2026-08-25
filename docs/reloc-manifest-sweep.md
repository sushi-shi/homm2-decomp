# The DIR32 site manifest

`config/delink_relocs.tsv` substitutes for the base-relocation directory the
retail image does not have. It is generated, not hand-maintained:

```sh
homm2 audit reloc-sweep            # report the yield, write nothing
homm2 audit reloc-sweep --write    # replace the manifest, then `homm2 redelink`
```

The rules live in the delinker's own `scripts/find_relocs.py`, because nothing
about them is specific to this target. The build shell exports
`VOSTOK_DELINKER` to the pinned delinker source; point it at a checkout instead
if you are iterating on the script.

Target-specific overrides proven by candidate relocations, payload, and semantic
field layout live in `config/delink_reloc_inclusions.tsv` and
`config/delink_reloc_exclusions.tsv`. The wrapper applies those reviewed sets
before comparing or rewriting `config/delink_relocs.tsv`, so a future generic
sweep cannot drop a proven packed pointer or silently reintroduce a known
non-relocation.

## Why generated and not donated

The first channel was `homm2 audit reloc-donation`: a claimed function whose
compiled bytes are already masked-identical to retail donates the relocation
offsets its own object records. That is sound, and every row it ever produced
is reproduced by the sweep — but it is circular. A site arrives only *after* the
function is exact, so it can never be what makes a function exact, and the data
sections, where there is nothing compiled to compare against, cannot start at
all.

The cost was not theoretical. Before the sweep the delinked objects carried
14,349 DIR32 relocations against roughly 31,000 real sites, and 548 of the 1,166
claimed functions with any DIR32 site were being diffed against a retail body
that had bare literals where our object had relocations. `GetBitmap` is the
clean example: its only differing row was a `reloc DIR32 $L57506` present on our
side and missing on retail's — a permanent diff no source shape could close.

The sweep is site-anchored instead. It identifies a relocated field from how the
image encodes it and never consults a symbol, so it has nothing to bootstrap
from. Donation stays only as a cross-check (a non-empty `--write` merge now means
the sweep *lost* a site) and optional review of interior owner/addend aliases.
It writes no generated identity input. Source claims and explicit fail-closed
providers feed the symbol inventory directly.

## The rules, and what they are worth

PoL 2.0's `HEROES2W.EXE` is the same program built by the same toolchain family
and still carries its `.reloc` directory: 28,801 HIGHLOW sites, 25,930 `.text` /
76 `.rdata` / 2,795 `.data`. Neither image links MFC and the section sizes track
each other closely, so it is both a per-section expectation for this target and
a labelled set to score the rules on before they are pointed at a stripped
image:

```sh
find_relocs.py /path/to/HEROES2W.EXE --validate
```

| channel/target          |  sites | precision |
| ----------------------- | -----: | --------: |
| `code/data`             | 20,476 |    1.0000 |
| `switch/code`           |  2,531 |    1.0000 |
| `data/literal-start`    |  1,915 |    1.0000 |
| `code/literal-start`    |  1,794 |    1.0000 |
| `code/literal-interior` |    685 |    1.0000 |
| `data/literal-interior` |    513 |    0.9883 |
| `code/code`             |    438 |    1.0000 |
| `data/data`             |    181 |    0.9227 |
| `data/code`             |    135 |    0.8889 |

Overall 0.9988 precision at 0.9942 recall. Every class the code channel produces
was exact; the residual error is entirely in the data channel, and the 168
missed sites are dominated by unaligned pointers inside packed CRT tables that
an aligned scan does not reach.

Two classes are rejected by default: `unmapped`, and `data/code-isolated` — a
data dword pointing at code with no adjacent candidate dword. Pointer arrays are
contiguous, so an isolated code pointer measured 0.07 against real `.reloc` where
one inside a run measured 0.89.

`--function-starts` is deliberately **not** used here. It replaces the isolation
test with an inventory lookup, and `config/retail_functions.csv` is Ghidra
analysis output, not retail evidence: only 63% of this image's `.rdata`
code-pointer slots land on a boundary it knows. Measured against a comparably
partial inventory on PoL it raised precision to 0.9993 while silently rejecting
every real `.rdata` vtable slot, because a callback need never be called
directly.

## This target

30,626 sites, against PoL's 28,801 for the same program. The excess is expected:
VC6 pools vtables in `.rdata` where MSVC 4.2 kept them in `.data`, so this
image's `.rdata` is 0x4c26 against PoL's 0xc9d, and the vtables sit interleaved
with the `__real@` constant pool (the three-slot vtable at RVA `0xea42c` that
`executive::executive` installs is the first of them).

Delinked DIR32 relocations went 14,349 → 26,214. The gap to 30,626 is the data
sections: the delinker only materializes a data section for a reviewed data
manifest row, and there are none yet, so those sites convert as the data
campaign lands `DATA()` markers.

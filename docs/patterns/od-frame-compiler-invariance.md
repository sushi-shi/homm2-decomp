# The /Od frame layout is identical in MSVC 4.2 and VC6 SP5

Measured 2026-08-03 by compiling the same probes through both pinned compilers
(`build/toolchain/msvc/bin/CL.EXE` here and in the `decomp-pol-2.0` worktree) with
`/Od /MT /Gr /G5 /Ob1 /Z7` and reading `S_BPREL32` out of `.debug$S`. 4.2 emits
CodeView 4 records (`S_BPREL32 = 0x0200`, 16-bit type index), VC6 emits CodeView 5
(`0x1006`, 32-bit type index); same fields, different widths.

| probe | result |
|---|---|
| 10 long identifiers spanning distinct buckets, declared in REVERSE predicted order | identical |
| 16 short names spanning buckets 0..15, declared in reverse | identical |
| 4 names all in one bucket | identical (reverse declaration order, LIFO) |
| `int costs[7]; int resource;` | identical - arrays are NOT privileged |
| `for (int resource = 0; ...)` | identical - for-init merges with the enclosing block |
| explicit `{ }` inner block | identical - inner group sits BELOW the outer group |
| two sibling loops, each with its own for-init counter | identical - each scope its own group |

Refined 2026-08-03 (`homm2 audit od-oracle`, VC6): a for-INIT declaration merges
into the enclosing block and is indistinguishable from hoisting it to the top of
that block - `for (int loopy..) ..; for (int alpha..) ..;` lays out
`alpha(1) total(7) loopy(10)`, bucket-sorted across the whole function, exactly as
the hoisted form does. It is the loop BODY braces that make groups: with the
counters declared inside the bodies the same function gives
`total(7) i(9) j(10) | loopy | alpha`, two sibling groups laid out one after the
other (`loopy` at -16, `alpha` at -20 - sibling scopes do not share storage). So
`for (i32 i = ...)` is NOT a scope lever; `for (...) { i32 i; ... }` is.


So every axis of the model in `homm2/core/od_slots.py` holds on both compilers:
bucket from `key16(name) & 0xF`, within-bucket LIFO by declaration, lexical scope
grouping outermost-shallowest, register-argument spills below all locals.

## Differential fuzz: no construct makes them disagree

Hand-picked probes only test what someone thought to try, so the space was fuzzed
(`scratchpad/framefuzz.py`): randomly generated function bodies compiled through
BOTH compilers and their local orders diffed. Varied across trials - scope shape
(`{}`, `if`, `else`, `while`, `do`, `for`-body, `switch`-case), sibling versus
nested groups, declaration position, `goto` jumping across scopes, multi-declarator
statements (`int a, b;`), `static` locals, references, `const`, initialised versus
not, and types from `char`/`short`/`int`/`long`/`float`/`double` through arrays,
structs, pointers and an object with a constructor and destructor.

**110 shapes compiled by both compilers, 0 divergences.** A separate pass confirmed
**0 asymmetric compiles** - every rejected shape was rejected by both, so no
construct is accepted by one front end and not the other.

Flags were then tested as a variable, since the two builds do not share them
(PoL base is `/Od /MT /Gr /G5 /Ob1 /QIfdiv`; Buka base is
`/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT`). `/GX` was the prime suspect
because exception state occupies the frame. It changes nothing here:

```
4.2 PoL-base     resource@-0x4  costs@-0x20
VC6 no /GX       resource@-0x4  costs@-0x20
VC6 Buka-base    resource@-0x4  costs@-0x20     <- /GX makes no difference
```

All three real configurations produce the PoL layout, and none reproduces Buka's
`costs@-0x1c resource@-0x20`. Adding the lexical block does. So for this function
the block is not one available explanation among several - it is the only one left
standing after the toolchain was ruled out empirically.

## Which flags move the frame at all

Testing only the flags known to differ can only confirm what was already suspected,
so ~55 flags were swept against four body shapes on both compilers
(`scratchpad/flagsweep.py`). Exactly three do anything, and all three behave
IDENTICALLY on 4.2 and VC6:

| flag | effect | when |
|---|---|---|
| `/GX` | every local shifts 12 bytes deeper | only when a local has a destructor |
| `/Zp1`, `/Zp2` | shift +4 | only when a local is an aggregate |
| everything else | none | - |

`/GX`'s mechanism is visible in the prologue: it pushes the SEH registration
record before reserving locals, so `sub esp` is unchanged but every local moves.

```
without /GX   push ebp; mov ebp,esp; sub esp,0x10          alpha@-0x4
with    /GX   push ebp; mov ebp,esp; push -1; push 0;
              mov eax,fs:[0]; push eax; mov fs:[0],esp;
              sub esp,0x10                                  alpha@-0x10
```

This matters because Buka's base carries `/GX` and PoL's does not, so a function
with a destructor-bearing local has a 12-byte-deeper frame in Buka than in PoL.
Our build does compile `/GX`, so we inherit the shift automatically - but only if
our source declares the same objects. It is a small vein in practice: only four
non-exact functions show the `+12` frame signature.

`/Gy` is measured and changes NOTHING. Reaching that took two harness fixes worth
recording, because both failure modes are silent:

- Under `/Gy` each function gets its own COMDAT `.debug$S` in ADDITION to the
  module-level one, and **only the module-level section carries the 4-byte
  CodeView signature**. A per-COMDAT section begins directly with a record
  (`2b 00 0b 10` = length 0x2b, `S_GPROC32`). Skipping four bytes unconditionally
  desynchronises the walk and yields nothing - and it is precisely the
  per-function debug info that matters. Parse both ways and keep whichever
  decodes.
- An earlier sweep reported that emptiness as a `/Gy` REORDER on every body,
  because "no locals read" compared unequal to the baseline. An unreadable frame
  is UNKNOWN, not different; conflating the two manufactures a finding.

A mis-assigned `/Zp` in `config/units.toml` would shift every aggregate-bearing
frame in a whole TU at once, so it is worth checking before blaming source on such
a unit.

**Consequence:** "the compiler changed" is not available as an explanation for any
frame residual. Two flags do change frames, but identically on both compilers, so
they explain absolute offsets - never a difference between the two builds beyond
`/GX`'s uniform shift. Everything else is evidence about the source.

Useful filter that fell out of the sweep: of the non-exact functions, **299 have
exactly retail's frame SIZE**. Their problem is ordering or content, not a missing
or extra local, so frame-size levers are wasted on them.

## Why this matters

CLAUDE.md hedges that the model "was solved against MSVC 4.2 (cl 10.20)" and that
every prediction is "a hypothesis until the od-frames/od-oracle harness re-validates
it against VC6". For frame layout that re-validation has now happened and the answer
is that nothing changed. `homm2 audit od-oracle` reproduces any of these on demand.

The consequence is the useful part. Because the algorithm is invariant, a frame-slot
difference between our compile and retail can NEVER be explained by "the compiler
changed". The inputs are the declared names, their declaration order, their lexical
scope, and the set of declarations - all of them source facts. So a slot mismatch is
evidence about the SOURCE, and a slot mismatch between PoL 2.0 retail and Buka 2.1
retail for the same function is evidence that the 2.1 source differed.

Worked case: `??0recruitUnit@@QAE@PAVarmyGroup@@HPAF@Z` is `/Od` in both builds,
both retail frames are `0x24` with `this` at `-0x24` and the same item set (one
`int[7]`, one `int`), and the 2.0 tree's source is character-identical to ours and
matches 2.0's retail at 100.0. Yet 2.0 retail puts the array at `-0x20` and the
scalar at `-0x04`, while Buka puts the array at `-0x1c` and the scalar at `-0x20`.
Invariant compiler plus identical source cannot yield two layouts, so the 2.1
declarations differ.

## The trap this creates

Renaming a local until the buckets line up reproduces retail's bytes, but many
different names share a bucket, so the match is SUFFICIENT and never UNIQUE. Such a
rename is an unresolved claim about what the developers wrote, not a recovery of it,
and belongs in `docs/cross-version-spellings.md` as an OPEN row - not in the source
as a settled fact.

Levers that carry no naming claim, and are therefore preferable:

- swapping two SAME-BUCKET declarations (changes only their order)
- moving a declaration between lexical scope groups
- declaring a local retail's frame allocates and ours does not

## Resolved: the 2.1 declarations differ by LEXICAL BLOCK, not by name

The systematic cause was found the same day - see
[od-slot-scope-groups](od-slot-scope-groups.md) for the layout rule it exploits.
2.1 wraps a run of statements in `{ }` where 2.0 did not, which drops the locals
declared there into a deeper scope group. That is an ordinary source edit and needs
no per-function narrative, which is exactly what a rename story lacked.

`??0recruitUnit` is the proof. `bucket(resource)=3 < bucket(costs)=12`, so NO
declaration order can put the array first and a rename looked forced. It was not:

```
2.0 retail    resource @-0x04   costs @-0x20      <- flat function scope
Buka retail   costs    @-0x1c   resource @-0x20   <- resource inside a { } block
```

One identifier, one compiler, both frames. The earlier `resource` -> `n` rename was
withdrawn and both constructors stay EXACT with the 2.0 identifier and a block.
`advManager::SystemOptions` and `swapManager::{SwapArtifacts,Open}` closed the same
way, no identifier touched.

So the preference order for a displacement-only residual is: scope move first,
then a same-bucket declaration swap, then a missing declaration, and a rename only
when none of those can express the required order - and then as an OPEN row in
`docs/cross-version-spellings.md`, never as a settled fact.

## How far the block explanation actually reaches (measured)

Two-sided census over all 473 functions whose frame currently disagrees with
retail. A row is usable as 2.0-vs-2.1 evidence only when our source is still
token-identical to the 2.0 tree's, the 2.0 line matched its own retail at 100.0,
and the two instruction streams are aligned - so the frame is the only difference:

| | |
|---|---|
| our source has drifted from 2.0's | 351 |
| streams not aligned - a real code difference too | 96 |
| argument set moved / no local flip | 5 |
| **usable 2.0-vs-2.1 evidence** | **21** |

Of the 21, **zero** are declaration-order-alone, **zero** are same-bucket swaps and
**zero** are a single bucket change: all of them need extra lexical groups. But the
group COUNT is the tell, because with enough groups any permutation is expressible,
so "needs more groups" is only an explanation when the number is small:

| groups Buka's order needs | count | reading |
|---|---|---|
| 2 | 1 | a plausible brace |
| 3 | 6 | borderline |
| 4-6, for 5-14 locals | 14 | not a brace story - an arbitrary permutation |

Direction is balanced - 70 locals move shallower, 65 deeper - with no
aggregate-vs-scalar pattern and no consistently-moving local. So the block finding
is solid where it was demonstrated (`recruitUnit`, `SystemOptions`,
`SwapArtifacts`/`Open` - all one extra group, all with ordinary identifiers) but it
is **not** a general account of the remaining population: two thirds of the usable
evidence is an arbitrary reshuffle that no credible brace structure produces. On
that part the cause is per-function, or the premise is wrong for it - and 9 of the
21 are precisely the manufactured-name cases described below, where neither tree
holds a developer identifier and no source fact is recoverable from the frame at all.

One operational consequence. 351 of the 473 are already unusable because our copy
drifted from 2.0's, and campaign renames are a large part of that drift: renaming a
local to fix a frame consumes the very channel that makes frames evidence about the
source. That is a second, independent reason to prefer the order/scope/missing-
declaration levers over a rename.

## Caveat on "the 2.0 source is identical"

That statement carries no information about developer intent for functions whose
locals are already machine-manufactured. `advManager::ValidMove` has 14 locals in
exact ascending bucket order (`southDirection_e`, `directionX_j`, `centerX_p`, ...),
`swapManager::DrawSelector` runs 0..10, and `TestDynamicWindow` is literally
`p q r s t u v w p1 p2` = buckets 0..9. The `_e`/`_j`/`_16` suffixes are
bucket-tuning artifacts, and the 2.0 tree carries the same manufactured names, so
neither tree holds ground truth there. The invariance argument only bites on
functions whose names are plainly natural - which is why `recruitUnit` was the
right case to reason from.

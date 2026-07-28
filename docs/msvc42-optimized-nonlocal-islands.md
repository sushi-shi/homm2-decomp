# MSVC 4.2 optimized nonlocal matching lessons

These are durable observations from optimized (`/O2`) reconstruction. They are
selection rules, not claims that one source spelling is universally original.

## Distant source changes can alter the prologue

Register allocation and scheduling are nonlocal. In `FlipIconToBitmap`, changing
which dim counter was decremented changed the emitted setup/prologue blocks even
though the dim loops occur hundreds of bytes later. A setup shape that was weak
in one body became the highest-fuzzy and best-topology shape after distant loop
ownership changed.

Consequences:

- Do not permanently reject a legitimate setup, declaration order, or helper
  boundary from one parent island.
- Retry surviving large structures after a distant ownership change.
- Build Cartesian products for interacting regions instead of optimizing each
  region independently.
- Permit a temporarily worse parent when it opens a new compiler-state orbit.

## Fuzzy, topology, size, and relocations are independent objectives

Observed source/state islands can independently improve:

- fuzzy score;
- exact block-size count;
- structural frontier;
- total function size;
- relocation count;
- ordered relocation offsets and identities.

Examples from the same effective function campaign included a highest-fuzzy
island with an extra relocation, a lower-fuzzy island with more exact blocks,
and a compiler state with exact retail size but worse topology than a
different state. None was exact.

Select descendants in this order:

1. credible semantics and types;
2. canonical CFG/edge compatibility;
3. block-size topology and structural frontier;
4. complete ordered relocations;
5. retail size and bytes;
6. fuzzy score as a prioritization/tie signal.

Exact closure still requires all retail bytes, retail size, and ordered
relocation identity/addends. Equal relocation counts or exact size alone prove
nothing.

## Source-variable ownership is visible even under optimization

MSVC can coalesce two source locals in one island and preserve their separate
registers or stack publications in another. Important examples:

- Eagerly initializing a literal count at dispatch emitted a copy in the wrong
  block. Leaving it uninitialized and assigning it only in the branch that
  semantically owns the value moved the copy to the retail block and removed an
  excess relocation.
- Letting a copied dim count own loop mutation made two complete dim loops
  block-size exact. Merely changing `i32` to `u32`, or splitting the assignment,
  did not explain the result by itself.
- A direct pixel expression could be closer than explicit `palette` and `pixel`
  temporaries. The temporaries looked more like the instruction listing but
  gave the optimizer different lifetime information.

Prefer semantically truthful ownership first. Then cross plausible scopes,
types, test owners, and mutation owners as independent permuter axes.

## Retail-looking stack stores do not imply `volatile`

Repeated stores of a local temporary can arise from register pressure,
addressability, control-flow joins, and old optimizer behavior. Adding
`volatile` to reproduce those stores can drastically change CFG and size.

In the clipped-literal experiment, `volatile pendingSkip` added a block,
changed flow, and expanded far beyond retail. It was a structural
contradiction. Test ordinary value/reference/pointer/helper ownership and
compiler states before considering qualifiers.

## Declaration order is a compiler-state lever, not semantic evidence

Function-static declaration order changed code generation only when crossed
with particular setup and inner-loop shapes. Historical, semantic, address,
type-clustered, and reverse orders can be useful bounded diagnostic axes.

Do not infer original source order from normalized private symbol spelling.
Do not reorder `DATA(...)` addresses or generated placement metadata to improve
a score. Source declaration order may be permuted; retail addresses remain
authoritative and unchanged.

Storage scope and declaration order are separate axes. Moving a static between
function and file scope can change symbol topology, reachability, and optimizer
state even when every access and address is unchanged. Establish storage scope
from family evidence and raw COFF/relocation behavior, then permute declaration
order within that scope. A source-order experiment never authorizes changing
the retail address attached to a `DATA(...)` annotation.

## Negative results are orbit-local

Two source arms compiling byte-identically do not establish original spelling.
They establish only that the tested optimizer state erased the distinction.
Similarly, a complete Cartesian matrix that is flat or worse rejects its axes
only for that parent source and state family.

This matters because a later change to a distant loop counter, local lifetime,
static scope, helper boundary, or declaration order can make a previously
invisible expression choice observable. Do not repeat flat matrices
indiscriminately, but do retry an old axis when a new evidence-backed parent
changes ownership or lifetime. Record both the parent hash/shape and the state
set so “tried” has a precise meaning.

The converse is equally important: a local-removal arm surviving at the same
bytes does not prove the original lacked that local. At `/O2`, dead aliases,
compound assignments, split cursor increments, and single expressions may all
collapse. Choose among byte-equivalent spellings by semantic clarity and
family consistency until another island makes the distinction observable.

## Relocation placement exposes ownership

Relocation totals are too coarse for optimized reconstruction. In
`FlipIconToBitmap`, moving a literal-count assignment into the branch that
semantically owned it removed an extra relocation and moved the publication
into the retail block. That was stronger evidence than the fuzzy-score change.

When a candidate has the correct relocation target but the wrong source site,
inspect the enclosing block and the source lifetime that caused the address to
be materialized there. The useful axes are usually scope, initialization site,
mutation owner, helper boundary, and shared-tail ownership—not cosmetic
arithmetic rewrites.

## Structural frontiers are clues, not scalar scores

An exact CFG with several size-only blocks is a strong parent because control
ownership is credible, but it is not closure. Track the earliest mismatching
block and also whether a change fixes a later block without regressing earlier
ones. A nonlocal optimizer change can move the first divergence in either
direction, so retain independent best-fuzzy, best-topology, and furthest-
frontier artifacts rather than forcing them into one score.

Exact total size can coexist with worse block boundaries and wrong bytes.
Exact block sizes can coexist with wrong instruction selection. Equal
relocation counts can conceal a wrong ordered site. These measurements select
what to inspect next; only the full byte and ordered-relocation audit proves
the match.

## TU-state probes must retain multiple winners

An unchanged-source census can produce many byte/relocation states with the
same CFG. Retain and inspect separately:

- best fuzzy;
- best topology;
- furthest structural frontier;
- exact-size states;
- exact-relocation-count states.

The best topology state may score lower and have a worse total size than the
fuzzy winner. Layer legitimate source shapes across a reproducible state set;
never retain probe declarations in reconstructed code.

## Generated and normalized artifacts have distinct roles

- Raw compiler objects are not necessarily the copies read by objdiff/sema.
  Use `homm2 build` to refresh normalized objects before quoting results.
- `match_variants.py` and `tu_state_noise.py` restore source and retain
  disposable clue objects under `build/`.
- Generated status/MAX/configuration is script-owned. Never edit it to promote
  an island.
- Attempt source and conclusions belong under
  `docs/matching/<function>/`; generated probe declarations do not.

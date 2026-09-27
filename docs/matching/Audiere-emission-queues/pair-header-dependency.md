# Late std::pair processing: header ownership and output boundary

Measured on 2026-09-27 in matcher-2, branch `matcher/bss-audiere`, using the
existing diagnostic proxy described in `inprocess-proxy-trace.md`. No additional
hooks, compiler modifications, production source edits, or root-worktree writes
were made. This follow-up identifies the actual late pair constructor and tests
its header ownership; it does not propose a retained source mutation.

## Actual source dependency

The exact pinned input chain is:

1. `vendor/audiere-1.9.2/audiere.h:16` includes `<vector>`; line17 includes
   `<string>`.
2. `build/toolchain/msvc/include/VECTOR:246` defines `_Vbase` as unsigned int.
   The concrete `vector<_Bool, _Bool_allocator>` specialization at line249 owns
   `_Vbtype _Vec` (line567), where `_Vbtype` is `vector<_Vbase, _A>` (line254).
3. Its `_Eq` at VECTOR:539 calls `_Vec._Eq(_X._Vec)`. The primary vector's `_Eq`
   at VECTOR:213 calls `equal(begin(), end(), _X.begin())`.
4. `build/toolchain/msvc/include/XUTILITY:28` defines the three-argument `equal`
   template; line30 calls `mismatch(_F, _L, _X)`.
5. XUTILITY:87 defines that `mismatch`; line90 returns
   `pair<_II1, _II2>(_F, _X)`.
6. `build/toolchain/msvc/include/UTILITY:21` defines the two-reference pair
   constructor, initializing real fields `first` and `second`.

The relevant instantiated pair is `pair<const unsigned int*, const unsigned
int*>`, mangled class owner `?$pair@PBIPBI`. It is not the converting constructor
member template or the default constructor. Its arguments come directly from
mismatch's two current input iterators.

The existing actual115 trace connects each call by exact symbol pointer and
current-function context: vector<bool>::_Eq calls vector<unsigned int>::_Eq at
event831; the latter calls equal at2022; equal calls mismatch at3278; mismatch
calls pair's constructor at3518. The pair symbol is78aa60b4, its owner78aa4598.
The latter setup occurs only at event3534, after INIT3527, with scanner queue
insertion3532 and resume3533. Selected source-context events are preserved in
`build/audiere-pair-dependency/actual-chain.log`.

There is no semantic call from ctype initialization to this pair constructor.
The constructor is discovered through the ordinary vector header dependency
chain before INIT and processed by the specialization rescan afterward.

## Complete diagnostic product

`build/audiere-pair-dependency/probe.py` enumerates the full Cartesian product
`<vector> present/absent` × `<string> present/absent`, preserving vendor order
when both are included. Each input contains only those include directives; the
empty arm contains no declarations. All four arms are compiled twice, with the
ordinary compiler and with the existing six-hook proxy, using the production
Audiere /Od /Ob1 /GX /Gy /MT flags. All eight compiles succeeded with no timeout,
no trap cutoff, and no omitted product states. Every paired object is byte equal
except for possible COFF timestamp bytes. Full traces, source, logs, and results
are retained below that directory.

| Header input | Pair constructor setup | INIT entry | Emitted function symbols |
| --- | --- | --- | --- |
| neither | absent | absent | none |
| string | absent | event1998 | three ctype helpers |
| vector | event1303, phase0 | absent | none |
| vector, string | event2667, phase1 | event2660 | three ctype helpers |

The vector+string output has identical raw section records to string alone.
`ownership-audit.json` records that comparison and exact input-header hashes.
The experiment reproduces the post-INIT pair processing without any game
function, synthetic call, dummy storage, or invented template parameter.

## Processing is not emitted code

`coff-inventory.json` inspects every COFF symbol, including undefined symbols,
in all four controls and the actual115 Audiere control. None contains a symbol
for `?$pair@PBIPBI`. Vector alone emits no text functions. Thus this particular
late frontend body does not supply an emitted or linked code contribution.
The actual ordinary object already establishes that Node does emit its 44-byte
body before the ctype helpers; frontend timing alone is not a replacement for
that output evidence.

The Node is needed by the concrete public functions. In the measured parent its
user-defined destructor takes the ordinary member-body setup path returning at
10419607, before INIT. The unused pair takes the specialization-body setup path
returning at10420934 after its late dependency is discovered. Both have kind0
and clear registration bit08; neither observation makes them synthesized pending
kind4 destructors. The shared kind is not a guarantee of identical ownership or
emission behavior.

The static scanner1040ae89 walks class-specialization member records and queues
eligible bodies through1040af2f. Its raw predicates examine symbol+2 bit04,
symbol+1c bits03, symbol+25 bit02, and owner/global force conditions. Those raw
predicates are preserved in the existing compiler RE artifacts; this experiment
does not assign unsupported C++ meanings to their bits.

## Scoped disposition

A real late template-dependency rescan exists, but the observed pair is unused
header processing with no COFF contribution. The evidence does not show how to
move the early-required Node destructor into a late emitted-code owner while
retaining Purge's calls and Play's inline cleanup. Introducing unrelated pair,
vector, or deep generic call chains would not reconstruct that missing ownership.
No new game-source matrix is justified by this route alone. This closes the
bounded dependency audit, not all possible source/build ownership mechanisms.

# VC6 post-initializer queues: corrected bounded audit

Measured from the pinned C1XX.DLL on 2026-09-27 in matcher-2,
`matcher/bss-audiere`. This is a read-only compiler analysis; no game-source
mutation matrix or output correction was performed. Binary hashes and raw
instruction extracts are under `build/audiere-emission-re/`.

The 115-byte source parent emits `S A N G R`, where S is the OutputStream
smart-pointer destructor, A its assignment, N the node destructor, and G/R the
39-byte guarded initializer and 18-byte registration wrapper. Its desired
native order is `S A G R N`. All public functions and their ordered relocation
semantics are preserved in that parent. Root localized the remaining 115 image
bytes to the N/G/R span, two Purge call operands, and one CRT pointer.

## Reconfirmed driver ordering

Fresh disassembly of `1040abd0` tests the three queue heads at `1040abda`,
`1040abee`, and `1040abfb`. Nonempty heads branch to their drains. Only the empty
path reaches initializer state checks and `call 1044b373` at `1040adcc`.
After clearing initializer lists, the path reaches:

```
1040ac7e  call 1040ae53
1040ac85  jne  1040ae1a
1040ac8b  call 1040ae89
1040ac92  jne  1040ae24
...
1040ae1a  call 10423efc
1040ae1f  call 10425299
1040ae24  call 1040abd0
```

Thus there are two post-initializer rescan paths, with AE53 taking precedence.
The architecture alone does not establish that every post-initializer function
must come from the template-specialization scanner AE89. The older section
11.5's exclusive-route inference exceeds its own driver evidence.

## Corrected pending-table predicate

`1040ae53` first rejects a nonzero `DAT_104E69FC` or null `DAT_104D7410`.
It has a separate immediate-true path when the first two pointers of
`DAT_104D7534` compare equal; otherwise it iterates the pending table.
At `104b5c76` it sign-extends the byte at item+0x32. The comparison against0x10
at `104b5c7a` is followed by **unsigned** JA to true. For the range0..16,
`104b5c85` indexes the following bytes at `104b5cb4`:

```
kind: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10
byte: 00 01 01 01 01 01 01 01 01 01 01 00 00 00 00 00 00
```

The two dwords at `104b5cac` are:

```
index0 -> 104b5c92  advance iterator, continue searching
index1 -> 1040ae83  mov al,1; return
```

Therefore kinds1..10 return true. Kinds0 and11..16 are skipped. Values outside
that unsigned range also return true (including negative sign-extended bytes).
This is the **opposite** of the selection stated in old section11.2. Exact
binary-derived table bytes, destinations and decoded cases are retained in
`pending-kind-table.json`; `pending-body-cold.asm` records all instructions.
No C++ construct names are assigned to these internal numbers by this table
alone.

## The alternate path can enqueue A/B

`10425299` walks the same pending table and dispatches through
`10425360`/`10425394`. Within the latter, `104253c8..104253dc` computes an index:

```
index = (~DAT_104D7BD8 | 0xbfffffff) >> 30
```

That index is3 when bit30 is clear, or2 when it is set. At
`1042540c..1042544a`, a missing queue item is created and queued via10427303
using `0x104DA6C0 + 8*index`: queueA atA6D8 or queueB atA6D0, not queueC.
The caller's recursion consequently offers a post-initializer route through
A/B. Numeric kinds1..10 have dedicated handling at10425456..10425493, including
special branches for2 and3. Their semantic C++ identities still require proof.

This does not yet demonstrate that the Audiere node can take the route: a
possible compiler route and a source construct that actually selects it are
separate claims. No dynamic trace of the real Node's queue record was taken.

## Pending-list registration and the retained pass

Fresh bounded Ghidra decompilation and instruction cross-references identify
`1041c5bb` as a direct append operation for DAT_104D7410. It accepts an item only
when its low five tag bits are7 or8 and item+0x38 has bit0x08 set. It appends the
item to the counted list and clears that flag, preventing that same registration
from repeating without another state change. `1041b4f9`, a call-expression
construction path, reaches this helper at1041b664 and1041b7df. The initializer
walker itself invokes1041b4f9 for its registered functions, so there is a concrete
way for initializer processing to discover pending callable work. This confirms
a mechanism rather than a new Audiere dependency: the measured ctype initializer
functions do not call or own the node destructor.

The fresh10425299 decompilation also shows a distinct retained subset. Kinds
0xB..0xD are moved to a new pending list while the two segment-table pointers are
unequal; if those pointers compare equal, they dispatch immediately. Kinds0 and
0xE..0x10 are skipped. Kind0x11 dispatches using the item itself as owner; default
cases resolve the owner through the signed index at item+6. This narrows the
meaning of the AE53 filter: it is not a general "all pending items" predicate.
The retained subset does not trigger it while that segment state remains active.
It would be premature to identify the retained subset as ordinary destructors;
no source-to-kind mapping has been demonstrated.

The scratch project and15 bounded function decompilations are retained in
`build/audiere-emission-re/`. Whole-program analysis was stopped after an
unrelated giant function consumed the pass; selected addresses were then
explicitly disassembled and decompiled, each with a15-second bound. Decompiled
pseudocode is checked against the raw instructions and table where conclusions
are drawn. `pending-xrefs.txt` records direct binary references to both global
lists, and the full disassembly is retained for further bounded tracing.

## Measured source frontier

The preserved 12-arm primary-boundary matrix, three destructor-kind states,
and six explicit-concrete-destructor states total21 complete source states.
A fresh read-only census (`source-state-phase-census.json`, object hashes
included) finds15 defined node destructors, all before G, and six extern-class
states with no defined N. Those six shrink Play. The concrete-constructor
specialization preserves S/A ordering; relocating the primary destructor body
does not cross initialization. Concrete destructor specialization changes its
position within the pre-initializer output, but moves it earlier. These are
observations of emitted phases, not proof of which internal queue each used.

The pinned standard header defines `locale::id ctype<_E>::id` as a namespace-
scope template static data member. It has no source-local accessor and no
semantic dependency on the Audiere node. G/R and their empty cleanup match95
retail instances by ordered target addresses; the prior owner audit is at
`docs/matching/Audiere-helper-identities/ctype-static-owner-audit.cpp`.
Requiring N from a fabricated initializer, accessor or wrapper would not recover
that evidence and is not a proposed lever.

The bounded result is a corrected compiler frontier, not a universal
impossibility claim. A useful next mechanism investigation is identifying the
producer and semantic kind of the actual Node record inDAT_104D7410, and
whether its eligibility can be delayed without suppressing Play's real inline
cleanup. Neither the corrected predicate nor the tested ownership forms alone
provide a new justified C++ source change.

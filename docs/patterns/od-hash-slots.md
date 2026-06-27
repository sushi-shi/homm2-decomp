# /Od local stack slots follow MSVC symbol-HASH order, not declaration order

**tags:** `topic:regalloc` `topic:stack` `cpp:locals` `toolchain:vc42` `topic:od`

## symptom
A function's instruction *selection* matches the retail byte-for-byte, but every
`mov ...,-0xN(%ebp)` references the **wrong frame offset** — the locals are
permuted (e.g. retail has `i@-0x4, buf@-0x8, j@-0xc` but your build emits
`buf@-0x4, i@-0x8, j@-0xc`). Reordering the C declarations does **nothing**.

## cause
MSVC 4.2 `/Od` assigns frame offsets by walking its **local symbol hash table**,
not by declaration or first-use order. The bucket a local lands in is a function
of its **identifier spelling**, so the frame layout depends on the *names* you
pick, not their order or type. `this` (the spilled thiscall pointer) and the
compiler's own scratch temporaries hash *after* all named locals, so they always
take the deepest slots — which is why leaf functions whose only locals are
compiler temps (e.g. the `delete` double-temp) match automatically, while any
function with named loop counters / scratch pointers needs the names tuned.

The order is a *total order over names*: slot(-0x4) = hash-earliest, then -0x8,
-0xc, ... To match, choose names whose hashes sort into the retail slot order.
Names are arbitrary at the binary level — only the hash rank matters, so a loop
counter may have to be named `nb` and a pointer `i` to land correctly.

## fix
1. Build, disasm both sides, read off the retail slot→role map (which offset
   holds the loop counter, which holds the `new` buffer, ...).
2. Find names in ascending hash order for those roles. Fast probe loop
   (one wine `cl` per try) — keep the function body fixed, `sed` the names,
   grep the resulting `-0xN` offsets:
   ```sh
   sed -e "s/N1/$a/g;s/N2/$b/g;s/N3/$c/g" tmpl.cpp > t.cpp
   cl /c /Od /MT /Gr t.cpp ; llvm-objdump -d t.obj | grep -oP 'movl .*-0x[0-9a-f]+'
   ```
   Empirically `hash(nb) < hash(i) < hash(j)`; assign roles so the slot you
   want at -0x4 gets `nb`, the next gets `i`, etc.
3. A retail frame that reserves an **unused** slot at -0x4 (and spills `this`
   to -0x8) means the original had one declared-but-unreferenced local — add a
   throwaway `int n;` to reproduce the reservation.

## example (fullMap::GetNewCellExtraIndex)
```cpp
// retail frame: loop1@-4, newbuf@-8, loop2@-c  ->  names sort to match
int nb;            // loop1 counter  -> -0x4   (NOT semantic; hash-chosen)
mapCellExtra *i;   // operator new   -> -0x8
int j;             // loop2 counter  -> -0xc
```
With the natural `int i; mapCellExtra *nb; int j;` the `new` pointer grabs -0x4
and the loop var is pushed to -0x8 — a guaranteed miss. Swapping the *names*
(not the order) is what fixes it. This made GetNewCellExtraIndex, and (with a
dummy local) Init, byte-identical.

## note
This is the dominant /Od matching cost: instruction selection is otherwise very
literal and easy to hit. Any multi-local function will need a name search. A
worthwhile infra investment is reversing the exact MSVC 4.2 identifier hash so
names can be *computed* rather than brute-forced (see report).

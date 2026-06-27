# /Od local stack slots follow MSVC symbol-HASH order, not declaration order

**tags:** `topic:regalloc` `topic:stack` `cpp:locals` `toolchain:vc42` `topic:od`

## symptom
A function's instruction *selection* matches the retail byte-for-byte, but every
`mov ...,-0xN(%ebp)` references the **wrong frame offset** — the locals are
permuted (e.g. retail has `i@-0x4, buf@-0x8, j@-0xc` but your build emits
`buf@-0x4, i@-0x8, j@-0xc`). Reordering the C declarations does **nothing**.

> ⚠️ **objdiff's fuzzy% HIDES this.** A differing displacement (`-0x4` vs `-0x10`)
> is scored as a *partial* instruction match, not a miss — so a function with EVERY
> local on the wrong slot can read **97%+ fuzzy** and look "almost done" when it is
> structurally wrong. ALWAYS confirm by diffing with the **`(%ebp)` displacements
> visible**: disasm both objs, normalize jump/call targets only, KEEP `-0xN(%ebp)`.
> If the offsets don't line up, it's this bug — fix the NAMES, not the logic.
> (On GetNewCellExtra* the locals were fully permuted behind a 97% fuzzy until
> diffed this way.)

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
literal and easy to hit. Any multi-local function will need a name search.

## SOLVED — the exact hash (no more brute force)
Reverse-engineered from the toolchain itself (read, not guessed): the identifier
lexer in **C1XX.EXE `FUN_0041d1a2`** computes, as it scans each identifier:

```c
unsigned h = 0;
for (char *p = ident; isidchar(*p); p++)        // class table DAT_004b41c0[c] & 0x10
    h = (h >> 4) + h*4 + (unsigned char)*p;     // SHR 4 ; LEA +EBP*4 ; LEA +char
unsigned key = (h ^ (h >> 16)) & 0xffff;        // 16-bit fold, stored in name record +0x8
```

The back end (**C2.EXE `FUN_0044bb5e`/`FUN_0044c4df`**) keeps a 16-bucket per-scope
table; a local lands in `bucket = key & 0xF` and is **head-inserted (LIFO)**. The
frame layout walks bucket 0..15, each chain newest-first → slot order is

```
sort locals by (bucket ascending, declaration index DESCENDING)
```

The `>>16` fold only bites once `h >= 2^16` (~5+ chars), which is why naive
rolling-hash guesses fit short names but not long ones. Verified byte-exact vs the
retail compiler on 146 single-name buckets and 40 random multi-name layouts.

**Use `scripts/od_slots.py`** — pure function, no compiler in the loop:
`bucket(name)`, `slot_order(decl_names)`, `predict_offsets()`, `solve(n)`, and
`solve_layout(slots)` (name a target layout from a role/candidate list). This
replaces the per-function wine-`cl` name search entirely;
`scripts/od_oracle.py` verifies against the real compiler.

**Full model** — including the per-scope 16-bucket tables, the VC4.2 old-for-scope
quirk, no-slot-reuse, and the multi-scope layout algorithm — is documented in
**[../od-stack-layout.md](../od-stack-layout.md)**.

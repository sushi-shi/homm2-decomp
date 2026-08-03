# od-slot-solve

**Symptom.** Function byte-identical except one-byte `[ebp-N]` displacements
swapped between locals (the disp-only class in the graded census; objdiff
fuzzy hides it — diff raw bytes with reloc fields masked).

**Rule (VC6 SP5, re-validated by probe).** /Od slots assign by
`sort(locals, key=(bucket(name), -decl_index))`, first sorted local at -0x4,
each slot rounded to 4 bytes. `bucket = ((h ^ h>>16) & 0xF)` over the
VC hash `h = (h>>4) + h*4 + ch` (homm2/core/od_slots.py).

**Workflow.**
1. Byte-diff ours vs retail (mask reloc fields): collect the displacement
   permutation.
2. Solve name buckets: pick NATURAL names whose buckets order the slots as
   retail wants (`od_slots.bucket`); never ship hash gibberish — enumerate
   plausible 1996 spellings and take a bucket-correct pair/tuple.
3. One rebuild; function flips exact.

Proven: SetAudiereSampleVolume / SetAllAudiereSampleVolumes — retail wants
the float below the node pointer; `convertedVolume`(b15)+`node`(b0) is
impossible, `sampleVolume`(b5)+`sampleNode`(b9) is natural and exact.

## `this` is not in the bucket sort (VC6 SP5, byte-proven)

For a `__thiscall` member, the spilled `this` pointer takes the slot
IMMEDIATELY AFTER the last named local, whatever its own name would hash to,
and the compiler's own temps follow it. `philAI::CheckBerserk` (0x7f41f) is the
witness: `heroPtr` (bucket 12) sits at -0x1c and `this` at -0x20, even though
`bucket("this") == 11` would have placed `this` above it.

Consequence for solving: ignore `this` entirely, solve the named locals against
`retail_slots[0 .. n-1]`, and read `this`'s offset as a *check* that your named
count is right. A frame whose `this` is one slot too deep means the source has
one named local retail left as a compiler temp - see
[compiler-temp-vs-named-local](compiler-temp-vs-named-local.md).

## Suffix tuning keeps the TU's naming style

The PHILAI/GAME family carries machine-picked names of the form
`<meaning><digits>` (`eventCell9`, `pathIndex3`). Re-solving a frame does not
require inventing new words: feed `solve_layout` the candidate lists
`[cur, base+"0", base+"1", ...]` and it returns a suffix per role, so the whole
diff is a digit change plus a declaration reorder. 14 PHILAI frames (up to 28
named locals) closed this way with zero fallbacks.

## Declared-but-unreferenced locals are real frame content

`philAI::DoAI` has 28 bytes of frame no instruction touches, below the last
live local and above `this`; `philAI::RVOfPosition` has one such dword between
its last live local and `debugObjectType`. VC6 allocates a completely
unreferenced local (including an unreferenced array), so the reconstruction
models them as `i32 unusedVars1[7];` / `i32 unusedValue3;` placed by bucket like
any other name. Without them the frame is short and every displacement below
the hole is wrong.


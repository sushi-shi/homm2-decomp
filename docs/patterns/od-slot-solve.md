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


## The base is -0x4 only without an EH frame (VC6 SP5, byte-proven)

`/GX` units open the frame with the `__except_list` chain, which owns
`-0x4` (the state index), `-0x8` (the handler) and `-0xc` (the saved link).
The bucket sort then starts at `-0x10`, and every predicted offset shifts by
`0xc`. `townObject::townObject` (0xa4740, `base_gx`-adjacent TOWNMGR frame with
the SEH prologue) is the witness:

```
push ebp / mov ebp,esp
push 0xffffffff        ; [ebp-0x4]
push $L62369           ; [ebp-0x8]
mov eax, fs:[__except_list] / push eax   ; [ebp-0xc]
mov fs:[__except_list], esp
sub esp, 0x38
```

A `char[16]` participates at its own size, so the slot walk is not uniform:

```
retail                                   ours (before)
mov [ebp-0x18], ecx   ; x                mov [ebp-0x28], ecx
mov [ebp-0x20], ecx   ; y                mov [ebp-0x34], ecx
mov [ebp-0x10], ecx   ; w                mov [ebp-0x20], ecx
mov [ebp-0x14], ecx   ; h                mov [ebp-0x24], ecx
mov [ebp-0x34], edx   ; building id      mov [ebp-0x2c], edx
lea eax, [ebp-0x30]   ; char name[16]    lea eax, [ebp-0x1c]
```

`predict_offsets(['name','x','y','tempY','w','h','id_h'], {'name': 16})`
returns `w -0x10, h -0x14, x -0x18, tempY -0x1c, y -0x20, name -0x30,
id_h -0x34` once `0xc` is added back - retail exactly. The closing edit was
`fileName` -> `name` (bucket 0 -> 13) and `buildingId_h` -> `id_h`
(bucket 9 -> 15); declaration order was already right and did not move. The
function went 99.87% -> EXACT on the first rebuild.

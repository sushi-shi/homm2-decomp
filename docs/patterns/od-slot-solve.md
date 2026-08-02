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

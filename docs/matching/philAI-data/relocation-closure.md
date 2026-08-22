# `SOURCE/PHILAI` data closure

The retained `DATA` and `DATA_COMPGEN` claims were measured after a forced redelink
and full build. `SOURCE/PHILAI` is exact for all compared payload sections:

- `.CRT$XCU`: 8/8 bytes
- `.bss`: 10,292/10,292 bytes
- `.data`: 632/632 bytes
- `.rdata`: 680/680 bytes
- total data: 11,612/11,612 bytes

The linked relocation audit reports no `SOURCE/PHILAI` identity divergence and no
unresolved candidate data target. This closes the 88 function-to-data relocation sites
which previously referenced 17 retail-folded floating COMDAT allocations.

The raw whole-object command

```sh
homm2 data-relocs --unit SOURCE/PHILAI \
  build/objdiff/base/SOURCE/PHILAI.obj \
  build/delink/SOURCE/PHILAI.c.obj \
  -o build/phil-data-relocs.json
```

retains four compiler-metadata residuals. Two `.CRT$XCU` cells point at the same retail
initializer bodies but compare VC6-local `$E` names against the reviewed semantic
compiler-function names. Two base `.xdata$x` relocations have no target entry because
the delinked comparison object intentionally omits exception metadata. None is a source
data allocation, payload, addend, or function-to-data identity residual. Their closure
belongs to compiler-metadata normalization rather than PHILAI data reconstruction.

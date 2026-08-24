/*
executive::ShutDownSystem manager walk
======================================

PoL 2.0 RVA 0xd16c0; Buka 2.1 RVA 0xc5010.

Complete clean VC4.2 product: 2/2
--------------------------------
  build/shutdown-system-walk-manifest.json
  build/source-variant-batch/shutdown-system-walk/results.json

PoL's assignment-in-condition walk and Buka's initialized-current/tail-
assignment walk both emit the same clean 99.600000%, 134-byte state with
exact 10/10 CFG and ordered 11/11 relocations.

Final-source compiler-state audit:
  build/shutdown-system-final-state-summary.json
  build/shutdown-system-final-replay-state-summary.json
  seed 1213156658 forest/top trial 3: exact, 134 bytes, exact 10/10 CFG,
  complete ordered 11/11 relocations; MAX 100 at hash 418e597f4d58.

Disposition: invariant matching-decomp port of Buka's manager walk.
No gameplay change.
*/

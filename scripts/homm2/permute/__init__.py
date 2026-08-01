"""homm2.permute - `homm2 permute` - the measured source-variant search.

The campaign rule is that matching hypotheses are never tested as one-off manual
edit-compile cycles: localize the divergence, then run ONE bounded complete matrix.
This package is that matrix. Its modules layer, so enter through the frontend
unless you are deliberately driving a lower stage:

  match_variants          THE entry point. One bounded product of exact hand-authored
                          axes x conservative AST shapes x parser-visible TU state.
  generate_ast_variants   libclang mutation generator. Never compiles or edits the
                          source; emits byte ranges the runner validates first.
  batch_source_variants   the runner. Byte-exact substitutions from a JSON manifest
                          only - no regex rewrites, source restored after each compile.
  tu_state_noise          TU-state island engine. Temporarily inserts parser-visible
                          declarations, compiles, scores, and restores immediately.
  tu_state_metrics        COFF metrics for tu_state_noise: raw .text bytes and the
                          ordered relocation stream, hashed separately.

Sweep drivers, for running the above unattended over a queue:

  recover_residual_functions   exhausts the live non-100% queue.
  recover_historical_exact     re-earns MAX for functions that were exact historically.

Everything here can WRITE: the engines edit src/ in place and the drivers touch
config/match_baseline.tsv. Their restore and ledger-integrity contracts are pinned
by the colocated tests - run `homm2 selftest permute` after changing any of them.
"""

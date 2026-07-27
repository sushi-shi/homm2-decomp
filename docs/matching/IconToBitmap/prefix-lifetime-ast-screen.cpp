/*
IconToBitmap, retail RVA 0x000d0570, size 0x4ed.
Greedy prefix declaration/lifetime screen, 2026-07-27.

The block graph is already exact at 80/80, but the first disassembly
divergence assigns entryX and the selected IconEntry pointer to different
register owners.  A conservative AST screen first generated 48 distinct
depth-one/two combinations drawn from:

  declaration_hoist, declaration_merge, declaration_split, and
  independent_statement_order.

Only 23/48 candidates compiled.  All 25 declaration-hoist candidates were
malformed by the generator, which inserted a stray `)` at function entry.
That run is diagnostic only and does not count as declaration-hoist coverage:

  build/icon2b-prefix-lifetime-ast-screen-manifest-20260727.json
  build/match-variants/icon2b-prefix-lifetime-ast-screen-20260727/results.json

The valid declaration-merge and independent-statement families were then
regenerated at depth one.  All 23 candidates compiled in 18.41 seconds and
source was restored:

  build/icon2b-prefix-valid-ast-screen-manifest-20260727.json
  build/match-variants/icon2b-prefix-valid-ast-screen-20260727/results.json

Only swapping the independent gIcEntry and gIcSrc publications exceeded the
clean 80.945050% source, reaching 80.972530%.  It remained size 1219 with
78/83 relocations and the weaker 62-exact/18-size-only block partition.  More
importantly, retail publishes gIcEntry before gIcSrc, so this reversed order
contradicts the observed relocation/instruction sequence and is not a valid
descendant for a full state product.

All other valid variants were byte-identical to or below the clean source.
No generated AST form is retained.  Declaration hoisting remains untested
until the generator is fixed or an exact-span hand axis is used.  The next
greedy prefix experiment must change the lifetime of the selected entry and
computed coordinates while preserving retail publication order, rather than
permuting already-proven independent stores.
*/

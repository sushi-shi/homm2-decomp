/*
FlipIconToBitmapColorTable, retail RVA 0x000d9790, size 0x54d.
Width/x0 register-storage hint census, 2026-07-27.

The first setup divergence assigns the selected entry width and transformed
x0 to different registers from retail.  Prior matrices covered expression
order, references, output helpers, snapshots, and assignment expressions, but
not the period-plausible C++ `register` storage hint.

Width and x0 were independent plain/register axes.  All four source shapes
were crossed with the clean state plus 50 varied declaration-forest states:
204/204 cells compiled in 161.93 seconds, the run was not truncated, and
source was restored.

  build/iconf2bc-register-width-x0-axes-20260727.json
  build/iconf2bc-register-width-x0-manifest-20260727.json
  build/match-variants/iconf2bc-register-width-x0-20260727/results.json

MSVC 4.2 ignored both hints for executable code: all four shapes had identical
text and normalized relocation hashes in every corresponding state.  The best
states reached 87.294710%, size 1338, 84/83 relocations, and exact 76-block
flow with 66 exact and ten size-only blocks.  This remains below the retained
88.4584% MAX and does not extend the known setup prefix.

Disposition: preserve the ordinary unqualified locals.  The retail width/x0
register ownership must come from a different real lifetime or dependency
boundary, not a `register` spelling.  No generated state or experimental
source is retained.
*/

#if 0
register i32 w = entries[frame].w;
register i32 x0 = x - entries[frame].x;
#endif

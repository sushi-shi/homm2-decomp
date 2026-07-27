/*
IconToBitmap, retail RVA 0x000d0570, size 0x4ed.
Literal-run source publication lifetime, 2026-07-27.

The earlier literal matrices varied copy ownership and sharing but did not
vary who owns the source endpoint across the literal run.  This matrix tested
five structures at the common run tail: direct compound publication, direct
assignment, parameterized expression and incremental inline helpers, and an
endpoint computed immediately after reading the command and kept live across
the clipping diamond.

All five reviewed structures were crossed with the clean state and all 50
declaration-forest states: 255/255 builds completed in 178.49 seconds and the
source was restored.

  build/icon2b-literal-source-publication-axes-20260727.json
  build/icon2b-literal-source-publication-states-20260727.json
  build/match-variants/icon2b-literal-source-publication-states-20260727/results.json

Clean results:

  direct compound/assignment         79.3187%, 1220 bytes, 79/83 relocations
  parameterized inline helpers       80.9451%, 1219 bytes, 78/83 relocations
  precomputed endpoint               77.3324%, 1233 bytes, 79/83 relocations

The direct forms and parameterized helpers reproduced the durable 81.494510%
MAX in their known TU states.  The precomputed endpoint established a
different, larger structural orbit.  Its best state was 79.560440%, 1234
bytes, and 79/83 relocations at declaration-forest trials 14, 41, and 43.
That island retained exact 80/80 flow but reduced the exact block-body count
from 63 to 59 (21 size-only blocks).  It neither approached the retail size
1261 nor supplied any of the four missing ordered relocations.

Disposition: preserve the endpoint-owned version as a credible lower island,
but do not retain it in the reconstructed source.  Its descendants should
only be revisited if another structural change specifically calls for a
literal endpoint live across the clipping diamond.  No generated source,
probe declaration, configuration, or MAX update is retained.
*/

#if 0
static inline u8* AdvanceIconLiteralSource(u8* src, i32 count) {
    return src + count;
}

/* parameterized publication */
X += command;
gIcRun = command;
gIcSrc = AdvanceIconLiteralSource(gIcSrc, command);

/* endpoint-owned publication */
u32 count = command;
u8* literalEnd = gIcSrc + command;
/* existing clipping diamond */
X += command;
gIcRun = command;
gIcSrc = literalEnd;
#endif

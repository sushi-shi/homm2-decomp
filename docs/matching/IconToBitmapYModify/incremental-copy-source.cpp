/*
IconToBitmapYModify, retail RVA 0x000da270, size 0x588.

This file records the clipped literal-copy source-pointer campaign. It is
evidence, not compiled game source.

The retained trial-39 island began with exact 64/64 flow and 63 exact block
sizes plus one size-only tail block. Retail builds the two clipped source
pointers incrementally, while the reconstructed source originally used:

    gYMSrc + (clipX - gYMX)

Four incremental ownership forms plus the open expression were first applied
to both nested copy arms and crossed with the clean state and all 50 top forest
states:

  build/match-variants/icon2by-incremental-copy-source-20260727/

The source-pointer step form reached 98.350250% at trial 39, but applying it to
both arms shortened the object to 1406 bytes, dropped to 129/130 relocations,
and regressed the block partition to 61 exact plus three size-only blocks. An
inline helper matrix reproduced this orbit byte-for-byte:

  build/match-variants/icon2by-copy-source-helper-20260727/

The helper therefore did not explain retail ownership and was not retained.
The two arms were then varied independently, 4 x 51 candidates:

  build/match-variants/icon2by-independent-copy-source-20260727/

Changing only the right-outside arm preserved the 63/1 partition and improved
the trial-39 island to 98.159900%. A final five-shape evaluation-order census
for only that arm completed 255/255 candidates:

  build/match-variants/icon2by-outside-copy-order-20260727/

The strongest developer-plausible form was:

    u8* copySrc = gYMSrc;
    copySrc -= gYMX;
    copySrc += clipX;
    memcpy(gYMRow + clipX, copySrc, clipW);

At trial 39 it reaches 98.642136%, size 1418, 131/130 relocations, exact 64
block flow, and the same 63-exact/1-size-only partition. This arm-specific
incremental construction is retained. It raises MAX substantially without
accepting the structurally inferior both-arm orbit. Exact closure remains open:
the single tail block is two bytes long and has one additional relocation.

After the source-hash epoch was refreshed, the official replay used:

  scripts/tu_state_noise.py --trials 50 --only-trial 39
      --insertion top --families forest --record-max --retain-best

It generated hash `ab6c1718b271.14e6f82ad29f`, recorded 98.6421%, restored
the authored source, and retained paired clue objects at:

  build/tu-state-noise/icon2by-outside-copy-trial39-record-max-20260727/
*/

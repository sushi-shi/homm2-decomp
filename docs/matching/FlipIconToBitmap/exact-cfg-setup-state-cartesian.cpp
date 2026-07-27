/*
FlipIconToBitmap (RVA 0x000d1ba0, retail size 1265)
Exact-CFG setup ownership x TU-state Cartesian, 2026-07-27.

Earlier work tested the exact-CFG two-stage clip-bottom structure against 24
clean setup shapes, and separately crossed its internal dim spellings with
clean plus 50 forest states.  That left an important product untested: the 24
entry/Y/right-end setup shapes themselves had never each received the same
state census.

This complete matrix fixed the exact 76-block clip-bottom CFG and crossed:

  entry/source ownership:
    direct byte cursor, typed table root, explicit entry-X snapshot,
    or indexed fields before entry binding;

  final-Y ownership:
    mutate the formal, local sum, or direct global publication;

  right-end publication:
    separate statements or assignment-result initialization;

  compiler state:
    clean plus 50 top-inserted declaration-forest states.

All 24 x 51 = 1224 variants completed in 1149.15 seconds.  The run was not
wall-time-truncated and restored source.  It produced 439 normalized states.

The best result was 86.509285%, size 1244, 82/81 relocations, at forest trial 1
with a typed root, local Y sum, and either right-end spelling.  Its CFG remains
exact 76/76, with 67 exact and nine size-only block bodies.  This improves the
clean 86.217510% setup branch but remains below the durable 86.8594% MAX of the
live 75-block source.

Final-Y ownership split the relocation topology into distinct families:

  local/direct publication: usually 82/81, retaining one surplus occurrence;
  formal mutation: usually 79/81, removing two required global reads.

Compiler states did produce 96 equal-count 81/81 objects, proving that raw
count can move independently.  Every one still failed ordered relocation
identity/addend comparison.  Their best score was only 84.326260%, size 1252,
with 65 exact and 11 size-only blocks.  No exact relocation topology or exact
closure appeared.

Artifacts:
  build/iconf2b-exact-cfg-setup-states-axes-20260727.json
  build/iconf2b-exact-cfg-setup-states-manifest-20260727.json
  build/match-variants/iconf2b-exact-cfg-setup-states-20260727/results.json

Disposition: retain the exact-CFG branch as structural evidence but keep the
current reconstructed source and official 86.8594% MAX.  No generated state,
experimental source, or configuration is retained.
*/

#if 0
// Exact-CFG clip-bottom seed used for every setup/state combination.
if (clip != ICON_DRAW_NO_CLIP) {
    i32 clipBottom;
    if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY) {
        clipBottom = clipY + clipH;
    } else {
        clipBottom = clipY + clipH;
        if (clipBottom >= entry->h + gFlipY) {
            clip = ICON_DRAW_NO_CLIP;
            goto clip_setup_done;
        }
    }
    clip = ICON_DRAW_CLIP;
    gFlipClipR = clipX + clipW - 1;
    gFlipClipB = clipBottom - 1;
}
clip_setup_done:
#endif

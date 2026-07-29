/*
CreateJoinFile source-line provenance and declaration-forest matrix

Target: SOURCE/GAME CreateJoinFile, RVA 0x00085107.

Retail reads the function-static i16 createJoinFileSourceLineBase before each
of the three H2_ALLOC_AT and three H2_FREE_AT calls.  The reconstruction passed
the six final line numbers as folded literals, omitting six relocations and 36
bytes.  The reviewed structural arm restores the original provenance:

    createJoinFileSourceLineBase + 0x0d
    createJoinFileSourceLineBase + 0x16
    createJoinFileSourceLineBase + 0x21
    createJoinFileSourceLineBase + 0x53
    createJoinFileSourceLineBase + 0x55
    createJoinFileSourceLineBase + 0x57

This source arm was crossed independently with both equivalent loop
predicates:

    diffSize1 > position1
    position1 < diffSize1

and with the clean TU state plus 50 declaration-forest states.

Artifacts:

    build/create-join-file-axes.json
    build/create-join-file-manifest.json
    build/source-variant-batch/create-join-file/results.json

The complete 2 x 2 x 51 product ran all 204 candidates without truncation.
The line-base arm restored retail size (974 bytes), all 77 ordered relocation
sites, and the exact 24-block graph.  Both loop spellings closed in the same
six forest states, so the retained source keeps its existing spelling.

Forest trial 8 from seed 1213156658 was replayed independently against the
integrated source:

    build/create-join-file-trial8-summary.json
    build/tu-state-noise/create-join-file-trial8/

The replay produced exact target bytes, retail size, 24/24 exact blocks, and
complete ordered relocation identity.  The exact 100.0000% current-hash MAX
was recorded.  Generated declarations were restored and are not retained in
source.
*/

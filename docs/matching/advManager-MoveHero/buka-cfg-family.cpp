// advManager::MoveHero @ PoL RVA 0x0000e51f
//
// Baseline PoL source:
//   fall-through at the cursor-cell tail; nested empty-arm/goto adjacent
//   handling; stoppingEvent owned by MAP_OBJECT_CASTLE
//   97.095955%, 4616/4660 bytes, 187/197 blocks, 158/158 relocations
//
// Complete structural product:
//   build/source-variant-batch/move-hero-structure/results.json
//   8/8 cells = tail (PoL/Buka) * adjacent (PoL/Buka) * label owner
//   (CASTLE/default)
//
// Complete structural/state product:
//   build/source-variant-batch/move-hero-structure-state/results.json
//   8 arms * (clean + 50 forest/top states) = 408/408 cells
//
//   family                                    best score   size  blocks
//   PoL tail, PoL adjacent, CASTLE label       97.301025   4616  187/197
//   Buka tail, PoL adjacent, default label     98.791530   4646  193/197
//   Buka tail, Buka adjacent, CASTLE label     98.921090   4651  194/197
//   Buka tail, Buka adjacent, default label    98.925964   4651  194/197
//
// The selected family reaches its best at forest trials 9, 42, 47, and 50.
// Its canonical topology rank is also best: edge delta 23, predecessor delta
// 8, flow-kind 101, shifted targets 62, and three missing blocks. Moving the
// label back into CASTLE keeps size/relocation count but worsens the paired
// topology to edge delta 59 and predecessor delta 19.
//
// Internal spelling product on the selected structural parent:
//   build/source-variant-batch/move-hero-internal-state/results.json
//   4 arms * (clean + 50 forest/top states) = 204/204 cells
//   Buka direction*pixels step products and PoL pixels*direction products are
//   byte- and relocation-identical in all paired states. Cursor-first origin
//   sums win every paired state: best 98.925964% versus 98.924010% for Buka
//   origin-first sums. Retained source therefore uses the Buka CFG and step
//   products with PoL origin sums.
//
// Unchanged-source clue artifacts:
//   build/move-hero-selected-state-summary.json
//   build/tu-state-noise/move-hero-selected
//   50/50 forest/top trials, eight unique byte/relocation states; best trial
//   9 repeats 98.925964%, 4651 bytes, and 158/158 relocations. No exact island.
//
// Focused relocation review reports five unresolved owner-relative sites at
// candidate offsets 0xf216 (x2), 0xf24b, 0xf257, and 0xf293. The direct block
// helper raises IndexError on this large-switch function; the complete public
// permuter products above retain the reviewed canonical block metrics.
//
// TU-state consequence: the selected trial perturbs unchanged CURSOR siblings.
// StopCursor falls 99.970146 -> 99.940300 and MoveOrigin 99.975310 ->
// 97.580246; TurnTo, ProcessIncomingSingleMapChange,
// ProcessIncomingGroupMapChange, and SendMapChange retain exact scores but
// change raw byte/relocation state. Recover each sibling independently.
//
// Disposition: matching decomp; retain Buka's two explicit movementDone jumps,
// forward adjacentDone guards, default-owned stoppingEvent label, and neutral
// direction-first step products. Keep PoL cursor-first origin sums. No gameplay
// change; the remaining residual includes non-donor version/API differences.

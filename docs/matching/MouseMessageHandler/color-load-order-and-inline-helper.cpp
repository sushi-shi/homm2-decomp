/*
MouseMessageHandler (RVA 0x000cde60, retail size 876)
Color-global load ownership and inline-helper experiments, 2026-07-27.

The live function already has the retail control-flow shape: 48/48 blocks,
47 exact blocks plus one provisional-boundary size-only tail block, and exact
flow. homm2 relocs reports 55/55 sites with no base-only site. The sole real
instruction divergence is the ordered identity of two relocation sites:

  candidate: eax = bLastOnscreenMouseColor; ecx = gbColorMice;
  retail:    eax = gbColorMice;             ecx = bLastOnscreenMouseColor;

Both then compare ecx against eax. The trailing displayed byte difference is
outside the semantic body and comes from the provisional object boundary.

Completed matrices:

  1. Operand order:
     bLastOnscreenMouseColor != gbColorMice
     gbColorMice != bLastOnscreenMouseColor

     Both arms were crossed with clean plus 50 declaration-forest states.
     All 102/102 variants compiled and were byte-identical at 99.788140%,
     size 876, 59/59 raw comparison relocations.

  2. Value ownership:
     direct globals; initialized and assigned snapshots of either global.

     The clean five-arm matrix retained the direct result at 99.788140%.
     Every snapshot form moved to the same broader register-scheduling orbit
     at 99.364410%, size 876, 59/59 raw comparison relocations. That orbit did
     not reverse just the two desired loads and was rejected.

  3. Additional clean structural forms:
     empty equality arm; two snapshots in retail load order; assignment
     expression; pointer; reference; and explicit change boolean.

     The empty equality arm was byte-identical to direct source. All other
     forms reproduced the 99.364410% snapshot orbit.

  4. Inline helper:
     MouseMessageHandler's duplicated cursor-transition body is semantically
     CheckChangeCursor(x, y, 0). A disposable explicit-inline helper plus call
     compiled byte-identically to the direct body. Its complete clean-plus-50
     declaration-forest census also emitted only the 99.788140% island.

Artifacts:
  build/match-variants/mouse-message-color-order-axes.json
  build/match-variants/mouse-message-color-order-states.json
  build/match-variants/mouse-message-color-order-islands/results.json
  build/match-variants/mouse-message-color-snapshot-axes.json
  build/match-variants/mouse-message-color-snapshot-clean-islands/results.json
  build/match-variants/mouse-message-color-structure-axes.json
  build/match-variants/mouse-message-color-structure-clean-islands/results.json
  build/match-variants/mouse-message-inline-helper-clean.json
  build/match-variants/mouse-message-inline-helper-clean-islands/results.json
  build/match-variants/mouse-message-inline-helper-states.json
  build/match-variants/mouse-message-inline-helper-state-islands/results.json

Disposition: retain the current direct source and the existing hash-scoped
MAX. No tested structure or complete state census produced a higher island.
The inline-helper equivalence is credible structural evidence, but it does not
distinguish original authorship and therefore does not justify a source edit.
*/

#if 0
// Operand reversal: byte-neutral in clean and all 50 tested states.
if (gbColorMice != bLastOnscreenMouseColor)
    gpMouseManager->SetColorMice(1);

// Snapshot ownership: enters the lower, broader scheduling orbit.
i32 currentMouseColor = gbColorMice;
i32 lastOnscreenMouseColor = bLastOnscreenMouseColor;
if (lastOnscreenMouseColor != currentMouseColor)
    gpMouseManager->SetColorMice(1);

// Equality-arm spelling: byte-neutral.
if (bLastOnscreenMouseColor == gbColorMice) {
} else {
    gpMouseManager->SetColorMice(1);
}

// The tested inline-helper shape used this call after reading x and y.
CheckChangeCursorInline(x, y, 0);
#endif

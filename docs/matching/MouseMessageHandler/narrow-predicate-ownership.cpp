/*
MouseMessageHandler (RVA 0x000cde60, retail size 876)
Narrow color-predicate ownership experiment, 2026-07-27.

Fresh evidence
--------------

The live normalized report retains MAX 99.9576. The semantic listing has one
real body divergence:

  candidate:
    eax = bLastOnscreenMouseColor;
    ecx = gbColorMice;
    cmp ecx, eax;

  retail:
    eax = gbColorMice;
    ecx = bLastOnscreenMouseColor;
    cmp ecx, eax;

The CFG remains 48/48 with exact flow and 47 exact blocks plus the known
provisional-boundary size-only block. homm2 relocs reports 55/55 semantic
sites. The displayed trailing bytes are outside the 876-byte semantic body.

The earlier ledger tested direct operand reversal, snapshots, an explicit
change boolean, and ownership of the whole duplicated cursor-transition body.
This matrix instead tested a narrow predicate-helper boundary. It was motivated
by the retail ordering looking like right-to-left evaluation of
ColorsDiffer(last, current): current is loaded first, then last.

Reviewed arms
-------------

All helper definitions below were disposable compiler input. The matrix
crossed each arm with the clean state and 50 top-of-TU declaration-forest
states. All 306/306 cells compiled in 261.06 seconds without truncation and
the source was restored.

Every arm produced exactly the same three state-dependent text islands:
99.364410%, 99.618645%, and 99.788140%, always at retail size 876. Each arm's
best block classification remained 48/48, 47 exact plus one size-only.
No helper survived as a distinct inlined code shape and no exact relocation
ordering appeared.

Artifacts:
  build/mouse-message-predicate-helper-axes-20260727.json
  build/mouse-message-predicate-helper-clean-manifest-20260727.json
  build/match-variants/mouse-message-predicate-helper-clean-20260727/results.json
  build/mouse-message-predicate-helper-top-states-manifest-20260727b.json
  build/match-variants/mouse-message-predicate-helper-top-states-20260727b/results.json

A separate unchanged-source top-of-TU census completed 51/51 cells in
55.80 seconds. It emitted the same islands, found no exact state, and did not
advance the durable MAX:

  build/mouse-message-clean-top-states-manifest-20260727.json
  build/match-variants/mouse-message-clean-top-states-20260727/results.json

Disposition: reject the helper declarations and calls. They contribute no
semantic or emitted structural distinction, so the reconstructed source stays
direct. Preserve the durable hash-scoped MAX 99.9576; the runner's lower raw
variant scores do not lower historical evidence.
*/

#if 0
static inline i32 MouseColorsDiffer(i32 lastColor, i32 currentColor) {
    return lastColor != currentColor;
}

static inline i32 MouseColorsDifferReversed(i32 currentColor, i32 lastColor) {
    return currentColor != lastColor;
}

static inline i32 MouseColorsDifferDirect(void) {
    return bLastOnscreenMouseColor != gbColorMice;
}

static inline i32 MouseLastColorDiffersFrom(i32 currentColor) {
    return bLastOnscreenMouseColor != currentColor;
}

static inline i32 MouseCurrentColorDiffersFrom(i32 lastColor) {
    return gbColorMice != lastColor;
}

if (MouseColorsDiffer(bLastOnscreenMouseColor, gbColorMice))
    gpMouseManager->SetColorMice(1);
if (MouseColorsDifferReversed(gbColorMice, bLastOnscreenMouseColor))
    gpMouseManager->SetColorMice(1);
if (MouseColorsDifferDirect())
    gpMouseManager->SetColorMice(1);
if (MouseLastColorDiffersFrom(gbColorMice))
    gpMouseManager->SetColorMice(1);
if (MouseCurrentColorDiffersFrom(bLastOnscreenMouseColor))
    gpMouseManager->SetColorMice(1);
#endif

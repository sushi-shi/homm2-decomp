// Experiment: ownership of the setup clipping Y value.
// Artifact: build/match-variants/iconf2bc-current-y-clean-20260726/
// Best: 86.380356%. Disposition: rejected.

#if 0
// Arm 1: publish Y, then reload the global in the clipping scope.
gFCY = Y;
if (clip != ICON_DRAW_NO_CLIP) {
    i32 currentY = gFCY;
}

// Arm 2: retain the local published value.
gFCY = Y;
if (clip != ICON_DRAW_NO_CLIP) {
    i32 currentY = Y;
}
#endif

// CombatClipDrawToBuffer extent setup using the icon-family Entries accessor.
//
// Clean result: 77.098360%, 702 bytes, 37/37 relocations.
// Best of clean plus 50 forest states: 77.106560% (trials 13 and 25).
//
// Artifacts:
//   build/combatclip-direct-entries-states-manifest.json
//   build/match-variants/combatclip-direct-entries-states/results.json
//
// Disposition: rejected for this function. This is consistent with the exact
// FillToBuffer sibling's source style, but it creates a substantially longer
// codegen island here and does not approach retail size.

if (gbComputeExtent != 0) {
    if (orientation != ICON_DRAW_NORMAL) {
        limits->right = x - Entries()[frame].x;
        limits->left = limits->right - Entries()[frame].w + 1;
    } else {
        limits->left = Entries()[frame].x + x;
        limits->right = Entries()[frame].w + limits->left - 1;
    }
    limits->top = Entries()[frame].y + y;
    limits->bottom = Entries()[frame].h + limits->top - 1;
}

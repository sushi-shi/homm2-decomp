// CombatClipDrawToBuffer extent setup with a materialized IconEntry pointer.
//
// Clean result: 81.647540%, 657 bytes, 37/37 relocations.
// Best of clean plus 50 forest states: 83.340164% (trials 13 and 25).
//
// Artifacts:
//   build/combatclip-entry-pointer-states-manifest.json
//   build/match-variants/combatclip-entry-pointer-states/results.json
//
// Disposition: preserved as a coherent lower island, but rejected for the
// retained source. The pointer stays live and collapses repeated m_data loads,
// making the result 42 bytes shorter than retail.

if (gbComputeExtent != 0) {
    IconDrawOrientation mirror = orientation;
    IconEntry* entry = &Entries()[frame];
    if (mirror == ICON_DRAW_FLIPPED) {
        limits->right = x - entry->x;
        limits->left = limits->right - entry->w + 1;
    } else {
        limits->left = entry->x + x;
        limits->right = entry->w + limits->left - 1;
    }
    limits->top = entry->y + y;
    limits->bottom = entry->h + limits->top - 1;
}

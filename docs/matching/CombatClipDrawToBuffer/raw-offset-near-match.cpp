// CombatClipDrawToBuffer extent setup: retained source and best structural family.
//
// Clean result: 96.938520%, 691 compared bytes versus 699 retail bytes,
// 37/37 relocations, and a 39-block CFG (36 exact blocks, 3 size-only).
// A clean-plus-50 forest census did not produce a higher target state.
//
// Artifacts:
//   build/combatclip-raw-offset-states-manifest.json
//   build/match-variants/combatclip-raw-offset-states/results.json
//
// Disposition: retained. Retail and candidate have the same CFG and relocation
// identities, but register allocation in the extent prefix shifts relocation
// sites by eight bytes. This remains the durable 96.9385% MAX island.

if (gbComputeExtent != 0) {
    IconDrawOrientation mirror = orientation;
    i32 entryOffset = frame * sizeof(IconEntry);
    if (mirror != ICON_DRAW_NORMAL) {
        limits->right =
            x - reinterpret_cast<IconEntry*>(m_data + entryOffset)->x;
        limits->left =
            limits->right - reinterpret_cast<IconEntry*>(m_data + entryOffset)->w + 1;
    } else {
        limits->left = reinterpret_cast<IconEntry*>(m_data + entryOffset)->x + x;
        limits->right =
            reinterpret_cast<IconEntry*>(m_data + entryOffset)->w + limits->left - 1;
    }
    limits->top = reinterpret_cast<IconEntry*>(m_data + entryOffset)->y + y;
    limits->bottom =
        reinterpret_cast<IconEntry*>(m_data + entryOffset)->h + limits->top - 1;
}

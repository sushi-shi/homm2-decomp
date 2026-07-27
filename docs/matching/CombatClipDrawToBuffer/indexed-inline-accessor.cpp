// CombatClipDrawToBuffer extent setup with an indexed inline member accessor.
//
// The tested declaration and definition were:
//
//   inline struct IconEntry& Entries(i32 frame);
//
//   inline IconEntry& icon::Entries(i32 frame) {
//       return Entries()[frame];
//   }
//
// Clean result: 80.540985%, 703 bytes, 37/37 relocations.
// Best of clean plus 50 forest states: 80.549180% (trial 40).
//
// Artifacts:
//   build/combatclip-indexed-accessor-states-manifest.json
//   build/match-variants/combatclip-indexed-accessor-states/results.json
//
// Disposition: rejected. This was a plausible /Ob1 structure and a historical
// source family, but the inlined accessor is four bytes longer than retail and
// its state census never enters the raw-offset orbit.

if (gbComputeExtent != 0) {
    if (orientation == ICON_DRAW_FLIPPED) {
        limits->right = x - Entries(frame).x;
        limits->left = limits->right - Entries(frame).w + 1;
    } else {
        limits->left = Entries(frame).x + x;
        limits->right = Entries(frame).w + limits->left - 1;
    }
    limits->top = Entries(frame).y + y;
    limits->bottom = Entries(frame).h + limits->top - 1;
}

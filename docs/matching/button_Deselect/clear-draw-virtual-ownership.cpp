/*
button::Deselect, retail RVA 0x000ddd10, size 0x83.
Selected-flag clear and virtual Draw ownership, 2026-07-27.

The function has exact size, exact 3/3 basic blocks, and exact 4/4 ordered
relocations. Its two-byte residual is one instruction schedule: retail loads
the vtable pointer from this before storing the cleared m_flags word, while
the candidate stores m_flags and then loads the vtable pointer.

An unchanged-source top-of-TU declaration-forest census completed 50/50
trials and found one normalized state only:

  build/button-deselect-top-states-20260727.json
  build/tu-state-noise/button-deselect-top-20260727

The first structural matrix tested direct code plus six inline ownership
boundaries: flag-by-reference, flag-by-value, Draw ownership, separate
clear/Draw owners, and two combined parameter orders. The complete
7 * (clean + 50) = 357/357 product was byte-identical at 99.777780%, size
131, with all four relocations:

  build/button-deselect-draw-owner-axes-20260727.json
  build/button-deselect-draw-owner-manifest-20260727.json
  build/match-variants/button-deselect-draw-owner-20260727/results.json

The second 357/357 product tested button/widget pointer and reference owners
plus button/widget virtual member pointers:

  build/button-deselect-virtual-owner-axes-20260727.json
  build/button-deselect-virtual-owner-manifest-20260727.json
  build/match-variants/button-deselect-virtual-owner-20260727/results.json

Ordinary object pointers and references again canonicalize to the retained
99.777780% object in every state. The two member-pointer forms survive as a
different mechanism, but fall to 74.955560%, size 130, five relocations, and
zero instruction-count-exact blocks. They contradict retail's compact normal
virtual-call sequence.

Disposition: retain the existing private inline helper and durable MAX. The
normal C++ clear/Draw ownership surface is exhausted for the current target
hash. A future attempt needs a new compiler state or independent surrounding
structure, not another pointer/reference spelling.
*/

#if 0
button* drawOwner = this;
m_flags &= ~WIDGET_FLAG_SELECTED;
drawOwner->Draw(); // Byte-identical to the retained source.

void (button::*drawMethod)(void) = &button::Draw;
m_flags &= ~WIDGET_FLAG_SELECTED;
(this->*drawMethod)(); // Distinct but structurally worse.
#endif

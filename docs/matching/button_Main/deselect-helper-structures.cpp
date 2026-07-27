/*
button::Main, RVA 0x000dd6d0: DeselectSelected helper structures
(2026-07-27).

The repeated residual belongs to the explicit inline helper, so its real
source structure was varied once rather than editing five expanded call sites
independently.
*/

#if 0
// Starting local-copy form.
WidgetFlagStorage flags = m_flags;
if (!HAS(flags, WIDGET_FLAG_SELECTED))
    return MESSAGE_DISPATCH_CONTINUE;
flags &= ~WIDGET_FLAG_SELECTED;
m_flags = flags;
Draw();

// Retained direct-update form.
if (!HAS(m_flags, WIDGET_FLAG_SELECTED))
    return MESSAGE_DISPATCH_CONTINUE;
m_flags &= ~WIDGET_FLAG_SELECTED;
Draw();

// Sibling-widget assignment idiom.
WidgetFlagStorage flags = m_flags;
if (!HAS(flags, WIDGET_FLAG_SELECTED))
    return MESSAGE_DISPATCH_CONTINUE;
m_flags = flags & ~WIDGET_FLAG_SELECTED;
Draw();

// Pointer/reference owner aliases.
button *self = this;       // button&, and widget* arms were also tested
WidgetFlagStorage flags = m_flags;
if (!HAS(flags, WIDGET_FLAG_SELECTED))
    return MESSAGE_DISPATCH_CONTINUE;
flags &= ~WIDGET_FLAG_SELECTED;
m_flags = flags;
self->Draw();

// Reversed positive arm.
WidgetFlagStorage flags = m_flags;
if (HAS(flags, WIDGET_FLAG_SELECTED)) {
    flags &= ~WIDGET_FLAG_SELECTED;
    m_flags = flags;
    Draw();
    // Existing update/message tail.
    return MESSAGE_DISPATCH_FORWARD;
}
return MESSAGE_DISPATCH_CONTINUE;
#endif

/*
Clean structural matrices:

  build/button-main-deselect-helper-axes.json
  build/button-main-deselect-helper-clean.json
  build/match-variants/button-main-deselect-helper-clean-20260727/
  build/button-main-deselect-helper-clean2.json
  build/match-variants/button-main-deselect-helper-clean2-20260727/

All non-reversed arms retained 71/71 exact blocks and 36/36 relocations.
The direct update improved clean Main from 97.032120% to 98.008570% at exact
size.  Pointer/reference aliases produced a coherent but one-byte-short
96.299790% island.  The reversed arm kept 71 blocks but changed four block
bodies and scored 90.779440%; it was preserved as a lower structural result
and not retained.

The baseline, direct update, pointer aliases, and reversed arm were all
crossed completely with clean plus 50 forest states:

  build/button-main-deselect-helper-states.json
  build/match-variants/button-main-deselect-helper-states-20260727/

The direct update reached the same best object as the old helper under trial
15 (text e058079846483b31, 98.907920%), while improving the clean source.
The old helper needed trial 30 for that object.  Alias islands topped out at
97.177730%; the reversed arm reached 92.655240%.

Additional base-cast, reference, one-expression flag assignment, nested
visual-only inline, and nested visual-plus-region inline forms were then
compiled.  The base cast, assignment expression, and both nested helpers were
codegen-identical to the old clean helper; the reference matched the
one-byte-short pointer island:

  build/button-main-deselect-helper-clean3.json
  build/match-variants/button-main-deselect-helper-clean3-20260727/

Because these arms did not open distinct object states, their unchanged
effective code was already represented by the completed state matrices above.

The retained direct-update form received an expanded 200-forest census:

  build/tu-state-noise/button-main-direct-expanded-20260727/

It found only 12 distinct byte/relocation states.  The 98.907920% object
recurred six times and remained best, with the same 30-byte virtual-call
scheduling residual.  The direct form is retained because it raises live Main
to 98.008570%, keeps exact size/CFG/relocations, and also restores the clean
button::Select sibling to its already-recorded exact MAX.  Generated probes
were restored.
*/

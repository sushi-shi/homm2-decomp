/*
button::Main, RVA 0x000dd6d0: direct public-Deselect inline family
(2026-07-27).

One plausible original-source family is that Main called an explicitly inline
public button::Deselect directly, rather than the private DeselectSelected
helper used by the reconstruction.  The public definition was moved before
Main, all five helper calls were redirected to Deselect, and the later wrapper
was removed.
*/

#if 0
inline MessageDispatchResult button::Deselect(tag_message& msg) {
    // Existing complete deselection body.
}

MessageDispatchResult button::Main(tag_message& msg) {
    // ...
    return Deselect(msg);
}
#endif

/*
This source order cannot reproduce retail topology: making the definition
visible soon enough for /Ob1 to inline it also places its emitted external
body before Main, whereas retail places Deselect after Select.  Leaving the
definition after Main preserves function order but makes it unavailable for
inlining at Main's call sites.  The experiment was restored without retaining
source or changing generated manifests.
*/

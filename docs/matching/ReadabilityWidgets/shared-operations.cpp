// Readability structural matrices: ReadabilityWidgets
// Not build input. All matrices use --min-depth 0 --max-depth 0
// --state-trials 0 and --limit equal to the complete exact-axis product.
// Exact flags are the tool audit results, not rounded fuzzy percentages.
// Preserving a pre-existing retail residual is not a new exact closure.
// Macro/helper declarations are in their corresponding owner headers.

// h02-widget: src/BASE/BUTTON.cpp, RVA 0xd3890
// Artifacts: build/readability/h02-widget-{axes,manifest}.json;
//            build/readability/h02-widget-matrix/results.json
// Complete: 3/3
// {"candidate_relocs": 22, "candidate_size": 1240, "choices": {"candidate": "baseline", "h02-widget": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "3febfe8644754f29", "retail_relocs": 22, "text_sha": "3e27942e29cda4d9"}
// {"candidate_relocs": 22, "candidate_size": 1270, "choices": {"candidate": "baseline", "h02-widget": "method"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 1270, retail 1240", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "3febfe8644754f29", "retail_relocs": 22, "text_sha": "9828125ca63ec67a"}
// {"candidate_relocs": 22, "candidate_size": 1240, "choices": {"candidate": "baseline", "h02-widget": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "3febfe8644754f29", "retail_relocs": 22, "text_sha": "3e27942e29cda4d9"}
// Axis: h02-widget
// Arm: explicit
if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                    SET_WIDGET_MESSAGE

// Arm: method
if (ContainsLocalPoint(x, y)) {
                    SET_WIDGET_MESSAGE

// Arm: macro
if (WIDGET_CONTAINS_LOCAL_POINT(*this, x, y)) {
                    SET_WIDGET_MESSAGE

// h03-geometry: src/BASE/BUTTON.cpp, RVA 0xd3710
// Artifacts: build/readability/h03-geometry-{axes,manifest}.json;
//            build/readability/h03-geometry-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 30, "candidate_size": 277, "choices": {"candidate": "baseline", "h03-geometry": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "e434bcb419d5fe05", "retail_relocs": 30, "text_sha": "1ef8c9f72f9c9e4c"}
// {"candidate_relocs": 30, "candidate_size": 277, "choices": {"candidate": "baseline", "h03-geometry": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "e434bcb419d5fe05", "retail_relocs": 30, "text_sha": "1ef8c9f72f9c9e4c"}
// Axis: h03-geometry
// Arm: explicit
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();

// Arm: macro
    READ_WIDGET_GEOMETRY(*this, gpResourceManager);

// h05-copy: src/BASE/listbox.cpp, RVA 0xcee60
// Artifacts: build/readability/h05-copy-{axes,manifest}.json;
//            build/readability/h05-copy-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 23, "candidate_size": 1164, "choices": {"candidate": "baseline", "h05-copy": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "ea16a2faf0b7535c", "retail_relocs": 23, "text_sha": "c39f256423b0d67f"}
// {"candidate_relocs": 23, "candidate_size": 1164, "choices": {"candidate": "baseline", "h05-copy": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "ea16a2faf0b7535c", "retail_relocs": 23, "text_sha": "c39f256423b0d67f"}
// Axis: h05-copy
// Arm: explicit
                    newItems[m_itemCount] = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
                    strcpy(newItems[m_itemCount], text);

// Arm: macro
                    ALLOC_COPY_STRING(newItems[m_itemCount], text);

// h11-icon-fit: src/BASE/Icon2b.cpp, RVA 0xc14c0
// Artifacts: build/readability/h11-icon-fit-{axes,manifest}.json;
//            build/readability/h11-icon-fit-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 211, "candidate_size": 1872, "choices": {"candidate": "baseline", "h11-icon-fit": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "493853a2149e48d0", "retail_relocs": 211, "text_sha": "f16f929c170e465b"}
// {"candidate_relocs": 211, "candidate_size": 1872, "choices": {"candidate": "baseline", "h11-icon-fit": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "493853a2149e48d0", "retail_relocs": 211, "text_sha": "f16f929c170e465b"}
// Axis: h11-icon-fit
// Arm: explicit
s_left >= clipX && s_left + s_entry->w <= clipX + clipW && s_y >= clipY
            && s_y + s_entry->h <= clipY + clipH

// Arm: macro
ICON_FITS_CLIP(s_left, s_y, s_entry->w, s_entry->h, clipX, clipY, clipW, clipH)

// h13-coords: src/BASE/INPUTMGR.cpp, RVA 0xbc390
// Artifacts: build/readability/h13-coords-{axes,manifest}.json;
//            build/readability/h13-coords-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 50, "candidate_size": 899, "choices": {"candidate": "baseline", "h13-coords": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "4fad8b180fb7ddbe", "retail_relocs": 50, "text_sha": "d12f0d22ea0f1a26"}
// {"candidate_relocs": 50, "candidate_size": 899, "choices": {"candidate": "baseline", "h13-coords": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "4fad8b180fb7ddbe", "retail_relocs": 50, "text_sha": "d12f0d22ea0f1a26"}
// Axis: h13-coords
// Arm: explicit
(static_cast<i16>(messageData) * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth

// Arm: macro
CLIENT_TO_GAME_X(static_cast<i16>(messageData))

// h15-config: src/BASE/INPUTMGR.cpp, RVA 0xbbf40
// Artifacts: build/readability/h15-config-{axes,manifest}.json;
//            build/readability/h15-config-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 55, "candidate_size": 1100, "choices": {"candidate": "baseline", "h15-config": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "931ded25513cfd1c", "retail_relocs": 55, "text_sha": "b65fbe47e7d1e251"}
// {"candidate_relocs": 55, "candidate_size": 1100, "choices": {"candidate": "baseline", "h15-config": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "931ded25513cfd1c", "retail_relocs": 55, "text_sha": "b65fbe47e7d1e251"}
// Axis: h15-config
// Arm: explicit
SetFullScreenStatus(1 - gConfig.gfx[IDX(giCurExe)].fullScreen);

// Arm: macro
SetFullScreenStatus(1 - CURRENT_GRAPHICS_CONFIG.fullScreen);

// h19-summary: src/SOURCE/SMACKMGR.cpp, RVA 0x95b18
// Artifacts: build/readability/h19-summary-{axes,manifest}.json;
//            build/readability/h19-summary-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 88, "candidate_size": 579, "choices": {"candidate": "baseline", "shared_macro": "private"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "76afedd61f1d7b86", "retail_relocs": 82, "text_sha": "044a7f29d8a19627"}
// {"candidate_relocs": 88, "candidate_size": 579, "choices": {"candidate": "baseline", "shared_macro": "shared"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "76afedd61f1d7b86", "retail_relocs": 82, "text_sha": "044a7f29d8a19627"}
// Axis: shared_macro
// Arm: private
#define LOG_SUMMARY_VALUE(format, value)                                                           \
    sprintf(gText, format, value);                                                                 \
    LogStr(gText)

// Arm: shared


// Disposition: see docs/readability/implementation.md for retained subsets,
// caller boundaries and the combined original-snapshot object comparison.

// Rejected H02 method declaration present for the complete three-arm matrix:
// Owner: widget class, include/BASE/widget.h. Removed after measurement.
inline bool ContainsLocalPoint(i32 x, i32 y) const {
    return x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height;
}

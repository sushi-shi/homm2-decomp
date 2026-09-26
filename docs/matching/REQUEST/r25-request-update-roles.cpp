// VC6 SP5 REQUEST source family r25-request-update-roles; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r25-request-update-roles/results.json; RVA 0x90ab6
// Complete 2 arms; elapsed 1.667126s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 2718 / 7ee6c6bc25f6fda0 / e89cb2a46eab0aa7 / 60:60 / 60,0,0,0 / True
// 1 / {'source_family': 'filter_index', 'candidate': 'baseline'} / 2739 / c9111e06f14cc7f4 / a28160bc39959727 / 60:60 / 60,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |     if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
// |         for (i = 0; i < IDX(FILE_REQUESTER_MAP_SIZE_COUNT); ++i) {
// |             message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// |             message.payload.widget.id = FILE_REQUESTER_FILTER_SMALL + i;
// |             message.payload.widget.data.value =
// |                 (i == IDX(giMapSizeFilter)) + i * FILTER_FRAME_STEP + FILTER_FRAME_BASE;
// |             m_window->BroadcastMessage(message);
// ALTERNATIVE filter_index (unified source delta from BEFORE):
// | --- before
// | +++ filter_index
// | @@ -1,7 +1,7 @@
// |      if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
// | -        for (i = 0; i < IDX(FILE_REQUESTER_MAP_SIZE_COUNT); ++i) {
// | +        for (i32 filterIndex = 0; filterIndex < IDX(FILE_REQUESTER_MAP_SIZE_COUNT); ++filterIndex) {
// |              message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// | -            message.payload.widget.id = FILE_REQUESTER_FILTER_SMALL + i;
// | +            message.payload.widget.id = FILE_REQUESTER_FILTER_SMALL + filterIndex;
// |              message.payload.widget.data.value =
// | -                (i == IDX(giMapSizeFilter)) + i * FILTER_FRAME_STEP + FILTER_FRAME_BASE;
// | +                (filterIndex == IDX(giMapSizeFilter)) + filterIndex * FILTER_FRAME_STEP + FILTER_FRAME_BASE;
// |              m_window->BroadcastMessage(message);

// VC6 SP5 Overview source family r24-overview-main-roles; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r24-overview-main-roles/results.json; RVA 0x7bda1
// Complete 2 arms; elapsed 1.74842s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 9cebe67a573ac9ef / 42:42 / 42,0,0,0 / True
// 1 / {'source_family': 'record_resource_lighthouse', 'candidate': 'baseline'} / 1202 / be6e933b176ef38a / 0f1c112830eac454 / 42:42 / 42,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |     lighthouseCount = 0;
// |     for (mine = 0; mine < GAME_MINE_COUNT; mine++) {
// |         if (m_mineOwners[mine] == giCurPlayer) {
// |             if (m_mines[mine].resourceType <= MINE_TYPE_GOLD) {
// |                 mineCounts[IDX(m_mines[mine].resourceType)]++;
// |             } else if (m_mines[mine].resourceType == MINE_TYPE_LIGHTHOUSE) {
// |                 lighthouseCount++;
// |             }
// |         }
// |     }
// |     for (mine = 0; mine < IDX(RES_COUNT); mine++) {
// |         message.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
// |         message.payload.widget.data.text = gText;
// |         sprintf(gText, "%d", static_cast<i32>(mineCounts[mine]));
// |         message.payload.widget.id = mine + MINE_FIRST_WIDGET;
// |         overWin->BroadcastMessage(message);
// |     }
// |     for (mine = OVERVIEW_VISIBLE_ROWS; mine > 0; mine--) {
// |         if (mine > lighthouseCount) {
// |             message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
// |             message.payload.widget.id = mine + LIGHTHOUSE_WIDGET_ID_OFFSET;
// |             message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// ALTERNATIVE record_resource_lighthouse (unified source delta from BEFORE):
// | --- before
// | +++ record_resource_lighthouse
// | @@ -1,22 +1,22 @@
// |      lighthouseCount = 0;
// | -    for (mine = 0; mine < GAME_MINE_COUNT; mine++) {
// | -        if (m_mineOwners[mine] == giCurPlayer) {
// | -            if (m_mines[mine].resourceType <= MINE_TYPE_GOLD) {
// | -                mineCounts[IDX(m_mines[mine].resourceType)]++;
// | -            } else if (m_mines[mine].resourceType == MINE_TYPE_LIGHTHOUSE) {
// | +    for (i32 mineRecord = 0; mineRecord < GAME_MINE_COUNT; mineRecord++) {
// | +        if (m_mineOwners[mineRecord] == giCurPlayer) {
// | +            if (m_mines[mineRecord].resourceType <= MINE_TYPE_GOLD) {
// | +                mineCounts[IDX(m_mines[mineRecord].resourceType)]++;
// | +            } else if (m_mines[mineRecord].resourceType == MINE_TYPE_LIGHTHOUSE) {
// |                  lighthouseCount++;
// |              }
// |          }
// |      }
// | -    for (mine = 0; mine < IDX(RES_COUNT); mine++) {
// | +    for (i32 resourceIndex = 0; resourceIndex < IDX(RES_COUNT); resourceIndex++) {
// |          message.payload.widget.command = OVERVIEW_WIDGET_SET_TEXT;
// |          message.payload.widget.data.text = gText;
// | -        sprintf(gText, "%d", static_cast<i32>(mineCounts[mine]));
// | -        message.payload.widget.id = mine + MINE_FIRST_WIDGET;
// | +        sprintf(gText, "%d", static_cast<i32>(mineCounts[resourceIndex]));
// | +        message.payload.widget.id = resourceIndex + MINE_FIRST_WIDGET;
// |          overWin->BroadcastMessage(message);
// |      }
// | -    for (mine = OVERVIEW_VISIBLE_ROWS; mine > 0; mine--) {
// | -        if (mine > lighthouseCount) {
// | +    for (i32 lighthouseIcon = OVERVIEW_VISIBLE_ROWS; lighthouseIcon > 0; lighthouseIcon--) {
// | +        if (lighthouseIcon > lighthouseCount) {
// |              message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
// | -            message.payload.widget.id = mine + LIGHTHOUSE_WIDGET_ID_OFFSET;
// | +            message.payload.widget.id = lighthouseIcon + LIGHTHOUSE_WIDGET_ID_OFFSET;
// |              message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);

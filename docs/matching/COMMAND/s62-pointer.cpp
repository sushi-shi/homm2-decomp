// VC6 SP5 COMMAND source family s62-pointer; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-pointer/results.json; RVA 0x2c8fe
// Complete 8 arms; elapsed 6.552322s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 90 / 83dfe5d90f5dcbe2 / e3b0c44298fc1c14 / 9:9 / 9,0,0,0 / True
// 1 / {'local_role_subset': 'result', 'candidate': 'baseline'} / 90 / 4a987de83ac668cb / e3b0c44298fc1c14 / 9:9 / 9,0,0,0 / True
// 2 / {'local_role_subset': 'y', 'candidate': 'baseline'} / 76 / 029db599d15bfe17 / e3b0c44298fc1c14 / 9:9 / 8,1,0,0 / True
// 3 / {'local_role_subset': 'y_result', 'candidate': 'baseline'} / 76 / 8e4e7839a6e8dc22 / e3b0c44298fc1c14 / 9:9 / 8,1,0,0 / True
// 4 / {'local_role_subset': 'x', 'candidate': 'baseline'} / 76 / 89e455baf8894f2f / e3b0c44298fc1c14 / 9:9 / 8,1,0,0 / True
// 5 / {'local_role_subset': 'x_result', 'candidate': 'baseline'} / 76 / a57df675d90d8c42 / e3b0c44298fc1c14 / 9:9 / 8,1,0,0 / True
// 6 / {'local_role_subset': 'x_y', 'candidate': 'baseline'} / 62 / f38eaa7409db5a92 / e3b0c44298fc1c14 / 9:9 / 8,1,0,0 / True
// 7 / {'local_role_subset': 'x_y_result', 'candidate': 'baseline'} / 62 / 694fce0acacb9be2 / e3b0c44298fc1c14 / 9:9 / 8,1,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// |     i32 H2_UNUSED(x);
// |     i32 H2_UNUSED(y);
// |     i32 H2_UNUSED(unusedResult);
// |     if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// |         return POINTER_VIEW;
// |     } else {
// |         switch (command) {
// |             case COMBAT_MESSAGE_COMMAND_MOVE:
// |             case COMBAT_MESSAGE_COMMAND_FLY:
// |             case COMBAT_MESSAGE_COMMAND_SHOOT:
// |             case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
// |                 x = hexIndex % COMBAT_GRID_ROW_LENGTH;
// |                 y = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |                 return IDX(command);
// |             }
// |             default:
// |                 return IDX(command);
// |         }
// |     }
// | }
// ALTERNATIVE result (unified source delta from BEFORE):
// | --- before
// | +++ result
// | @@ -1,7 +1,6 @@
// |  i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// |      i32 H2_UNUSED(x);
// |      i32 H2_UNUSED(y);
// | -    i32 H2_UNUSED(unusedResult);
// |      if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// |          return POINTER_VIEW;
// |      } else {
// ALTERNATIVE y (unified source delta from BEFORE):
// | --- before
// | +++ y
// | @@ -1,6 +1,5 @@
// |  i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// |      i32 H2_UNUSED(x);
// | -    i32 H2_UNUSED(y);
// |      i32 H2_UNUSED(unusedResult);
// |      if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// |          return POINTER_VIEW;
// | @@ -11,7 +10,6 @@
// |              case COMBAT_MESSAGE_COMMAND_SHOOT:
// |              case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
// |                  x = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -                y = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |                  return IDX(command);
// |              }
// |              default:
// ALTERNATIVE y_result (unified source delta from BEFORE):
// | --- before
// | +++ y_result
// | @@ -1,7 +1,5 @@
// |  i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// |      i32 H2_UNUSED(x);
// | -    i32 H2_UNUSED(y);
// | -    i32 H2_UNUSED(unusedResult);
// |      if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// |          return POINTER_VIEW;
// |      } else {
// | @@ -11,7 +9,6 @@
// |              case COMBAT_MESSAGE_COMMAND_SHOOT:
// |              case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
// |                  x = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -                y = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |                  return IDX(command);
// |              }
// |              default:
// ALTERNATIVE x (unified source delta from BEFORE):
// | --- before
// | +++ x
// | @@ -1,5 +1,4 @@
// |  i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// | -    i32 H2_UNUSED(x);
// |      i32 H2_UNUSED(y);
// |      i32 H2_UNUSED(unusedResult);
// |      if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// | @@ -10,7 +9,6 @@
// |              case COMBAT_MESSAGE_COMMAND_FLY:
// |              case COMBAT_MESSAGE_COMMAND_SHOOT:
// |              case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
// | -                x = hexIndex % COMBAT_GRID_ROW_LENGTH;
// |                  y = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |                  return IDX(command);
// |              }
// ALTERNATIVE x_result (unified source delta from BEFORE):
// | --- before
// | +++ x_result
// | @@ -1,7 +1,5 @@
// |  i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// | -    i32 H2_UNUSED(x);
// |      i32 H2_UNUSED(y);
// | -    i32 H2_UNUSED(unusedResult);
// |      if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// |          return POINTER_VIEW;
// |      } else {
// | @@ -10,7 +8,6 @@
// |              case COMBAT_MESSAGE_COMMAND_FLY:
// |              case COMBAT_MESSAGE_COMMAND_SHOOT:
// |              case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
// | -                x = hexIndex % COMBAT_GRID_ROW_LENGTH;
// |                  y = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |                  return IDX(command);
// |              }
// ALTERNATIVE x_y (unified source delta from BEFORE):
// | --- before
// | +++ x_y
// | @@ -1,6 +1,4 @@
// |  i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// | -    i32 H2_UNUSED(x);
// | -    i32 H2_UNUSED(y);
// |      i32 H2_UNUSED(unusedResult);
// |      if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// |          return POINTER_VIEW;
// | @@ -10,8 +8,6 @@
// |              case COMBAT_MESSAGE_COMMAND_FLY:
// |              case COMBAT_MESSAGE_COMMAND_SHOOT:
// |              case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
// | -                x = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -                y = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |                  return IDX(command);
// |              }
// |              default:
// ALTERNATIVE x_y_result (unified source delta from BEFORE):
// | --- before
// | +++ x_y_result
// | @@ -1,7 +1,4 @@
// |  i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
// | -    i32 H2_UNUSED(x);
// | -    i32 H2_UNUSED(y);
// | -    i32 H2_UNUSED(unusedResult);
// |      if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
// |          return POINTER_VIEW;
// |      } else {
// | @@ -10,8 +7,6 @@
// |              case COMBAT_MESSAGE_COMMAND_FLY:
// |              case COMBAT_MESSAGE_COMMAND_SHOOT:
// |              case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
// | -                x = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -                y = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |                  return IDX(command);
// |              }
// |              default:

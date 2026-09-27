// VC6 SP5 kbwin source family s67-kbwin-about-slots; parentf0502d36.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s67-kbwin-about-slots/results.json; RVA 0x71883
// Complete 4 arms; elapsed 2.338s; input restored.
// Disposition: retain original; every tested alternative changes text.
// No EH/numerical exceptions in this TU; all sixteen retail checks are complete.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 103 / d93a4df25b4d185d / 76edafa415aa6f1f / 8:8 / 8,0,0,0 / True
// 1 / {'source_family': 'notification', 'candidate': 'baseline'} / 88 / eb1a41fda90f5c55 / fb14b4dc56a2d33d / 8:8 / 7,1,0,0 / True
// 2 / {'source_family': 'command_window', 'candidate': 'baseline'} / 98 / c1716ad320141868 / ee03203f4fa2ad5d / 8:8 / 7,1,0,0 / True
// 3 / {'source_family': 'command_window_notification', 'candidate': 'baseline'} / 82 / b78ab2e2ca807c0f / e1de15f4ba732aa9 / 8:8 / 7,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | BOOL CALLBACK AppAbout(HWND dialog, UINT message, WPARAM messageParam, LPARAM messageData) {
// |     i32 command;
// |     HWND H2_UNUSED(commandWindow2);
// |     u16 H2_UNUSED(notificationType1);
// |
// |     switch (message) {
// |         case WM_INITDIALOG:
// |             return 1;
// |         case WM_COMMAND:
// |             command = LOWORD(messageParam);
// |             commandWindow2 = reinterpret_cast<HWND>(messageData);
// |             notificationType1 = HIWORD(messageParam);
// |             if (command == IDOK)
// |                 EndDialog(dialog, 1);
// |             break;
// |     }
// |     PollSound();
// |     return 0;
// | }
// ALTERNATIVE notification (unified source delta from BEFORE):
// | --- before
// | +++ notification
// | @@ -1,7 +1,6 @@
// |  BOOL CALLBACK AppAbout(HWND dialog, UINT message, WPARAM messageParam, LPARAM messageData) {
// |      i32 command;
// |      HWND H2_UNUSED(commandWindow2);
// | -    u16 H2_UNUSED(notificationType1);
// |
// |      switch (message) {
// |          case WM_INITDIALOG:
// | @@ -9,7 +8,6 @@
// |          case WM_COMMAND:
// |              command = LOWORD(messageParam);
// |              commandWindow2 = reinterpret_cast<HWND>(messageData);
// | -            notificationType1 = HIWORD(messageParam);
// |              if (command == IDOK)
// |                  EndDialog(dialog, 1);
// |              break;
// ALTERNATIVE command_window (unified source delta from BEFORE):
// | --- before
// | +++ command_window
// | @@ -1,6 +1,5 @@
// |  BOOL CALLBACK AppAbout(HWND dialog, UINT message, WPARAM messageParam, LPARAM messageData) {
// |      i32 command;
// | -    HWND H2_UNUSED(commandWindow2);
// |      u16 H2_UNUSED(notificationType1);
// |
// |      switch (message) {
// | @@ -8,7 +7,6 @@
// |              return 1;
// |          case WM_COMMAND:
// |              command = LOWORD(messageParam);
// | -            commandWindow2 = reinterpret_cast<HWND>(messageData);
// |              notificationType1 = HIWORD(messageParam);
// |              if (command == IDOK)
// |                  EndDialog(dialog, 1);
// ALTERNATIVE command_window_notification (unified source delta from BEFORE):
// | --- before
// | +++ command_window_notification
// | @@ -1,15 +1,11 @@
// |  BOOL CALLBACK AppAbout(HWND dialog, UINT message, WPARAM messageParam, LPARAM messageData) {
// |      i32 command;
// | -    HWND H2_UNUSED(commandWindow2);
// | -    u16 H2_UNUSED(notificationType1);
// |
// |      switch (message) {
// |          case WM_INITDIALOG:
// |              return 1;
// |          case WM_COMMAND:
// |              command = LOWORD(messageParam);
// | -            commandWindow2 = reinterpret_cast<HWND>(messageData);
// | -            notificationType1 = HIWORD(messageParam);
// |              if (command == IDOK)
// |                  EndDialog(dialog, 1);
// |              break;

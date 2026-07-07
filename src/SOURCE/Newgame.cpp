// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Newgame.obj   from: (directly linked into exe)
// functions: 16   data: 8
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_all.h>
VA(0x004b6f40, 0x1d5)
void game::GetMap(void) {}

VA(0x004b7115, 0x77)
void game::ProcessNewMap(struct SMapHeader *) {}

VA(0x004b718c, 0x491)
void game::InitNewGame(struct SMapHeader *) {}

VA(0x004b761d, 0x81)
void game::SetupNetPlayerNames(void) {}

VA(0x004b769e, 0xaca)
int game::NewGame(void) { return 0; }

VA(0x004b8168, 0xf1)
void game::CleanUpNewGameWindow(void) {}

VA(0x004b8259, 0x67d)
void game::InitNewGameWindow(void) {}

VA(0x004b88d6, 0x59c)
void game::UpdateNewGameWindow(void) {}

VA(0x004b8e72, 0xf46)
int NewGameHandler(struct tag_message &) { return 0; }

VA(0x004b9db8, 0x418)
int game::ProcessNGKeyPress(struct tag_message &) { return 0; }

VA(0x004ba1d0, 0x11e)
void game::NGKPSetupDisplayString(char *, unsigned short int) {}

VA(0x004ba2ee, 0xae)
void game::DrawNGKPDisplayString(int) {}

VA(0x004ba39c, 0xb71)
void game::ShowScenInfo(void) {}

VA(0x004baf0d, 0x1c7)
void game::GetLossConditionText(char *) {}

VA(0x004bb0d4, 0x2fb)
void game::GetVictoryConditionText(char *) {}

VA(0x004bb3cf, 0x2a7)
int game::GetSideDesc(char *, int, int) { return 0; }

// ---- data / globals / vtables ----
DATA(0x0051cd20)  // int gbNewGameDialogOver
DATA(0x0051cd24)  // int NGKPcursorFlashOn
DATA(0x00533150)  // char * cNGKPDisplay
DATA(0x00533154)  // int gbNewGameShadowHidden
DATA(0x00533158)  // char * cNGKPCore
DATA(0x0053315c)  // int NGKPcursorIndex
DATA(0x00533160)  // char * * cTextReceivedBuffer
DATA(0x0053316c)  // class icon * NGKPBkg

// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\x_arena.obj   from: (directly linked into exe)
// functions: 3   data: 3
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x004c0080, 0x35c)
int DoArenaDialog(void);

RVA(0x004c03dc, 0x25f)
int ArenaWindowHandler(struct tag_message &);

RVA(0x004c063b, 0x150)
void UpdateArenaIcons(void);

// ---- data / globals / vtables ----
DATA(0x005331a8)  // int choice
DATA(0x005331b0)  // class iconWidget * * skillWidget
DATA(0x005331bc)  // class heroWindow * arenaWinPtr

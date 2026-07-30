#ifndef HOMM2_PLATFORM_NETWORK_H
#define HOMM2_PLATFORM_NETWORK_H

#include <Ints.h>

enum class ComBaudRate : i32 {
    COM_BAUD_2400 = 1,
    COM_BAUD_4800 = 2,
    COM_BAUD_9600 = 3,
    COM_BAUD_19200 = 4,
    COM_BAUD_38400 = 5,
};
using enum ComBaudRate;

inline constexpr i32 SERIAL_BYTE_SIZE = 8;

i16 com_init(u8 port, ComBaudRate baudRate, i32 options);
void com_term(i16 port);
i16 com_rcv(i16 port, u16 size, void* data);
i16 com_snd(i16 port, u16 priority, u16 size, void* data, i32 options);
void comm_wrt_task();

i16 dpnet_init();
void dpnet_term();
i32 dpnet_snd(i32 destination, i32 size, void* data);
i16 dpnet_rcv(i16 source, u16 size, void* data);
void dpProcessMessages();
i32 dpWaitForFirstGuest();
i32 dpWaitForExtraGuests();
i32 dpWaitForHost();

i32 nbnet_init();
extern "C" void __fastcall nb_term();
extern "C" u16 __fastcall nb_rcv(i16 size, void* data);
extern "C" u16 __fastcall nb_snd(i16 destination, i16 size, void* data);
void nb_thr_ctl();
i8 InitNetHost();
i8 InitNetGuest();
i8 WaitForHost();
i8 WaitForGuest();

i16 wsnet_init();
void wsnet_term();
i32 wsnet_snd(i32 destination, i32 size, void* data);
i16 wsnet_rcv(i16 source, u16 size, void* data);
void wsProcessMessages();
i32 wsWaitForFirstGuest();
i32 wsWaitForExtraGuests();
i32 wsWaitForHost();

void network_remove_player(i32 position);

#endif

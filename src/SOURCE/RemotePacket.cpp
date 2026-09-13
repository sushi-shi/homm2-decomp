#include <SOURCE/REMOTE.h>
#include <cstring>

namespace {
constexpr u32 CRC_FEEDBACK_BIT = 0x08000000;
constexpr i32 CRC_WORD_BIT_COUNT = 16;
constexpr i32 CRC_STORAGE_WORD_COUNT = 2;
}

i32 calc_crc_long(u8* data, i32 length) {
    u32 q;
    u32 cksum;
    u32 sum;

    cksum = 0;
    sum = 0;

    while (length-- != 0) {
        q = cksum & CRC_FEEDBACK_BIT;
        cksum <<= 1;
        cksum += *data;
        sum += *data;
        if (q != 0)
            cksum++;
        data++;
    }
    cksum += sum >> CRC_WORD_BIT_COUNT;
    cksum += sum << CRC_WORD_BIT_COUNT;
    return cksum;
}

void calc_crc(u16* crc, u8* data, i32 length) {
    *crc = static_cast<u16>(calc_crc_long(data, length));
}

i32 EncodePacket(u8* data, char source, char destination, i32 length) {
    if (length < 0 || length > 255 || (length > 0 && data == nullptr))
        return 0;
    u16 crc[CRC_STORAGE_WORD_COUNT];

    PacketSend.header.source = source;
    PacketSend.header.destination = destination;
    PacketSend.header.reserved = 0;
    PacketSend.header.payloadSize = length;
    crc[0] = 0;
    PacketSend.header.crc = crc[0];
    if (length > 0)
        memcpy(PacketSend.payload, data, length);
    calc_crc(crc, PacketSend.UnsignedBytes(), length + REMOTE_PACKET_HEADER_SIZE);
    PacketSend.header.crc = crc[0];
    return length + REMOTE_PACKET_HEADER_SIZE;
}

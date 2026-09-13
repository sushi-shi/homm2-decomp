#include <SOURCE/REMOTE.h>
#include <SOURCE/ADVMGR_TYPES.h>
#include <array>
#include <cstring>

RemotePacket PacketSend{};

int main() {
    static_assert(sizeof(RemoteMessage) == REMOTE_MESSAGE_SIZE);
    RemoteMessage outgoing{};
    outgoing.sender = 4;
    outgoing.id = 123456;
    outgoing.type = REMOTE_MESSAGE_RELIABLE;
    outgoing.payloadSize = sizeof(AdventureRemoteSave);
    const AdventureRemoteSave save{1234, -123, 456, 1};
    std::memcpy(outgoing.payload, &save, sizeof(save));
    // Deliberately odd-addressed wire storage; readers must copy, not alias.
    std::array<char, REMOTE_MESSAGE_SIZE + 1> wire{};
    std::memcpy(wire.data() + 1, &outgoing, sizeof(outgoing));
    const auto incoming = ReadRemoteMessage(wire.data() + 1);
    const auto restored = ReadRemotePayload<AdventureRemoteSave>(incoming);
    if (incoming.sender != 4 || incoming.id != 123456
        || incoming.type != REMOTE_MESSAGE_RELIABLE
        || std::memcmp(&save, &restored, sizeof(save)) != 0)
        return 6;
    static_assert(sizeof(PacketSend) == REMOTE_ENCODED_BUFFER_SIZE);
    u8 payload[] = {10, 20, 30};
    const u8 expected[] = {1, 2, 0, 3, 0xce, 2, 10, 20, 30};
    if (EncodePacket(payload, 1, 2, 3) != sizeof(expected)
        || std::memcmp(PacketSend.Bytes(), expected, sizeof(expected)) != 0)
        return 1;
    std::array<u8, 255> largest{};
    for (int index = 0; index < 255; ++index) largest[index] = index;
    for (int length = 0; length <= 255; ++length) {
        std::memset(PacketSend.Bytes(), 0x5a, sizeof(PacketSend));
        if (EncodePacket(largest.data(), 3, 4, length) != length + 6
            || PacketSend.header.payloadSize != length
            || std::memcmp(PacketSend.payload, largest.data(), length) != 0)
            return 2;
        for (int index = length + 6; index < sizeof(PacketSend); ++index)
            if (PacketSend.UnsignedBytes()[index] != 0x5a) return 3;
    }
    const RemotePacket before = PacketSend;
    if (EncodePacket(largest.data(), 1, 2, 256) != 0
        || EncodePacket(largest.data(), 1, 2, -1) != 0
        || EncodePacket(nullptr, 1, 2, 1) != 0
        || std::memcmp(&before, &PacketSend, sizeof(before)) != 0)
        return 4;
    return EncodePacket(nullptr, 1, 2, 0) == 6 ? 0 : 5;
}

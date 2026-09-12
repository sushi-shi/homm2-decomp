// S25: 4/4 independent EncodePacket/DecodePacket array-or-scalar arms,
// build/s25-{axes,manifest}.json; build/s25-batch/results.json, 3.46s.
// All125-byte EncodePacket targets exact; final16 native funcs/allocated
// sections identical and independent125/269-byte retail proofs pass.
// Before: u16 crc[2]/crc2[2], element0 accesses and decayed-array output.
// Both retained scalar arms follow. See docs/reconstruction/S25.md.

i32 EncodePacket(u8* data, char source, char destination, i32 length) {
    u16 crc;

    REMOTE_PACKET(PacketSend)->source = source;
    REMOTE_PACKET(PacketSend)->destination = destination;
    REMOTE_PACKET(PacketSend)->reserved = 0;
    REMOTE_PACKET(PacketSend)->payloadSize = static_cast<char>(length);
    crc = 0;
    REMOTE_PACKET(PacketSend)->crc = crc;
    memcpy(PacketSend + REMOTE_PACKET_HEADER_SIZE, data, length);
    calc_crc(&crc, reinterpret_cast<u8*>(PacketSend), length + REMOTE_PACKET_HEADER_SIZE);
    REMOTE_PACKET(PacketSend)->crc = crc;
    return length + REMOTE_PACKET_HEADER_SIZE;
}

i32 DecodePacket(u8* data, i32) {
    u16 crc;
    i32 H2_UNUSED(res);
    u16 crc2;
    char text[REMOTE_ERROR_TEXT_SIZE];
    u32 len;

    crc2 = 0;
    if (REMOTE_PACKET(packet)->destination != giThisNetPos
        && REMOTE_PACKET(packet)->destination != REMOTE_BROADCAST_PLAYER) {
        sprintf(
            text,
            "not mine %d\n",
            REMOTE_PACKET(packet)->destination
        );
        LogStr(text);
        return 0;
    }
    len = static_cast<u8>(REMOTE_PACKET(packet)->payloadSize);
    crc = REMOTE_PACKET(packet)->crc;
    REMOTE_PACKET(packet)->crc = 0;
    calc_crc(&crc2, reinterpret_cast<u8*>(packet), len + REMOTE_PACKET_HEADER_SIZE);
    if (crc != crc2) {
        sprintf(
            text,
            "CRC Check Failed CRC 1 %d CRC 2 %d\n",
            crc,
            crc2
        );
        LogStr(text);
        return 0;
    }
    memcpy(data, packet + REMOTE_PACKET_HEADER_SIZE, len);
    return 1;
}

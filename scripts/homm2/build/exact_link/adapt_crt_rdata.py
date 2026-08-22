#!/usr/bin/env python3
"""Give late selected VC6 CRT .rdata contributions their reviewed retail order."""

import struct


def reviewed_group(member_index, member_count):
    if member_count != 177:
        raise RuntimeError(f"expected 177 selected CRT members, found {member_count}")
    # Candidate order is already retail-exact through heapinit.obj (rank 73).
    # Retail then owns the syserr strings before 87cdisp/crt0msg, and owns the
    # timeset day/month tables before tzset and the remaining locale tail.
    if member_index == 169:
        return 0
    if 74 <= member_index < 150:
        return 1
    if member_index == 174:
        return 2
    if member_index == 150:
        return 3
    return 4


def adapt(member_index, member_count, payload):
    if member_index < 74:
        return payload
    group = reviewed_group(member_index, member_count)
    data = bytearray(payload)
    section_count = struct.unpack_from("<H", data, 2)[0]
    optional_size = struct.unpack_from("<H", data, 16)[0]
    section_table = 20 + optional_size
    for index in range(section_count):
        header = section_table + index * 40
        name = bytes(data[header:header + 8].rstrip(b"\0"))
        size = struct.unpack_from("<I", data, header + 16)[0]
        if name == b".rdata" and size:
            data[header:header + 8] = f".rdata${group}".encode("ascii")
    return bytes(data)

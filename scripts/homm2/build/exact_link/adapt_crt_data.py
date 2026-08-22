#!/usr/bin/env python3
"""Give selected VC6 CRT .data contributions their reviewed retail order."""

import struct


ORDER = [
    ("exsup.obj", 2),
    ("fpinit.obj", 1),
    ("sqrt.obj", 2),
    ("atan.obj", 2),
    ("dosmap.obj", 1),
    ("pow.obj", 2),
    ("cos.obj", 2),
    ("sin.obj", 2),
    ("wincrt0.obj", 1),
    ("frame.obj", 1),
    ("tidtable.obj", 1),
    ("hooks.obj", 1),
    ("output.obj", 2),
    ("cmiscdat.obj", 1),
    ("mlock.obj", 1),
    ("setlocal.obj", 1),
    ("setlocal.obj", 3),
    ("ctype.obj", 1),
    ("nlsdata1.obj", 1),
    ("87disp.obj", 2),
    ("heapinit.obj", 1),
    ("sbheap.obj", 1),
    ("syserr.obj", 1),
    ("87tran.obj", 2),
    ("87triga.obj", 2),
    ("winxfltr.obj", 1),
    ("ioinit.obj", 1),
    ("crt0msg.obj", 2),
    ("_file.obj", 4),
    ("intrncvt.obj", 1),
    ("getqloc.obj", 2),
    ("getqloc.obj", 38),
    ("getqloc.obj", 39),
    ("getqloc.obj", 61),
    ("adj_fdiv.obj", 2),
    ("util.obj", 1),
    ("mbctype.obj", 1),
    ("strftime.obj", 2),
    ("strftime.obj", 3),
    ("lconv.obj", 1),
    ("fpexcept.obj", 1),
    ("matherr.obj", 1),
    ("fpctrl.obj", 1),
    ("timeset.obj", 2),
    ("tzset.obj", 2),
    ("constpow.obj", 1),
    ("days.obj", 1),
]

RANKS = {identity: rank for rank, identity in enumerate(ORDER)}
ALIGN_16 = {
    ("dosmap.obj", 1),
    ("frame.obj", 1),
    ("heapinit.obj", 1),
    ("tidtable.obj", 1),
}
DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"


def rank_name(rank):
    return (".data$" + DIGITS[rank // 36] + DIGITS[rank % 36]).encode("ascii")


def adapt(member_name, payload):
    data = bytearray(payload)
    section_count = struct.unpack_from("<H", data, 2)[0]
    optional_size = struct.unpack_from("<H", data, 16)[0]
    section_table = 20 + optional_size
    changed = []
    for index in range(section_count):
        header = section_table + index * 40
        name = bytes(data[header:header + 8].rstrip(b"\0"))
        size = struct.unpack_from("<I", data, header + 16)[0]
        identity = (member_name.lower(), index + 1)
        if name != b".data" or not size:
            continue
        if identity not in RANKS:
            raise RuntimeError(f"unranked CRT .data contribution: {identity}")
        rank = RANKS[identity]
        data[header:header + 8] = rank_name(rank)
        if identity in ALIGN_16:
            characteristics = struct.unpack_from("<I", data, header + 36)[0]
            characteristics = (characteristics & ~0x00F00000) | 0x00500000
            struct.pack_into("<I", data, header + 36, characteristics)
        changed.append(identity)
    expected = [identity for identity in ORDER if identity[0] == member_name.lower()]
    if changed != expected:
        raise RuntimeError(
            f"CRT .data contribution mismatch for {member_name}: "
            f"expected {expected}, found {changed}"
        )
    return bytes(data)

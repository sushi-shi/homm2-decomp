"""Frame-slot names for our own objects, read from VC6 /Z7 CodeView records.

`od_slots` predicts where retail put a local; this reports where OUR compile
actually put ours. Having both sides named turns a slot solve into a bijection
you can read off, instead of one you have to infer twice.

The compiler writes `S_BPREL32` (0x1006) records into `.debug$S` under /Z7, each
carrying an ebp displacement and the source identifier that owns it, nested
inside `S_GPROC32`/`S_LPROC32` (0x100a/0x100b) blocks closed by `S_END` (0x0006).
"""

import struct

SYM_END = 0x0006
SYM_BPREL32 = 0x1006
SYM_GPROC32 = 0x100A
SYM_LPROC32 = 0x100B
PROC_NAME_OFFSET = 35


def _sections(obj: bytes):
    """Yield (name, pointer, size) for every COFF section."""
    section_count = struct.unpack_from("<H", obj, 2)[0]
    symbol_offset, symbol_count = struct.unpack_from("<II", obj, 8)
    string_table = symbol_offset + symbol_count * 18
    for index in range(section_count):
        header = 20 + index * 40
        name = obj[header:header + 8]
        if name[:1] == b"/":
            start = string_table + int(name[1:].rstrip(b"\0"))
            name = obj[start:].split(b"\0", 1)[0]
        else:
            name = name.rstrip(b"\0")
        size, pointer = struct.unpack_from("<II", obj, header + 16)
        yield name, pointer, size


def _length_prefixed(body: bytes, offset: int) -> str:
    length = body[offset]
    return body[offset + 1:offset + 1 + length].decode("latin1", "replace")


def _proc_name(body: bytes) -> str:
    """The decorated name in an S_?PROC32 record.

    The fixed part's width moved between CodeView revisions, so accept the name
    at its usual offset and otherwise scan the few plausible ones for a string
    that looks decorated.
    """
    name = _length_prefixed(body, PROC_NAME_OFFSET)
    if name.startswith(("?", "_", "@")):
        return name
    for offset in (32, 33, 34, 35, 36):
        if offset < len(body):
            candidate = _length_prefixed(body, offset)
            if candidate.startswith(("?", "_", "@")):
                return candidate
    return name


def frame_names(obj: bytes) -> dict[str, list[tuple[int, str]]]:
    """Map each function in the object to its (ebp displacement, name) slots."""
    slots: dict[str, list[tuple[int, str]]] = {}
    for name, pointer, size in _sections(obj):
        if name != b".debug$S":
            continue
        blob = obj[pointer:pointer + size]
        # A signature dword precedes the records in CV4/CV5 streams.
        cursor = 4 if struct.unpack_from("<I", blob, 0)[0] in (1, 2) else 0
        open_procs: list[str] = []
        while cursor + 4 <= len(blob):
            record_length, record_type = struct.unpack_from("<HH", blob, cursor)
            if record_length < 2:
                break
            body = blob[cursor + 4:cursor + 2 + record_length]
            if record_type in (SYM_GPROC32, SYM_LPROC32):
                function = _proc_name(body)
                open_procs.append(function)
                slots.setdefault(function, [])
            elif record_type == SYM_END:
                if open_procs:
                    open_procs.pop()
            elif record_type == SYM_BPREL32 and len(body) >= 9:
                if open_procs:
                    displacement = struct.unpack_from("<i", body, 0)[0]
                    slots[open_procs[-1]].append(
                        (displacement, _length_prefixed(body, 8))
                    )
            cursor += 2 + record_length
    return slots


def format_frames(slots: dict[str, list[tuple[int, str]]], match: str = "") -> str:
    """Render deepest-slot-first, which is the order od_slots reasons in."""
    lines = []
    for function, entries in slots.items():
        if match and match not in function:
            continue
        lines.append(f"== {function}")
        seen = set()
        for displacement, name in sorted(entries, key=lambda entry: -entry[0]):
            if (displacement, name) in seen:
                continue
            seen.add((displacement, name))
            lines.append(f"  ebp{displacement:+#07x}  {name}")
    return "\n".join(lines)

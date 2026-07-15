#!/usr/bin/env python3
"""Generate a VC 4.2-compatible middleware import library.

VC 4.2 ``LIB /DEF`` emits old-style COFF import members, but it adds an extra
leading underscore to already-decorated exports and cannot express WinG's
decorated local thunk plus undecorated DLL lookup.  Generate those old-style
members first, then rewrite their COFF symbol/string payloads to the exact
retail ABI before combining them with VC 4.2 LIB.EXE.
"""

from __future__ import annotations

import argparse
import os
import shutil
import struct
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path

from homm2.build.cc_wrap import HOMM2_DIR, ensure_wineserver, find_ci, msvc_dir, winepath_w


@dataclass(frozen=True)
class ImportSpec:
    dll: str
    symbol: str
    ordinal_or_hint: int
    noname: bool = False
    lookup_name: str | None = None


# Preserve retail IAT order and hint values.  Hints are advisory to the loader,
# but retaining them makes the linked import records directly comparable.
MSS_IMPORTS = (
    ("_AIL_set_sample_loop_count@8", 119),
    ("_AIL_set_sample_playback_rate@8", 121),
    ("_AIL_start_sample@4", 142),
    ("_AIL_set_sample_type@12", 123),
    ("_AIL_sample_volume@4", 100),
    ("_AIL_set_sample_volume@8", 125),
    ("_AIL_midiOutOpen@12", 39),
    ("_AIL_startup@0", 146),
    ("_AIL_set_preference@8", 115),
    ("_AIL_waveOutOpen@16", 162),
    ("_AIL_last_error@0", 31),
    ("_AIL_redbook_tracks@4", 70),
    ("_AIL_redbook_track_info@16", 69),
    ("_AIL_redbook_close@4", 56),
    ("_AIL_redbook_open@4", 59),
    ("_AIL_init_sample@4", 29),
    ("_AIL_serve@0", 109),
    ("_AIL_stop_sequence@4", 149),
    ("_AIL_midiOutClose@4", 38),
    ("_AIL_release_sequence_handle@4", 88),
    ("_AIL_set_sequence_loop_count@8", 126),
    ("_AIL_sequence_status@4", 105),
    ("_AIL_resume_sequence@4", 91),
    ("_AIL_init_sequence@12", 30),
    ("_AIL_start_sequence@4", 143),
    ("_AIL_set_XMIDI_master_volume@8", 111),
    ("_AIL_shutdown@0", 140),
    ("_AIL_allocate_sequence_handle@4", 7),
    ("_AIL_allocate_sample_handle@4", 6),
    ("_AIL_sample_status@4", 98),
    ("_AIL_end_sample@4", 21),
    ("_AIL_set_sample_address@12", 116),
    ("_AIL_get_preference@4", 28),
)

SMACK_IMPORTS = (
    ("_SmackSoundUseDirectSound@4", 38),
    ("_SmackSoundUseMSS@4", 33),
    ("_SmackNextFrame@4", 21),
    ("_SmackToBufferRect@8", 28),
    ("_SmackDoFrame@4", 19),
    ("_SmackToBuffer@28", 23),
    ("_SmackClose@4", 18),
    ("_SmackOpen@12", 14),
    ("_SmackWait@4", 32),
    ("_SmackSummary@8", 20),
)

WING_IMPORTS = (
    ("_WinGCreateDC@0", 2, "WinGCreateDC"),
    ("_WinGCreateBitmap@12", 1, "WinGCreateBitmap"),
    ("_WinGRecommendDIBFormat@4", 7, "WinGRecommendDIBFormat"),
    ("_WinGSetDIBColorTable@16", 8, "WinGSetDIBColorTable"),
    ("_WinGStretchBlt@40", 9, "WinGStretchBlt"),
    ("_WinGBitBlt@32", 0, "WinGBitBlt"),
)

# LINK 3.00 pulls old-style import-library members in response to unresolved
# symbols.  These explicit roots reproduce the retail intra-DLL IAT order when
# the vendor libraries precede game objects on the command line.
LINK300_FORCE_WING_IMPORTS = (
    "_WinGBitBlt@32",
    "_WinGCreateDC@0",
    "_WinGCreateBitmap@12",
    "_WinGRecommendDIBFormat@4",
    "_WinGSetDIBColorTable@16",
    "_WinGStretchBlt@40",
)
LINK300_FORCE_SMACK_IMPORTS = (
    "_SmackWait@4",
    "_SmackOpen@12",
    "_SmackClose@4",
    "_SmackSummary@8",
    "_SmackToBuffer@28",
    "_SmackDoFrame@4",
    "_SmackToBufferRect@8",
    "_SmackNextFrame@4",
    "_SmackSoundUseMSS@4",
    "_SmackSoundUseDirectSound@4",
)
LINK300_FORCE_MSS_IMPORTS = tuple(symbol for symbol, _ in reversed(MSS_IMPORTS))
LINK300_FORCED_VENDOR_IMPORTS = (
    LINK300_FORCE_WING_IMPORTS
    + LINK300_FORCE_SMACK_IMPORTS
    + LINK300_FORCE_MSS_IMPORTS
)


def import_specs() -> tuple[ImportSpec, ...]:
    specs = [ImportSpec("mss32.dll", symbol, hint) for symbol, hint in MSS_IMPORTS]
    specs.extend(
        ImportSpec("smackw32.DLL", symbol, ordinal, noname=True)
        for symbol, ordinal in SMACK_IMPORTS
    )
    specs.extend(
        ImportSpec("WING32.dll", symbol, hint, lookup_name=lookup)
        for symbol, hint, lookup in WING_IMPORTS
    )
    return tuple(specs)


def _archive_entries(data: bytes):
    if not data.startswith(b"!<arch>\n"):
        raise ValueError("not a COFF archive")
    offset = 8
    while offset < len(data):
        if offset + 60 > len(data):
            raise ValueError("truncated archive member header")
        header = data[offset:offset + 60]
        if header[58:60] != b"`\n":
            raise ValueError("invalid archive member trailer")
        try:
            size = int(header[48:58].decode("ascii").strip())
        except ValueError as error:
            raise ValueError("invalid archive member size") from error
        name = header[:16].decode("ascii", "replace").rstrip()
        start = offset + 60
        end = start + size
        if end > len(data):
            raise ValueError("truncated archive member")
        yield name, offset, start, end
        offset = end + (size & 1)


def parse_archive_members(data: bytes) -> list[bytes]:
    """Return object members from a Microsoft archive, excluding linker tables."""
    return [data[start:end] for name, _, start, end in _archive_entries(data)
            if name not in ("/", "//")]


def _coff_names(data: bytearray) -> tuple[list[tuple[int, int, str]], int, int]:
    if len(data) < 20:
        raise ValueError("truncated COFF object")
    machine, section_count, _, symbol_offset, symbol_count, optional_size, _ = struct.unpack_from(
        "<HHIIIHH", data, 0
    )
    if machine != 0x014C:
        raise ValueError("expected an i386 COFF object")
    string_offset = symbol_offset + symbol_count * 18
    if string_offset + 4 > len(data):
        raise ValueError("invalid COFF symbol table")
    string_size = struct.unpack_from("<I", data, string_offset)[0]
    if string_offset + string_size > len(data):
        raise ValueError("invalid COFF string table")

    names = []
    index = 0
    while index < symbol_count:
        entry = symbol_offset + index * 18
        zeroes, name_offset = struct.unpack_from("<II", data, entry)
        if zeroes == 0:
            start = string_offset + name_offset
            end = data.find(0, start, string_offset + string_size)
            if name_offset < 4 or end < 0:
                raise ValueError("invalid long COFF symbol name")
            names.append((start, end - start, data[start:end].decode("ascii")))
        else:
            raw = bytes(data[entry:entry + 8])
            names.append((entry, len(raw.rstrip(b"\0")), raw.rstrip(b"\0").decode("ascii")))
        index += 1 + data[entry + 17]
    return names, section_count, 20 + optional_size


def _rewrite_symbols(data: bytearray, replacements: dict[str, str]) -> bytearray:
    """Rebuild the COFF string table after renaming symbols.

    VC 4.2 shares string suffixes: ``__AIL_startup@0`` can point inside the
    storage for ``__imp___AIL_startup@0``.  In-place shortening would therefore
    corrupt the other symbol, so every long name gets a fresh stable offset.
    """
    _, _, _, symbol_offset, symbol_count, _, _ = struct.unpack_from("<HHIIIHH", data, 0)
    old_string_offset = symbol_offset + symbol_count * 18
    old_string_size = struct.unpack_from("<I", data, old_string_offset)[0]
    old_string_end = old_string_offset + old_string_size
    names = _coff_names(data)[0]

    rewritten = bytearray(data[:old_string_offset])
    records = []
    name_index = 0
    symbol_index = 0
    seen_replacements = set()
    while symbol_index < symbol_count:
        entry = symbol_offset + symbol_index * 18
        _, _, old_name = names[name_index]
        name_index += 1
        new_name = replacements.get(old_name, old_name)
        if old_name in replacements:
            seen_replacements.add(old_name)
        records.append((entry, new_name))
        symbol_index += 1 + data[entry + 17]

    string_table = bytearray(b"\0\0\0\0")
    string_offsets = {}
    long_names = sorted(
        {name for _, name in records if len(name.encode("ascii")) > 8},
        key=lambda name: (-len(name), name),
    )
    for name in long_names:
        container = next((other for other in string_offsets if other.endswith(name)), None)
        if container is not None:
            string_offsets[name] = string_offsets[container] + len(container) - len(name)
        else:
            string_offsets[name] = len(string_table)
            string_table.extend(name.encode("ascii") + b"\0")

    for entry, new_name in records:
        encoded = new_name.encode("ascii")
        if len(encoded) <= 8:
            rewritten[entry:entry + 8] = encoded.ljust(8, b"\0")
        else:
            struct.pack_into("<II", rewritten, entry, 0, string_offsets[new_name])

    missing = set(replacements) - seen_replacements
    if missing:
        raise ValueError(f"missing generated COFF symbols: {sorted(missing)}")
    if len(string_table) > old_string_size:
        raise ValueError("rewritten COFF string table grew")
    string_table.extend(b"\0" * (old_string_size - len(string_table)))
    struct.pack_into("<I", string_table, 0, old_string_size)
    rewritten.extend(string_table)
    rewritten.extend(data[old_string_end:])
    if len(rewritten) != len(data):
        raise ValueError("rewritten COFF member changed size")
    return rewritten


def _patch_lookup_section(data: bytearray, hint: int, lookup_name: str) -> None:
    _, section_count, section_offset = _coff_names(data)
    for index in range(section_count):
        header = section_offset + index * 40
        name = bytes(data[header:header + 8]).rstrip(b"\0")
        if name != b".idata$6":
            continue
        raw_size, raw_offset = struct.unpack_from("<II", data, header + 16)
        payload = struct.pack("<H", hint) + lookup_name.encode("ascii") + b"\0"
        if len(payload) > raw_size:
            raise ValueError(f"lookup name {lookup_name!r} does not fit .idata$6")
        data[raw_offset:raw_offset + raw_size] = payload + b"\0" * (raw_size - len(payload))
        return
    raise ValueError(f"named import {lookup_name!r} has no .idata$6 section")


def _patch_descriptor_dll_name(data: bytearray, dll: str) -> bool:
    names = {name for _, _, name in _coff_names(data)[0]}
    if not any(name.startswith("__IMPORT_DESCRIPTOR_") for name in names):
        return False
    _, section_count, section_offset = _coff_names(data)
    for index in range(section_count):
        header = section_offset + index * 40
        if bytes(data[header:header + 8]).rstrip(b"\0") != b".idata$6":
            continue
        raw_size, raw_offset = struct.unpack_from("<II", data, header + 16)
        payload = dll.encode("ascii") + b"\0"
        if len(payload) > raw_size:
            raise ValueError(f"DLL name {dll!r} does not fit descriptor .idata$6")
        data[raw_offset:raw_offset + raw_size] = payload + b"\0" * (raw_size - len(payload))
        return True
    raise ValueError(f"import descriptor for {dll!r} has no .idata$6 section")


def patch_import_member(member: bytes, specs: tuple[ImportSpec, ...]) -> tuple[bytes, bool]:
    """Correct a generated import member; return (bytes, is_function_member)."""
    data = bytearray(member)
    names = {name for _, _, name in _coff_names(data)[0]}
    for spec in specs:
        generated = "_" + spec.symbol
        if generated not in names:
            continue
        generated_imp = "__imp_" + generated
        wanted_imp = "__imp_" + spec.symbol
        data = _rewrite_symbols(
            data,
            {generated: spec.symbol, generated_imp: wanted_imp},
        )
        if not spec.noname:
            _patch_lookup_section(data, spec.ordinal_or_hint, spec.lookup_name or spec.symbol)
        struct.pack_into("<I", data, 4, 0)
        return bytes(data), True
    _patch_descriptor_dll_name(data, specs[0].dll)
    struct.pack_into("<I", data, 4, 0)
    return bytes(data), False


def _patch_linker_names(payload: bytes, replacements: dict[str, str], second: bool) -> bytes:
    data = bytearray(payload)
    if second:
        if len(data) < 4:
            raise ValueError("truncated second archive linker member")
        member_count = struct.unpack_from("<I", data, 0)[0]
        cursor = 4 + member_count * 4
        if cursor + 4 > len(data):
            raise ValueError("invalid second archive linker member")
        symbol_count = struct.unpack_from("<I", data, cursor)[0]
        indices_offset = cursor + 4
        names_offset = indices_offset + symbol_count * 2
        member_indices = list(struct.unpack_from(
            f"<{symbol_count}H", data, indices_offset
        ))
    else:
        if len(data) < 4:
            raise ValueError("truncated first archive linker member")
        symbol_count = struct.unpack_from(">I", data, 0)[0]
        names_offset = 4 + symbol_count * 4
    if names_offset > len(data):
        raise ValueError("invalid archive linker-member name offset")

    names = []
    cursor = names_offset
    for _ in range(symbol_count):
        end = data.find(0, cursor)
        if end < 0:
            raise ValueError("truncated archive linker-member symbol")
        name = bytes(data[cursor:end]).decode("ascii")
        names.append(replacements.get(name, name))
        cursor = end + 1
    if second:
        # The second linker member is binary-searched by symbol name.  Dropping
        # one decoration underscore changes sort order, so reorder its member
        # indices with the renamed strings.
        ordered = sorted(zip(names, member_indices), key=lambda item: item[0].encode("ascii"))
        names = [name for name, _ in ordered]
        member_indices = [index for _, index in ordered]
        struct.pack_into(f"<{symbol_count}H", data, indices_offset, *member_indices)
    encoded = b"".join(name.encode("ascii") + b"\0" for name in names)
    capacity = len(data) - names_offset
    if len(encoded) > capacity:
        raise ValueError("rewritten archive linker names grew")
    data[names_offset:] = encoded + b"\0" * (capacity - len(encoded))
    return bytes(data)


def patch_import_archive(archive: bytes, specs: tuple[ImportSpec, ...]) -> bytes:
    data = bytearray(archive)
    replacements = {}
    for spec in specs:
        generated = "_" + spec.symbol
        replacements[generated] = spec.symbol
        replacements["__imp_" + generated] = "__imp_" + spec.symbol

    linker_index = 0
    function_count = 0
    for name, header_offset, start, end in _archive_entries(archive):
        data[header_offset + 16:header_offset + 28] = b"0".ljust(12, b" ")
        if name == "/":
            data[start:end] = _patch_linker_names(
                archive[start:end], replacements, second=linker_index == 1
            )
            linker_index += 1
        elif name != "//":
            patched, is_function = patch_import_member(archive[start:end], specs)
            data[start:end] = patched
            function_count += int(is_function)
    if linker_index != 2:
        raise ValueError(f"expected two archive linker members, found {linker_index}")
    if function_count != len(specs):
        raise ValueError(f"patched {function_count} imports, expected {len(specs)}")
    return bytes(data)


def _write_def(path: Path, dll: str, specs: tuple[ImportSpec, ...]) -> None:
    lines = [f"LIBRARY {dll}", "EXPORTS"]
    for spec in specs:
        suffix = f" @{spec.ordinal_or_hint} NONAME" if spec.noname else ""
        lines.append(f"    {spec.symbol}{suffix}")
    path.write_text("\n".join(lines) + "\n", encoding="ascii")


def _run_lib(lib_exe: Path, args: list[str]) -> None:
    subprocess.run(["wine", str(lib_exe), "/NOLOGO", *args], check=True)


def generate_import_libraries(out_dir: Path) -> tuple[Path, Path, Path]:
    if shutil.which("wine") is None or shutil.which("winepath") is None:
        raise RuntimeError("wine/winepath not found; run inside `nix develop .#build`")
    lib_exe = find_ci(msvc_dir() / "bin", "lib.exe")
    if lib_exe is None:
        raise RuntimeError("VC 4.2 LIB.EXE not found; run inside `nix develop .#build`")
    if not Path(os.environ.get("WINEPREFIX", "")).is_dir():
        os.environ["WINEPREFIX"] = str(HOMM2_DIR / "build/wineprefix")
    os.environ.setdefault("WINEDEBUG", "fixme-all,err-kerberos")
    ensure_wineserver()

    out_dir = out_dir.resolve()
    out_dir.mkdir(parents=True, exist_ok=True)
    specs = import_specs()
    by_dll = {dll: tuple(spec for spec in specs if spec.dll == dll)
              for dll in ("mss32.dll", "smackw32.DLL", "WING32.dll")}

    with tempfile.TemporaryDirectory(prefix="homm2-vendor-imports-") as temp_name:
        temp = Path(temp_name)
        outputs = []
        for dll_index, (dll, dll_specs) in enumerate(by_dll.items()):
            stem = f"vendor-{dll_index}"
            def_path = temp / f"{stem}.def"
            lib_path = temp / f"{stem}.lib"
            _write_def(def_path, dll, dll_specs)
            _run_lib(
                lib_exe,
                ["/MACHINE:IX86", f"/DEF:{winepath_w(def_path)}",
                 f"/OUT:{winepath_w(lib_path)}"],
            )
            suffix = ("mss", "smack", "wing")[dll_index]
            output = out_dir / f"vendor-imports-{suffix}.lib"
            output.write_bytes(patch_import_archive(lib_path.read_bytes(), dll_specs))
            outputs.append(output)
        return tuple(outputs)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    outputs = generate_import_libraries(args.out_dir)
    print(
        f"generated {', '.join(str(path) for path in outputs)}: "
        f"{len(MSS_IMPORTS)} Miles, {len(SMACK_IMPORTS)} Smacker, "
        f"{len(WING_IMPORTS)} WinG imports"
    )


if __name__ == "__main__":
    main()

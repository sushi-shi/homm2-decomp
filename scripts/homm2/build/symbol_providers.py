"""Non-source symbol providers for the stripped retail image.

Source annotations remain the primary identity channel.  This module supplies
the two kinds that source cannot name: PE import slots and reviewed
compiler-generated data.  Provider rows are validated against current binary
evidence and never inferred from a previous generated inventory.
"""

from __future__ import annotations

import csv
import re
import struct
from dataclasses import dataclass
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile
from homm2.build.fixed_asm import claims as fixed_asm_claims
from homm2.build.gen_vendor_imports import import_specs


@dataclass(frozen=True, order=True)
class ProviderClaim:
    rva: int
    name: str
    unit: str
    size: int
    kind: str
    provenance: str


def assembly_claims() -> list[ProviderClaim]:
    return [
        ProviderClaim(
            claim.rva, claim.name, unit, claim.size, claim.kind,
            "fixed-masm",
        )
        for unit, _source, claim in fixed_asm_claims()
    ]


def _pe_import_slots(path: Path):
    """Yield ``(iat_rva, dll, name-or-ordinal)`` in retail IAT order."""
    data = Path(path).read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    coff = pe + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    section_table = optional + optional_size
    sections = []
    for index in range(section_count):
        offset = section_table + index * 40
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
            "<IIII", data, offset + 8
        )
        sections.append((rva, max(virtual_size, raw_size), raw_offset))

    def rva_offset(rva: int) -> int:
        for base, size, raw in sections:
            if base <= rva < base + size:
                return raw + rva - base
        raise ValueError(f"import RVA 0x{rva:x} is outside {path}")

    def c_string(rva: int) -> str:
        start = rva_offset(rva)
        end = data.find(b"\0", start)
        if end < 0:
            raise ValueError(f"unterminated import string at RVA 0x{rva:x}")
        return data[start:end].decode("ascii")

    directory_rva = struct.unpack_from("<I", data, optional + 104)[0]
    if not directory_rva:
        return
    descriptor = rva_offset(directory_rva)
    while True:
        lookup_rva, timestamp, forwarder, name_rva, iat_rva = struct.unpack_from(
            "<IIIII", data, descriptor
        )
        if not any((lookup_rva, timestamp, forwarder, name_rva, iat_rva)):
            break
        dll = c_string(name_rva)
        thunk = rva_offset(lookup_rva or iat_rva)
        index = 0
        while True:
            value = struct.unpack_from("<I", data, thunk + index * 4)[0]
            if not value:
                break
            if value & 0x80000000:
                identity: str | int = value & 0xFFFF
            else:
                hint_name = rva_offset(value)
                end = data.find(b"\0", hint_name + 2)
                if end < 0:
                    raise ValueError(
                        f"unterminated import name at RVA 0x{value:x}"
                    )
                identity = data[hint_name + 2:end].decode("ascii")
            yield iat_rva + index * 4, dll, identity
            index += 1
        descriptor += 20


def _archive_members(path: Path):
    data = Path(path).read_bytes()
    if not data.startswith(b"!<arch>\n"):
        raise ValueError(f"not a COFF archive: {path}")
    offset = 8
    while offset:
        if offset == len(data):
            break
        if offset + 60 > len(data) or data[offset + 58:offset + 60] != b"`\n":
            raise ValueError(f"malformed COFF archive: {path}")
        try:
            size = int(data[offset + 48:offset + 58].decode("ascii").strip())
        except ValueError as error:
            raise ValueError(f"malformed archive member size: {path}") from error
        start = offset + 60
        end = start + size
        if end > len(data):
            raise ValueError(f"truncated COFF archive member: {path}")
        yield data[start:end]
        offset = end + (size & 1)


def _short_import_records(path: Path, expected_dll: str):
    """Return exact lookup/ordinal spellings from a pinned SDK import library."""
    names: dict[str, str] = {}
    ordinals: dict[int, str] = {}
    for member in _archive_members(path):
        if len(member) < 20 or member[:4] != b"\0\0\xff\xff":
            continue
        _s1, _s2, _version, _machine, _stamp, size, ordinal, flags = (
            struct.unpack_from("<HHHHIIHH", member)
        )
        strings = member[20:20 + size].split(b"\0")
        if len(strings) < 3:
            raise ValueError(f"malformed short import in {path}")
        public = strings[0].decode("ascii")
        dll = strings[1].decode("ascii")
        if dll.lower() != expected_dll.lower():
            continue
        name = "__imp_" + public
        name_type = (flags >> 2) & 0x7
        if name_type == 0:
            key: str | int = ordinal
            target = ordinals
        elif name_type == 1:
            key = public
            target = names
        elif name_type == 2:
            key = public[1:] if public[:1] in "_@?" else public
            target = names
        elif name_type == 3:
            key = public[1:] if public[:1] in "_@?" else public
            key = key.split("@", 1)[0]
            target = names
        elif name_type == 4 and len(strings) >= 4:
            key = strings[2].decode("ascii")
            target = names
        else:
            raise ValueError(f"unsupported short-import name type in {path}")
        previous = target.setdefault(key, name)
        if previous != name:
            raise ValueError(
                f"{path}: import {key} names both {previous} and {name}"
            )
    return names, ordinals


_STDCALL_SUFFIX = re.compile(r"@\d+$")


def _lookup_key(name: str) -> str:
    return _STDCALL_SUFFIX.sub("", name.removeprefix("_"))


def _candidate_import_names(base_root: Path) -> dict[str, set[str]]:
    """Lookup name -> exact candidate ``__imp_`` spellings."""
    result: dict[str, set[str]] = {}
    for path in sorted(Path(base_root).rglob("*.obj")):
        coff = CoffFile(path)
        for symbol in coff.symbols.values():
            if symbol.section != 0 or symbol.value != 0:
                continue
            symbol_type, storage_class = struct.unpack_from(
                "<HB", coff.data, symbol.offset + 14
            )
            if storage_class != 2:
                continue
            if symbol.name.startswith("__imp_"):
                imported = symbol.name
                caller = symbol.name[len("__imp_"):]
            elif symbol_type == 0x20 and symbol.name.startswith("_"):
                caller = symbol.name
                imported = "__imp_" + caller
            else:
                continue
            result.setdefault(_lookup_key(caller), set()).add(imported)
    return result


def import_claims(exe: Path, base_root: Path, toolchain_lib: Path) -> list[ProviderClaim]:
    candidates = _candidate_import_names(base_root)
    library_names: dict[tuple[str, str], str] = {}
    ordinals: dict[tuple[str, int], str] = {}
    dlls = {dll.lower(): dll for _rva, dll, _identity in _pe_import_slots(exe)}
    for dll_lower, dll in dlls.items():
        library = Path(toolchain_lib) / (Path(dll).stem.upper() + ".LIB")
        if not library.is_file():
            continue
        names, ordinal_names = _short_import_records(library, dll)
        library_names.update({
            (dll_lower, lookup): name for lookup, name in names.items()
        })
        ordinals.update(
            ((dll_lower, ordinal), name)
            for ordinal, name in ordinal_names.items()
        )
    for spec in import_specs():
        if not spec.noname:
            continue
        ordinals[(spec.dll.lower(), spec.ordinal_or_hint)] = "__imp_" + spec.symbol

    rows = []
    seen_names: dict[str, int] = {}
    for rva, dll, identity in _pe_import_slots(exe):
        if isinstance(identity, int):
            name = ordinals.get((dll.lower(), identity))
            if name is None:
                raise ValueError(
                    f"no exact caller spelling for {dll} ordinal {identity}"
                )
        else:
            direct = "__imp_" + identity
            names = candidates.get(_lookup_key(identity), set())
            if direct in names:
                name = direct
            elif len(names) == 1:
                name = next(iter(names))
            elif (dll.lower(), identity) in library_names:
                name = library_names[(dll.lower(), identity)]
            else:
                raise ValueError(
                    f"{dll}!{identity} has {len(names)} candidate import "
                    f"spellings: {sorted(names)}"
                )
        previous = seen_names.setdefault(name, rva)
        if previous != rva:
            raise ValueError(
                f"import identity {name} occupies both 0x{previous:x} and 0x{rva:x}"
            )
        rows.append(ProviderClaim(
            rva, name, "_imports", 4, "data", "retail-pe-import",
        ))
    return sorted(rows)


def compiler_data_claims(
    table: Path, base_root: Path,
) -> list[ProviderClaim]:
    with Path(table).open(newline="", encoding="utf-8") as stream:
        raw = list(csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t",
        ))
    required = {"rva", "size", "name", "class", "owner"}
    if not raw or set(raw[0]) != required:
        raise ValueError(f"{table}: expected columns {sorted(required)}")

    commons: dict[str, list[tuple[Path, int]]] = {}
    objects = sorted(Path(base_root).rglob("*.obj"))
    if not objects:
        raise ValueError(f"no candidate objects under {base_root}")
    for path in objects:
        coff = CoffFile(path)
        for symbol in coff.symbols.values():
            symbol_type, storage_class = struct.unpack_from(
                "<HB", coff.data, symbol.offset + 14
            )
            if (symbol.section == 0 and symbol.value > 0
                    and symbol_type == 0 and storage_class == 2):
                commons.setdefault(symbol.name, []).append((path, symbol.value))

    rows = []
    rvas = set()
    names = set()
    for row in raw:
        rva = int(row["rva"], 0)
        size = int(row["size"], 0)
        name = row["name"].strip()
        if row["class"] != "common" or size <= 0 or name.startswith("$"):
            raise ValueError(f"{table}: invalid compiler-data row {row}")
        if rva in rvas or name in names:
            raise ValueError(f"{table}: duplicate compiler-data identity {row}")
        emitted = commons.get(name, [])
        if not emitted:
            raise ValueError(
                f"{name} at 0x{rva:x} is not emitted by any candidate object"
            )
        wrong = [(path, actual) for path, actual in emitted if actual != size]
        if wrong:
            path, actual = wrong[0]
            raise ValueError(
                f"{name}: {path} emits size 0x{actual:x}, expected 0x{size:x}"
            )
        rvas.add(rva)
        names.add(name)
        rows.append(ProviderClaim(
            rva, name, "_data", size, "data",
            f"reviewed-compiler-data:{row['class']}",
        ))
    return sorted(rows)

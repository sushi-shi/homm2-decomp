#!/usr/bin/env python3
"""Link the reconstruction and emit a PE/RVA audit against the retail image.

The normal objdiff build remains relocatable-object only. This module is the
explicit final-link path used by ``ninja link`` and ``homm2 link``. Object
order is the config/units.toml manifest order - the stripped image carries no
order oracle - and the per-unit anchor audit is the instrument that surfaces
misordered units as source-marker anchors accumulate.
"""
import argparse
import csv
import hashlib
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import tomllib
from collections import defaultdict
from pathlib import Path

from homm2.build.annotated_data import source_definitions as annotated_source_definitions
from homm2.build.extract_resources import read_pe_resources
from homm2.build.gen_vendor_imports import FORCED_VENDOR_IMPORTS
from homm2.build.reloc_owners import load_reviewed_highlow_sites

SCRIPT_DIR = Path(__file__).resolve().parent
REPO = next((p for p in SCRIPT_DIR.parents if (p / "flake.nix").exists()), SCRIPT_DIR)
RETAIL_EXE = REPO / "build/orig/HMM2PL.exe"
RELOC_MANIFEST = REPO / "config/delink_relocs.tsv"
REQUIRED_INITIALIZED_STORAGE = REPO / "config/required_initialized_storage.tsv"
IMAGE_BASE = 0x400000
PE32_MAGIC = 0x10B
COFF_SECTION_HEADER_SIZE = 40
COFF_FILE_HEADER_SIZE = 20
IMAGE_SCN_ALIGN_MASK = 0x00F00000
IMAGE_SCN_MEM_EXECUTE = 0x20000000
IMAGE_DIRECTORY_ENTRY_IMPORT = 1
IMAGE_DIRECTORY_ENTRY_BASERELOC = 5
IMPORT_DESCRIPTOR_SIZE = 20
IMPORT_ORDINAL_FLAG32 = 0x80000000
IMAGE_REL_BASED_HIGHLOW = 3

# Read from the retail PE optional header: VC6 link, image base 0x400000,
# WINDOWS 4.0 subsystem, stack 66112/4096, heap 1048576/4096, and no debug
# data of any kind. Section and file alignment are both 0x1000 (the VC6
# /OPT:WIN98 default). Provisional until the linked bytes prove otherwise.
RETAIL_LINK_FLAGS = (
    "/MACHINE:IX86",
    "/BASE:0x400000",
    "/SUBSYSTEM:WINDOWS,4.0",
    "/STACK:66112,4096",
    "/HEAP:1048576,4096",
    "/INCREMENTAL:NO",
    "/OPT:NOREF",
)
SYSTEM_LIBS_BEFORE_VENDOR = (
    "KERNEL32.LIB",
    "USER32.LIB",
    "GDI32.LIB",
    "WSOCK32.LIB",
)
SYSTEM_LIBS_AFTER_VENDOR = (
    "NETAPI32.LIB",
    "WINMM.LIB",
    "ADVAPI32.LIB",
)


def build_link_command(link_exe, map_path, output, object_paths, import_libraries,
                       resource_path):
    """Compose the LINK input and forced-import order."""
    command = [
        "wine", str(link_exe), *RETAIL_LINK_FLAGS,
        *("/INCLUDE:" + symbol for symbol in FORCED_VENDOR_IMPORTS),
        "/MAP:" + str(map_path),
        "/OUT:" + str(output),
        *SYSTEM_LIBS_BEFORE_VENDOR,
        *map(str, import_libraries),
        *SYSTEM_LIBS_AFTER_VENDOR,
        *map(str, object_paths),
        str(resource_path),
    ]
    return command


def strip_coff_export_directives(source, destination):
    """Copy a candidate COFF while removing source-only ``dllexport`` directives.

    Several reconstructed inline bodies need ``dllexport`` to make the pinned MSVC retain
    the same out-of-line COMDAT as retail.  Those compiler directives are not
    retail final-link inputs, however: the executable exports only AppAbout and
    AppWndProc.  Keep the object used for byte comparison untouched and sanitize
    only the disposable final-link copy.
    """
    source = Path(source)
    destination = Path(destination)
    payload = bytearray(source.read_bytes())
    if len(payload) < COFF_FILE_HEADER_SIZE:
        raise ValueError("short COFF object: %s" % source)
    section_count = struct.unpack_from("<H", payload, 2)[0]
    optional_size = struct.unpack_from("<H", payload, 16)[0]
    first_section = COFF_FILE_HEADER_SIZE + optional_size
    changed = False
    for index in range(section_count):
        header = first_section + index * COFF_SECTION_HEADER_SIZE
        name = bytes(payload[header:header + 8]).rstrip(b"\0")
        if name != b".drectve":
            continue
        raw_size, raw_offset = struct.unpack_from("<II", payload, header + 16)
        directives = bytes(payload[raw_offset:raw_offset + raw_size]).decode("latin-1")
        tokens = directives.split()
        retained = [
            token for token in tokens
            if not token.lower().startswith(("-export:", "/export:"))
        ]
        replacement = " ".join(retained).encode("latin-1")
        if len(replacement) > raw_size:
            raise ValueError("sanitized .drectve grew in %s" % source)
        if len(retained) != len(tokens):
            payload[raw_offset:raw_offset + raw_size] = replacement.ljust(raw_size, b" ")
            changed = True
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(payload)
    return changed


def final_link_objects(objects, units, output_root):
    """Return disposable COFF copies stripped of source-only directives."""
    root = Path(output_root)
    result = []
    stripped = 0
    if len(objects) != len(units):
        raise ValueError("final-link object/unit count differs")
    for index, (source, _unit) in enumerate(zip(map(Path, objects), units)):
        destination = root / ("%03d" % index) / source.name
        stripped += strip_coff_export_directives(source, destination)
        result.append(destination.resolve())
    return result, stripped


def die(message):
    print("[link_exe] ERROR: %s" % message, file=sys.stderr)
    return 1


def find_ci(directory, name):
    if not directory.is_dir():
        return None
    return next((p for p in directory.iterdir() if p.name.lower() == name.lower()), None)


def msvc_dir():
    candidate = Path(os.environ.get("MSVC_DIR", ""))
    if candidate and find_ci(candidate / "bin", "link.exe"):
        return candidate
    return REPO / "build/toolchain/msvc"


def resolve_link_executable(toolchain, override=None):
    override = override or os.environ.get("HOMM2_LINK_EXE")
    if override:
        path = Path(override).expanduser().resolve()
        if not path.is_file():
            raise RuntimeError("linker override is not a file: %s" % path)
        return path, ("argument" if override != os.environ.get("HOMM2_LINK_EXE") else
                      "HOMM2_LINK_EXE")
    path = find_ci(Path(toolchain) / "bin", "link.exe")
    if not path:
        raise RuntimeError("LINK.EXE not found under %s/bin" % toolchain)
    return path, "toolchain default"


def link_environment(library_path, tool_directory, environ=None):
    environment = dict(os.environ if environ is None else environ)
    environment["LIB"] = str(library_path)
    existing_path = environment.get("PATH")
    environment["PATH"] = (str(tool_directory) + os.pathsep + existing_path
                           if existing_path else str(tool_directory))
    return environment


def sibling_tool_identities(link_exe):
    identities = {}
    for name in ("CVTRES.EXE", "MSPDB60.DLL"):
        path = find_ci(Path(link_exe).parent, name)
        if path is not None:
            identities[name] = {
                "path": str(path),
                "sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
            }
    return identities


def winepath_w(path):
    return winepaths_w([path])[0]


def winepaths_w(paths):
    output = subprocess.check_output(
        ["winepath", "-w", *(str(Path(path).resolve()) for path in paths)], text=True,
        stderr=subprocess.DEVNULL).splitlines()
    if len(output) != len(paths):
        raise RuntimeError("winepath returned %d paths for %d inputs" % (len(output), len(paths)))
    return output


def load_link_order(units_path=None, symbols_path=None):
    """Object order for the final link: the units.toml manifest order.

    This target carries no image-side order oracle, so the manifest is the
    tracked, reviewable statement of the link command line. Function anchors
    from the source inventory let the link audit surface misordered units as
    anchor-RVA monotonicity violations; corrections land as units.toml diffs.
    Retail CRT member ordering is deferred until linked-byte evidence exists.
    """
    units_path = Path(units_path or REPO / "config/units.toml")
    symbols_path = Path(symbols_path or REPO / "build/gen/symbol_names.csv")
    manifest = tomllib.loads(units_path.read_text())
    units = manifest.get("unit", [])
    functions = defaultdict(list)
    with symbols_path.open(newline="") as f:
        for row in csv.DictReader(f):
            if row["kind"] == "func":
                functions[row["unit"]].append((int(row["rva"], 16), row["name"]))

    ordered = []
    seen = set()
    for manifest_index, unit in enumerate(units):
        name = unit["unit"]
        if name in seen:
            raise ValueError("duplicate manifest unit: %s" % name)
        seen.add(name)
        ordered_functions = sorted(functions.get(name, []))
        first_rva, first_symbol = (ordered_functions[0] if ordered_functions
                                   else (None, None))
        ordered.append({
            "unit": name,
            "source": unit["source"],
            "manifest_index": manifest_index,
            "first_function_rva": first_rva,
            "first_function_symbol": first_symbol,
            "function_anchors": ordered_functions,
            "order_evidence": "units-manifest",
            "object": REPO / ("build/objdiff/base/%s.obj" % name),
        })
    return ordered


def read_pe(path):
    data = Path(path).read_bytes()
    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ValueError("not a PE file: %s" % path)
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError("missing PE signature: %s" % path)
    coff = pe_offset + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    if struct.unpack_from("<H", data, optional)[0] != PE32_MAGIC:
        raise ValueError("expected PE32 optional header: %s" % path)
    section_offset = optional + optional_size
    sections = {}
    section_order = []
    for index in range(section_count):
        offset = section_offset + index * COFF_SECTION_HEADER_SIZE
        name = data[offset:offset + 8].split(b"\0", 1)[0].decode("ascii", "replace")
        virtual_size, rva, raw_size = struct.unpack_from("<III", data, offset + 8)
        sections[name] = {
            "rva": rva,
            "virtual_size": virtual_size,
            "raw_size": raw_size,
            "characteristics": struct.unpack_from("<I", data, offset + 36)[0],
        }
        section_order.append(name)
    resource_rva, resource_size = struct.unpack_from(
        "<II", data, optional + 96 + 2 * 8)
    return {
        "linker_version": "%d.%d" % struct.unpack_from("<BB", data, optional + 2),
        "size_of_code": struct.unpack_from("<I", data, optional + 4)[0],
        "size_of_initialized_data": struct.unpack_from("<I", data, optional + 8)[0],
        "size_of_uninitialized_data": struct.unpack_from("<I", data, optional + 12)[0],
        "entry_point_rva": struct.unpack_from("<I", data, optional + 16)[0],
        "image_base": struct.unpack_from("<I", data, optional + 28)[0],
        "section_alignment": struct.unpack_from("<I", data, optional + 32)[0],
        "file_alignment": struct.unpack_from("<I", data, optional + 36)[0],
        "stack_reserve": struct.unpack_from("<I", data, optional + 72)[0],
        "stack_commit": struct.unpack_from("<I", data, optional + 76)[0],
        "heap_reserve": struct.unpack_from("<I", data, optional + 80)[0],
        "heap_commit": struct.unpack_from("<I", data, optional + 84)[0],
        "sections": sections,
        "section_order": section_order,
        "resource_directory": {"rva": resource_rva, "size": resource_size},
    }


def compare_pe_section_bytes(retail_path, candidate_path, name, range_limit=32):
    """Compare one PE section byte-for-byte at section-relative offsets."""
    def payload(path):
        data = Path(path).read_bytes()
        pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
        coff = pe_offset + 4
        section_count = struct.unpack_from("<H", data, coff + 2)[0]
        optional_size = struct.unpack_from("<H", data, coff + 16)[0]
        section_offset = coff + 20 + optional_size
        for index in range(section_count):
            offset = section_offset + index * COFF_SECTION_HEADER_SIZE
            section_name = data[offset:offset + 8].split(b"\0", 1)[0].decode(
                "ascii", "replace")
            if section_name != name:
                continue
            raw_size, raw_offset = struct.unpack_from("<II", data, offset + 16)
            return data[raw_offset:raw_offset + raw_size]
        raise ValueError("missing PE section %s in %s" % (name, path))

    retail = payload(retail_path)
    candidate = payload(candidate_path)
    common_size = min(len(retail), len(candidate))
    matched = sum(retail[index] == candidate[index] for index in range(common_size))
    total_size = max(len(retail), len(candidate))
    ranges = []
    index = 0
    while index < common_size and len(ranges) < range_limit:
        if retail[index] == candidate[index]:
            index += 1
            continue
        start = index
        while index < common_size and retail[index] != candidate[index]:
            index += 1
        ranges.append({
            "offset": start,
            "size": index - start,
            "retail_hex": retail[start:min(index, start + 16)].hex(),
            "candidate_hex": candidate[start:min(index, start + 16)].hex(),
        })
    if len(retail) != len(candidate) and len(ranges) < range_limit:
        ranges.append({
            "offset": common_size,
            "size": abs(len(retail) - len(candidate)),
            "kind": "candidate-tail" if len(candidate) > len(retail) else "retail-tail",
            "retail_hex": retail[common_size:common_size + 16].hex(),
            "candidate_hex": candidate[common_size:common_size + 16].hex(),
        })
    mismatch_count = total_size - matched
    return {
        "exact": retail == candidate,
        "retail_size": len(retail),
        "candidate_size": len(candidate),
        "matched_bytes": matched,
        "mismatched_bytes": mismatch_count,
        "match_percent": round(matched * 100.0 / total_size, 6) if total_size else 100.0,
        "retail_sha256": hashlib.sha256(retail).hexdigest(),
        "candidate_sha256": hashlib.sha256(candidate).hexdigest(),
        "first_mismatch_ranges": ranges,
        "range_limit": range_limit,
    }


def pe_section_raw_zero_tail_start(path, name):
    """Return the RVA where a PE section's final all-zero raw tail begins."""
    data = Path(path).read_bytes()
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    coff = pe_offset + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    section_offset = coff + 20 + optional_size
    for index in range(section_count):
        offset = section_offset + index * COFF_SECTION_HEADER_SIZE
        section_name = data[offset:offset + 8].split(b"\0", 1)[0].decode(
            "ascii", "replace")
        if section_name != name:
            continue
        rva, raw_size, raw_offset = struct.unpack_from("<III", data, offset + 12)
        payload = data[raw_offset:raw_offset + raw_size]
        return rva + len(payload.rstrip(b"\0"))
    raise ValueError("missing PE section %s in %s" % (name, path))


def read_pe_payload_evidence(path, rva, size, audit_kind="bytes"):
    """Read one PE span with relocation-normalized and optional pointer-target evidence."""
    data = Path(path).read_bytes()
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    coff = pe_offset + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    image_base = struct.unpack_from("<I", data, optional + 28)[0]
    section_offset = optional + optional_size
    sections = []
    for index in range(section_count):
        offset = section_offset + index * COFF_SECTION_HEADER_SIZE
        virtual_size, section_rva, raw_size, raw_offset = struct.unpack_from(
            "<IIII", data, offset + 8)
        sections.append({
            "rva": section_rva,
            "virtual_size": virtual_size,
            "raw_size": raw_size,
            "raw_offset": raw_offset,
        })

    def raw_offset(rva):
        for section in sections:
            extent = max(section["virtual_size"], section["raw_size"])
            if section["rva"] <= rva < section["rva"] + extent:
                delta = rva - section["rva"]
                if delta >= section["raw_size"]:
                    return None
                return section["raw_offset"] + delta
        raise ValueError("RVA 0x%x is outside PE sections in %s" % (rva, path))

    # The image ships no base-relocation directory; the reviewed site manifest
    # is the only absolute-relocation inventory, so ledger highlow evidence is
    # counted against it (zero sites until sites are reviewed in).
    relocation_rvas = load_reviewed_highlow_sites(RELOC_MANIFEST)

    def read_span(span_rva, span_size):
        payload = bytearray(span_size)
        remaining = span_size
        payload_offset = 0
        current_rva = span_rva
        while remaining:
            section = next((section for section in sections
                            if section["rva"] <= current_rva <
                            section["rva"] + max(section["virtual_size"],
                                                 section["raw_size"])), None)
            if section is None:
                raise ValueError("PE span at RVA 0x%x is outside sections in %s" %
                                 (span_rva, path))
            delta = current_rva - section["rva"]
            chunk = min(remaining,
                        max(section["virtual_size"], section["raw_size"]) - delta)
            raw_chunk = min(chunk, max(section["raw_size"] - delta, 0))
            if raw_chunk:
                start = section["raw_offset"] + delta
                payload[payload_offset:payload_offset + raw_chunk] = data[start:start + raw_chunk]
            remaining -= chunk
            payload_offset += chunk
            current_rva += chunk
        return payload

    payload = read_span(rva, size)
    relative_relocations = sorted(
        relocation - rva for relocation in relocation_rvas if rva <= relocation < rva + size)
    normalized = bytearray(payload)
    for offset in relative_relocations:
        if offset + 4 > size:
            raise ValueError("HIGHLOW relocation crosses audited span in %s" % path)
        normalized[offset:offset + 4] = b"\0\0\0\0"
    evidence = {
        "size": size,
        "nonzero_byte_count": sum(byte != 0 for byte in payload),
        "sha256": hashlib.sha256(payload).hexdigest(),
        "normalized_sha256": hashlib.sha256(normalized).hexdigest(),
        "highlow_base_relocation_count": len(relative_relocations),
        "highlow_relative_offsets": relative_relocations,
    }
    if audit_kind == "cstring-pointer-table":
        targets = []
        aliases = {}
        alias_pattern = []
        for offset in relative_relocations:
            target_va = struct.unpack_from("<I", payload, offset)[0]
            target_rva = target_va - image_base
            target_offset = raw_offset(target_rva)
            if target_offset is None:
                raise ValueError("pointer target RVA 0x%x has no raw bytes in %s" %
                                 (target_rva, path))
            end = data.index(0, target_offset)
            target = data[target_offset:end]
            targets.append({
                "rva": target_rva,
                "sha256": hashlib.sha256(target).hexdigest(),
                "text": target.decode("cp1252"),
            })
            if target_va not in aliases:
                aliases[target_va] = len(aliases)
            alias_pattern.append(aliases[target_va])
        evidence["cstring_targets"] = targets
        evidence["pointer_target_alias_pattern"] = alias_pattern
    elif audit_kind != "bytes":
        raise ValueError("unknown initialized-storage audit kind: %s" % audit_kind)
    return evidence


def add_payload_evidence(public_symbols, retail_path, candidate_path, required):
    """Attach retail and candidate evidence only for explicitly reviewed enrollments."""
    by_name = {row["name"]: row for row in public_symbols["symbols"]}
    for expectation in required:
        row = by_name.get(expectation["name"])
        if row is None:
            continue
        # The inventory's data sizes are provisional; the reviewed enrollment
        # is authoritative here.
        size = expectation["size"]
        readable_size = expectation.get("readable_size")
        row["retail_payload"] = read_pe_payload_evidence(
            retail_path, int(row["retail_rva"], 16), size, expectation["audit"])
        if readable_size is not None:
            row["retail_readable_payload"] = read_pe_payload_evidence(
                retail_path, int(row["retail_rva"], 16), readable_size, "bytes")
        if row["candidate_count"] == 1:
            row["candidate_payload"] = read_pe_payload_evidence(
                candidate_path, int(row["candidate_rva"], 16), size, expectation["audit"])
            if readable_size is not None:
                row["candidate_readable_payload"] = read_pe_payload_evidence(
                    candidate_path, int(row["candidate_rva"], 16), readable_size, "bytes")


def read_imports(path):
    data = Path(path).read_bytes()
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    coff = pe_offset + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    section_offset = optional + optional_size
    sections = []
    for index in range(section_count):
        offset = section_offset + index * COFF_SECTION_HEADER_SIZE
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from("<IIII", data, offset + 8)
        sections.append((rva, max(virtual_size, raw_size), raw_offset))

    def rva_offset(rva):
        for section_rva, size, raw_offset in sections:
            if section_rva <= rva < section_rva + size:
                return raw_offset + rva - section_rva
        raise ValueError("RVA 0x%x is outside raw PE sections in %s" % (rva, path))

    def c_string(offset):
        end = data.index(0, offset)
        return data[offset:end].decode("ascii", "replace")

    import_rva, _import_size = struct.unpack_from(
        "<II", data, optional + 96 + IMAGE_DIRECTORY_ENTRY_IMPORT * 8)
    if not import_rva:
        return []
    imports = []
    descriptor = rva_offset(import_rva)
    while True:
        lookup_rva, timestamp, forwarder, name_rva, address_rva = struct.unpack_from(
            "<IIIII", data, descriptor)
        if not any((lookup_rva, timestamp, forwarder, name_rva, address_rva)):
            break
        dll = c_string(rva_offset(name_rva))
        symbols = []
        thunk = rva_offset(lookup_rva or address_rva)
        while True:
            value = struct.unpack_from("<I", data, thunk)[0]
            thunk += 4
            if not value:
                break
            if value & IMPORT_ORDINAL_FLAG32:
                symbols.append({"ordinal": value & 0xFFFF})
            else:
                import_offset = rva_offset(value)
                symbols.append({
                    "name": c_string(import_offset + 2),
                    "hint": struct.unpack_from("<H", data, import_offset)[0],
                })
        imports.append({"dll": dll, "symbols": symbols})
        descriptor += IMPORT_DESCRIPTOR_SIZE
    return imports


def vendor_imports(imports):
    vendor = {"smackw32.dll", "mss32.dll", "wing32.dll"}
    return [entry for entry in imports if entry["dll"].lower() in vendor]


def normalized_vendor_imports(imports):
    """Preserve DLL order but ignore IAT extraction order within each exact ABI set."""
    result = []
    for entry in vendor_imports(imports):
        symbols = sorted(entry["symbols"], key=lambda symbol: json.dumps(symbol, sort_keys=True))
        result.append({"dll": entry["dll"], "symbols": symbols})
    return result


def normalized_dll_import(imports, dll):
    entry = next((item for item in imports if item["dll"].lower() == dll.lower()), None)
    if entry is None:
        return None
    symbols = sorted(entry["symbols"], key=lambda symbol: json.dumps(symbol, sort_keys=True))
    return {"dll": entry["dll"].lower(), "symbols": symbols}


def import_diagnostics(retail_imports, candidate_imports):
    """Compare the complete import ABI and its linker-sensitive ordering."""
    retail_by_dll = {entry["dll"].lower(): entry for entry in retail_imports}
    candidate_by_dll = {entry["dll"].lower(): entry for entry in candidate_imports}
    dlls = list(retail_by_dll)
    dlls.extend(dll for dll in candidate_by_dll if dll not in retail_by_dll)
    per_dll = []
    for dll in dlls:
        retail = retail_by_dll.get(dll)
        candidate = candidate_by_dll.get(dll)
        retail_symbols = retail["symbols"] if retail else []
        candidate_symbols = candidate["symbols"] if candidate else []
        key = lambda symbol: json.dumps(symbol, sort_keys=True)
        per_dll.append({
            "dll": dll,
            "retail_present": retail is not None,
            "candidate_present": candidate is not None,
            "retail_symbol_count": len(retail_symbols),
            "candidate_symbol_count": len(candidate_symbols),
            "abi_matches_retail": (retail is not None and candidate is not None and
                                   sorted(retail_symbols, key=key) ==
                                   sorted(candidate_symbols, key=key)),
            "iat_order_matches_retail": (retail is not None and candidate is not None and
                                         retail_symbols == candidate_symbols),
        })
    retail_dll_order = [entry["dll"].lower() for entry in retail_imports]
    candidate_dll_order = [entry["dll"].lower() for entry in candidate_imports]
    return {
        "retail": retail_imports,
        "candidate": candidate_imports,
        "dll_order_matches_retail": retail_dll_order == candidate_dll_order,
        "complete_abi_matches_retail": all(row["abi_matches_retail"] for row in per_dll),
        "complete_iat_order_matches_retail": (
            retail_dll_order == candidate_dll_order and
            all(row["iat_order_matches_retail"] for row in per_dll)),
        "per_dll": per_dll,
    }


def parse_unresolved(output):
    symbols = sorted(set(re.findall(r"unresolved external symbol\s+(\S+)", output)))
    classes = defaultdict(list)
    for symbol in symbols:
        if "Smack" in symbol:
            cls = "smackw32.dll"
        elif "AIL_" in symbol:
            cls = "mss32.dll"
        elif "WinG" in symbol:
            cls = "WING32.dll"
        elif symbol.startswith("__imp_"):
            cls = "other import"
        else:
            cls = "project or runtime"
        classes[cls].append(symbol)
    return {"count": len(symbols), "symbols": symbols, "classes": dict(sorted(classes.items()))}


def parse_map_symbols(path):
    symbols = defaultdict(list)
    for record in parse_map_symbol_records(path):
        symbols[record["name"]].append(record["va"])
    return dict(symbols)


def decode_map_symbol_name(value):
    """Decode LINK MAP's printable octal escapes without changing decorated syntax."""
    return re.sub(r"\\([0-7]{3})", lambda match: chr(int(match.group(1), 8)), value)


def parse_map_symbol_records(path):
    records = []
    if not Path(path).exists():
        return records
    address = re.compile(r"^([0-9A-Fa-f]{4}):([0-9A-Fa-f]{8})$")
    absolute = re.compile(r"^[0-9A-Fa-f]{8}$")
    for line in Path(path).read_text(errors="replace").splitlines():
        fields = line.split()
        match = address.match(fields[0]) if len(fields) >= 3 else None
        if not match or not absolute.match(fields[2]):
            continue
        records.append({
            "segment": int(match.group(1), 16),
            "offset": int(match.group(2), 16),
            "name": decode_map_symbol_name(fields[1]),
            "va": int(fields[2], 16),
            "flag": fields[3] if len(fields) >= 5 else None,
            "object": fields[-1] if len(fields) >= 4 else None,
        })
    return records


def parse_map_contributions(path):
    contributions = []
    if not Path(path).exists():
        return contributions
    pattern = re.compile(
        r"^\s*([0-9A-Fa-f]{4}):([0-9A-Fa-f]{8})\s+([0-9A-Fa-f]+)H\s+(\S+)\s+(\S+)")
    for line in Path(path).read_text(errors="replace").splitlines():
        match = pattern.match(line)
        if match:
            contributions.append({
                "segment": int(match.group(1), 16),
                "offset": int(match.group(2), 16),
                "size": int(match.group(3), 16),
                "name": match.group(4),
                "class": match.group(5),
            })
    return contributions


def read_coff_section(path, section_name):
    """Read the raw and alignment-rounded size of one COFF object section."""
    data = Path(path).read_bytes()
    if len(data) < COFF_FILE_HEADER_SIZE:
        raise ValueError("truncated COFF object: %s" % path)
    section_count = struct.unpack_from("<H", data, 2)[0]
    optional_size = struct.unpack_from("<H", data, 16)[0]
    section_offset = COFF_FILE_HEADER_SIZE + optional_size
    if section_offset + section_count * COFF_SECTION_HEADER_SIZE > len(data):
        raise ValueError("truncated COFF section table: %s" % path)
    for index in range(section_count):
        offset = section_offset + index * COFF_SECTION_HEADER_SIZE
        name = data[offset:offset + 8].split(b"\0", 1)[0].decode("ascii", "replace")
        if name != section_name:
            continue
        raw_size = struct.unpack_from("<I", data, offset + 16)[0]
        characteristics = struct.unpack_from("<I", data, offset + 36)[0]
        alignment_code = (characteristics & IMAGE_SCN_ALIGN_MASK) >> 20
        alignment = 1 << (alignment_code - 1) if alignment_code else 1
        aligned_size = (raw_size + alignment - 1) & -alignment
        return {
            "raw_size": raw_size,
            "alignment": alignment,
            "aligned_size": aligned_size,
        }
    raise ValueError("COFF object %s has no %s section" % (path, section_name))


def load_claimed_data_symbols(path=None):
    """Every source-claimed data row in the inventory, sorted by address."""
    path = Path(path or REPO / "build/gen/symbol_names.csv")
    symbols = []
    with path.open(newline="") as f:
        for row in csv.DictReader(f):
            if row["kind"] != "data":
                continue
            if row.get("provenance", "") == "pe-reloc-constant":
                # Synthetic alias rows name relocation targets, not claims.
                continue
            symbols.append({
                "name": row["name"],
                "unit": row["unit"],
                "rva": int(row["rva"], 16),
                "size": int(row["size"], 16),
                "provenance": row.get("provenance", ""),
            })
    symbols.sort(key=lambda row: (row["rva"], row["name"]))
    return symbols


def load_claimed_function_symbols(path=None):
    """Every recovered function identity in the source/compgen inventory."""
    path = Path(path or REPO / "build/gen/symbol_names.csv")
    symbols = []
    with path.open(newline="") as f:
        for row in csv.DictReader(f):
            if row["kind"] != "func":
                continue
            symbols.append({
                "name": row["name"],
                "unit": row["unit"],
                "rva": int(row["rva"], 16),
                "size": int(row["size"], 16),
                "provenance": row.get("provenance", ""),
            })
    symbols.sort(key=lambda row: (row["rva"], row["name"]))
    return symbols


def function_placement_diagnostics(candidate, map_path, retail_symbols=None):
    """Join every recovered function to the candidate MAP by exact linker name."""
    retail_symbols = (load_claimed_function_symbols() if retail_symbols is None
                      else sorted(retail_symbols, key=lambda row: (row["rva"], row["name"])))
    candidate_rvas = defaultdict(set)
    for record in parse_map_symbol_records(map_path):
        candidate_rvas[record["name"]].add(record["va"] - candidate["image_base"])

    rows = []
    for symbol in retail_symbols:
        matches = sorted(candidate_rvas.get(symbol["name"], ()))
        candidate_rva = matches[0] if len(matches) == 1 else None
        delta = candidate_rva - symbol["rva"] if candidate_rva is not None else None
        rows.append({
            **symbol,
            "retail_rva": "0x%x" % symbol["rva"],
            "candidate_count": len(matches),
            "candidate_rva": "0x%x" % candidate_rva if candidate_rva is not None else None,
            "candidate_rvas": ["0x%x" % rva for rva in matches] if len(matches) > 1 else None,
            "delta": delta,
            "status": ("exact" if delta == 0 else "displaced" if delta is not None else
                       "missing" if not matches else "ambiguous"),
        })

    def summary(selected):
        return {
            "total": len(selected),
            "exact_rva": sum(row["status"] == "exact" for row in selected),
            "displaced_rva": sum(row["status"] == "displaced" for row in selected),
            "missing": sum(row["status"] == "missing" for row in selected),
            "ambiguous": sum(row["status"] == "ambiguous" for row in selected),
        }

    provenances = sorted(set(row["provenance"] for row in rows))
    source_rows = [row for row in rows if row["provenance"].startswith("source-")]
    return {
        "summary": summary(rows),
        "source_summary": summary(source_rows),
        "by_provenance": {
            provenance: summary([row for row in rows if row["provenance"] == provenance])
            for provenance in provenances
        },
        "residuals": [row for row in rows if row["status"] != "exact"],
        "functions": rows,
        "interpretation": (
            "Candidate addresses come only from exact decorated-name MAP joins. Duplicate MAP "
            "rows at one RVA are collapsed; missing compiler-generated names remain explicit."),
    }


def load_required_initialized_storage(path=None):
    path = Path(path or REQUIRED_INITIALIZED_STORAGE)
    if not path.exists():
        raise RuntimeError("required initialized-storage enrollment missing: %s" % path)
    with path.open(newline="") as f:
        rows = csv.DictReader(
            (line for line in f if not line.lstrip().startswith("#")), delimiter="\t")
        result = []
        for row in rows:
            expectation = {
                "name": row["name"],
                "unit": row["unit"],
                "size": int(row["size"], 0),
                "retail_sha256": row["retail_sha256"],
                "highlow_count": int(row["highlow_count"], 0),
                "audit": row["audit"],
            }
            readable_size = row.get("readable_size")
            readable_sha256 = row.get("readable_sha256")
            if bool(readable_size) != bool(readable_sha256):
                raise RuntimeError(
                    "readable_size and readable_sha256 must be provided together "
                    "for %s" % row["name"])
            if readable_size:
                expectation["readable_size"] = int(readable_size, 0)
                expectation["readable_sha256"] = readable_sha256
            result.append(expectation)
        return result


def required_initialized_storage_diagnostics(public_symbols, required, source_sizes=None):
    by_name = {row["name"]: row for row in public_symbols["symbols"]}
    source_sizes = source_sizes or {}
    rows = []
    for expectation in required:
        symbol = by_name.get(expectation["name"])
        readable_size = expectation.get("readable_size")
        source_size = (source_sizes.get(
            (expectation["unit"], int(symbol["retail_rva"], 16)))
            if source_sizes and symbol is not None and symbol.get("retail_rva") else None)
        if symbol is None:
            status = "retail-symbol-missing"
        elif symbol["unit"] != expectation["unit"]:
            status = "owner-mismatch"
        elif symbol["candidate_count"] != 1:
            status = symbol["status"]
        elif not symbol["storage_class_matches"]:
            status = "storage-class-mismatch"
        elif source_size is not None and source_size != expectation["size"]:
            status = "source-extent-mismatch"
        elif (symbol["retail_payload"]["size"] != expectation["size"] or
              symbol["retail_payload"]["sha256"] != expectation["retail_sha256"] or
              symbol["retail_payload"]["highlow_base_relocation_count"] !=
              expectation["highlow_count"]):
            status = "reviewed-retail-evidence-mismatch"
        elif symbol.get("candidate_payload") is None:
            status = "candidate-payload-missing"
        elif (readable_size is not None and
              (symbol.get("retail_readable_payload") is None or
               symbol["retail_readable_payload"]["size"] != readable_size or
               symbol["retail_readable_payload"]["sha256"] !=
               expectation["readable_sha256"])):
            status = "reviewed-readable-evidence-mismatch"
        elif (readable_size is not None and
              symbol.get("candidate_readable_payload") is None):
            status = "candidate-readable-payload-missing"
        elif (readable_size is not None and
              (symbol["candidate_readable_payload"]["normalized_sha256"] !=
               symbol["retail_readable_payload"]["normalized_sha256"] or
               symbol["candidate_readable_payload"]["highlow_relative_offsets"] !=
               symbol["retail_readable_payload"]["highlow_relative_offsets"])):
            status = "readable-span-mismatch"
        elif (symbol["candidate_payload"]["highlow_relative_offsets"] !=
              symbol["retail_payload"]["highlow_relative_offsets"]):
            status = "relocation-pattern-mismatch"
        elif (symbol["candidate_payload"]["normalized_sha256"] !=
              symbol["retail_payload"]["normalized_sha256"]):
            status = "normalized-payload-mismatch"
        elif (expectation["audit"] == "cstring-pointer-table" and
              [target["sha256"] for target in symbol["candidate_payload"]["cstring_targets"]] !=
              [target["sha256"] for target in symbol["retail_payload"]["cstring_targets"]]):
            status = "pointer-target-content-mismatch"
        elif (expectation["audit"] == "cstring-pointer-table" and
              symbol["candidate_payload"]["pointer_target_alias_pattern"] !=
              symbol["retail_payload"]["pointer_target_alias_pattern"]):
            status = "pointer-target-alias-mismatch"
        else:
            status = "verified"
        rows.append({
            "name": expectation["name"],
            "unit": expectation["unit"],
            "status": status,
            "retail_storage_class": (
                symbol["retail_storage"]["class"] if symbol is not None else None),
            "candidate_storage_class": (
                symbol["candidate_storage"]["class"]
                if symbol is not None and symbol["candidate_storage"] else None),
            "audit": expectation["audit"],
            "expected_readable_size": readable_size,
            "expected_readable_sha256": expectation.get("readable_sha256"),
            "expected_size": expectation["size"],
            "source_definition_size": source_size,
            "expected_retail_sha256": expectation["retail_sha256"],
            "expected_highlow_count": expectation["highlow_count"],
            "retail_payload": symbol.get("retail_payload") if symbol is not None else None,
            "candidate_payload": symbol.get("candidate_payload") if symbol is not None else None,
            "retail_readable_payload": (
                symbol.get("retail_readable_payload") if symbol is not None else None),
            "candidate_readable_payload": (
                symbol.get("candidate_readable_payload") if symbol is not None else None),
        })
    violations = [row for row in rows if row["status"] != "verified"]
    return {
        "required": len(rows),
        "verified": len(rows) - len(violations),
        "violations": violations,
        "symbols": rows,
        "note": (
            "This checked enrollment contains reviewed initializer recoveries only. It pins "
            "retail size/digest/relocation evidence and compares candidate relocation-normalized "
            "bytes. CString pointer tables also compare target content and alias identity. The "
            "known transitional backlog remains visible in public_symbols; every recovered batch "
            "must be enrolled so initialized storage cannot silently regress."),
    }


def classify_pe_storage(pe, rva):
    readonly = pe["sections"].get(".rdata")
    if readonly and readonly["rva"] <= rva < readonly["rva"] + readonly["virtual_size"]:
        return {"class": "rdata", "section": ".rdata",
                "section_offset": rva - readonly["rva"]}
    writable = pe["sections"].get(".data")
    if writable and writable["rva"] <= rva < writable["rva"] + writable["virtual_size"]:
        offset = rva - writable["rva"]
        storage_class = ("data-initialized" if offset < writable["raw_size"]
                         else "data-loader-zero-tail")
        return {"class": storage_class, "section": ".data", "section_offset": offset}
    for name, section in pe["sections"].items():
        extent = max(section["virtual_size"], section["raw_size"])
        if section["rva"] <= rva < section["rva"] + extent:
            return {"class": "other-section", "section": name,
                    "section_offset": rva - section["rva"]}
    return {"class": "outside-image", "section": None, "section_offset": None}


def classify_candidate_storage(candidate, record, contributions):
    contribution = next((row for row in contributions
                         if row["segment"] == record["segment"] and
                         row["offset"] <= record["offset"] < row["offset"] + row["size"]), None)
    contribution_name = contribution["name"] if contribution else None
    lower_name = contribution_name.lower() if contribution_name else ""
    lower_object = (record["object"] or "").lower()
    if lower_object == "<common>" or lower_name in {".bss", "bss", "common"}:
        storage_class = "data-loader-zero"
        section = ".data"
    elif lower_name.startswith(".rdata"):
        storage_class = "rdata"
        section = ".rdata"
    elif lower_name.startswith(".data") or lower_name.startswith(".crt"):
        storage_class = "data-initialized"
        section = ".data"
    else:
        fallback = classify_pe_storage(candidate, record["va"] - candidate["image_base"])
        storage_class = fallback["class"]
        section = fallback["section"]
    return {
        "class": storage_class,
        "section": section,
        "map_contribution": contribution_name,
        "map_class": contribution["class"] if contribution else None,
        "map_object": record["object"],
        "segment": record["segment"],
        "segment_offset": record["offset"],
    }


def classify_missing_public_data(symbol, candidate_records):
    name = symbol["name"]
    unit = symbol.get("unit") or ""
    candidate_names = set(candidate_records)
    if name.startswith("??_C@") and not unit.startswith(("SOURCE/", "BASE/", "EDITOR/")):
        return {
            "root_cause": "runtime-library-private-literal",
            "actionable": False,
            "evidence": (
                "Retail public is a compiler-generated literal owned by runtime compiland %s; "
                "the current LIBCMT member selection/version does not expose the same identity."
                % unit),
        }
    import_equivalents = {
        "WING32_IMPORT_DESCRIPTOR": "__IMPORT_DESCRIPTOR_WING32",
        "NULL_IMPORT_DESCRIPTOR": "__NULL_IMPORT_DESCRIPTOR",
    }
    if name in import_equivalents:
        equivalent = import_equivalents[name]
        return {
            "root_cause": "legacy-import-library-bookkeeping-name",
            "actionable": False,
            "candidate_equivalent": equivalent if equivalent in candidate_names else None,
            "evidence": (
                "Retail wing32.def import bookkeeping uses the legacy %s name; the generated "
                "import library exposes %s%s. This is linker-owned storage, not a source global."
                % (name, equivalent,
                   " at a different candidate RVA" if equivalent in candidate_names
                   else " (not present)")),
        }
    if name.startswith("__imp__"):
        return {
            "root_cause": "unreferenced-system-import",
            "actionable": True,
            "evidence": (
                "Retail IAT public has no exact candidate MAP entry. Audit the owning call-site "
                "relocation before changing the system import set."),
        }
    if unit.startswith(("SOURCE/", "BASE/", "EDITOR/")):
        return {
            "root_cause": "project-definition-absent",
            "actionable": True,
            "evidence": (
                "Retail public is owned by reconstructed project unit %s, but no exact candidate "
                "MAP definition exists." % unit),
        }
    return {
        "root_cause": "unsupported-or-unlinked-owner",
        "actionable": False,
        "evidence": "No exact candidate MAP definition or proven semantic equivalent exists.",
    }


def static_symbol_diagnostics(retail, candidate, map_path, retail_symbols=None,
                              retail_data_zero_tail_start=None):
    retail_symbols = (load_claimed_data_symbols() if retail_symbols is None
                      else sorted(retail_symbols, key=lambda row: (row["rva"], row["name"])))
    contributions = parse_map_contributions(map_path)
    candidate_records = defaultdict(list)
    for record in parse_map_symbol_records(map_path):
        candidate_records[record["name"]].append(record)

    def normalized_storage(storage_class):
        if storage_class in {"data-loader-zero", "data-loader-zero-tail",
                             "data-loader-zero-padding"}:
            return "data-loader-zero"
        return storage_class

    rows = []
    for symbol in retail_symbols:
        retail_storage = classify_pe_storage(retail, symbol["rva"])
        matches = candidate_records.get(symbol["name"], [])
        row = {
            "name": symbol["name"],
            "unit": symbol.get("unit"),
            "size": symbol.get("size"),
            "retail_rva": "0x%x" % symbol["rva"],
            "retail_storage": retail_storage,
            "candidate_count": len(matches),
            "candidate_rva": None,
            "candidate_storage": None,
            "delta": None,
            "storage_class_matches": None,
            "status": "missing",
        }
        if len(matches) > 1:
            row["status"] = "ambiguous"
            row["candidate_rvas"] = [
                "0x%x" % (record["va"] - candidate["image_base"]) for record in matches]
        elif len(matches) == 1:
            record = matches[0]
            candidate_rva = record["va"] - candidate["image_base"]
            candidate_storage = classify_candidate_storage(candidate, record, contributions)
            if (retail_storage["class"] == "data-initialized" and
                    candidate_storage["class"] == "data-loader-zero" and
                    retail_data_zero_tail_start is not None and
                    symbol["rva"] >= retail_data_zero_tail_start):
                retail_storage = dict(retail_storage)
                retail_storage.update({
                    "class": "data-loader-zero-padding",
                    "raw_zero_tail_start": retail_data_zero_tail_start,
                    "evidence": (
                        "Retail bytes from this symbol through the end of the raw .data "
                        "payload are zero file-alignment padding; candidate MAP independently "
                        "classifies the allocation as .bss."),
                })
                row["retail_storage"] = retail_storage
            candidate_section = candidate["sections"].get(candidate_storage["section"])
            candidate_storage["section_offset"] = (
                candidate_rva - candidate_section["rva"] if candidate_section else None)
            class_matches = (normalized_storage(retail_storage["class"]) ==
                             normalized_storage(candidate_storage["class"]))
            delta = candidate_rva - symbol["rva"]
            same_section = retail_storage["section"] == candidate_storage["section"]
            section_relative_delta = (
                candidate_storage["section_offset"] - retail_storage["section_offset"]
                if same_section and candidate_storage["section_offset"] is not None and
                retail_storage["section_offset"] is not None else None)
            if not class_matches:
                status = "storage-class-mismatch"
            elif delta:
                status = "displaced"
            else:
                status = "exact"
            row.update({
                "candidate_rva": "0x%x" % candidate_rva,
                "candidate_storage": candidate_storage,
                "delta": delta,
                "section_relative_delta": section_relative_delta,
                "storage_class_matches": class_matches,
                "status": status,
            })
        else:
            row.update(classify_missing_public_data(symbol, candidate_records))
        rows.append(row)

    runs = []
    active = None
    for index, row in enumerate(rows):
        if row["candidate_count"] != 1:
            active = None
            continue
        run_key = (row["delta"], row["retail_storage"]["class"],
                   row["candidate_storage"]["class"], row["storage_class_matches"])
        if active is None or active["_key"] != run_key:
            active = {
                "_key": run_key,
                "start_index": index,
                "end_index": index,
                "count": 1,
                "first_symbol": row["name"],
                "last_symbol": row["name"],
                "retail_start_rva": row["retail_rva"],
                "retail_end_rva": row["retail_rva"],
                "candidate_start_rva": row["candidate_rva"],
                "candidate_end_rva": row["candidate_rva"],
                "delta": row["delta"],
                "retail_storage_class": row["retail_storage"]["class"],
                "candidate_storage_class": row["candidate_storage"]["class"],
                "storage_class_matches": row["storage_class_matches"],
            }
            runs.append(active)
        else:
            active["end_index"] = index
            active["count"] += 1
            active["last_symbol"] = row["name"]
            active["retail_end_rva"] = row["retail_rva"]
            active["candidate_end_rva"] = row["candidate_rva"]
    for run in runs:
        del run["_key"]

    section_relative_divergences = []
    previous_by_section = {}
    for row in rows:
        if row["candidate_count"] != 1 or row.get("section_relative_delta") is None:
            continue
        section = row["retail_storage"]["section"]
        previous = previous_by_section.get(section)
        if row["section_relative_delta"] != 0 and row["section_relative_delta"] != previous:
            section_relative_divergences.append({
                "name": row["name"],
                "unit": row["unit"],
                "section": section,
                "retail_rva": row["retail_rva"],
                "candidate_rva": row["candidate_rva"],
                "retail_section_offset": row["retail_storage"]["section_offset"],
                "candidate_section_offset": row["candidate_storage"]["section_offset"],
                "section_relative_delta": row["section_relative_delta"],
                "previous_section_relative_delta": previous,
            })
        previous_by_section[section] = row["section_relative_delta"]

    run_starts = {run["start_index"]: run for run in runs}
    divergences = []
    previous_delta = None
    for index, row in enumerate(rows):
        if row["candidate_count"] != 1:
            divergences.append({
                "kind": row["status"],
                "name": row["name"],
                "retail_rva": row["retail_rva"],
                "candidate_count": row["candidate_count"],
            })
            continue
        run = run_starts.get(index)
        if run and (run["delta"] != 0 or not run["storage_class_matches"]):
            divergences.append({
                "kind": ("storage-class-run-start" if not run["storage_class_matches"]
                         else "displacement-run-start"),
                "name": row["name"],
                "retail_rva": row["retail_rva"],
                "candidate_rva": row["candidate_rva"],
                "delta": row["delta"],
                "previous_unique_delta": previous_delta,
                "retail_storage_class": run["retail_storage_class"],
                "candidate_storage_class": run["candidate_storage_class"],
                "run_symbol_count": run["count"],
            })
        previous_delta = row["delta"]

    status_counts = defaultdict(int)
    retail_class_counts = defaultdict(int)
    candidate_class_counts = defaultdict(int)
    missing_root_causes = defaultdict(int)
    for row in rows:
        status_counts[row["status"]] += 1
        retail_class_counts[row["retail_storage"]["class"]] += 1
        if row["candidate_storage"]:
            candidate_class_counts[row["candidate_storage"]["class"]] += 1
        if row["status"] == "missing":
            missing_root_causes[row["root_cause"]] += 1
    unique = sum(1 for row in rows if row["candidate_count"] == 1)
    class_matches = sum(1 for row in rows if row["storage_class_matches"] is True)
    class_mismatches = sum(1 for row in rows if row["storage_class_matches"] is False)
    return {
        "retail_inventory": (
            "Source-claimed data symbols classified by retail RVA; synthetic PDB "
            "procedure records are not used."),
        "summary": {
            "retail_public_data_symbols": len(rows),
            "candidate_unique_name_matches": unique,
            "candidate_missing": status_counts["missing"],
            "candidate_ambiguous": status_counts["ambiguous"],
            "exact_rva": sum(1 for row in rows
                             if row["candidate_count"] == 1 and row["delta"] == 0),
            "displaced_rva": sum(1 for row in rows
                                 if row["candidate_count"] == 1 and row["delta"] != 0),
            "storage_class_matches": class_matches,
            "storage_class_mismatches": class_mismatches,
            "status_counts": dict(sorted(status_counts.items())),
            "retail_storage_class_counts": dict(sorted(retail_class_counts.items())),
            "candidate_storage_class_counts": dict(sorted(candidate_class_counts.items())),
            "missing_root_cause_counts": dict(sorted(missing_root_causes.items())),
            "constant_displacement_runs": len(runs),
            "divergence_run_starts": sum(
                1 for run in runs if run["delta"] != 0 or not run["storage_class_matches"]),
            "section_relative_divergence_run_starts": len(section_relative_divergences),
        },
        "first_divergences": divergences[:20],
        "first_section_relative_divergences": section_relative_divergences[:20],
        "missing_symbols": [{
            key: row.get(key) for key in (
                "name", "unit", "retail_rva", "size", "retail_storage", "root_cause",
                "actionable", "candidate_equivalent", "evidence")
        } for row in rows if row["status"] == "missing"],
        "displacement_runs": runs,
        "symbols": rows,
        "interpretation": (
            "Only exact decorated-name matches are correlated. A constant-displacement run is "
            "reported once at its first symbol; later symbols in that run are cumulative layout "
            "consequences, not independent requests for padding or forced addresses. Section-"
            "relative deltas remove whole-section RVA drift and expose the first contribution "
            "change within each section."),
    }
def write_missing_data_report(path, public_symbols):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    columns = [
        "retail_rva", "name", "unit", "size", "root_cause", "actionable",
        "candidate_equivalent", "evidence",
    ]
    with path.open("w", newline="") as output:
        writer = csv.DictWriter(output, fieldnames=columns, delimiter="\t", extrasaction="ignore")
        writer.writeheader()
        writer.writerows(public_symbols["missing_symbols"])


def section_diagnostics(retail, candidate):
    result = []
    names = list(retail["sections"])
    names.extend(name for name in candidate["sections"] if name not in retail["sections"])
    for name in names:
        expected = retail["sections"].get(name)
        actual = candidate["sections"].get(name)
        delta = None
        if expected and actual:
            delta = {key: actual[key] - expected[key]
                     for key in ("rva", "virtual_size", "raw_size")}
        result.append({"name": name, "retail": expected, "candidate": actual, "delta": delta})
    return result


def resource_diagnostics(retail_path, candidate_path, retail, candidate):
    retail_resources = read_pe_resources(retail_path)
    candidate_resources = read_pe_resources(candidate_path)

    def identity(resource):
        return (resource["type"], resource["name"], resource["language"])

    def record(resource):
        return {
            "type": resource["type"],
            "name": resource["name"],
            "language": resource["language"],
            "size": len(resource["data"]),
            "sha256": hashlib.sha256(resource["data"]).hexdigest(),
        }

    retail_records = [record(resource) for resource in retail_resources]
    candidate_records = [record(resource) for resource in candidate_resources]
    retail_by_identity = {identity(resource): resource for resource in retail_resources}
    candidate_by_identity = {identity(resource): resource for resource in candidate_resources}
    identities_match = set(retail_by_identity) == set(candidate_by_identity)
    payloads_match = identities_match and all(
        retail_by_identity[key]["data"] == candidate_by_identity[key]["data"]
        for key in retail_by_identity)
    payload_order_matches = ([identity(resource) for resource in retail_resources] ==
                             [identity(resource) for resource in candidate_resources])
    expected = retail["sections"].get(".rsrc")
    actual = candidate["sections"].get(".rsrc")
    section_size_matches = bool(expected and actual and
                                expected["raw_size"] == actual["raw_size"] and
                                expected["virtual_size"] == actual["virtual_size"])
    characteristics_match = bool(expected and actual and
                                 expected["characteristics"] == actual["characteristics"])
    section_order_matches = retail["section_order"] == candidate["section_order"]
    return {
        "semantic_match": (identities_match and payloads_match and payload_order_matches and
                           section_size_matches and characteristics_match and
                           section_order_matches),
        "inventory_matches": identities_match,
        "payloads_match": payloads_match,
        "payload_order_matches": payload_order_matches,
        "section_size_matches": section_size_matches,
        "section_characteristics_match": characteristics_match,
        "section_order_matches": section_order_matches,
        "retail_section": expected,
        "candidate_section": actual,
        "section_rva_delta": actual["rva"] - expected["rva"] if expected and actual else None,
        "retail_directory": retail["resource_directory"],
        "candidate_directory": candidate["resource_directory"],
        "retail_section_order": retail["section_order"],
        "candidate_section_order": candidate["section_order"],
        "retail_directory_timestamps": sorted(set(
            resource["directory_timestamp"] for resource in retail_resources)),
        "candidate_directory_timestamps": sorted(set(
            resource["directory_timestamp"] for resource in candidate_resources)),
        "retail": retail_records,
        "candidate": candidate_records,
        "note": (
            "Directory timestamps are linker-generated and intentionally excluded from the "
            "semantic match. Resource identities, languages, payload bytes, payload order, "
            "section sizes, section characteristics, and PE section order are enforced."),
    }


def static_storage_diagnostics(retail, candidate, map_path, retail_symbols=None,
                               retail_path=RETAIL_EXE):
    def section_pair(name):
        expected = retail["sections"].get(name, {"raw_size": 0, "virtual_size": 0, "rva": 0})
        actual = candidate["sections"].get(name, {"raw_size": 0, "virtual_size": 0, "rva": 0})
        return {
            "retail": expected,
            "candidate": actual,
            "delta": {key: actual[key] - expected[key]
                      for key in ("rva", "raw_size", "virtual_size")},
            "candidate_to_retail_percent": {
                key: round(actual[key] * 100.0 / expected[key], 4) if expected[key] else None
                for key in ("raw_size", "virtual_size")
            },
        }

    writable = section_pair(".data")
    readonly = section_pair(".rdata")
    expected_data = writable["retail"]
    actual_data = writable["candidate"]
    contributions = parse_map_contributions(map_path)
    section_names = list(candidate["sections"])
    data_segment = section_names.index(".data") + 1 if ".data" in section_names else None
    data_contributions = [row for row in contributions if row["segment"] == data_segment]
    zero_names = {".bss", "bss", "common"}
    zero_fill = [row for row in data_contributions if row["name"].lower() in zero_names]
    initialized = [row for row in data_contributions if row not in zero_fill]
    return {
        "read_only_initialized": readonly,
        "writable": writable,
        "retail_writable_loader_zero_tail_bytes": max(
            expected_data["virtual_size"] - expected_data["raw_size"], 0),
        "candidate_writable_loader_zero_tail_bytes": max(
            actual_data["virtual_size"] - actual_data["raw_size"], 0),
        "candidate_map_initialized_contribution_bytes": sum(row["size"] for row in initialized),
        "candidate_map_zero_fill_common_contribution_bytes": sum(row["size"] for row in zero_fill),
        "candidate_map_zero_fill_common_contributions": zero_fill,
        "retail_zero_fill_common_contribution_bytes": None,
        "retail_pe_size_of_uninitialized_data": retail["size_of_uninitialized_data"],
        "candidate_pe_size_of_uninitialized_data": candidate["size_of_uninitialized_data"],
        "public_symbols": static_symbol_diagnostics(
            retail, candidate, map_path, retail_symbols=retail_symbols,
            retail_data_zero_tail_start=pe_section_raw_zero_tail_start(
                retail_path, ".data")),
        "retail_zero_fill_note": (
            "Retail has no map. PE evidence proves only a loader-zero writable .data tail; "
            "it does not prove that the entire tail was an independent .bss/common contribution."),
    }


def compare_file_bytes(retail_path, candidate_path):
    retail = Path(retail_path).read_bytes()
    candidate = Path(candidate_path).read_bytes()
    common = min(len(retail), len(candidate))
    matched = sum(retail[index] == candidate[index] for index in range(common))
    total = max(len(retail), len(candidate))
    return {
        "exact": retail == candidate,
        "retail_size": len(retail),
        "candidate_size": len(candidate),
        "matched_bytes": matched,
        "mismatched_bytes": total - matched,
        "match_percent": round(matched * 100.0 / total, 6) if total else 100.0,
        "retail_sha256": hashlib.sha256(retail).hexdigest(),
        "candidate_sha256": hashlib.sha256(candidate).hexdigest(),
    }


def audit_existing_link(output, map_path=None, report_path=None,
                        required_initialized_path=None, strict=False):
    """Audit the executable produced by the current direct Ninja link."""
    output = Path(output).resolve()
    map_path = Path(map_path or output.with_suffix(".map")).resolve()
    report_path = Path(report_path or output.with_suffix(".link.json")).resolve()
    missing_data_path = output.with_suffix(".missing-data.tsv")
    if not RETAIL_EXE.exists():
        raise RuntimeError("retail executable missing: %s" % RETAIL_EXE)
    if not output.exists():
        raise RuntimeError("candidate executable missing: %s" % output)
    if not map_path.exists():
        raise RuntimeError("candidate MAP missing: %s" % map_path)

    retail = read_pe(RETAIL_EXE)
    candidate = read_pe(output)
    resources = resource_diagnostics(RETAIL_EXE, output, retail, candidate)
    imports = import_diagnostics(read_imports(RETAIL_EXE), read_imports(output))
    static_storage = static_storage_diagnostics(retail, candidate, map_path)
    static_storage["section_bytes"] = {
        name: compare_pe_section_bytes(RETAIL_EXE, output, name)
        for name in (".rdata", ".data")
    }
    required = load_required_initialized_storage(required_initialized_path)
    source_sizes = {
        (row.unit, row.rva): row.size
        for row in annotated_source_definitions(REPO / "src", REPO)
    }
    add_payload_evidence(
        static_storage["public_symbols"], RETAIL_EXE, output, required)
    static_storage["required_initialized"] = required_initialized_storage_diagnostics(
        static_storage["public_symbols"], required, source_sizes)

    section_bytes = {
        name: compare_pe_section_bytes(RETAIL_EXE, output, name)
        for name in retail["section_order"]
        if name in candidate["sections"]
    }
    file_bytes = compare_file_bytes(RETAIL_EXE, output)
    functions = function_placement_diagnostics(candidate, map_path)
    closure = {
        "file_bytes_exact": file_bytes["exact"],
        "section_geometry_exact": all(
            row["retail"] == row["candidate"] for row in section_diagnostics(retail, candidate)),
        "section_bytes_exact": (retail["section_order"] == candidate["section_order"] and
                                all(row["exact"] for row in section_bytes.values())),
        "imports_exact": imports["complete_iat_order_matches_retail"],
        "resources_exact": resources["semantic_match"],
        "source_function_rvas_exact": not any(
            row["status"] != "exact" for row in functions["functions"]
            if row["provenance"].startswith("source-")),
        "reviewed_function_rvas_exact": not functions["residuals"],
        "required_initialized_storage_exact": not static_storage[
            "required_initialized"]["violations"],
    }
    report = {
        "status": "exact" if file_bytes["exact"] else "different",
        "mode": "audit-existing-direct-link",
        "candidate_path": str(output),
        "map_path": str(map_path),
        "retail": retail,
        "candidate": candidate,
        "entry_point_delta": candidate["entry_point_rva"] - retail["entry_point_rva"],
        "sections": section_diagnostics(retail, candidate),
        "section_bytes": section_bytes,
        "file_bytes": file_bytes,
        "imports": imports,
        "resources": resources,
        "static_storage": static_storage,
        "function_placement": functions,
        "closure": closure,
    }
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    write_missing_data_report(missing_data_path, static_storage["public_symbols"])

    function_summary = functions["source_summary"]
    print("link audit: sections %d/%d byte-exact; whole file %.6f%%" % (
        sum(row["exact"] for row in section_bytes.values()), len(section_bytes),
        file_bytes["match_percent"]))
    print("link audit: source function RVAs %d/%d exact; %d displaced, %d unavailable" % (
        function_summary["exact_rva"], function_summary["total"],
        function_summary["displaced_rva"],
        function_summary["missing"] + function_summary["ambiguous"]))
    print("link audit: import ABI %s; IAT order %s; resources %s" % (
        "matches" if imports["complete_abi_matches_retail"] else "differs",
        "matches" if imports["complete_iat_order_matches_retail"] else "differs",
        "match" if resources["semantic_match"] else "differ"))
    print("link audit: %s" % report_path)
    return 0 if not strict or file_bytes["exact"] else 1


def write_order_response(path):
    """Write a relocatable LINK response file in units.toml manifest order."""
    path = Path(path).resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    order = load_link_order()
    lines = []
    for row in order:
        relative = os.path.relpath(row["object"], path.parent).replace("/", "\\")
        lines.append('"%s"' % relative)
    path.write_text("\n".join(lines) + "\n", encoding="ascii")
    anchors = [row["first_function_rva"] for row in order
               if row["first_function_rva"] is not None]
    violations = sum(1 for previous, current in zip(anchors, anchors[1:])
                     if current < previous)
    print("link order: %d manifest units -> %s (%d anchors, %d order violations)" %
          (len(order), path, len(anchors), violations))
    return 0


def read_order_response(path):
    path = Path(path).resolve()
    objects = []
    for line in path.read_text(encoding="ascii").splitlines():
        value = line.strip()
        if not value:
            continue
        if len(value) < 2 or value[0] != '"' or value[-1] != '"':
            raise ValueError("invalid object response line: %s" % line)
        objects.append((path.parent / value[1:-1].replace("\\", "/")).resolve())
    return objects


def run_link(output, order_response, imports_libraries, resource_path, linker_override=None,
             required_initialized_path=None):
    output = Path(output).resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    report_path = output.with_suffix(".link.json")
    missing_data_path = output.with_suffix(".missing-data.tsv")
    map_path = output.with_suffix(".map")
    log_path = output.with_suffix(".link.log")
    toolchain = msvc_dir()
    link_exe, linker_source = resolve_link_executable(toolchain, linker_override)
    if shutil.which("wine") is None or shutil.which("winepath") is None:
        raise RuntimeError("wine and winepath are required; use `nix develop .#build`")
    if not RETAIL_EXE.exists():
        raise RuntimeError("retail executable missing: %s" % RETAIL_EXE)
    order_response = Path(order_response).resolve()
    imports_libraries = [Path(path).resolve() for path in imports_libraries]
    resource_path = Path(resource_path).resolve()
    if not order_response.exists():
        raise RuntimeError("link-order response missing: %s" % order_response)
    missing_imports = [str(path) for path in imports_libraries if not path.exists()]
    if missing_imports:
        raise RuntimeError("middleware import libraries missing:\n  " + "\n  ".join(missing_imports))
    if not resource_path.exists():
        raise RuntimeError("reconstructed resource input missing: %s" % resource_path)

    order = load_link_order()
    missing = [str(row["object"]) for row in order if not row["object"].exists()]
    if missing:
        raise RuntimeError("missing reconstruction objects:\n  " + "\n  ".join(missing))
    response_objects = read_order_response(order_response)
    expected_objects = [row["object"].resolve() for row in order]
    if response_objects != expected_objects:
        raise RuntimeError("link-order response does not match the units.toml order")
    for stale in (output, map_path, missing_data_path):
        stale.unlink(missing_ok=True)
    if find_ci(toolchain / "lib", "LIBCMT.LIB") is None:
        raise RuntimeError("LIBCMT.LIB is missing from the toolchain")
    library_path = ";".join(winepaths_w([toolchain / "lib"]))
    link_objects, stripped_export_objects = final_link_objects(
        response_objects, [row["unit"] for row in order],
        output.parent / "objects-final")
    command = build_link_command(
        link_exe,
        winepath_w(map_path),
        winepath_w(output),
        winepaths_w(link_objects),
        winepaths_w(imports_libraries),
        winepath_w(resource_path),
    )
    run = subprocess.run(command, cwd=output.parent,
                         env=link_environment(library_path, link_exe.parent), text=True,
                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    log_path.write_text(run.stdout, encoding="utf-8")

    unresolved = parse_unresolved(run.stdout)
    retail = read_pe(RETAIL_EXE)
    candidate = read_pe(output) if output.exists() else None
    retail_imports = read_imports(RETAIL_EXE)
    candidate_imports = read_imports(output) if candidate else []
    vendor_import_order_match = (vendor_imports(candidate_imports) == vendor_imports(retail_imports)
                                 if candidate else False)
    vendor_import_abi_match = (
        normalized_vendor_imports(candidate_imports) == normalized_vendor_imports(retail_imports)
        if candidate else False)
    advapi_import_abi_match = (
        normalized_dll_import(candidate_imports, "ADVAPI32.dll") ==
        normalized_dll_import(retail_imports, "ADVAPI32.dll")
        if candidate else False)
    resources = (resource_diagnostics(RETAIL_EXE, output, retail, candidate)
                 if candidate else None)
    resource_match = bool(resources and resources["semantic_match"])
    banner = next((line.strip() for line in run.stdout.splitlines()
                   if "Incremental Linker Version" in line), None)
    report = {
        "status": ("linked" if run.returncode == 0 and candidate and vendor_import_abi_match and
                   vendor_import_order_match and advapi_import_abi_match and resource_match else
                   "resource-mismatch" if run.returncode == 0 and candidate and
                   vendor_import_abi_match and vendor_import_order_match and advapi_import_abi_match else
                   "system-import-mismatch" if run.returncode == 0 and candidate and
                   vendor_import_abi_match and vendor_import_order_match else
                   "vendor-import-order-mismatch" if run.returncode == 0 and candidate and
                   vendor_import_abi_match else
                   "vendor-import-mismatch" if run.returncode == 0 and candidate else "failed"),
        "return_code": run.returncode,
        "linker": {
            "path": str(link_exe),
            "selection_source": linker_source,
            "sha256": hashlib.sha256(link_exe.read_bytes()).hexdigest(),
            "sibling_tools": sibling_tool_identities(link_exe),
            "banner": banner,
            "retail_pe_version": retail["linker_version"],
            "retail_final_linker_evidence": "PE32 MajorLinkerVersion.MinorLinkerVersion",
            "version_matches_retail": bool(
                candidate and candidate["linker_version"] == retail["linker_version"]),
        },
        "order_source": "config/units.toml manifest order",
        "crt_order": "deferred",
        "link_flags": list(RETAIL_LINK_FLAGS),
        "forced_vendor_imports": list(FORCED_VENDOR_IMPORTS),
        "stripped_source_export_objects": stripped_export_objects,
        "link_input_order": {
            "system_libraries_before_vendor": list(SYSTEM_LIBS_BEFORE_VENDOR),
            "vendor_import_libraries": [str(path) for path in imports_libraries],
            "system_libraries_after_vendor": list(SYSTEM_LIBS_AFTER_VENDOR),
            "objects": [str(path) for path in response_objects],
            "resource": str(resource_path),
        },
        "library_search": {"mechanism": "LIB environment", "path": library_path},
        "resource_input": str(resource_path),
        "resources": resources,
        "unresolved": unresolved,
        "retail": retail,
        "candidate": candidate,
        "entry_point_delta": (candidate["entry_point_rva"] - retail["entry_point_rva"]
                              if candidate else None),
        "sections": section_diagnostics(retail, candidate) if candidate else [],
        "static_storage": static_storage_diagnostics(retail, candidate, map_path)
                          if candidate else None,
        "imports": {
            "retail": retail_imports,
            "candidate": candidate_imports,
            "vendor_abi_matches_retail": vendor_import_abi_match,
            "vendor_iat_order_matches_retail": vendor_import_order_match,
            "advapi_abi_matches_retail": advapi_import_abi_match,
        },
        "units": [],
    }
    required_storage_ok = True
    if report["static_storage"]:
        report["static_storage"]["section_bytes"] = {
            name: compare_pe_section_bytes(RETAIL_EXE, output, name)
            for name in (".rdata", ".data")
        }
        required = load_required_initialized_storage(required_initialized_path)
        source_sizes = {
            (row.unit, row.rva): row.size
            for row in annotated_source_definitions(REPO / "src", REPO)
        }
        add_payload_evidence(
            report["static_storage"]["public_symbols"], RETAIL_EXE, output, required)
        required_diagnostics = required_initialized_storage_diagnostics(
            report["static_storage"]["public_symbols"], required, source_sizes)
        report["static_storage"]["required_initialized"] = required_diagnostics
        required_storage_ok = not required_diagnostics["violations"]
        if not required_storage_ok and report["status"] == "linked":
            report["status"] = "required-initialized-storage-mismatch"
    map_symbols = parse_map_symbols(map_path)
    previous_anchor = None
    anchor_order_violations = 0
    for row in order:
        anchor = next(((rva, symbol, map_symbols[symbol][0])
                       for rva, symbol in row["function_anchors"] if symbol in map_symbols), None)
        expected_rva, anchor_symbol, actual_va = (anchor if anchor else
                                                   (row["first_function_rva"],
                                                    row["first_function_symbol"], None))
        if row["first_function_rva"] is not None:
            if (previous_anchor is not None
                    and row["first_function_rva"] < previous_anchor):
                anchor_order_violations += 1
            previous_anchor = row["first_function_rva"]
        candidate_text = read_coff_section(row["object"], ".text")
        report["units"].append({
            "unit": row["unit"],
            "rva_anchor": anchor_symbol,
            "retail_rva": ("0x%x" % expected_rva
                           if expected_rva is not None else None),
            "candidate_text_raw_size": "0x%x" % candidate_text["raw_size"],
            "candidate_text_alignment": "0x%x" % candidate_text["alignment"],
            "candidate_text_aligned_size": "0x%x" % candidate_text["aligned_size"],
            "candidate_rva": "0x%x" % (actual_va - IMAGE_BASE) if actual_va is not None else None,
            "delta": (actual_va - IMAGE_BASE - expected_rva
                      if actual_va is not None and expected_rva is not None
                      else None),
        })
    # The order audit: units.toml claims the layout; retail anchors falsify it.
    report["anchor_order_violations"] = anchor_order_violations
    report_path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    if report["static_storage"]:
        write_missing_data_report(missing_data_path, report["static_storage"]["public_symbols"])

    if run.stdout.strip():
        print(run.stdout.rstrip())
    if candidate:
        missing_anchors = sum(1 for unit in report["units"] if unit["delta"] is None)
        displaced = sum(1 for unit in report["units"]
                        if unit["delta"] is not None and unit["delta"] != 0)
        print("link audit: %d manifest-ordered units; %d RVA anchors displaced, "
              "%d unavailable; %d anchor-order violations" %
              (len(order), displaced, missing_anchors, anchor_order_violations))
        print("link audit: entry RVA delta %+d; linker %s vs retail %s" %
              (report["entry_point_delta"], candidate["linker_version"], retail["linker_version"]))
        storage = report["static_storage"]
        print("link audit: .rdata raw/virtual %d/%d vs retail %d/%d; .data %d/%d vs %d/%d" %
              (storage["read_only_initialized"]["candidate"]["raw_size"],
               storage["read_only_initialized"]["candidate"]["virtual_size"],
               storage["read_only_initialized"]["retail"]["raw_size"],
               storage["read_only_initialized"]["retail"]["virtual_size"],
               storage["writable"]["candidate"]["raw_size"],
               storage["writable"]["candidate"]["virtual_size"],
               storage["writable"]["retail"]["raw_size"],
               storage["writable"]["retail"]["virtual_size"]))
        for name in (".rdata", ".data"):
            byte_audit = storage["section_bytes"][name]
            print("link audit: %s bytes %s; %d/%d equal (%.6f%%), %d mismatched" %
                  (name, "EXACT" if byte_audit["exact"] else "DIFFER",
                   byte_audit["matched_bytes"],
                   max(byte_audit["retail_size"], byte_audit["candidate_size"]),
                   byte_audit["match_percent"], byte_audit["mismatched_bytes"]))
        print("link audit: candidate map initialized/zero-fill contributions %d/%d bytes" %
              (storage["candidate_map_initialized_contribution_bytes"],
               storage["candidate_map_zero_fill_common_contribution_bytes"]))
        public_summary = storage["public_symbols"]["summary"]
        print("link audit: static public symbols %d; unique %d, missing %d, ambiguous %d; "
              "exact RVA %d, displaced %d" %
              (public_summary["retail_public_data_symbols"],
               public_summary["candidate_unique_name_matches"],
               public_summary["candidate_missing"],
               public_summary["candidate_ambiguous"],
               public_summary["exact_rva"],
               public_summary["displaced_rva"]))
        print("link audit: static storage class matches %d, mismatches %d; %d displacement runs" %
              (public_summary["storage_class_matches"],
               public_summary["storage_class_mismatches"],
               public_summary["constant_displacement_runs"]))
        required_storage = storage["required_initialized"]
        print("link audit: required initialized storage %d/%d verified" %
              (required_storage["verified"], required_storage["required"]))
        for violation in required_storage["violations"]:
            print("link audit: required initialized storage FAIL %s (%s): %s; retail %s, "
                  "candidate %s" %
                  (violation["name"], violation["unit"], violation["status"],
                   violation["retail_storage_class"],
                   violation["candidate_storage_class"]))
        for verified in required_storage["symbols"]:
            if verified["status"] != "verified":
                continue
            payload = verified["retail_payload"]
            print("link audit: required initialized storage OK %s: candidate normalized bytes "
                  "and targets match; retail %d/%d nonzero bytes, %d HIGHLOW relocations, "
                  "sha256 %s" %
                  (verified["name"], payload["nonzero_byte_count"], payload["size"],
                   payload["highlow_base_relocation_count"], payload["sha256"]))
        if public_summary["missing_root_cause_counts"]:
            print("link audit: missing public-data causes: %s" % ", ".join(
                "%s=%d" % item
                for item in public_summary["missing_root_cause_counts"].items()))
        for divergence in storage["public_symbols"]["first_divergences"][:3]:
            print("link audit: static first divergence %s: %s retail %s candidate %s delta %s" %
                  (divergence["kind"], divergence["name"], divergence["retail_rva"],
                   divergence.get("candidate_rva", "unavailable"),
                   "%+d" % divergence["delta"] if divergence.get("delta") is not None
                   else "unavailable"))
        for divergence in storage["public_symbols"]["first_section_relative_divergences"][:2]:
            print("link audit: %s first relative drift: %s (%s) offset %#x -> %#x, delta %+d" %
                  (divergence["section"], divergence["name"], divergence["unit"],
                   divergence["retail_section_offset"],
                   divergence["candidate_section_offset"],
                   divergence["section_relative_delta"]))
        print("link audit: vendor import ABI %s; intra-DLL IAT order %s" %
              ("matches retail" if vendor_import_abi_match else "DIFFERS FROM RETAIL",
               "matches retail" if vendor_import_order_match else "differs"))
        print("link audit: ADVAPI import ABI %s" %
              ("matches retail" if advapi_import_abi_match else "DIFFERS FROM RETAIL"))
        print("link audit: resources %s; %d leaves; .rsrc raw/virtual %d/%d, RVA delta %+d" %
              ("match retail" if resource_match else "DIFFER FROM RETAIL",
               len(resources["candidate"]),
               resources["candidate_section"]["raw_size"],
               resources["candidate_section"]["virtual_size"],
               resources["section_rva_delta"]))
    if unresolved["count"]:
        print("link audit: %d unresolved symbols: %s" %
              (unresolved["count"], ", ".join("%s=%d" % (key, len(value))
                                               for key, value in unresolved["classes"].items())))
    print("link audit: %s" % report_path)
    print("link audit: %s" % missing_data_path)
    return (0 if run.returncode == 0 and output.exists() and vendor_import_abi_match and
            vendor_import_order_match and advapi_import_abi_match and resource_match and
            required_storage_ok else
            (run.returncode or 1))


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", default=str(REPO / "build/link/HMM2PL.exe"))
    parser.add_argument("--map", dest="map_path")
    parser.add_argument("--report")
    parser.add_argument("--audit-existing", action="store_true",
                        help="audit the current direct-link EXE/MAP without relinking")
    parser.add_argument("--strict", action="store_true",
                        help="make an audit-existing byte difference fail")
    parser.add_argument("--order", default=str(REPO / "build/link/objects.rsp"))
    parser.add_argument("--imports", action="append")
    parser.add_argument("--resource", default=str(REPO / "build/link/HMM2PL.res"))
    parser.add_argument("--linker", help="alternate LINK.EXE for an isolated A/B link")
    parser.add_argument(
        "--required-initialized", default=str(REQUIRED_INITIALIZED_STORAGE),
        help="checked TSV enrollment of reviewed globals that must retain retail storage class")
    parser.add_argument("--write-order", metavar="PATH")
    args = parser.parse_args(argv)
    try:
        if args.write_order:
            return write_order_response(args.write_order)
        if args.audit_existing:
            return audit_existing_link(
                args.out, args.map_path, args.report, args.required_initialized, args.strict)
        imports = args.imports or [
            str(REPO / "build/link/vendor-imports-smack.lib"),
            str(REPO / "build/link/vendor-imports-mss.lib"),
            str(REPO / "build/link/vendor-imports-wing.lib"),
            str(REPO / "build/link/system-imports-advapi.lib"),
        ]
        return run_link(args.out, args.order, imports, args.resource, args.linker,
                        args.required_initialized)
    except (OSError, ValueError, RuntimeError) as exc:
        return die(str(exc))


if __name__ == "__main__":
    sys.exit(main())

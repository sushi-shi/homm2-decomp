#!/usr/bin/env python3
"""Link the reconstruction in retail translation-unit order and emit a PE/RVA audit.

The normal objdiff build remains relocatable-object only. This module is the explicit final-link
path used by ``ninja link`` and ``homm2 link``. Object order comes from executable contribution
records in CodeView NB09 ``sstModule``; missing or ambiguous evidence is a hard error.
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

from homm2.build.extract_resources import read_pe_resources
from homm2.build.build_libcmt_gfy import (
    build_library as build_gfy_libcmt, expected_retail_literals,
)
from homm2.build.gen_vendor_imports import LINK300_FORCED_VENDOR_IMPORTS

SCRIPT_DIR = Path(__file__).resolve().parent
REPO = next((p for p in SCRIPT_DIR.parents if (p / "flake.nix").exists()), SCRIPT_DIR)
RETAIL_EXE = REPO / "build/orig/HEROES2W.EXE"
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
NB09_SST_MODULE = 0x120
NB09_SST_ALIGN_SYM = 0x125
CODEVIEW_S_COMPILE = 0x0001
CODEVIEW_S_THUNK32 = 0x0206

RETAIL_LINK_FLAGS = (
    "/MACHINE:IX86",
    "/DEBUG:NOTMAPPED,MINIMAL",
    "/DEBUGTYPE:CV",
    "/PDB:NONE",
    "/BASE:0x400000",
    "/ALIGN:0x1000",
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
    """Compose the proven LINK 3.00 input and forced-import order."""
    command = [
        "wine", str(link_exe), *RETAIL_LINK_FLAGS,
        *("/INCLUDE:" + symbol for symbol in LINK300_FORCED_VENDOR_IMPORTS),
        "/MAP:" + str(map_path),
        "/OUT:" + str(output),
        *SYSTEM_LIBS_BEFORE_VENDOR,
        *map(str, import_libraries),
        *SYSTEM_LIBS_AFTER_VENDOR,
        *map(str, object_paths),
        str(resource_path),
    ]
    return command


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
    return path, "VC4.2 toolchain default"


def link_environment(library_path, tool_directory, environ=None):
    environment = dict(os.environ if environ is None else environ)
    environment["LIB"] = str(library_path)
    existing_path = environment.get("PATH")
    environment["PATH"] = (str(tool_directory) + os.pathsep + existing_path
                           if existing_path else str(tool_directory))
    return environment


def audit_runtime_literal_storage(candidate, map_path, symbols_path=None):
    symbols_path = symbols_path or REPO / "build/gen/symbol_names.csv"
    expected = expected_retail_literals(symbols_path)
    records = parse_map_symbol_records(map_path)
    by_name = defaultdict(list)
    for record in records:
        by_name[record["name"]].append(record)
    failures = []
    verified = []
    for name, unit in sorted(expected.items()):
        matches = [record for record in by_name.get(name, [])
                   if (record["object"] or "").lower() ==
                   "libcmt:%s.obj" % unit.lower()]
        if len(matches) != 1:
            failures.append({"name": name, "unit": unit,
                             "reason": "expected one LIBCMT map record, found %d" %
                             len(matches)})
            continue
        storage = classify_pe_storage(
            candidate, matches[0]["va"] - candidate["image_base"])
        row = {"name": name, "unit": unit, "storage": storage}
        if storage["class"] != "data-initialized":
            row["reason"] = "expected data-initialized, found %s" % storage["class"]
            failures.append(row)
        else:
            verified.append(row)
    return {"required": len(expected), "verified": len(verified),
            "failures": failures, "symbols": verified}


def sibling_tool_identities(link_exe):
    identities = {}
    for name in ("CVPACK.EXE", "CVTRES.EXE", "MSPDB40.DLL", "MSPDB41.DLL"):
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


def read_nb09_module_contributions(path, executable_only=True):
    """Return module stem -> sstModule ranges, optionally limited to executable sections."""
    data = Path(path).read_bytes()
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    section_count = struct.unpack_from("<H", data, pe_offset + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe_offset + 20)[0]
    section_offset = pe_offset + 24 + optional_size
    executable_segments = set()
    sections = {}
    for index in range(section_count):
        offset = section_offset + index * COFF_SECTION_HEADER_SIZE
        name = data[offset:offset + 8].split(b"\0", 1)[0].decode("ascii", "replace")
        virtual_size, rva, raw_size = struct.unpack_from("<III", data, offset + 8)
        characteristics = struct.unpack_from("<I", data, offset + 36)[0]
        segment = index + 1
        sections[segment] = {
            "name": name,
            "rva": rva,
            "virtual_size": virtual_size,
            "raw_size": raw_size,
            "characteristics": characteristics,
        }
        if characteristics & IMAGE_SCN_MEM_EXECUTE:
            executable_segments.add(segment)

    tail = data.rfind(b"NB09")
    if tail < 0:
        raise ValueError("retail PE has no trailing NB09 directory pointer")
    codeview_base = len(data) - struct.unpack_from("<I", data, tail + 4)[0]
    if data[codeview_base:codeview_base + 4] != b"NB09":
        raise ValueError("invalid NB09 base pointer")
    directory = codeview_base + struct.unpack_from("<I", data, codeview_base + 4)[0]
    header_size, entry_size = struct.unpack_from("<HH", data, directory)
    entry_count = struct.unpack_from("<I", data, directory + 4)[0]
    modules = defaultdict(list)
    for index in range(entry_count):
        entry = directory + header_size + index * entry_size
        subsection, _module_index, offset, size = struct.unpack_from("<HHii", data, entry)
        if subsection != NB09_SST_MODULE:
            continue
        blob = data[codeview_base + offset:codeview_base + offset + size]
        segment_count = struct.unpack_from("<H", blob, 4)[0]
        cursor = 8
        contributions = []
        for _ in range(segment_count):
            segment, _pad, contribution_offset, contribution_size = struct.unpack_from(
                "<HHII", blob, cursor)
            cursor += 12
            if (not executable_only or segment in executable_segments) and contribution_size:
                section = sections.get(segment)
                if section is None:
                    raise ValueError("NB09 contribution names unknown PE segment %d" % segment)
                contributions.append({
                    "section": segment,
                    "section_name": section["name"],
                    "section_rva": section["rva"],
                    "section_virtual_size": section["virtual_size"],
                    "section_raw_size": section["raw_size"],
                    "offset": contribution_offset,
                    "size": contribution_size,
                    "rva": section["rva"] + contribution_offset,
                })
        name_length = blob[cursor]
        name = blob[cursor + 1:cursor + 1 + name_length].decode("latin1", "replace")
        stem = name.replace("\\", "/").rsplit("/", 1)[-1]
        if stem.lower().endswith(".obj"):
            stem = stem[:-4]
        modules[stem.lower()].append({"module": name, "contributions": contributions})
    return modules


def decode_s_compile_banner(body):
    if len(body) < 5:
        raise ValueError("truncated CodeView S_COMPILE record")
    length = body[4]
    if len(body) < 5 + length:
        raise ValueError("truncated CodeView S_COMPILE banner")
    return body[5:5 + length].decode("latin1", "replace")


def read_nb09_tool_provenance(path):
    data = Path(path).read_bytes()
    tail = data.rfind(b"NB09")
    if tail < 0:
        raise ValueError("retail PE has no trailing NB09 directory pointer")
    codeview_base = len(data) - struct.unpack_from("<I", data, tail + 4)[0]
    directory = codeview_base + struct.unpack_from("<I", data, codeview_base + 4)[0]
    header_size, entry_size = struct.unpack_from("<HH", data, directory)
    entry_count = struct.unpack_from("<I", data, directory + 4)[0]
    entries = [struct.unpack_from(
        "<HHii", data, directory + header_size + index * entry_size)
        for index in range(entry_count)]

    modules = {}
    for subsection, module, offset, size in entries:
        if subsection != NB09_SST_MODULE:
            continue
        blob = data[codeview_base + offset:codeview_base + offset + size]
        segment_count = struct.unpack_from("<H", blob, 4)[0]
        cursor = 8 + segment_count * 12
        name_length = blob[cursor]
        modules[module] = blob[cursor + 1:cursor + 1 + name_length].decode(
            "latin1", "replace")

    compile_rows = []
    for subsection, module, offset, size in entries:
        if subsection != NB09_SST_ALIGN_SYM:
            continue
        blob = data[codeview_base + offset:codeview_base + offset + size]
        cursor = 4
        records = []
        while cursor + 4 <= len(blob):
            record_length, record_type = struct.unpack_from("<HH", blob, cursor)
            if not record_length:
                break
            body = blob[cursor + 4:cursor + 2 + record_length]
            records.append((record_type, body))
            cursor += 2 + record_length
        has_thunk = any(record_type == CODEVIEW_S_THUNK32 for record_type, _ in records)
        for record_type, body in records:
            if record_type == CODEVIEW_S_COMPILE:
                compile_rows.append({
                    "banner": decode_s_compile_banner(body),
                    "module": modules.get(module, "<unknown module %d>" % module),
                    "has_thunk": has_thunk,
                })

    banners = []
    for banner in sorted(set(row["banner"] for row in compile_rows)):
        rows = [row for row in compile_rows if row["banner"] == banner]
        banners.append({
            "banner": banner,
            "count": len(rows),
            "thunk_module_count": sum(row["has_thunk"] for row in rows),
            "module_names": sorted(set(row["module"] for row in rows)),
        })
    return {
        "compile_record_count": len(compile_rows),
        "banners": banners,
        "interpretation": (
            "S_COMPILE banners describe their owning NB09 modules. The LINK 2.60 records "
            "are import-thunk modules, not direct evidence for the final executable linker."),
    }


def load_retail_order(units_path=None, symbols_path=None, retail_exe=None,
                      module_contributions=None):
    units_path = Path(units_path or REPO / "config/units.toml")
    symbols_path = Path(symbols_path or REPO / "build/gen/symbol_names.csv")
    manifest = tomllib.loads(units_path.read_text())
    units = manifest.get("unit", [])
    functions = defaultdict(list)
    with symbols_path.open(newline="") as f:
        for row in csv.DictReader(f):
            if row["kind"] == "func":
                functions[row["unit"]].append((int(row["rva"], 16), row["name"]))

    modules = (read_nb09_module_contributions(retail_exe or RETAIL_EXE)
               if module_contributions is None else module_contributions)
    ordered = []
    seen = set()
    for manifest_index, unit in enumerate(units):
        name = unit["unit"]
        if name in seen:
            raise ValueError("duplicate manifest unit: %s" % name)
        seen.add(name)
        stem = Path(unit["source"]).stem.lower()
        module_records = modules.get(stem, [])
        if len(module_records) != 1:
            raise ValueError("expected one NB09 module named %s for %s, found %d" %
                             (stem, name, len(module_records)))
        contributions = module_records[0]["contributions"]
        if len(contributions) != 1:
            raise ValueError("expected one executable NB09 contribution for %s, found %d" %
                             (name, len(contributions)))
        contribution = contributions[0]
        ordered_functions = sorted(functions.get(name, []))
        if ordered_functions:
            first_rva, first_symbol = ordered_functions[0]
        else:
            # Data-only TUs can still carry compiler-generated executable startup code.
            # Their NB09 module contribution is sufficient ordering evidence even when
            # the public symbol stream exposes no function anchor.
            first_rva, first_symbol = contribution["rva"], None
        ordered.append({
            "unit": name,
            "source": unit["source"],
            "manifest_index": manifest_index,
            "first_function_rva": first_rva,
            "first_function_symbol": first_symbol,
            "function_anchors": ordered_functions,
            "order_evidence": ("public-function" if ordered_functions
                               else "module-contribution"),
            "contribution_section": contribution["section"],
            "contribution_offset": contribution["offset"],
            "contribution_size": contribution["size"],
            "contribution_rva": contribution["rva"],
            "object": REPO / ("build/objdiff/base/%s.obj" % name),
        })
    ordered.sort(key=lambda row: (row["contribution_section"], row["contribution_offset"],
                                  row["manifest_index"]))
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

    relocation_rvas = []
    directory = optional + 96 + IMAGE_DIRECTORY_ENTRY_BASERELOC * 8
    relocation_rva, relocation_size = struct.unpack_from("<II", data, directory)
    if relocation_rva and relocation_size:
        cursor = raw_offset(relocation_rva)
        if cursor is None:
            raise ValueError("PE base-relocation directory has no raw bytes in %s" % path)
        end = cursor + relocation_size
        while cursor + 8 <= end:
            page_rva, block_size = struct.unpack_from("<II", data, cursor)
            if block_size < 8 or cursor + block_size > end:
                raise ValueError("invalid PE base-relocation block in %s" % path)
            for entry_offset in range(cursor + 8, cursor + block_size, 2):
                entry = struct.unpack_from("<H", data, entry_offset)[0]
                if entry >> 12 == IMAGE_REL_BASED_HIGHLOW:
                    relocation_rvas.append(page_rva + (entry & 0x0FFF))
            cursor += block_size

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
        # Minimal NB09 public records do not carry type sizes. symbol_names.csv uses
        # the next-public gap, so the reviewed enrollment is authoritative here.
        size = expectation["size"]
        row["retail_payload"] = read_pe_payload_evidence(
            retail_path, int(row["retail_rva"], 16), size, expectation["audit"])
        if row["candidate_count"] == 1:
            row["candidate_payload"] = read_pe_payload_evidence(
                candidate_path, int(row["candidate_rva"], 16), size, expectation["audit"])


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


def load_retail_data_symbols(path=None):
    path = Path(path or REPO / "build/gen/symbol_names.csv")
    symbols = []
    with path.open(newline="") as f:
        for row in csv.DictReader(f):
            provenance = row.get("provenance")
            if provenance is None:
                # Persistent worktrees can retain the pre-provenance generated schema.
                # In that schema only the _const unit is synthetic; all other data rows
                # originate in the retained public stream.
                provenance = ("pe-reloc-constant" if row["unit"] == "_const"
                              else "cv-public-data")
            if row["kind"] != "data" or provenance != "cv-public-data":
                continue
            symbols.append({
                "name": row["name"],
                "unit": row["unit"],
                "rva": int(row["rva"], 16),
                "size": int(row["size"], 16),
                "provenance": provenance,
            })
    symbols.sort(key=lambda row: (row["rva"], row["name"]))
    return symbols


def load_required_initialized_storage(path=None):
    path = Path(path or REQUIRED_INITIALIZED_STORAGE)
    if not path.exists():
        raise RuntimeError("required initialized-storage enrollment missing: %s" % path)
    with path.open(newline="") as f:
        rows = csv.DictReader(
            (line for line in f if not line.lstrip().startswith("#")), delimiter="\t")
        return [{
            "name": row["name"],
            "unit": row["unit"],
            "size": int(row["size"], 0),
            "retail_sha256": row["retail_sha256"],
            "highlow_count": int(row["highlow_count"], 0),
            "audit": row["audit"],
        } for row in rows]


def required_initialized_storage_diagnostics(public_symbols, required):
    by_name = {row["name"]: row for row in public_symbols["symbols"]}
    rows = []
    for expectation in required:
        symbol = by_name.get(expectation["name"])
        if symbol is None:
            status = "retail-symbol-missing"
        elif symbol["unit"] != expectation["unit"]:
            status = "owner-mismatch"
        elif symbol["candidate_count"] != 1:
            status = symbol["status"]
        elif not symbol["storage_class_matches"]:
            status = "storage-class-mismatch"
        elif (symbol["retail_payload"]["size"] != expectation["size"] or
              symbol["retail_payload"]["sha256"] != expectation["retail_sha256"] or
              symbol["retail_payload"]["highlow_base_relocation_count"] !=
              expectation["highlow_count"]):
            status = "reviewed-retail-evidence-mismatch"
        elif symbol.get("candidate_payload") is None:
            status = "candidate-payload-missing"
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
            "expected_size": expectation["size"],
            "expected_retail_sha256": expectation["retail_sha256"],
            "expected_highlow_count": expectation["highlow_count"],
            "retail_payload": symbol.get("retail_payload") if symbol is not None else None,
            "candidate_payload": symbol.get("candidate_payload") if symbol is not None else None,
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


def static_symbol_diagnostics(retail, candidate, map_path, retail_symbols=None):
    retail_symbols = (load_retail_data_symbols() if retail_symbols is None
                      else sorted(retail_symbols, key=lambda row: (row["rva"], row["name"])))
    contributions = parse_map_contributions(map_path)
    candidate_records = defaultdict(list)
    for record in parse_map_symbol_records(map_path):
        candidate_records[record["name"]].append(record)

    def normalized_storage(storage_class):
        if storage_class in {"data-loader-zero", "data-loader-zero-tail"}:
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
            "Shipping NB09 S_PUB32 symbols classified as data by retail RVA; synthetic PDB "
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


def static_storage_diagnostics(retail, candidate, map_path, retail_symbols=None):
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
            retail, candidate, map_path, retail_symbols=retail_symbols),
        "retail_zero_fill_note": (
            "Retail has no map. PE evidence proves only a loader-zero writable .data tail; "
            "it does not prove that the entire tail was an independent .bss/common contribution."),
    }


def write_order_response(path):
    """Write a relocatable LINK response file in authoritative NB09 object order."""
    path = Path(path).resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    order = load_retail_order()
    lines = []
    for row in order:
        relative = os.path.relpath(row["object"], path.parent).replace("/", "\\")
        lines.append('"%s"' % relative)
    path.write_text("\n".join(lines) + "\n", encoding="ascii")
    print("link order: %d NB09 contributions -> %s" % (len(order), path))
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

    order = load_retail_order()
    missing = [str(row["object"]) for row in order if not row["object"].exists()]
    if missing:
        raise RuntimeError("missing reconstruction objects:\n  " + "\n  ".join(missing))
    response_objects = read_order_response(order_response)
    expected_objects = [row["object"].resolve() for row in order]
    if response_objects != expected_objects:
        raise RuntimeError("link-order response does not match current NB09 contribution order")
    for stale in (output, map_path, missing_data_path):
        stale.unlink(missing_ok=True)
    original_libcmt = find_ci(toolchain / "lib", "LIBCMT.LIB")
    if original_libcmt is None:
        raise RuntimeError("LIBCMT.LIB is missing from the VC4.2 toolchain")
    derived_crt_dir = output.parent / "crt"
    derived_libcmt = derived_crt_dir / "LIBCMT.LIB"
    runtime_library = build_gfy_libcmt(
        original_libcmt, derived_libcmt,
        derived_crt_dir / "LIBCMT.gfy.json")
    library_path = ";".join(winepaths_w((derived_crt_dir, toolchain / "lib")))
    command = build_link_command(
        link_exe,
        winepath_w(map_path),
        winepath_w(output),
        winepaths_w(response_objects),
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
    runtime_literals = (audit_runtime_literal_storage(candidate, map_path)
                        if candidate else {
                            "required": 102, "verified": 0,
                            "failures": [{"reason": "candidate executable is missing"}],
                            "symbols": [],
                        })
    runtime_literals_ok = not runtime_literals["failures"]
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
            "retail_codeview_tool_provenance": read_nb09_tool_provenance(RETAIL_EXE),
            "version_matches_retail": bool(
                candidate and candidate["linker_version"] == retail["linker_version"]),
        },
        "order_source": "NB09 sstModule executable contribution order",
        "link_flags": list(RETAIL_LINK_FLAGS),
        "forced_vendor_imports": list(LINK300_FORCED_VENDOR_IMPORTS),
        "link_input_order": {
            "system_libraries_before_vendor": list(SYSTEM_LIBS_BEFORE_VENDOR),
            "vendor_import_libraries": [str(path) for path in imports_libraries],
            "system_libraries_after_vendor": list(SYSTEM_LIBS_AFTER_VENDOR),
            "objects": [str(path) for path in response_objects],
            "resource": str(resource_path),
        },
        "library_search": {"mechanism": "LIB environment", "path": library_path},
        "runtime_library": runtime_library,
        "runtime_literal_storage": runtime_literals,
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
        required = load_required_initialized_storage(required_initialized_path)
        add_payload_evidence(
            report["static_storage"]["public_symbols"], RETAIL_EXE, output, required)
        required_diagnostics = required_initialized_storage_diagnostics(
            report["static_storage"]["public_symbols"], required)
        report["static_storage"]["required_initialized"] = required_diagnostics
        required_storage_ok = not required_diagnostics["violations"]
        if not required_storage_ok and report["status"] == "linked":
            report["status"] = "required-initialized-storage-mismatch"
    if not runtime_literals_ok and report["status"] == "linked":
        report["status"] = "runtime-literal-storage-mismatch"
    map_symbols = parse_map_symbols(map_path)
    for row in order:
        anchor = next(((rva, symbol, map_symbols[symbol][0])
                       for rva, symbol in row["function_anchors"] if symbol in map_symbols), None)
        expected_rva, anchor_symbol, actual_va = (anchor if anchor else
                                                   (row["first_function_rva"],
                                                    row["first_function_symbol"], None))
        candidate_text = read_coff_section(row["object"], ".text")
        report["units"].append({
            "unit": row["unit"],
            "rva_anchor": anchor_symbol,
            "retail_rva": "0x%x" % expected_rva,
            "contribution_rva": "0x%x" % row["contribution_rva"],
            "contribution_size": "0x%x" % row["contribution_size"],
            "candidate_text_raw_size": "0x%x" % candidate_text["raw_size"],
            "candidate_text_alignment": "0x%x" % candidate_text["alignment"],
            "candidate_text_aligned_size": "0x%x" % candidate_text["aligned_size"],
            "candidate_text_aligned_size_delta": (
                candidate_text["aligned_size"] - row["contribution_size"]),
            "candidate_rva": "0x%x" % (actual_va - IMAGE_BASE) if actual_va is not None else None,
            "delta": actual_va - IMAGE_BASE - expected_rva if actual_va is not None else None,
        })
    report_path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    if report["static_storage"]:
        write_missing_data_report(missing_data_path, report["static_storage"]["public_symbols"])

    if run.stdout.strip():
        print(run.stdout.rstrip())
    if candidate:
        missing_anchors = sum(1 for unit in report["units"] if unit["delta"] is None)
        displaced = sum(1 for unit in report["units"]
                        if unit["delta"] is not None and unit["delta"] != 0)
        print("link audit: %d NB09-ordered units; %d RVA anchors displaced, %d unavailable" %
              (len(order), displaced, missing_anchors))
        first_size_mismatch = next(
            (unit for unit in report["units"]
             if unit["candidate_text_aligned_size_delta"] != 0), None)
        if first_size_mismatch:
            print("link audit: first .text contribution size mismatch %s: %s aligned vs %s "
                  "retail (%+d bytes)" %
                  (first_size_mismatch["unit"],
                   first_size_mismatch["candidate_text_aligned_size"],
                   first_size_mismatch["contribution_size"],
                   first_size_mismatch["candidate_text_aligned_size_delta"]))
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
        print("link audit: VC4.2 /Gf runtime literals %d/%d in initialized .data" %
              (runtime_literals["verified"], runtime_literals["required"]))
        for failure in runtime_literals["failures"][:3]:
            print("link audit: VC4.2 /Gf runtime literal FAIL %s: %s" %
                  (failure.get("name", "<link>"), failure["reason"]))
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
            required_storage_ok and runtime_literals_ok else
            (run.returncode or 1))


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", default=str(REPO / "build/link/HEROES2W.EXE"))
    parser.add_argument("--order", default=str(REPO / "build/link/objects.rsp"))
    parser.add_argument("--imports", action="append")
    parser.add_argument("--resource", default=str(REPO / "build/link/HEROES2W.res"))
    parser.add_argument("--linker", help="alternate LINK.EXE for an isolated A/B link")
    parser.add_argument(
        "--required-initialized", default=str(REQUIRED_INITIALIZED_STORAGE),
        help="checked TSV enrollment of reviewed globals that must retain retail storage class")
    parser.add_argument("--write-order", metavar="PATH")
    args = parser.parse_args(argv)
    try:
        if args.write_order:
            return write_order_response(args.write_order)
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

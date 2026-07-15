#!/usr/bin/env python3
"""Recover structured Win32 resources from the retail PE into a standard .res file.

The extractor walks the PE resource directory and emits one Win32 .res record per
leaf. It deliberately does not copy the raw .rsrc section: LINK rebuilds the directory,
RVA-bearing data entries, alignment, and section contribution from structured records.
"""
import argparse
import json
import struct
import sys
from collections import Counter
from pathlib import Path

PE32_MAGIC = 0x10B
RESOURCE_DIRECTORY_INDEX = 2
COFF_SECTION_HEADER_SIZE = 40
RESOURCE_DIRECTORY_HEADER_SIZE = 16
RESOURCE_DIRECTORY_ENTRY_SIZE = 8
RESOURCE_DATA_ENTRY_SIZE = 16
RESOURCE_NAME_IS_STRING = 0x80000000
RESOURCE_ENTRY_IS_DIRECTORY = 0x80000000
RESOURCE_OFFSET_MASK = 0x7FFFFFFF
RESOURCE_ORDINAL_MARKER = 0xFFFF
RESOURCE_MEMORY_FLAGS = 0x1030


def align(value, alignment=4):
    return (value + alignment - 1) & ~(alignment - 1)


def read_pe_resources(path):
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
    directory_offset = optional + 96 + RESOURCE_DIRECTORY_INDEX * 8
    resource_rva, resource_size = struct.unpack_from("<II", data, directory_offset)
    if not resource_rva or not resource_size:
        raise ValueError("PE has no resource directory: %s" % path)

    sections = []
    section_offset = optional + optional_size
    for index in range(section_count):
        offset = section_offset + index * COFF_SECTION_HEADER_SIZE
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from("<IIII", data, offset + 8)
        sections.append((rva, max(virtual_size, raw_size), raw_offset, raw_size))

    def rva_offset(rva, size=1):
        for section_rva, extent, raw_offset, raw_size in sections:
            relative = rva - section_rva
            if 0 <= relative and relative + size <= min(extent, raw_size):
                return raw_offset + relative
        raise ValueError("resource RVA 0x%x size 0x%x is outside raw PE sections" % (rva, size))

    resource_base = rva_offset(resource_rva)

    def resource_offset(relative, size):
        if relative < 0 or relative + size > resource_size:
            raise ValueError("resource-relative range 0x%x..0x%x exceeds directory size 0x%x" %
                             (relative, relative + size, resource_size))
        return resource_base + relative

    def decode_name(value):
        if not value & RESOURCE_NAME_IS_STRING:
            return value & 0xFFFF
        relative = value & RESOURCE_OFFSET_MASK
        offset = resource_offset(relative, 2)
        length = struct.unpack_from("<H", data, offset)[0]
        raw = data[offset + 2:offset + 2 + length * 2]
        if len(raw) != length * 2:
            raise ValueError("truncated resource name at 0x%x" % relative)
        return raw.decode("utf-16le")

    resources = []
    visited = set()

    def walk(relative, path_parts):
        if relative in visited:
            raise ValueError("cyclic resource directory at 0x%x" % relative)
        visited.add(relative)
        offset = resource_offset(relative, RESOURCE_DIRECTORY_HEADER_SIZE)
        characteristics, timestamp, major, minor, named_count, id_count = struct.unpack_from(
            "<IIHHHH", data, offset)
        entry_count = named_count + id_count
        entries_offset = resource_offset(
            relative + RESOURCE_DIRECTORY_HEADER_SIZE,
            entry_count * RESOURCE_DIRECTORY_ENTRY_SIZE)
        for index in range(entry_count):
            name_value, child_value = struct.unpack_from(
                "<II", data, entries_offset + index * RESOURCE_DIRECTORY_ENTRY_SIZE)
            name = decode_name(name_value)
            child_relative = child_value & RESOURCE_OFFSET_MASK
            if child_value & RESOURCE_ENTRY_IS_DIRECTORY:
                walk(child_relative, path_parts + [name])
                continue
            if len(path_parts) != 2:
                raise ValueError("resource leaf does not have type/name/language hierarchy")
            leaf_offset = resource_offset(child_relative, RESOURCE_DATA_ENTRY_SIZE)
            data_rva, data_size, codepage, reserved = struct.unpack_from("<IIII", data, leaf_offset)
            payload_offset = rva_offset(data_rva, data_size)
            resources.append({
                "type": path_parts[0],
                "name": path_parts[1],
                "language": name,
                "data": data[payload_offset:payload_offset + data_size],
                "data_rva": data_rva,
                "codepage": codepage,
                "reserved": reserved,
                "directory_characteristics": characteristics,
                "directory_timestamp": timestamp,
                "directory_version": [major, minor],
            })
        visited.remove(relative)

    walk(0, [])
    resources.sort(key=lambda row: (row["data_rva"], str(row["type"]), str(row["name"])))
    return resources


def encode_identifier(value):
    if isinstance(value, int):
        if not 0 <= value <= 0xFFFF:
            raise ValueError("resource ordinal is outside WORD range: %d" % value)
        return struct.pack("<HH", RESOURCE_ORDINAL_MARKER, value)
    return value.encode("utf-16le") + b"\0\0"


def encode_res_record(resource):
    identifiers = encode_identifier(resource["type"]) + encode_identifier(resource["name"])
    identifiers += b"\0" * (align(len(identifiers)) - len(identifiers))
    header_tail = struct.pack(
        "<IHHII", 0, RESOURCE_MEMORY_FLAGS, resource["language"], 0, 0)
    header_size = 8 + len(identifiers) + len(header_tail)
    payload = resource["data"]
    record = struct.pack("<II", len(payload), header_size) + identifiers + header_tail + payload
    return record + b"\0" * (align(len(record)) - len(record))


def write_res(resources, path):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    null_resource = {"type": 0, "name": 0, "language": 0, "data": b""}
    data = encode_res_record(null_resource)
    data += b"".join(encode_res_record(resource) for resource in resources)
    path.write_bytes(data)


def resource_summary(resources):
    counts = Counter(str(resource["type"]) for resource in resources)
    return {
        "count": len(resources),
        "payload_bytes": sum(len(resource["data"]) for resource in resources),
        "types": dict(sorted(counts.items())),
        "resources": [{
            "type": resource["type"],
            "name": resource["name"],
            "language": resource["language"],
            "size": len(resource["data"]),
            "retail_data_rva": "0x%x" % resource["data_rva"],
        } for resource in resources],
    }


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--report")
    args = parser.parse_args(argv)
    try:
        resources = read_pe_resources(args.exe)
        write_res(resources, args.out)
        summary = resource_summary(resources)
        if args.report:
            report_path = Path(args.report)
            report_path.parent.mkdir(parents=True, exist_ok=True)
            report_path.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
        print("resources: %d leaves, %d payload bytes -> %s" %
              (summary["count"], summary["payload_bytes"], args.out))
        return 0
    except (OSError, ValueError, struct.error) as exc:
        print("[extract_resources] ERROR: %s" % exc, file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())

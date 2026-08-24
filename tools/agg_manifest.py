#!/usr/bin/env python3
"""Create or compare hash-only manifests for Heroes II AGG archives.

The tool never extracts entry payloads. Its JSON output contains archive and
entry metadata (name, ID, size and SHA-256), which is enough to determine the
minimal external locale overlay without adding original game assets to source
control.
"""

import argparse
import hashlib
import json
import pathlib
import struct


ENTRY = struct.Struct("<III")
NAME_BYTES = 15
CHUNK_BYTES = 1024 * 1024


class AggError(ValueError):
    pass


def hash_stream(stream, size=None):
    digest = hashlib.sha256()
    remaining = size
    while remaining is None or remaining > 0:
        amount = CHUNK_BYTES if remaining is None else min(CHUNK_BYTES, remaining)
        block = stream.read(amount)
        if not block:
            break
        digest.update(block)
        if remaining is not None:
            remaining -= len(block)
    if remaining not in (None, 0):
        raise AggError("truncated entry payload")
    return digest.hexdigest()


def parse(path):
    path = pathlib.Path(path)
    archive_size = path.stat().st_size
    if archive_size < 2:
        raise AggError(f"{path}: file is too small")

    with path.open("rb") as stream:
        count_data = stream.read(2)
        count = struct.unpack("<H", count_data)[0]
        directory_end = 2 + count * ENTRY.size
        names_offset = archive_size - count * NAME_BYTES
        if count == 0 or directory_end > names_offset:
            raise AggError(f"{path}: invalid entry count {count}")

        directory = []
        for index in range(count):
            data = stream.read(ENTRY.size)
            if len(data) != ENTRY.size:
                raise AggError(f"{path}: truncated directory")
            identifier, offset, size = ENTRY.unpack(data)
            if offset < directory_end or offset + size > names_offset:
                raise AggError(
                    f"{path}: entry {index} range {offset}+{size} is outside payload area"
                )
            directory.append((identifier, offset, size))

        stream.seek(names_offset)
        names = []
        for index in range(count):
            raw = stream.read(NAME_BYTES)
            if len(raw) != NAME_BYTES:
                raise AggError(f"{path}: truncated name table")
            name = raw.split(b"\0", 1)[0].decode("ascii", errors="strict")
            if not name:
                raise AggError(f"{path}: entry {index} has an empty name")
            names.append(name)

        entries = []
        seen_ids = set()
        for index, ((identifier, offset, size), name) in enumerate(zip(directory, names)):
            if identifier in seen_ids:
                raise AggError(f"{path}: duplicate entry ID 0x{identifier:08x}")
            seen_ids.add(identifier)
            stream.seek(offset)
            entries.append({
                "name": name,
                "id": f"0x{identifier:08x}",
                "size": size,
                "sha256": hash_stream(stream, size),
            })

        stream.seek(0)
        archive_sha256 = hash_stream(stream)

    return {
        "format": "homm2-agg-manifest-v1",
        "archive": path.name,
        "archive_size": archive_size,
        "archive_sha256": archive_sha256,
        "entry_count": count,
        "entries": entries,
    }


def compare(primary, localized):
    primary_by_id = {entry["id"]: entry for entry in primary["entries"]}
    localized_by_id = {entry["id"]: entry for entry in localized["entries"]}
    changes = []
    for identifier in sorted(primary_by_id.keys() | localized_by_id.keys()):
        base = primary_by_id.get(identifier)
        locale = localized_by_id.get(identifier)
        if base is None:
            status = "added"
        elif locale is None:
            status = "missing"
        elif base["sha256"] == locale["sha256"] and base["size"] == locale["size"]:
            continue
        else:
            status = "changed"
        changes.append({
            "id": identifier,
            "status": status,
            "primary": base,
            "localized": locale,
        })
    return {
        "format": "homm2-agg-comparison-v1",
        "primary": {
            key: primary[key]
            for key in ("archive", "archive_size", "archive_sha256", "entry_count")
        },
        "localized": {
            key: localized[key]
            for key in ("archive", "archive_size", "archive_sha256", "entry_count")
        },
        "changed_entry_count": len(changes),
        "changes": changes,
    }


def write_json(path, value):
    text = json.dumps(value, ensure_ascii=False, indent=2, sort_keys=True) + "\n"
    if path == "-":
        print(text, end="")
    else:
        pathlib.Path(path).write_text(text, encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest="command", required=True)
    manifest_parser = subparsers.add_parser("manifest")
    manifest_parser.add_argument("archive")
    manifest_parser.add_argument("output")
    compare_parser = subparsers.add_parser("compare")
    compare_parser.add_argument("primary")
    compare_parser.add_argument("localized")
    compare_parser.add_argument("output")
    args = parser.parse_args()

    if args.command == "manifest":
        write_json(args.output, parse(args.archive))
    else:
        write_json(args.output, compare(parse(args.primary), parse(args.localized)))


if __name__ == "__main__":
    main()

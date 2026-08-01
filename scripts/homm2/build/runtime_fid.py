"""Identify reviewed retail CRT procedures against the pinned LIBCMT archive.

The matcher is intentionally narrow and fail-closed. It takes reviewed runtime
seeds from ``config/library_labels.csv``, restricts candidate signatures to the
LIBCMT member with the same compiland name, and requires one relocation-masked
COFF identity at the retail RVA. Reviewed code-only spans may be an exact prefix
of a library symbol extent that also owns embedded tables or alternate entries.
The tracked output is ``config/library_labels.csv``.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import io
import struct
from dataclasses import dataclass
from pathlib import Path

from homm2.build.build_libcmt_gfy import archive_entries
from homm2.build.link_exe import PINNED_VC40_LIBCMT_SHA256


REPO = Path(__file__).resolve().parents[3]
DEFAULT_ARCHIVE = REPO / "build/toolchain/link300/lib/LIBCMT.LIB"
DEFAULT_EXE = REPO / "build/orig/HMM2PL.exe"
DEFAULT_LABELS = REPO / "config/library_labels.csv"

IMAGE_SCN_CNT_CODE = 0x00000020
IMAGE_SCN_MEM_EXECUTE = 0x20000000
FUNCTION_TYPE = 0x0020
EXTERNAL_STORAGE = 2
STATIC_STORAGE = 3
RELOCATION_WIDTH = {
    0x0006: 4,  # IMAGE_REL_I386_DIR32
    0x0007: 4,  # IMAGE_REL_I386_DIR32NB
    0x000A: 2,  # IMAGE_REL_I386_SECTION
    0x000B: 4,  # IMAGE_REL_I386_SECREL
    0x0014: 4,  # IMAGE_REL_I386_REL32
}


@dataclass(frozen=True)
class Signature:
    member: str
    name: str
    section: int
    start: int
    size: int
    payload: bytes
    mask: bytes


def _coff_name(raw: bytes, strings: bytes) -> str:
    if raw[:4] == bytes(4):
        offset = struct.unpack_from("<I", raw, 4)[0]
        if not 4 <= offset < len(strings):
            raise ValueError("invalid COFF string offset")
        end = strings.find(b"\0", offset)
        if end < 0:
            raise ValueError("unterminated COFF string")
        return strings[offset:end].decode("latin-1")
    return raw.rstrip(b"\0").decode("latin-1")


def signatures(payload: bytes, member: str) -> list[Signature]:
    if len(payload) < 20:
        return []
    machine, section_count, _, symbol_offset, symbol_count, optional_size, _ = (
        struct.unpack_from("<HHIIIHH", payload, 0))
    if machine != 0x014C or optional_size:
        return []
    section_table = 20
    string_offset = symbol_offset + symbol_count * 18
    if string_offset + 4 > len(payload):
        raise ValueError("truncated COFF symbol table: %s" % member)
    string_size = struct.unpack_from("<I", payload, string_offset)[0]
    strings = payload[string_offset:string_offset + string_size]
    if len(strings) != string_size:
        raise ValueError("truncated COFF string table: %s" % member)

    sections = {}
    for index in range(section_count):
        offset = section_table + index * 40
        raw_name = payload[offset:offset + 8]
        name = _coff_name(raw_name, strings)
        raw_size, raw_offset, reloc_offset = struct.unpack_from(
            "<III", payload, offset + 16)
        reloc_count = struct.unpack_from("<H", payload, offset + 32)[0]
        characteristics = struct.unpack_from("<I", payload, offset + 36)[0]
        sections[index + 1] = {
            "name": name,
            "raw_size": raw_size,
            "raw_offset": raw_offset,
            "reloc_offset": reloc_offset,
            "reloc_count": reloc_count,
            "characteristics": characteristics,
        }

    symbols = []
    index = 0
    while index < symbol_count:
        offset = symbol_offset + index * 18
        raw = payload[offset:offset + 18]
        name = _coff_name(raw[:8], strings)
        value, section, typ, storage, auxiliary = struct.unpack_from(
            "<IhHBB", raw, 8)
        symbols.append((name, value, section, typ, storage))
        index += 1 + auxiliary

    rows = []
    for name, start, section_index, typ, storage in symbols:
        section = sections.get(section_index)
        if (section is None or typ not in (0, FUNCTION_TYPE) or not name or
                name.startswith(".") or
                storage not in (EXTERNAL_STORAGE, STATIC_STORAGE) or
                not section["characteristics"] &
                    (IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE)):
            continue
        later = sorted(
            value for other_name, value, other_section, other_type, other_storage in symbols
            if (other_section == section_index and other_type in (0, FUNCTION_TYPE) and
                other_storage in (EXTERNAL_STORAGE, STATIC_STORAGE) and value > start and
                other_name and not other_name.startswith("."))
        )
        end = later[0] if later else section["raw_size"]
        if not 0 <= start < end <= section["raw_size"]:
            raise ValueError("invalid function extent for %s in %s" % (name, member))
        base = section["raw_offset"] + start
        body = payload[base:base + end - start]
        mask = bytearray(b"\xff" * len(body))
        for reloc_index in range(section["reloc_count"]):
            reloc = section["reloc_offset"] + reloc_index * 10
            site, _, typ = struct.unpack_from("<IIH", payload, reloc)
            relative = site - start
            width = RELOCATION_WIDTH.get(typ)
            if width is None:
                raise ValueError(
                    "unsupported relocation 0x%x in %s:%s" % (typ, member, name))
            if 0 <= relative < len(mask):
                if relative + width > len(mask):
                    raise ValueError("relocation crosses %s in %s" % (name, member))
                mask[relative:relative + width] = bytes(width)
        rows.append(Signature(
            member, name, section_index, start, len(body), body, bytes(mask)))
    return rows


def _member_basename(name: str) -> str:
    return name.replace("\\", "/").rsplit("/", 1)[-1].lower()


def library_signatures(path: Path) -> dict[str, list[Signature]]:
    payload = path.read_bytes()
    if hashlib.sha256(payload).hexdigest() != PINNED_VC40_LIBCMT_SHA256:
        raise ValueError("LIBCMT archive does not match the pinned VC4.0 hash")
    found = {}
    for entry in archive_entries(payload):
        member = _member_basename(entry.name)
        if not member.endswith(".obj"):
            continue
        found.setdefault(member, []).extend(
            signatures(payload[entry.data_offset:entry.data_end], member))
    return found


def _pe_image(path: Path) -> tuple[int, list[tuple[int, int, int, int]], bytes]:
    payload = path.read_bytes()
    pe = struct.unpack_from("<I", payload, 0x3C)[0]
    if payload[:2] != b"MZ" or payload[pe:pe + 4] != b"PE\0\0":
        raise ValueError("not a PE image: %s" % path)
    coff = pe + 4
    count = struct.unpack_from("<H", payload, coff + 2)[0]
    optional_size = struct.unpack_from("<H", payload, coff + 16)[0]
    optional = coff + 20
    image_base = struct.unpack_from("<I", payload, optional + 28)[0]
    section_table = optional + optional_size
    sections = []
    for index in range(count):
        offset = section_table + index * 40
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
            "<IIII", payload, offset + 8)
        sections.append((rva, max(virtual_size, raw_size), raw_offset, raw_size))
    return image_base, sections, payload


def _pe_bytes(image, rva: int, size: int) -> bytes:
    _, sections, payload = image
    for start, virtual_size, raw_offset, raw_size in sections:
        if start <= rva and rva + size <= start + min(virtual_size, raw_size):
            offset = raw_offset + rva - start
            return payload[offset:offset + size]
    raise ValueError("RVA 0x%x size 0x%x is not raw-backed" % (rva, size))


def _matches(retail: bytes, signature: Signature) -> bool:
    return len(retail) == signature.size and all(
        not mask or left == right
        for left, right, mask in zip(retail, signature.payload, signature.mask))


def _prefix_matches(retail: bytes, signature: Signature, size: int) -> bool:
    return size <= signature.size and _matches(
        retail[:size], Signature(
            signature.member, signature.name, signature.section, signature.start,
            size, signature.payload[:size], signature.mask[:size]))


def _interior_match(row, rows, member_signatures, image):
    """Return a proved owner-relative assembly entry, if one exists."""
    rva = int(row["rva"], 16)
    size = int(row["size"], 16)
    anchors = []
    for anchor in rows:
        if anchor["unit"] != row["unit"] or not anchor["name"].strip():
            continue
        anchor_rva = int(anchor["rva"], 16)
        anchor_size = int(anchor["size"], 16)
        retail = _pe_bytes(image, anchor_rva, anchor_size)
        for signature in member_signatures:
            if signature.name == anchor["name"] and _prefix_matches(
                    retail, signature, anchor_size):
                anchors.append((
                    signature.section, anchor_rva - signature.start))
    candidates = []
    for section, retail_base in sorted(set(anchors)):
        offset = rva - retail_base
        same_section = [signature for signature in member_signatures
                        if signature.section == section and signature.start <= offset]
        if not same_section:
            continue
        owners = [signature for signature in same_section
                  if offset + size <= signature.start + signature.size]
        if not owners:
            continue
        owner = max(owners, key=lambda signature: signature.start)
        relative = offset - owner.start
        retail = _pe_bytes(image, rva, size)
        candidate = Signature(
            owner.member, owner.name, owner.section, offset, size,
            owner.payload[relative:relative + size],
            owner.mask[relative:relative + size])
        if _matches(retail, candidate):
            name = (owner.name if relative == 0 else
                    "%s$entry_0x%x" % (owner.name, relative))
            candidates.append((name, owner.member))
    candidates = sorted(set(candidates))
    return candidates[0] if len(candidates) == 1 else None


def identify(archive: Path, exe: Path,
             seeds: list[dict[str, str]] | None = None) -> list[dict[str, str]]:
    candidates = library_signatures(archive)
    image = _pe_image(exe)
    seed_rows = list(seeds or ())
    output = []
    failures = []
    seed_starts = {(int(seed["rva"], 16), seed["unit"])
                   for seed in seed_rows}
    for row in seed_rows:
        rva = int(row["rva"], 16)
        size = int(row["size"], 16)
        member = row["unit"].lower() + ".obj"
        retail = _pe_bytes(image, rva, size)
        matched = []
        for signature in candidates.get(member, ()):
            candidate_retail = _pe_bytes(image, rva, signature.size)
            if _matches(candidate_retail, signature):
                matched.append(signature)
        named_matches = [signature for signature in matched
                         if signature.name == row.get("name", "").strip()]
        if named_matches:
            matched = named_matches
        if not matched:
            interior = _interior_match(
                row, seed_rows, candidates.get(member, ()), image)
            if interior is not None:
                name, matched_member = interior
                output.append({
                    "rva": "0x%x" % rva,
                    "size": "0x%x" % size,
                    "name": name,
                    "unit": row["unit"],
                    "library": "LIBCMT",
                    "member": matched_member,
                    "confidence": "HIGH",
                    "source": "exact-owner-relative-entry",
                })
                continue
        if len(matched) != 1:
            same_size = [signature for signature in candidates.get(member, ())
                         if signature.size == size]
            descriptions = []
            for signature in same_size:
                compared = sum(signature.mask)
                differing = sum(
                    1 for left, right, mask in
                    zip(retail, signature.payload, signature.mask)
                    if mask and left != right)
                descriptions.append("%s:%d/%d" % (
                    signature.name, differing, compared // 0xFF))
            exact_any_size = ",".join("%s:0x%x" % (signature.name, signature.size)
                                      for signature in matched)
            failures.append("0x%x %s: %d exact %s matches; exact=%s candidates=%s" % (
                rva, row["unit"], len(matched), member,
                exact_any_size or "none",
                ",".join(descriptions) or "none"))
            continue
        signature = matched[0]
        split_entry = any(
            unit == row["unit"] and rva < start < rva + signature.size
            for start, unit in seed_starts)
        reviewed_prefix = bool(row.get("name", "").strip()) and size < signature.size
        if signature.size != size and not reviewed_prefix:
            size = signature.size
        output.append({
            "rva": "0x%x" % rva,
            "size": "0x%x" % size,
            "name": signature.name,
            "unit": row["unit"],
            "library": "LIBCMT",
            "member": signature.member,
            "confidence": "HIGH",
            "source": ("exact-member-prefix-before-entry" if split_entry else
                       "exact-member-prefix-reviewed-span" if reviewed_prefix else
                       "exact-member-signature"),
        })
    if failures:
        raise ValueError("runtime FID failed:\n" + "\n".join(failures))
    return output


FIELDS = ("rva", "size", "name", "unit", "library", "member", "confidence", "source")


def csv_bytes(rows: list[dict[str, str]]) -> bytes:
    stream = io.StringIO(newline="")
    stream.write("# Exact runtime-library identities recovered from the pinned VC 4.0 LIBCMT member\n")
    stream.write("# objects. Regenerate and verify with `python3 -m homm2.build.runtime_fid`.\n")
    writer = csv.DictWriter(stream, fieldnames=FIELDS, lineterminator="\n")
    writer.writeheader()
    writer.writerows(rows)
    return stream.getvalue().encode("utf-8")


def label_seeds(path: Path) -> list[dict[str, str]]:
    if not path.exists():
        return []
    with path.open(newline="") as stream:
        rows = csv.DictReader(
            line for line in stream if not line.lstrip().startswith("#"))
        return [{
            "rva": row["rva"], "size": row["size"], "name": row["name"],
            "unit": row["unit"], "provenance": "fid-rebuild",
        } for row in rows]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path, default=DEFAULT_ARCHIVE)
    parser.add_argument("--exe", type=Path, default=DEFAULT_EXE)
    parser.add_argument("--output", type=Path, default=DEFAULT_LABELS)
    parser.add_argument("--seeds", type=Path, default=DEFAULT_LABELS)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args(argv)
    seeds = label_seeds(args.seeds)
    if not seeds:
        raise SystemExit("runtime FID requires reviewed RVA/unit seeds: %s" % args.seeds)
    labels = identify(args.archive, args.exe, seeds)
    payload = csv_bytes(labels)
    if args.check:
        if not args.output.exists() or args.output.read_bytes() != payload:
            raise SystemExit("runtime FID labels are stale: %s" % args.output)
        print("runtime FID: %s is current" % args.output)
        return 0
    args.output.write_bytes(payload)
    print("runtime FID: wrote %d exact labels to %s" % (
        len(labels), args.output))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

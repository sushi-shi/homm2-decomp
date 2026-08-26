#!/usr/bin/env python3
"""Build Project Ironfist's source assets into a Heroes II AGG archive."""

from __future__ import annotations

import argparse
import configparser
import hashlib
import struct
import sys
from pathlib import Path


ICN_FRAME = struct.Struct("<hhHHBI")
SHADOW_COLORS = (4, 3, 2, 1, 8, 9, 246, 247, 248, 249, 250, 251, 252, 253)

ANIMATIONS = (
    "start-move",
    "sub-start-move",
    "main-move",
    "sub-end-move",
    "end-move",
    "full-move",
    "move",
    "standing",
    "fidget-1",
    "fidget-2",
    "fidget-3",
    "fidget-4",
    "fidget-5",
    "dying",
    "wince",
    "wince-return",
    "melee-attack-up",
    "melee-attack-up-return",
    "melee-attack-up-2hex",
    "melee-attack-up-2hex-return",
    "melee-attack-forward",
    "melee-attack-forward-return",
    "melee-attack-forward-2hex",
    "melee-attack-forward-2hex-return",
    "melee-attack-down",
    "melee-attack-down-return",
    "melee-attack-down-2hex",
    "melee-attack-down-2hex-return",
    "ranged-attack-up",
    "ranged-attack-up-return",
    "ranged-attack-forward",
    "ranged-attack-forward-return",
    "ranged-attack-down",
    "ranged-attack-down-return",
)

FRM_FIELDS = (
    ("first-byte", "b", 1),
    ("blind-offset", "h", 2),
    ("start-move-anim-info", "b", 16),
    ("sub-start-anim-info", "b", 16),
    ("move-anim-info", "b", 16),
    ("sub-end-move-anim-info", "b", 16),
    ("end-move-anim-info", "b", 16),
    ("full-move-anim-info", "b", 16),
    ("walk-anim-info", "b", 16),
    ("num-fidgets", "b", 1),
    ("fidget-probabilities", "f", 5),
    ("fidget-info", "i", 5),
    ("max-fidget-delay", "i", 1),
    ("step-time", "i", 1),
    ("shooting-time", "i", 1),
    ("flight-speed", "i", 1),
    ("shoot-up-projectile-offset", "h", 2),
    ("shoot-forward-projectile-offset", "h", 2),
    ("shoot-down-projectile-offset", "h", 2),
    ("num-missile-directions", "b", 1),
    ("projectile-dir-angle", "f", 12),
    ("stack-number-display-facing-right-xoff", "i", 1),
    ("stack-number-display-facing-left-xoff", "i", 1),
) + tuple((f"{name}-animation-length", "b", 1) for name in ANIMATIONS) + tuple(
    (f"{name}-animation", "b", 16) for name in ANIMATIONS
)


class ResourceError(Exception):
    pass


def resource_sort_key(path_or_name: Path | str) -> tuple[str, str]:
    name = path_or_name.name if isinstance(path_or_name, Path) else path_or_name
    # Wine's FindFirstFile collation places '_' after letters at a shared prefix.
    return name.casefold().replace("_", "{"), name


def read_ini(path: Path) -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None)
    try:
        with path.open("r", encoding="utf-8-sig") as stream:
            parser.read_file(stream)
    except (OSError, UnicodeError, configparser.Error) as error:
        raise ResourceError(f"cannot read {path}: {error}") from error
    return parser


def read_indexed_bmp(path: Path) -> list[bytes]:
    try:
        data = path.read_bytes()
    except OSError as error:
        raise ResourceError(f"cannot read {path}: {error}") from error

    if len(data) < 54 or data[:2] != b"BM":
        raise ResourceError(f"{path}: expected a Windows BMP")

    pixel_offset = struct.unpack_from("<I", data, 10)[0]
    dib_size = struct.unpack_from("<I", data, 14)[0]
    width, height = struct.unpack_from("<ii", data, 18)
    planes, bits_per_pixel = struct.unpack_from("<HH", data, 26)
    compression = struct.unpack_from("<I", data, 30)[0]

    if dib_size < 40 or width <= 0 or height == 0:
        raise ResourceError(f"{path}: unsupported BMP dimensions or header")
    if planes != 1 or bits_per_pixel != 8 or compression != 0:
        raise ResourceError(f"{path}: expected an uncompressed 8-bit indexed BMP")

    row_size = (width + 3) & ~3
    row_count = abs(height)
    pixel_end = pixel_offset + row_size * row_count
    if pixel_offset < 14 + dib_size or pixel_end > len(data):
        raise ResourceError(f"{path}: truncated BMP pixel data")

    physical_rows = [
        data[pixel_offset + row * row_size : pixel_offset + row * row_size + width]
        for row in range(row_count)
    ]
    return list(reversed(physical_rows)) if height > 0 else physical_rows


def frame_bounds(rows: list[bytes]) -> tuple[int, int, int, int]:
    left = len(rows[0])
    top = len(rows)
    right = -1
    bottom = -1

    for y, row in enumerate(rows):
        for x, pixel in enumerate(row):
            if pixel:
                left = min(left, x)
                top = min(top, y)
                right = max(right, x)
                bottom = max(bottom, y)

    if right < 0:
        return 0, 0, 0, 0
    return left, top, right + 1, bottom + 1


def pixel_kind(pixel: int, mask: bool) -> int:
    if pixel == 0:
        return 1
    if mask:
        return 0
    if pixel == 5:
        return 2
    if pixel == 6:
        return 3
    if pixel == 7:
        return 4
    try:
        return 5 + SHADOW_COLORS.index(pixel)
    except ValueError:
        return 0


def same_kind_run(row: bytes, x: int, limit: int, mask: bool) -> int:
    kind = pixel_kind(row[x], mask)
    length = 1
    while length < limit and x + length < len(row):
        if pixel_kind(row[x + length], mask) != kind:
            break
        length += 1
    return length


def encode_icn_pixels(rows: list[bytes], mask: bool) -> bytes:
    encoded = bytearray()

    for row in rows:
        x = 0
        while x < len(row):
            kind = pixel_kind(row[x], mask)
            if kind == 0:
                length = same_kind_run(row, x, 0x7F, mask)
                encoded.append(length)
                if not mask:
                    encoded.extend(row[x : x + length])
            elif kind == 1:
                length = same_kind_run(row, x, 0x3F, mask)
                if x + length < len(row):
                    encoded.append(0x80 + length)
            else:
                length = same_kind_run(row, x, 0xFF, mask)
                if kind == 2:
                    command = 0x80
                elif kind == 3:
                    command = 0xC0
                elif kind == 4:
                    command = 0xC4
                else:
                    command = 0x40 + (kind - 5) * 4
                encoded.append(0xC0)
                if length >= 4:
                    encoded.extend((command, length))
                else:
                    encoded.append(command + length)
            x += length
        encoded.append(0)

    encoded.append(0x80)
    return bytes(encoded)


def build_icn(directory: Path) -> bytes:
    setup_path = directory / "setup.ini"
    setup = read_ini(setup_path) if setup_path.exists() else configparser.ConfigParser(interpolation=None)
    try:
        x_origin = setup.getint("Position", "X", fallback=0)
        y_origin = setup.getint("Position", "Y", fallback=0)
    except ValueError as error:
        raise ResourceError(f"{setup_path}: invalid position: {error}") from error

    frames = sorted(
        (path for path in directory.iterdir() if path.is_file() and path.suffix.casefold() == ".bmp"),
        key=resource_sort_key,
    )
    if len(frames) > 0xFFFF:
        raise ResourceError(f"{directory}: too many ICN frames")

    frame_table = bytearray()
    payload = bytearray()
    first_data_offset = len(frames) * ICN_FRAME.size

    for path in frames:
        rows = read_indexed_bmp(path)
        left, top, right, bottom = frame_bounds(rows)
        bits = setup.getint("Bits", path.name, fallback=0)
        if not 0 <= bits <= 0xFF:
            raise ResourceError(f"{setup_path}: invalid Bits value for {path.name}")

        cropped = [row[left:right] for row in rows[top:bottom]]
        offset = first_data_offset + len(payload)
        try:
            frame_table.extend(
                ICN_FRAME.pack(
                    x_origin + left,
                    y_origin + top,
                    right - left,
                    bottom - top,
                    bits,
                    offset,
                )
            )
        except struct.error as error:
            raise ResourceError(f"{path}: ICN frame metadata is out of range") from error
        payload.extend(encode_icn_pixels(cropped, bool(bits & 0x20)))

    size = len(frame_table) + len(payload)
    return struct.pack("<Hi", len(frames), size) + frame_table + payload


def parse_values(config: configparser.ConfigParser, path: Path, name: str, kind: str, count: int) -> list[int | float]:
    text = config.get("FRM", name, fallback="0")
    raw_values = [part.strip() for part in text.split(",")]
    values: list[int | float] = [0] * count
    if len(raw_values) > count:
        raise ResourceError(f"{path}: too many values for {name}")
    try:
        for index, value in enumerate(raw_values):
            values[index] = float(value) if kind == "f" else int(value, 10)
    except ValueError as error:
        raise ResourceError(f"{path}: invalid value for {name}: {error}") from error
    return values


def build_frm(path: Path) -> bytes:
    config = read_ini(path)
    output = bytearray()

    for name, kind, count in FRM_FIELDS:
        values = parse_values(config, path, name, kind, count)
        try:
            if kind == "b":
                output.extend((int(value) & 0xFF for value in values))
            else:
                output.extend(struct.pack(f"<{count}{kind}", *values))
        except (OverflowError, struct.error) as error:
            raise ResourceError(f"{path}: value out of range for {name}") from error

    if len(output) != 821:
        raise AssertionError(f"internal error: FRM size is {len(output)}, expected 821")
    return bytes(output)


def build_agg_bitmap(path: Path) -> bytes:
    try:
        raw = path.read_bytes()
    except OSError as error:
        raise ResourceError(f"cannot read {path}: {error}") from error
    if len(raw) >= 6 and raw[0] == 33:
        return raw

    rows = read_indexed_bmp(path)
    width = len(rows[0])
    height = len(rows)
    if width > 0xFFFF or height > 0xFFFF:
        raise ResourceError(f"{path}: bitmap is too large for an AGG bitmap")
    return struct.pack("<HHH", 33, width, height) + b"".join(rows)


def agg_hash(name: bytes) -> int:
    result = 0
    cumulative = 0
    for byte in reversed(name):
        if 97 <= byte <= 122:
            byte &= 0xDF
        cumulative = (cumulative + byte) & 0xFFFFFFFF
        result = ((result >> 25) + (result << 5) + cumulative + byte) & 0xFFFFFFFF
    return result


def build_entries(source: Path) -> list[tuple[str, bytes]]:
    entries: dict[str, tuple[str, bytes]] = {}

    def add(name: str, data: bytes) -> None:
        key = name.casefold()
        if key in entries:
            raise ResourceError(f"duplicate output resource name: {name}")
        entries[key] = (name, data)

    for directory in sorted(source.iterdir(), key=resource_sort_key):
        if directory.is_dir() and directory.name.casefold().endswith(".cadres"):
            add(directory.name[: -len(".cadres")], build_icn(directory))

    generated_frm: set[str] = set()
    for path in sorted(source.iterdir(), key=resource_sort_key):
        if path.is_file() and path.name.casefold().endswith(".bin.ini"):
            name = path.name[: -len(".ini")]
            add(name, build_frm(path))
            generated_frm.add(name.casefold())

    for path in sorted(source.iterdir(), key=resource_sort_key):
        if not path.is_file() or path.name.casefold() in generated_frm:
            continue
        if path.suffix.casefold() in (".bin", ".bmp", ".82m"):
            try:
                data = build_agg_bitmap(path) if path.suffix.casefold() == ".bmp" else path.read_bytes()
                add(path.name, data)
            except OSError as error:
                raise ResourceError(f"cannot read {path}: {error}") from error

    return sorted(entries.values(), key=lambda item: resource_sort_key(item[0]))


def build_agg(entries: list[tuple[str, bytes]]) -> bytes:
    if len(entries) > 0xFFFF:
        raise ResourceError("too many AGG entries")

    names = bytearray()
    records = bytearray()
    payload = bytearray()
    hashes: dict[int, str] = {}
    offset = 2 + 12 * len(entries)

    for name, data in entries:
        try:
            encoded_name = name.encode("ascii")
        except UnicodeEncodeError as error:
            raise ResourceError(f"AGG resource name is not ASCII: {name}") from error
        if len(encoded_name) > 14:
            raise ResourceError(f"AGG resource name is longer than 14 bytes: {name}")

        name_hash = agg_hash(encoded_name)
        if name_hash in hashes:
            raise ResourceError(f"AGG hash collision: {hashes[name_hash]} and {name}")
        hashes[name_hash] = name

        records.extend(struct.pack("<III", name_hash, offset, len(data)))
        names.extend(encoded_name.ljust(15, b"\0"))
        payload.extend(data)
        offset += len(data)

    return struct.pack("<H", len(entries)) + records + payload + names


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path, help="upstream assets/agg directory")
    parser.add_argument("output", type=Path, help="output ironfist.agg path")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if not args.source.is_dir():
        print(f"ironfist-pack: source directory does not exist: {args.source}", file=sys.stderr)
        return 1

    try:
        entries = build_entries(args.source)
        aggregate = build_agg(entries)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(aggregate)
    except (OSError, ResourceError) as error:
        print(f"ironfist-pack: {error}", file=sys.stderr)
        return 1

    digest = hashlib.sha256(aggregate).hexdigest()
    print(f"Built {args.output} ({len(entries)} entries, sha256 {digest})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

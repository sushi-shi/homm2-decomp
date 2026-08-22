#!/usr/bin/env python3
"""Recompose the reviewed retail CRT/import/funclet text tail from candidate bytes."""

import argparse
import re
import struct
import subprocess
from pathlib import Path

from homm2.build.assert_relocs import _load_pe_image, _pe_import_iat_identities


ROOT = next(path for path in Path(__file__).resolve().parents
            if (path / "flake.nix").exists())
RETAIL = ROOT / "build/orig/HMM2PL.exe"
OBJECT_ROOT = ROOT / "build/link/split-crt-archives/obj"
RETAIL_INITCOLL_SLOT = 0xE08DA


def section_header(payload, wanted):
    pe = struct.unpack_from("<I", payload, 0x3C)[0]
    count = struct.unpack_from("<H", payload, pe + 6)[0]
    optional_size = struct.unpack_from("<H", payload, pe + 20)[0]
    table = pe + 24 + optional_size
    for index in range(count):
        header = table + index * 40
        name = payload[header:header + 8].rstrip(b"\0").decode("ascii")
        if name == wanted:
            virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
                "<IIII", payload, header + 8
            )
            return header, virtual_size, rva, raw_size, raw_offset
    raise ValueError(f"missing PE section {wanted}")


def map_symbols(path):
    result = {}
    pattern = re.compile(
        r"^ 0001:([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\s+f\s+",
        re.IGNORECASE,
    )
    for line in path.read_text(errors="replace").splitlines():
        match = pattern.match(line)
        if match:
            result.setdefault(match.group(2), int(match.group(1), 16))
    return result


def map_text_x(path):
    match = re.search(
        r"^ 0001:([0-9a-f]{8}) ([0-9a-f]{8})H \.text\$x\s+CODE$",
        path.read_text(errors="replace"),
        re.MULTILINE | re.IGNORECASE,
    )
    if not match:
        raise ValueError("MAP has no .text$x contribution")
    return int(match.group(1), 16), int(match.group(2), 16)


def coff_code(path):
    data = path.read_bytes()
    count = struct.unpack_from("<H", data, 2)[0]
    table = 20 + struct.unpack_from("<H", data, 16)[0]
    for index in range(count):
        header = table + index * 40
        name = data[header:header + 8].rstrip(b"\0")
        characteristics = struct.unpack_from("<I", data, header + 36)[0]
        if not name.startswith(b".text") or not characteristics & 0x20:
            continue
        size, raw, reloc, reloc_count = struct.unpack_from(
            "<III4xH", data, header + 16
        )
        relocations = []
        for reloc_index in range(reloc_count):
            site, _symbol, kind = struct.unpack_from(
                "<IIH", data, reloc + reloc_index * 10
            )
            relocations.append((site, kind))
        return data[raw:raw + size], relocations
    raise ValueError(f"{path}: no code section")


def import_thunk_runs(path):
    image = _load_pe_image(path)
    identities = _pe_import_iat_identities(path)
    payload = image[0]
    _header, _virtual, rva, raw_size, raw = section_header(payload, ".text")
    data = payload[raw:raw + raw_size]
    rows = []
    for offset in range(len(data) - 5):
        if data[offset:offset + 2] == b"\xff\x25":
            target_va = struct.unpack_from("<I", data, offset + 2)[0]
            identity = identities.get(target_va - image[1])
            if identity is not None:
                rows.append((offset, target_va, identity))
    runs = []
    for row in rows:
        if not runs or row[0] != runs[-1][-1][0] + 6:
            runs.append([])
        runs[-1].append(row)
    return runs


def text_instructions(path, text_rva):
    output = subprocess.check_output(
        ["llvm-objdump", "-d", str(path)], text=True
    )
    pattern = re.compile(
        r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*([a-z][a-z0-9.]*)",
        re.IGNORECASE,
    )
    rows = []
    for line in output.splitlines():
        match = pattern.match(line)
        if not match:
            continue
        address = int(match.group(1), 16)
        raw = bytes.fromhex(match.group(2))
        rows.append((address - (0x400000 + text_rva), raw, match.group(3).lower()))
    return rows


def relocate_piece(piece, relocations, old_rva, new_rva, exact_moves):
    result = bytearray(piece)
    for site, kind in relocations:
        if site + 4 > len(result):
            raise ValueError("COFF relocation lies outside code contribution")
        value = struct.unpack_from("<I", result, site)[0]
        if kind == 0x14:  # IMAGE_REL_I386_REL32
            old_target = old_rva + site + 4 + struct.unpack("<i", result[site:site + 4])[0]
            new_target = exact_moves.get(old_target, old_target)
            struct.pack_into("<i", result, site, new_target - (new_rva + site + 4))
        elif kind == 0x6:  # IMAGE_REL_I386_DIR32
            struct.pack_into("<I", result, site, exact_moves.get(value, value))
    return bytes(result)


def relocate_funclets(piece, old_rva, new_rva, exact_moves, instructions):
    result = bytearray(piece)
    delta = new_rva - old_rva
    for instruction_rva, raw, mnemonic in instructions:
        if not old_rva <= instruction_rva < old_rva + len(result):
            continue
        if not (mnemonic.startswith("call") or mnemonic.startswith("j")) or len(raw) < 5:
            continue
        offset = instruction_rva - old_rva
        operand = offset + len(raw) - 4
        displacement = struct.unpack_from("<i", result, operand)[0]
        old_target = instruction_rva + len(raw) + displacement
        if old_rva <= old_target < old_rva + len(result):
            new_target = old_target + delta
        else:
            new_target = exact_moves.get(old_target, old_target)
        struct.pack_into(
            "<i", result, operand,
            new_target - (new_rva + offset + len(raw)),
        )
    return bytes(result)


def patch_references(payload, image, exact_moves, range_move, instructions):
    image_base = image[1]
    absolute_patches = 0
    relative_patches = 0
    old_range_start, old_range_end, range_delta = range_move
    _header, _virtual, text_rva, text_raw_size, text_raw = section_header(
        payload, ".text"
    )
    # Absolute pointers emitted into non-code sections are naturally word aligned.
    # Keep the scan section-local so overlapping instruction bytes can never be
    # mistaken for a pointer.
    for section_name in (".rdata", ".data"):
        _sh, _sv, section_rva, section_raw_size, section_raw = section_header(
            payload, section_name
        )
        for section_offset in range(0, section_raw_size - 3, 4):
            site = section_raw + section_offset
            value = struct.unpack_from("<I", payload, site)[0]
            text_offset = value - image_base - text_rva
            replacement = exact_moves.get(text_offset)
            if replacement is None and old_range_start <= text_offset < old_range_end:
                replacement = text_offset + range_delta
            if replacement is not None and replacement != text_offset:
                struct.pack_into(
                    "<I", payload, site, image_base + text_rva + replacement
                )
                absolute_patches += 1

    # In code, only inspect complete decoded instructions. Absolute operands may
    # begin at several byte positions; relative control-flow operands are the
    # final four bytes of a direct call/jump instruction.
    for instruction_rva, raw, mnemonic in instructions:
        if len(raw) >= 4:
            operand = len(raw) - 4
            value = struct.unpack_from("<I", raw, operand)[0]
            text_offset = value - image_base - text_rva
            replacement = exact_moves.get(text_offset)
            if replacement is None and old_range_start <= text_offset < old_range_end:
                replacement = text_offset + range_delta
            if replacement is not None and replacement != text_offset:
                struct.pack_into(
                    "<I", payload, text_raw + instruction_rva + operand,
                    image_base + text_rva + replacement,
                )
                absolute_patches += 1

        if not (mnemonic.startswith("call") or mnemonic.startswith("j")) or len(raw) < 5:
            continue
        operand = len(raw) - 4
        displacement = struct.unpack_from("<i", raw, operand)[0]
        old_target = instruction_rva + len(raw) + displacement
        replacement = exact_moves.get(old_target)
        if replacement is None and old_range_start <= old_target < old_range_end:
            replacement = old_target + range_delta
        if replacement is not None and replacement != old_target:
            struct.pack_into(
                "<i", payload, text_raw + instruction_rva + operand,
                replacement - (instruction_rva + len(raw)),
            )
            relative_patches += 1
    return absolute_patches, relative_patches


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("candidate", type=Path)
    parser.add_argument("map", type=Path)
    parser.add_argument("output", type=Path)
    options = parser.parse_args()

    candidate_image = _load_pe_image(options.candidate)
    payload = bytearray(candidate_image[0])
    text_header, _virtual, text_rva, text_raw_size, text_raw = section_header(
        payload, ".text"
    )
    retail_payload = RETAIL.read_bytes()
    _rh, retail_virtual, retail_text_rva, _rrs, _rro = section_header(
        retail_payload, ".text"
    )
    if text_rva != retail_text_rva:
        raise ValueError("candidate/retail .text RVA differs")

    symbols = map_symbols(options.map)
    old_funclet_start, funclet_size = map_text_x(options.map)
    old_funclet_end = old_funclet_start + funclet_size
    instructions = text_instructions(options.candidate, text_rva)
    candidate_runs = import_thunk_runs(options.candidate)
    candidate_tail_runs = [
        run for run in candidate_runs
        if run[0][0] >= symbols["__strdup"]
    ]
    candidate_run = max(candidate_tail_runs, key=len)
    candidate_thunks = {
        identity: (offset, target_va)
        for offset, target_va, identity in candidate_run
    }
    all_retail_runs = import_thunk_runs(RETAIL)
    retail_runs = [
        run for run in all_retail_runs
        if run[0][0] >= symbols["__strdup"]
    ]
    retail_main_end = max(retail_runs, key=len)[-1][0] + 6
    retail_thunks = {}
    for run in retail_runs:
        for offset, target_va, identity in run:
            if identity not in candidate_thunks:
                continue
            if identity in retail_thunks:
                raise ValueError(f"duplicate retail import thunk identity: {identity}")
            retail_thunks[identity] = (offset, target_va)
    if candidate_thunks.keys() != retail_thunks.keys():
        raise ValueError("candidate/retail import thunk identities differ")

    exact_moves = {
        candidate_thunks[identity][0]: retail_thunks[identity][0]
        for identity in candidate_thunks
    }
    candidate_early = max(
        (run for run in candidate_runs if run[0][0] < symbols["__strdup"]),
        key=len,
    )
    candidate_early_identities = {row[2] for row in candidate_early}
    retail_early_matches = [
        run for run in all_retail_runs
        if {row[2] for row in run} == candidate_early_identities
    ]
    if len(retail_early_matches) != 1:
        raise ValueError("could not uniquely identify the authored import-thunk run")
    retail_early = retail_early_matches[0]
    early_by_identity = {identity: (offset, target_va)
                         for offset, target_va, identity in retail_early}
    candidate_early_targets = {
        identity: target_va for _offset, target_va, identity in candidate_early
    }
    for old_offset, _target_va, identity in candidate_early:
        exact_moves[old_offset] = early_by_identity[identity][0]
    # The three-byte bodies are byte-identical, but their relocation owners
    # disambiguate them: the locale table targets the earlier init-collate slot,
    # while fpexcept targets matherr at initcoll's current candidate position.
    exact_moves[symbols["___init_collate"]] = RETAIL_INITCOLL_SLOT
    exact_moves[symbols["__matherr"]] = symbols["___init_collate"]
    matherr_raw, matherr_relocations = coff_code(OBJECT_ROOT / "matherr.obj")
    if matherr_relocations or len(matherr_raw) != 3:
        raise ValueError("unexpected matherr contribution shape")
    pieces = []
    for member, symbol in (
        ("tell.obj", "__tell"),
        ("access.obj", "__access"),
        ("strrev.obj", "__strrev"),
    ):
        raw, relocations = coff_code(OBJECT_ROOT / member)
        old_start = symbols[symbol]
        masked = set()
        for site, _kind in relocations:
            masked.update(range(site, site + 4))
        matches = []
        retail_text = retail_payload[
            section_header(retail_payload, ".text")[4]:
            section_header(retail_payload, ".text")[4] + text_raw_size
        ]
        for offset in range(len(retail_text) - len(raw) + 1):
            if all(retail_text[offset + index] == value
                   for index, value in enumerate(raw) if index not in masked):
                matches.append(offset)
        matches = [offset for offset in matches if offset >= retail_main_end]
        if len(matches) != 1:
            raise ValueError(f"could not uniquely locate retail {member}: {matches}")
        new_start = matches[0]
        exact_moves[old_start] = new_start
        linked = candidate_image[0][
            text_raw + old_start:text_raw + old_start + len(raw)
        ]
        pieces.append((old_start, new_start, linked, relocations))

    target_funclet_start = max(rva + 6 for rva, _va in retail_thunks.values())
    target_funclet_start = (target_funclet_start + 15) & ~15
    if target_funclet_start + funclet_size != retail_virtual:
        raise ValueError(
            f"reviewed funclet tail ends at 0x{target_funclet_start + funclet_size:x}, "
            f"retail .text ends at 0x{retail_virtual:x}"
        )
    funclet_delta = target_funclet_start - old_funclet_start

    absolute_patches, relative_patches = patch_references(
        payload,
        candidate_image,
        exact_moves,
        (old_funclet_start, old_funclet_end, funclet_delta),
        instructions,
    )

    tail_start = min(symbols["__matherr"], min(rva for rva, _va in candidate_thunks.values()))
    target_end = target_funclet_start + funclet_size
    early_start = min(candidate_early[0][0], retail_early[0][0])
    early_end = max(candidate_early[-1][0] + 6, retail_early[-1][0] + 6)
    payload[text_raw + early_start:text_raw + early_end] = b"\xcc" * (
        early_end - early_start
    )
    for target_rva, _retail_target_va, identity in retail_early:
        target_va = candidate_early_targets[identity]
        thunk = b"\xff\x25" + struct.pack("<I", target_va)
        payload[text_raw + target_rva:text_raw + target_rva + 6] = thunk
    payload[text_raw + tail_start:text_raw + target_end] = b"\xcc" * (
        target_end - tail_start
    )
    for identity, (target_rva, _retail_target_va) in retail_thunks.items():
        target_va = candidate_thunks[identity][1]
        thunk = b"\xff\x25" + struct.pack("<I", target_va)
        payload[text_raw + target_rva:text_raw + target_rva + 6] = thunk
    for old_start, new_start, raw, relocations in pieces:
        moved = relocate_piece(raw, relocations, old_start, new_start, exact_moves)
        payload[text_raw + new_start:text_raw + new_start + len(moved)] = moved
    payload[
        text_raw + RETAIL_INITCOLL_SLOT:
        text_raw + RETAIL_INITCOLL_SLOT + len(matherr_raw)
    ] = matherr_raw

    original_funclets = candidate_image[0][
        text_raw + old_funclet_start:text_raw + old_funclet_end
    ]
    moved_funclets = relocate_funclets(
        original_funclets, old_funclet_start, target_funclet_start, exact_moves,
        instructions,
    )
    payload[
        text_raw + target_funclet_start:text_raw + target_end
    ] = moved_funclets
    payload[text_raw + target_end:text_raw + text_raw_size] = b"\0" * (
        text_raw_size - target_end
    )
    struct.pack_into("<I", payload, text_header + 8, target_end)

    options.output.parent.mkdir(parents=True, exist_ok=True)
    options.output.write_bytes(payload)
    print(
        f"text tail normalized: {len(candidate_thunks)} CRT and "
        f"{len(candidate_early)} authored import thunks, "
        f"funclets +0x{funclet_delta:x}, {absolute_patches} absolute and "
        f"{relative_patches} relative references patched"
    )


if __name__ == "__main__":
    main()

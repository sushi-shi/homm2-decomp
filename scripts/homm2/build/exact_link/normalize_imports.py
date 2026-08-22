#!/usr/bin/env python3
"""Place candidate IAT/ILT slots in reviewed retail semantic order."""

import argparse
import struct
from pathlib import Path

from homm2.build.assert_relocs import _load_pe_image, _pe_import_iat_identities


ROOT = next(path for path in Path(__file__).resolve().parents
            if (path / "flake.nix").exists())


def rva_offset(image, rva):
    for _name, start, end, raw_offset, raw_size in image[2]:
        if start <= rva < end and rva - start < raw_size:
            return raw_offset + rva - start
    raise ValueError(f"RVA 0x{rva:x} is outside raw PE sections")


def descriptor_slots(path):
    image = _load_pe_image(path)
    payload, image_base, _sections = image
    pe = struct.unpack_from("<I", payload, 0x3C)[0]
    optional = pe + 24
    import_rva = struct.unpack_from("<I", payload, optional + 104)[0]
    identities = _pe_import_iat_identities(path)
    result = []
    descriptor_rva = import_rva
    while True:
        descriptor = struct.unpack_from(
            "<IIIII", payload, rva_offset(image, descriptor_rva)
        )
        lookup_rva, _stamp, _forwarder, _name_rva, address_rva = descriptor
        if not any(descriptor):
            break
        lookup_rva = lookup_rva or address_rva
        rows = []
        index = 0
        while True:
            slot_rva = address_rva + index * 4
            value = struct.unpack_from(
                "<I", payload, rva_offset(image, slot_rva)
            )[0]
            if value == 0:
                break
            rows.append((lookup_rva + index * 4, slot_rva, identities[slot_rva]))
            index += 1
        result.append(rows)
        descriptor_rva += 20
    return image, result


def c_string(image, rva):
    payload = image[0]
    result = bytearray()
    while True:
        value = payload[rva_offset(image, rva + len(result))]
        result.append(value)
        if value == 0:
            return bytes(result)


def semantic_name_layout(path):
    image = _load_pe_image(path)
    payload = image[0]
    pe = struct.unpack_from("<I", payload, 0x3C)[0]
    optional = pe + 24
    descriptor_rva = struct.unpack_from("<I", payload, optional + 104)[0]
    identities = _pe_import_iat_identities(path)
    descriptors = []
    records = {}
    while True:
        values = struct.unpack_from(
            "<IIIII", payload, rva_offset(image, descriptor_rva)
        )
        lookup_rva, _stamp, _forwarder, name_rva, address_rva = values
        if not any(values):
            break
        lookup_rva = lookup_rva or address_rva
        index = 0
        while True:
            value = struct.unpack_from(
                "<I", payload, rva_offset(image, lookup_rva + index * 4)
            )[0]
            if value == 0:
                break
            identity = identities[address_rva + index * 4]
            if not value & 0x80000000:
                name = c_string(image, value + 2)
                size = 2 + len(name)
                size += size & 1
                record = payload[
                    rva_offset(image, value):rva_offset(image, value) + size
                ]
                records[identity] = (value, record)
            index += 1
        descriptors.append({
            "descriptor_rva": descriptor_rva,
            "name_rva": name_rva,
            "name": c_string(image, name_rva),
        })
        descriptor_rva += 20
    return image, descriptors, records


def set_rdata_virtual_size(payload, target_end_rva):
    pe = struct.unpack_from("<I", payload, 0x3C)[0]
    section_count = struct.unpack_from("<H", payload, pe + 6)[0]
    optional_size = struct.unpack_from("<H", payload, pe + 20)[0]
    section_table = pe + 24 + optional_size
    for index in range(section_count):
        header = section_table + index * 40
        if payload[header:header + 8].rstrip(b"\0") != b".rdata":
            continue
        rva = struct.unpack_from("<I", payload, header + 12)[0]
        struct.pack_into("<I", payload, header + 8, target_end_rva - rva)
        return
    raise ValueError("candidate has no .rdata PE section")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("candidate", type=Path)
    parser.add_argument("output", type=Path)
    options = parser.parse_args()

    candidate_image, candidate_dlls = descriptor_slots(options.candidate)
    _retail_image, retail_dlls = descriptor_slots(ROOT / "build/orig/HMM2PL.exe")
    _, candidate_descriptors, candidate_names = semantic_name_layout(options.candidate)
    _, retail_descriptors, retail_names = semantic_name_layout(
        ROOT / "build/orig/HMM2PL.exe"
    )
    if len(candidate_dlls) != len(retail_dlls):
        raise ValueError("candidate/retail descriptor count differs")

    payload = bytearray(candidate_image[0])
    image_base = candidate_image[1]
    name_targets = {}
    name_writes = []
    for identity, (retail_rva, retail_record) in retail_names.items():
        candidate_rva, candidate_record = candidate_names[identity]
        if candidate_record != retail_record:
            raise ValueError(f"candidate/retail import record differs: {identity}")
        name_targets[identity] = retail_rva
        name_writes.append((retail_rva, candidate_record))
    if len(candidate_descriptors) != len(retail_descriptors):
        raise ValueError("candidate/retail import descriptor count differs")
    descriptor_name_writes = []
    for candidate_descriptor, retail_descriptor in zip(
        candidate_descriptors, retail_descriptors
    ):
        if candidate_descriptor["name"].lower() != retail_descriptor["name"].lower():
            raise ValueError("candidate/retail descriptor DLL name differs")
        descriptor_name_writes.append((
            candidate_descriptor["descriptor_rva"] + 12,
            retail_descriptor["name_rva"],
        ))
        name_writes.append((retail_descriptor["name_rva"], candidate_descriptor["name"]))

    candidate_spans = [
        (rva, rva + len(record)) for rva, record in candidate_names.values()
    ] + [
        (row["name_rva"], row["name_rva"] + len(row["name"]))
        for row in candidate_descriptors
    ]
    target_spans = [(rva, rva + len(record)) for rva, record in name_writes]
    clear_start = min(start for start, _end in candidate_spans + target_spans)
    clear_end = max(end for _start, end in candidate_spans + target_spans)
    payload[
        rva_offset(candidate_image, clear_start):rva_offset(candidate_image, clear_end)
    ] = b"\0" * (clear_end - clear_start)

    reference_map = {}
    slot_writes = []
    for candidate_rows, retail_rows in zip(candidate_dlls, retail_dlls):
        if {row[2] for row in candidate_rows} != {row[2] for row in retail_rows}:
            raise ValueError("candidate/retail DLL import identities differ")
        candidate_iat_rvas = [row[1] for row in candidate_rows]
        retail_iat_rvas = [row[1] for row in retail_rows]
        if candidate_iat_rvas != retail_iat_rvas:
            raise ValueError("candidate/retail IAT slot geometry differs")
        current = {}
        for lookup_rva, iat_rva, identity in candidate_rows:
            current[identity] = (
                struct.unpack_from(
                    "<I", payload, rva_offset(candidate_image, lookup_rva)
                )[0],
                struct.unpack_from(
                    "<I", payload, rva_offset(candidate_image, iat_rva)
                )[0],
                iat_rva,
            )
        for index, (_retail_lookup, _retail_iat, identity) in enumerate(retail_rows):
            lookup_value, iat_value, old_iat_rva = current[identity]
            lookup_value = name_targets.get(identity, lookup_value)
            iat_value = name_targets.get(identity, iat_value)
            new_iat_rva = candidate_rows[index][1]
            reference_map[image_base + old_iat_rva] = image_base + new_iat_rva
            slot_writes.append(
                (candidate_rows[index][0], new_iat_rva, lookup_value, iat_value)
            )

    reference_patches = 0
    offset = 0
    while offset + 4 <= len(payload):
        value = struct.unpack_from("<I", payload, offset)[0]
        replacement = reference_map.get(value)
        if replacement is None or replacement == value:
            offset += 1
            continue
        struct.pack_into("<I", payload, offset, replacement)
        reference_patches += 1
        offset += 4

    for lookup_rva, iat_rva, lookup_value, iat_value in slot_writes:
        struct.pack_into(
            "<I", payload, rva_offset(candidate_image, lookup_rva), lookup_value
        )
        struct.pack_into(
            "<I", payload, rva_offset(candidate_image, iat_rva), iat_value
        )
    for descriptor_name_site, name_rva in descriptor_name_writes:
        struct.pack_into(
            "<I", payload, rva_offset(candidate_image, descriptor_name_site), name_rva
        )
    for name_rva, record in name_writes:
        offset = rva_offset(candidate_image, name_rva)
        payload[offset:offset + len(record)] = record
    target_end = max(end for _start, end in target_spans)
    set_rdata_virtual_size(payload, target_end)

    options.output.parent.mkdir(parents=True, exist_ok=True)
    options.output.write_bytes(payload)
    print(
        f"IAT/ILT identities and semantic name records reordered; "
        f"patched {reference_patches} references"
    )


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Derive the VC4.2 LIBCMT variant built with /Gf /Gy instead of /GF /Gy."""

import argparse
import csv
import hashlib
import json
import shutil
import struct
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path


REPO = next((path for path in Path(__file__).resolve().parents
             if (path / "flake.nix").exists()), Path.cwd())
PINNED_LIBCMT_SHA256 = (
    "1b42eb48c0824798e2585573069f85c9c0b08df45db62ff2cc7a7aa5dd1b5ba2")
PINNED_DERIVED_LIBCMT_SHA256 = (
    "3b6520118834f596ca1266d8418a234d8fa053a1c895a85acda904fa0dfb8208")
PINNED_CL_SHA256 = (
    "c5bf7ad84482e8a54d5753fcbd3e648d8a1192f5ca8b8cf1f5d23b651750585f")
PINNED_MAKEFILE_SHA256 = (
    "ba26fcbdb3c0e0a16ac12be63f50e425080a253f5abce638291a1aab4ea90093")

IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040
IMAGE_SCN_LNK_COMDAT = 0x00001000
IMAGE_SCN_MEM_READ = 0x40000000
IMAGE_SCN_MEM_WRITE = 0x80000000
READONLY_LITERAL_CHARACTERISTICS = (
    IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_LNK_COMDAT |
    IMAGE_SCN_MEM_READ | 0x00300000)
WRITABLE_LITERAL_CHARACTERISTICS = (
    READONLY_LITERAL_CHARACTERISTICS | IMAGE_SCN_MEM_WRITE)


@dataclass(frozen=True)
class MemberSpec:
    literal_sections: int
    source: str | None = None
    source_sha256: str | None = None


MEMBERS = {
    "aw_map.obj": MemberSpec(
        2, "AW_MAP.C", "d8132c5d312df6ea4fd5decbde34a17b690ff4b0dead880bff49fad9277fee31"),
    "crt0msg.obj": MemberSpec(
        23, "CRT0MSG.C", "8be87092f703608b2c9d3582de624438f8bf7e66003938e007b5a4e0abf610f2"),
    "crtmbox.obj": MemberSpec(
        4, "CRTMBOX.C", "f1f198130947ea2fecaf97639694aa4b5052e10fe14bb94958f7750995dfdfa9"),
    "output.obj": MemberSpec(
        2, "OUTPUT.C", "9f58eff37b7afd66e79e37622fd216538a29fdd051b675c7141a438045c9fa56"),
    "perror.obj": MemberSpec(
        2, "PERROR.C", "601e4edd5007a52ca59999e63bb24560c7ede487c6346158c5d774f3c9e5ba91"),
    "syserr.obj": MemberSpec(
        40, "SYSERR.C", "7379ef8182df92bd34fe29a5c49a7b44b1913e835eebc95a84687e30861febb0"),
    # Microsoft ships these four members only in the binary MT_LIB sublibraries.
    "cvt.obj": MemberSpec(1),
    "fpexcept.obj": MemberSpec(27),
    "testfdiv.obj": MemberSpec(2),
    "x10fout.obj": MemberSpec(4),
}
EXPECTED_TRANSFORMED_SECTIONS = sum(
    spec.literal_sections for spec in MEMBERS.values())
EXPECTED_RETAIL_LITERAL_COUNT = 102

WHOLE_RETAIL_UNITS = {"crt0msg", "crtmbox", "fpexcept", "syserr", "x10fout"}
SINGLE_RETAIL_LITERALS = {
    "??_C@_02HFBK@?3?5?$AA@": "perror",
    "??_C@_06ONKE@?$CInull?$CJ?$AA@": "output",
    "??_C@_08OBID@KERNEL32?$AA@": "testfdiv",
    "??_C@_05OFLO@e?$CL000?$AA@": "cvt",
    "??_C@_01A@?$AA?$AA@": "aw_map",
}

OUTPUT_ARCHIVE_LITERAL = (
    "??_C@_1O@POHA@?$AA?$CI?$AAn?$AAu?$AAl?$AAl?$AA?$CJ?$AA?$AA?$AA?$AA?$AA?$AH?$AA"
    "?$AA?$AA?$AA?$AA?$AA?$AA?$9A?$AE?$;I@")
OUTPUT_LITERAL_DIFFERING_OFFSETS = (109, 112, 113)


@dataclass(frozen=True)
class ArchiveEntry:
    name: str
    header_offset: int
    data_offset: int
    data_end: int


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def _raw_archive_entries(data):
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
        start = offset + 60
        end = start + size
        if end > len(data):
            raise ValueError("truncated archive member")
        yield header[:16].decode("ascii", "replace").rstrip(), offset, start, end
        offset = end + (size & 1)


def archive_entries(data):
    raw_entries = list(_raw_archive_entries(data))
    longnames = next((data[start:end] for name, _, start, end in raw_entries
                      if name == "//"), b"")
    entries = []
    for raw_name, header, start, end in raw_entries:
        if raw_name.startswith("/") and raw_name[1:].isdigit():
            offset = int(raw_name[1:])
            if offset >= len(longnames):
                raise ValueError("invalid archive long-name offset")
            name_end = longnames.find(b"\0", offset)
            if name_end < 0:
                raise ValueError("unterminated archive long name")
            name = longnames[offset:name_end].decode("ascii")
        else:
            name = raw_name[:-1] if raw_name.endswith("/") else raw_name
        entries.append(ArchiveEntry(name, header, start, end))
    return entries


def _coff_symbol_name(data, entry, string_offset, string_size):
    zeroes, name_offset = struct.unpack_from("<II", data, entry)
    if zeroes:
        return data[entry:entry + 8].rstrip(b"\0").decode("ascii")
    if name_offset < 4 or name_offset >= string_size:
        raise ValueError("invalid COFF long symbol name")
    start = string_offset + name_offset
    end = data.find(b"\0", start, string_offset + string_size)
    if end < 0:
        raise ValueError("unterminated COFF long symbol name")
    return data[start:end].decode("ascii")


def literal_sections(data):
    if len(data) < 20:
        raise ValueError("truncated COFF object")
    machine, section_count, _, symbol_offset, symbol_count, optional_size, _ = (
        struct.unpack_from("<HHIIIHH", data, 0))
    if machine != 0x014C or optional_size:
        raise ValueError("expected an i386 COFF object without an optional header")
    section_offset = 20
    if section_offset + section_count * 40 > len(data):
        raise ValueError("truncated COFF section table")
    string_offset = symbol_offset + symbol_count * 18
    if string_offset + 4 > len(data):
        raise ValueError("invalid COFF symbol table")
    string_size = struct.unpack_from("<I", data, string_offset)[0]
    if string_size < 4 or string_offset + string_size > len(data):
        raise ValueError("invalid COFF string table")

    found = {}
    index = 0
    while index < symbol_count:
        entry = symbol_offset + index * 18
        section_number = struct.unpack_from("<h", data, entry + 12)[0]
        storage_class = data[entry + 16]
        aux_count = data[entry + 17]
        name = _coff_symbol_name(data, entry, string_offset, string_size)
        if storage_class == 2 and section_number > 0 and name.startswith("??_C@"):
            previous = found.setdefault(section_number, name)
            if previous != name:
                raise ValueError("multiple decorated literals share COFF section %d" %
                                 section_number)
        index += 1 + aux_count
    return found


def transform_object(data, expected_count=None):
    original = bytes(data)
    output = bytearray(original)
    literals = literal_sections(original)
    if expected_count is not None and len(literals) != expected_count:
        raise ValueError("expected %d decorated literal sections, found %d" %
                         (expected_count, len(literals)))
    changed_offsets = set()
    for section_number in sorted(literals):
        offset = 20 + (section_number - 1) * 40
        name = bytes(output[offset:offset + 8])
        characteristics = struct.unpack_from("<I", output, offset + 36)[0]
        if name != b".rdata\0\0" or characteristics != READONLY_LITERAL_CHARACTERISTICS:
            raise ValueError(
                "decorated literal section %d is not the VC4.2 /GF COMDAT shape" %
                section_number)
        output[offset:offset + 8] = b".data\0\0\0"
        struct.pack_into("<I", output, offset + 36,
                         WRITABLE_LITERAL_CHARACTERISTICS)
        changed_offsets.update(range(offset + 1, offset + 6))
        changed_offsets.add(offset + 39)

    _, _, _, symbol_offset, symbol_count, _, _ = struct.unpack_from(
        "<HHIIIHH", output, 0)
    renamed_section_symbols = set()
    index = 0
    while index < symbol_count:
        entry = symbol_offset + index * 18
        section_number = struct.unpack_from("<h", output, entry + 12)[0]
        storage_class = output[entry + 16]
        aux_count = output[entry + 17]
        if section_number in literals and storage_class == 3:
            if bytes(output[entry:entry + 8]) != b".rdata\0\0":
                raise ValueError("literal COFF section symbol is not .rdata")
            if section_number in renamed_section_symbols:
                raise ValueError("duplicate literal COFF section symbol")
            output[entry:entry + 8] = b".data\0\0\0"
            changed_offsets.update(range(entry + 1, entry + 6))
            renamed_section_symbols.add(section_number)
        index += 1 + aux_count
    if renamed_section_symbols != set(literals):
        raise ValueError("literal COFF section symbol is missing")
    differences = {index for index, (before, after) in
                   enumerate(zip(original, output)) if before != after}
    if len(output) != len(original) or differences != changed_offsets:
        raise ValueError("literal transform changed bytes outside section headers")
    return bytes(output), {
        "literal_sections": len(literals),
        "literal_names": [literals[number] for number in sorted(literals)],
        "changed_bytes": len(differences),
    }


def transform_archive(data, require_pinned_hash=True):
    if require_pinned_hash and sha256(data) != PINNED_LIBCMT_SHA256:
        raise ValueError("LIBCMT.LIB does not match the pinned VC4.2 archive")
    entries = archive_entries(data)
    targets = {}
    for entry in entries:
        basename = entry.name.replace("\\", "/").rsplit("/", 1)[-1].lower()
        if basename in MEMBERS:
            if basename in targets:
                raise ValueError("duplicate LIBCMT member: %s" % basename)
            targets[basename] = entry
    missing = sorted(set(MEMBERS) - set(targets))
    if missing:
        raise ValueError("LIBCMT members missing: %s" % ", ".join(missing))

    output = bytearray(data)
    members = []
    for basename, spec in sorted(MEMBERS.items()):
        entry = targets[basename]
        original = data[entry.data_offset:entry.data_end]
        transformed, evidence = transform_object(original, spec.literal_sections)
        if len(transformed) != len(original):
            raise ValueError("LIBCMT member size changed: %s" % basename)
        output[entry.data_offset:entry.data_end] = transformed
        members.append({
            "member": basename,
            "archive_name": entry.name,
            "source_available": spec.source is not None,
            **evidence,
        })
    if sum(row["literal_sections"] for row in members) != EXPECTED_TRANSFORMED_SECTIONS:
        raise ValueError("unexpected total transformed literal sections")
    return bytes(output), members


def expected_retail_literals(symbols_path):
    literals = {}
    with open(symbols_path, newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            name = row["name"]
            unit = row["unit"]
            if not name.startswith("??_C@"):
                continue
            if unit in WHOLE_RETAIL_UNITS or SINGLE_RETAIL_LITERALS.get(name) == unit:
                literals[name] = unit
    if len(literals) != EXPECTED_RETAIL_LITERAL_COUNT:
        raise ValueError("expected %d retail CRT literals, found %d" %
                         (EXPECTED_RETAIL_LITERAL_COUNT, len(literals)))
    return literals


def _storage_mismatch_keys(report):
    symbols = report["static_storage"]["public_symbols"]["symbols"]
    return {(row["name"], row["unit"])
            for row in symbols
            if row["candidate_count"] == 1 and not row["storage_class_matches"]}


def audit_link_ab(baseline_report_path, derived_report_path,
                  baseline_exe, derived_exe, symbols_path=None):
    from homm2.build.link_exe import read_pe, read_pe_payload_evidence

    baseline = json.loads(Path(baseline_report_path).read_text())
    derived = json.loads(Path(derived_report_path).read_text())
    baseline_mismatches = _storage_mismatch_keys(baseline)
    derived_mismatches = _storage_mismatch_keys(derived)
    fixed = baseline_mismatches - derived_mismatches
    introduced = derived_mismatches - baseline_mismatches
    expected = set(expected_retail_literals(
        symbols_path or REPO / "build/gen/symbol_names.csv").items())
    if fixed != expected:
        missing = sorted(expected - fixed)
        unexpected = sorted(fixed - expected)
        raise ValueError(
            "A/B storage fixes differ from the exact 102-symbol set: "
            "missing=%d unexpected=%d" % (len(missing), len(unexpected)))
    if introduced:
        raise ValueError("A/B link introduced %d storage-class mismatches" %
                         len(introduced))

    text_evidence = []
    for path in (baseline_exe, derived_exe):
        pe = read_pe(path)
        text = pe["sections"].get(".text")
        if text is None:
            raise ValueError("A/B PE has no .text section: %s" % path)
        text_evidence.append(read_pe_payload_evidence(
            path, text["rva"], text["raw_size"]))
    baseline_text, derived_text = text_evidence
    if (baseline_text["normalized_sha256"] !=
            derived_text["normalized_sha256"] or
            baseline_text["highlow_base_relocation_count"] !=
            derived_text["highlow_base_relocation_count"]):
        raise ValueError("A/B HIGHLOW-normalized .text differs")
    return {
        "fixed_storage_class_mismatches": len(fixed),
        "introduced_storage_class_mismatches": len(introduced),
        "baseline_storage_class_mismatches": len(baseline_mismatches),
        "derived_storage_class_mismatches": len(derived_mismatches),
        "fixed_symbols": [
            {"name": name, "unit": unit} for name, unit in sorted(fixed)],
        "text": {
            "raw_size": baseline_text["size"],
            "normalized_sha256": baseline_text["normalized_sha256"],
            "highlow_base_relocation_count":
                baseline_text["highlow_base_relocation_count"],
        },
    }


def build_library(input_path, output_path, report_path=None):
    input_path = Path(input_path)
    output_path = Path(output_path)
    source = input_path.read_bytes()
    derived, members = transform_archive(source)
    derived_sha256 = sha256(derived)
    if derived_sha256 != PINNED_DERIVED_LIBCMT_SHA256:
        raise ValueError("derived LIBCMT.LIB does not match the proved /Gf archive")
    report = {
        "input": str(input_path),
        "input_sha256": sha256(source),
        "output": str(output_path),
        "output_sha256": derived_sha256,
        "archive_size": len(source),
        "member_count": len(archive_entries(source)),
        "transformed_literal_sections": EXPECTED_TRANSFORMED_SECTIONS,
        "retail_storage_gate_literals": EXPECTED_RETAIL_LITERAL_COUNT,
        "members": members,
    }
    output_path.parent.mkdir(parents=True, exist_ok=True)
    temporary = output_path.with_suffix(output_path.suffix + ".tmp")
    temporary.write_bytes(derived)
    temporary.replace(output_path)
    if report_path:
        report_path = Path(report_path)
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    return report


def _normalize_proof_metadata(data):
    output = bytearray(data)
    output[4:8] = b"\0\0\0\0"
    _, _, _, symbol_offset, symbol_count, _, _ = struct.unpack_from(
        "<HHIIIHH", output, 0)
    index = 0
    while index < symbol_count:
        entry = symbol_offset + index * 18
        aux_count = output[entry + 17]
        if output[entry + 16] == 0x67:
            for aux_index in range(aux_count):
                start = entry + (aux_index + 1) * 18
                output[start:start + 18] = bytes(output[start:start + 18]).lower()
        index += 1 + aux_count
    return bytes(output)


def _prove_archive_object_identity(basename, rebuilt, archive):
    rebuilt = _normalize_proof_metadata(rebuilt)
    archive = _normalize_proof_metadata(archive)
    if rebuilt == archive:
        return {"archive_identity": "exact-after-metadata-normalization"}
    if basename != "output.obj":
        raise ValueError("uppercase /GF rebuild differs from archive: %s" % basename)

    rebuilt_names = literal_sections(rebuilt)
    archive_names = literal_sections(archive)
    differing_sections = [
        number for number in sorted(set(rebuilt_names) | set(archive_names))
        if rebuilt_names.get(number) != archive_names.get(number)
    ]
    archive_name = archive_names.get(1)
    rebuilt_name = rebuilt_names.get(1)
    name_differences = tuple(
        index for index, (left, right) in enumerate(zip(archive_name or "",
                                                        rebuilt_name or ""))
        if left != right)
    if (differing_sections != [1] or archive_name != OUTPUT_ARCHIVE_LITERAL or
            len(rebuilt_name or "") != len(archive_name or "") or
            name_differences != OUTPUT_LITERAL_DIFFERING_OFFSETS):
        raise ValueError(
            "OUTPUT archive literal identity exception changed: sections=%r "
            "archive=%r rebuilt=%r" %
            (differing_sections, archive_names, rebuilt_names))
    repaired = rebuilt.replace(
        rebuilt_name.encode("ascii"),
        OUTPUT_ARCHIVE_LITERAL.encode("ascii"))
    if repaired != archive:
        raise ValueError("OUTPUT rebuild differs outside the proved literal decoration")
    differences = [index for index, (left, right) in
                   enumerate(zip(rebuilt, archive)) if left != right]
    if len(rebuilt) != len(archive) or len(differences) != 3:
        raise ValueError("OUTPUT literal identity exception is not exactly three bytes")
    return {
        "archive_identity": "compiler-local-literal-name-diff",
        "archive_literal": OUTPUT_ARCHIVE_LITERAL,
        "rebuilt_literal": rebuilt_name,
        "differing_bytes": len(differences),
    }


def _winepath(path):
    return subprocess.check_output(
        ["winepath", "-w", str(Path(path).resolve())], text=True,
        stderr=subprocess.DEVNULL).strip()


def prove_sources(source_dir, libcmt_path, compiler_path, include_dir):
    source_dir = Path(source_dir)
    compiler_path = Path(compiler_path)
    if sha256(compiler_path.read_bytes()) != PINNED_CL_SHA256:
        raise ValueError("CL.EXE does not match the pinned VC4.2 compiler")
    makefile = source_dir / "MAKEFILE"
    if sha256(makefile.read_bytes()) != PINNED_MAKEFILE_SHA256:
        raise ValueError("CRT MAKEFILE does not match the verified VC4.2 source")
    makefile_text = makefile.read_text(encoding="latin-1")
    if "CC_OPTS_BASE=-c -nologo -Zelp8 -W3 -WX -GFy -DWIN32" not in makefile_text:
        raise ValueError("CRT MAKEFILE no longer carries the proved /GF /Gy flags")

    archive = Path(libcmt_path).read_bytes()
    entries = archive_entries(archive)
    originals = {}
    for entry in entries:
        basename = entry.name.replace("\\", "/").rsplit("/", 1)[-1].lower()
        if basename in MEMBERS:
            originals[basename] = archive[entry.data_offset:entry.data_end]

    proof = []
    with tempfile.TemporaryDirectory(prefix="homm2-vc42-gfy-") as temporary_name:
        temporary = Path(temporary_name)
        for basename, spec in sorted(MEMBERS.items()):
            if spec.source is None:
                continue
            source = source_dir / spec.source
            if sha256(source.read_bytes()) != spec.source_sha256:
                raise ValueError("CRT source hash mismatch: %s" % spec.source)
            local_source = temporary / spec.source.lower()
            shutil.copyfile(source, local_source)
            outputs = {}
            for pooling, variant in (("F", "readonly"), ("f", "writable")):
                output = temporary / (basename + "." + variant + ".obj")
                command = [
                    "wine", str(compiler_path), "-c", "-nologo", "-Zelp8",
                    "-W3", "-WX", "-G%sy" % pooling, "-DWIN32", "-GB",
                    "-Gi-", "-DWIN32_LEAN_AND_MEAN", "-DNOSERVICE",
                    "-D_MBCS", "-D_MB_MAP_DIRECT", "-D_CRTBLD", "-DWINHEAP",
                    "-D_MT", "-O2", "-I" + _winepath(source_dir),
                    "-I" + _winepath(include_dir), "-Fo" + _winepath(output),
                    local_source.name,
                ]
                subprocess.run(command, cwd=temporary, check=True)
                outputs[pooling] = output.read_bytes()
            transformed, _ = transform_object(outputs["F"], spec.literal_sections)
            if (_normalize_proof_metadata(transformed) !=
                    _normalize_proof_metadata(outputs["f"])):
                raise ValueError("/GF -> /Gf changed more than literal headers: %s" %
                                 basename)
            identity = _prove_archive_object_identity(
                basename, outputs["F"], originals[basename])
            proof.append({"member": basename, "source": spec.source,
                          "literal_sections": spec.literal_sections, **identity})
    return proof


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", default=str(
        REPO / "build/toolchain/msvc/lib/LIBCMT.LIB"))
    parser.add_argument("--output", default=str(
        REPO / "build/link/crt/LIBCMT.LIB"))
    parser.add_argument("--report", default=str(
        REPO / "build/link/crt/LIBCMT.gfy.json"))
    parser.add_argument("--prove-source")
    parser.add_argument("--compiler", default=str(
        REPO / "build/toolchain/msvc/bin/CL.EXE"))
    parser.add_argument("--include", default=str(
        REPO / "build/toolchain/msvc/include"))
    parser.add_argument("--baseline-link-report")
    parser.add_argument("--derived-link-report")
    parser.add_argument("--baseline-exe")
    parser.add_argument("--derived-exe")
    parser.add_argument("--ab-report")
    args = parser.parse_args(argv)
    if args.prove_source:
        proof = prove_sources(args.prove_source, args.input, args.compiler, args.include)
        print("LIBCMT /Gf source proof: %d members, %d literal sections" %
              (len(proof), sum(row["literal_sections"] for row in proof)))
    ab_arguments = (args.baseline_link_report, args.derived_link_report,
                    args.baseline_exe, args.derived_exe)
    if any(ab_arguments):
        if not all(ab_arguments):
            parser.error("all four A/B report and executable paths are required")
        ab_report = audit_link_ab(*ab_arguments)
        if args.ab_report:
            Path(args.ab_report).write_text(
                json.dumps(ab_report, indent=2, sort_keys=True) + "\n")
        print("LIBCMT /Gf A/B: %d exact storage fixes, no regressions; "
              ".text %s (%d HIGHLOW)" %
              (ab_report["fixed_storage_class_mismatches"],
               ab_report["text"]["normalized_sha256"][:12],
               ab_report["text"]["highlow_base_relocation_count"]))
    report = build_library(args.input, args.output, args.report)
    print("LIBCMT /Gf: %d member literals -> %s (%s)" %
          (report["transformed_literal_sections"], report["output"],
           report["output_sha256"][:12]))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

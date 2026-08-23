#!/usr/bin/env python3
"""Compile res/HMM2PL.rc with the era resource compiler and gate it byte-exactly.

The retail resources live as SOURCE: `res/HMM2PL.rc` carries the six menus, the
About dialog, and the VERSIONINFO as reviewed rc grammar, and `res/heroes.ico`
is the one retail-extracted binary (the 32x32 16-color program icon). This
driver compiles them with the era RC.EXE 5.00 + RCDLL.DLL under wine, then
byte-compares every compiled payload (type, name, language, bytes, and order)
against the retail image in both directions. Any drift fails the build.

The two RC binaries are pinned by SHA-256. They come from the same VC6-era
media family as the rest of the pinned toolchain (1999-04-30 builds); adding
them to scripts/toolchain/create-toolchain-release.py needs the install media
at hand, so until that lands the pins below are the provenance record.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import struct
import subprocess
import sys
from pathlib import Path

from homm2.build.extract_resources import read_pe_resources


ROOT = next(
    path for path in Path(__file__).resolve().parents if (path / "flake.nix").exists()
)
RC_EXE = ROOT / "build/toolchain/msvc/bin/RC.EXE"
RC_PINS = {
    "RC.EXE": "14a84379d318c76760bd550afaece03b5e8b39b49cbf6de7e0f7f0eac8216242",
    "RCDLL.DLL": "3c36f9b3d433eb5354655796659d6247684df8158b367d334969183b35454f3c",
}


def check_rc_binaries() -> None:
    for name, expected in RC_PINS.items():
        path = RC_EXE.parent / name
        if not path.exists():
            raise RuntimeError(
                f"{path} is missing: the era resource compiler (RC.EXE 5.00 + "
                "RCDLL.DLL, 1999-04-30) is not provisioned. See rc_res.py for "
                "the pinned SHA-256 provenance record."
            )
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        if digest != expected:
            raise RuntimeError(f"{path} sha256={digest}, expected {expected}")


def parse_res(blob: bytes) -> list[dict]:
    """RES32 records as {type, name, language, data}, skipping the null header."""

    def name_or_ordinal(data: bytes, offset: int):
        value = struct.unpack_from("<H", data, offset)[0]
        if value == 0xFFFF:
            return struct.unpack_from("<H", data, offset + 2)[0], offset + 4
        end = offset
        while struct.unpack_from("<H", data, end)[0] != 0:
            end += 2
        return data[offset:end].decode("utf-16-le"), end + 2

    records = []
    offset = 0
    while offset < len(blob):
        data_size, header_size = struct.unpack_from("<II", blob, offset)
        cursor = offset + 8
        rtype, cursor = name_or_ordinal(blob, cursor)
        rname, cursor = name_or_ordinal(blob, cursor)
        cursor = (cursor + 3) & ~3
        _data_version, _memory, language = struct.unpack_from("<IHH", blob, cursor)
        data = blob[offset + header_size: offset + header_size + data_size]
        if not (rtype == 0 and rname == 0):
            records.append(
                {"type": rtype, "name": rname, "language": language, "data": data}
            )
        offset = (offset + header_size + data_size + 3) & ~3
    return records


def compare(ours: list[dict], retail: list[dict]) -> list[str]:
    """Both-directions comparison: every mismatch line, empty when exact."""
    problems = []
    if len(ours) != len(retail):
        problems.append(f"payload count {len(ours)} != retail {len(retail)}")
    for index, (a, b) in enumerate(zip(ours, retail)):
        identity_a = (a["type"], a["name"], a["language"])
        identity_b = (b["type"], b["name"], b["language"])
        if identity_a != identity_b:
            problems.append(f"[{index}] identity {identity_a} != retail {identity_b}")
        elif a["data"] != b["data"]:
            first = next(
                (i for i in range(min(len(a["data"]), len(b["data"])))
                 if a["data"][i] != b["data"][i]),
                min(len(a["data"]), len(b["data"])),
            )
            problems.append(
                f"[{index}] {identity_a}: bytes differ "
                f"(sizes {len(a['data'])}/{len(b['data'])}, first at {first:#x})"
            )
    return problems


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rc", type=Path, default=ROOT / "res/HMM2PL.rc")
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--verify-exe", type=Path, default=ROOT / "build/orig/HMM2PL.exe")
    parser.add_argument("--report", type=Path)
    args = parser.parse_args(argv)

    check_rc_binaries()
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.unlink(missing_ok=True)
    env = dict(os.environ)
    env.setdefault("WINEDEBUG", "fixme-all")
    out_relative = os.path.relpath(args.out.resolve(), args.rc.resolve().parent)
    completed = subprocess.run(
        ["wine", str(RC_EXE), "/r", "/fo", out_relative.replace("/", "\\"),
         args.rc.name],
        cwd=args.rc.parent,
        env=env,
        capture_output=True,
        text=True,
    )
    if completed.returncode or not args.out.exists():
        sys.stderr.write(completed.stdout + completed.stderr)
        raise RuntimeError(f"era RC failed ({completed.returncode}) for {args.rc}")

    ours = parse_res(args.out.read_bytes())
    retail = read_pe_resources(args.verify_exe)
    problems = compare(ours, retail)
    if args.report:
        args.report.write_text(json.dumps({
            "count": len(ours),
            "payload_bytes": sum(len(r["data"]) for r in ours),
            "source": str(args.rc.resolve().relative_to(ROOT)),
            "exact": not problems,
            "problems": problems,
            "resources": [
                {"type": r["type"], "name": r["name"],
                 "language": r["language"], "size": len(r["data"])}
                for r in ours
            ],
        }, indent=1) + "\n")
    if problems:
        for line in problems:
            print("resource gate:", line, file=sys.stderr)
        raise RuntimeError(f"compiled resources differ from retail: {len(problems)} problem(s)")
    print(
        f"resource gate: {len(ours)} payloads, "
        f"{sum(len(r['data']) for r in ours)} bytes, byte-exact vs retail"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

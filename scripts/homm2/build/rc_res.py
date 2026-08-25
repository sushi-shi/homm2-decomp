#!/usr/bin/env python3
"""Compile res/HMM2PL.rc with the era resource compiler and gate it byte-exactly.

The retail resources live as SOURCE: `res/HMM2PL.rc` carries the six menus, the
About dialog, and the VERSIONINFO as reviewed rc grammar, and `res/heroes.ico`
is the one retail-extracted binary (the 32x32 16-color program icon). This
driver compiles them with the era RC.EXE 5.00 + RCDLL.DLL under wine, then
byte-compares every compiled payload (type, name, language, bytes, and order)
against the retail image in both directions. Any drift fails the build.

The two RC binaries are pinned by SHA-256 and packaged from the same VS6
Enterprise base disc as the rest of the toolchain. Their compiled output is
also compared payload-for-payload against the supported Buka retail image.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import struct
import sys
from pathlib import Path

from homm2.build.extract_resources import read_pe_resources
from homm2.core import wine
from homm2.core.coff import read_res


ROOT = next(
    path for path in Path(__file__).resolve().parents if (path / "flake.nix").exists()
)
RC_EXE = ROOT / "build/toolchain/msvc/bin/RC.EXE"
RC_PINS = {
    "RC.EXE": "582d0e68739b1128199d0ffc12eb62f48a17a3216a791fd6be948ff9e2eb2ffa",
    "RCDLL.DLL": "5932342fc326b056988cebc710b414592b81c9ba18b9ae247b838a6021f2e434",
}


def check_rc_binaries() -> None:
    for name, expected in RC_PINS.items():
        path = RC_EXE.parent / name
        if not path.exists():
            raise RuntimeError(
                f"{path} is missing: the era resource compiler (RC.EXE 5.00 + "
                "RCDLL.DLL from the VS6 Enterprise base disc) is not "
                "provisioned. See rc_res.py for "
                "the pinned SHA-256 provenance record."
            )
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        if digest != expected:
            raise RuntimeError(f"{path} sha256={digest}, expected {expected}")


parse_res = read_res  # the RES32 reader lives in homm2.core.coff


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


def extract_heroes_ico(retail: list[dict], destination: Path) -> None:
    """Rebuild res/heroes.ico from the retail RT_ICON + RT_GROUP_ICON payloads.

    The icon is retail artwork, so it is never committed; it is materialized
    from the locally supplied retail executable on every resource build.
    """
    group = next(r["data"] for r in retail if r["type"] == 14)
    image = next(r["data"] for r in retail if r["type"] == 3)
    reserved, container_type, count = struct.unpack_from("<HHH", group, 0)
    if (reserved, container_type, count) != (0, 1, 1):
        raise RuntimeError(f"unexpected RT_GROUP_ICON directory: {group.hex()}")
    width, height, colors, flags, planes, bits, size, _ordinal = struct.unpack_from(
        "<BBBBHHIH", group, 6
    )
    if size != len(image):
        raise RuntimeError(f"group says {size} icon bytes, image has {len(image)}")
    container = struct.pack("<HHH", 0, 1, 1) + struct.pack(
        "<BBBBHHII", width, height, colors, flags, planes, bits, size, 22
    ) + image
    if not destination.exists() or destination.read_bytes() != container:
        destination.write_bytes(container)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rc", type=Path, default=ROOT / "res/HMM2PL.rc")
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--verify-exe", type=Path, default=ROOT / "build/orig/HMM2PL.exe")
    parser.add_argument("--report", type=Path)
    args = parser.parse_args(argv)

    check_rc_binaries()
    retail = read_pe_resources(args.verify_exe)
    extract_heroes_ico(retail, args.rc.parent / "heroes.ico")
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.unlink(missing_ok=True)
    out_relative = os.path.relpath(args.out.resolve(), args.rc.resolve().parent)
    wine.run(RC_EXE, "/r", "/fo", out_relative.replace("/", "\\"),
             args.rc.name, cwd=args.rc.parent, quiet=True)
    if not args.out.exists():
        raise RuntimeError(f"era RC produced no output for {args.rc}")

    ours = parse_res(args.out.read_bytes())
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

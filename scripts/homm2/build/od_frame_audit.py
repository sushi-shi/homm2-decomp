"""Compare reconstructed and retail stack-frame evidence for every /Od function.

This is a diagnostic, not a matching gate.  It catches missing or extra local
storage immediately by comparing the prologue allocation, callee-saved register
pushes, and the ordered EBP-relative local references in paired COFF objects.
"""

import argparse
import csv
import re
import subprocess
import tomllib
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


REPO = Path(__file__).resolve().parents[3]
SYMBOLS = REPO / "build/gen/symbol_names.csv"
UNITS = REPO / "config/units.toml"
BASE = REPO / "build/objdiff/normalized/base"
TARGET = REPO / "build/objdiff/normalized/target"

_HEADER = re.compile(r"^[0-9a-fA-F]+\s+<(.+)>:$")
_INSTRUCTION = re.compile(
    r"^\s*([0-9a-fA-F]+):\s*(?:[0-9a-fA-F]{2} ?)*\s*$")
_LOCAL = re.compile(r"\[ebp\s*-\s*(0x[0-9a-f]+|[0-9]+)\]")


@dataclass(frozen=True)
class FrameProfile:
    has_frame_pointer: bool
    frame_size: int | None
    saved_registers: tuple[str, ...]
    local_references: tuple[int, ...]

    @property
    def local_offsets(self) -> tuple[int, ...]:
        return tuple(sorted(set(self.local_references)))


@dataclass(frozen=True)
class Finding:
    unit: str
    rva: int
    name: str
    categories: tuple[str, ...]
    base: FrameProfile
    target: FrameProfile


def _parse_instruction(line: str) -> str | None:
    if "\t" not in line:
        return None
    parts = line.split("\t")
    if not _INSTRUCTION.match(parts[0]):
        return None
    return " ".join(part.strip() for part in parts[1:] if part.strip()).lower()


def parse_disassembly(text: str, wanted: set[str]) -> dict[str, list[str]]:
    """Split one batched llvm-objdump result into requested instruction streams."""
    functions: dict[str, list[str]] = {}
    current: str | None = None
    for line in text.splitlines():
        header = _HEADER.match(line)
        if header:
            name = header.group(1)
            current = name if name in wanted else None
            if current is not None:
                functions.setdefault(current, [])
            continue
        instruction = _parse_instruction(line)
        if current is not None and instruction is not None:
            functions[current].append(instruction)
    return functions


def frame_profile(instructions: list[str]) -> FrameProfile:
    if (len(instructions) < 2 or instructions[0] != "push ebp"
            or instructions[1] != "mov ebp, esp"):
        return FrameProfile(False, None, (), ())

    position = 2
    frame_size = 0
    if position < len(instructions):
        match = re.fullmatch(r"sub esp, (0x[0-9a-f]+|[0-9]+)",
                             instructions[position])
        if match:
            frame_size = int(match.group(1), 0)
            position += 1
        elif instructions[position] == "push ecx":
            # VC commonly spells a four-byte local allocation this way.
            frame_size = 4
            position += 1

    saved = []
    while (position < len(instructions)
           and instructions[position] in ("push ebx", "push esi", "push edi")):
        saved.append(instructions[position].split()[1])
        position += 1

    references = []
    for instruction in instructions:
        references.extend(
            offset for offset in (
                int(value, 0) for value in _LOCAL.findall(instruction))
            if offset <= frame_size)
    return FrameProfile(True, frame_size, tuple(saved), tuple(references))


def compare_profiles(base: FrameProfile, target: FrameProfile) -> tuple[str, ...]:
    categories = []
    if base.has_frame_pointer != target.has_frame_pointer:
        categories.append("frame-pointer")
        return tuple(categories)
    if not target.has_frame_pointer:
        return ()
    if base.frame_size != target.frame_size:
        categories.append("frame-size")
    if base.saved_registers != target.saved_registers:
        categories.append("saved-registers")
    if base.local_offsets != target.local_offsets:
        categories.append("local-offsets")
    elif base.local_references != target.local_references:
        categories.append("local-reference-order")
    return tuple(categories)


def _od_units(path: Path) -> set[str]:
    manifest = tomllib.loads(path.read_text())
    flag_sets = manifest.get("flags", {})
    return {
        row["unit"] for row in manifest.get("unit", [])
        if "/Od" in flag_sets.get(row.get("flags", "base"), [])
    }


def _symbols(path: Path, od_units: set[str]) -> dict[str, list[dict]]:
    grouped: dict[str, list[dict]] = defaultdict(list)
    with path.open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind", "func") != "func" or row.get("unit") not in od_units:
                continue
            grouped[row["unit"]].append({
                "name": row["name"],
                "rva": int(row["rva"], 16),
            })
    return grouped


def _objdump(path: Path, names: list[str]) -> dict[str, list[str]]:
    result = subprocess.run(
        [
            "llvm-objdump",
            "-dr",
            "--x86-asm-syntax=intel",
            "--disassemble-symbols=" + ",".join(names),
            str(path),
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode:
        raise RuntimeError(
            f"llvm-objdump failed for {path}:\n{result.stderr.strip()}")
    return parse_disassembly(result.stdout, set(names))


def audit(
        symbols_path: Path = SYMBOLS,
        units_path: Path = UNITS,
        base_root: Path = BASE,
        target_root: Path = TARGET,
        only_unit: str | None = None,
        only_rva: int | None = None,
) -> tuple[list[Finding], dict[str, int]]:
    grouped = _symbols(symbols_path, _od_units(units_path))
    findings = []
    counts = defaultdict(int)
    for unit in sorted(grouped):
        if only_unit is not None and unit != only_unit:
            continue
        rows = [
            row for row in grouped[unit]
            if only_rva is None or row["rva"] == only_rva
        ]
        if not rows:
            continue
        base_path = base_root / f"{unit}.obj"
        target_path = target_root / f"{unit}.c.obj"
        if not base_path.is_file() or not target_path.is_file():
            counts["missing_object_units"] += 1
            continue
        names = [row["name"] for row in rows]
        base_functions = _objdump(base_path, names)
        target_functions = _objdump(target_path, names)
        for row in rows:
            name = row["name"]
            if name not in base_functions or name not in target_functions:
                counts["missing_symbol_pairs"] += 1
                continue
            base_profile = frame_profile(base_functions[name])
            target_profile = frame_profile(target_functions[name])
            if not target_profile.has_frame_pointer:
                counts["target_without_frame_pointer"] += 1
                continue
            counts["audited"] += 1
            categories = compare_profiles(base_profile, target_profile)
            if categories:
                findings.append(Finding(
                    unit, row["rva"], name, categories,
                    base_profile, target_profile))
                for category in categories:
                    counts[category] += 1
            else:
                counts["aligned"] += 1
    return findings, dict(counts)


def _offsets(values: tuple[int, ...]) -> str:
    return ",".join(f"-0x{value:x}" for value in values) or "-"


def _frame_hint(finding: Finding) -> str:
    base_size = finding.base.frame_size
    target_size = finding.target.frame_size
    if base_size is None or target_size is None or base_size == target_size:
        return "-"
    delta = target_size - base_size
    direction = "candidate-short" if delta > 0 else "candidate-long"
    magnitude = abs(delta)
    if magnitude % 4 == 0:
        return f"{direction}-by-{magnitude} ({magnitude // 4} dword-slot(s))"
    return f"{direction}-by-{magnitude}"


def _render(finding: Finding) -> str:
    return "\t".join([
        finding.unit,
        f"0x{finding.rva:x}",
        finding.name,
        ",".join(finding.categories),
        str(finding.base.frame_size),
        str(finding.target.frame_size),
        ",".join(finding.base.saved_registers) or "-",
        ",".join(finding.target.saved_registers) or "-",
        _offsets(finding.base.local_offsets),
        _offsets(finding.target.local_offsets),
        _frame_hint(finding),
    ])


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(
        description="Audit stack-frame evidence in all paired /Od functions.")
    parser.add_argument("--unit", help="restrict to one configured unit")
    parser.add_argument("--rva", type=lambda value: int(value, 0),
                        help="restrict to one inventory RVA")
    parser.add_argument("--output", type=Path,
                        help="write the complete mismatch TSV")
    parser.add_argument(
        "--check", action="store_true",
        help="return nonzero when mismatches exist (default is diagnostic only)")
    args = parser.parse_args(argv)

    findings, counts = audit(only_unit=args.unit, only_rva=args.rva)
    header = "\t".join([
        "unit", "rva", "name", "categories", "base_frame", "target_frame",
        "base_saved", "target_saved", "base_offsets", "target_offsets", "hint",
    ])
    lines = [header] + [_render(finding) for finding in findings]
    if args.output:
        output = args.output if args.output.is_absolute() else REPO / args.output
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text("\n".join(lines) + "\n")

    for line in lines if args.rva is not None else lines[:1]:
        print(line)
    if args.rva is None:
        print(
            "[od-frames] "
            f"audited={counts.get('audited', 0)} "
            f"aligned={counts.get('aligned', 0)} "
            f"mismatched={len(findings)} "
            f"frame-size={counts.get('frame-size', 0)} "
            f"saved-registers={counts.get('saved-registers', 0)} "
            f"local-offsets={counts.get('local-offsets', 0)} "
            f"local-reference-order={counts.get('local-reference-order', 0)}")
        if args.output:
            print(f"[od-frames] report: {output.relative_to(REPO)}")
        elif findings:
            print("[od-frames] pass --output <path> for the complete mismatch TSV")
    elif not findings:
        print(
            "[od-frames] "
            f"audited={counts.get('audited', 0)} "
            f"aligned={counts.get('aligned', 0)} "
            f"missing-pairs={counts.get('missing_symbol_pairs', 0)}")
    return int(args.check and bool(findings))


if __name__ == "__main__":
    raise SystemExit(main())

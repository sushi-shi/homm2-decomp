#!/usr/bin/env python3
"""Build a candidate-proved strict-relocation target without touching raw init output."""

import argparse
import hashlib
import shutil
import subprocess
import tempfile
from pathlib import Path

from homm2.build.reloc_alias_metadata import ALIAS_PREFIX, load_aliases


OUTPUT_MARKER = ".homm2-reloc-pdb-target"


def run(command):
    print("[reloc-target]", " ".join(str(part) for part in command), flush=True)
    subprocess.run([str(part) for part in command], check=True)


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def assert_no_metadata_symbols(output_dir):
    marker = ALIAS_PREFIX.encode("ascii")
    offenders = []
    for obj in sorted(Path(output_dir).rglob("*.obj")):
        if marker in obj.read_bytes():
            offenders.append(str(obj))
    if offenders:
        raise RuntimeError("delinker leaked PDB owner metadata into COFF: %s" %
                           ", ".join(offenders[:5]))


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--raw-dir", default="build/delink")
    parser.add_argument("--output-dir", default="build/delink-reloc")
    parser.add_argument("--base-dir", default="build/objdiff/base")
    parser.add_argument("--report", default="build/objdiff/report.json")
    parser.add_argument("--symbols", default="build/gen/symbol_names.csv")
    parser.add_argument("--aliases", default="build/gen/reloc_aliases.tsv")
    parser.add_argument("--pdb", default="build/pdb/HEROES2W.reloc.pdb")
    parser.add_argument("--exe", default="build/orig/HEROES2W.EXE")
    parser.add_argument("--delinker", default=shutil.which("vostok-delinker"))
    args = parser.parse_args(argv)
    raw_dir = Path(args.raw_dir).resolve()
    output_dir = Path(args.output_dir).resolve()
    if raw_dir == output_dir:
        raise SystemExit("raw and output directories must differ")
    if not raw_dir.is_dir():
        raise SystemExit("raw delink directory is missing: %s" % raw_dir)
    if not args.delinker:
        raise SystemExit("vostok-delinker is not on PATH")
    if output_dir.exists() and not (output_dir / OUTPUT_MARKER).is_file():
        raise SystemExit("refusing to replace unmarked output directory: %s" %
                         output_dir)

    run((
        "python3", "-m", "homm2.build.gen_reloc_aliases",
        "--base-dir", args.base_dir,
        "--target-dir", args.raw_dir,
        "--report", args.report,
        "--symbols", args.symbols,
        "--out", args.aliases,
    ))
    aliases = load_aliases(args.aliases)
    if not aliases:
        raise RuntimeError("paired proof produced no relocation aliases")
    run((
        "python3", "-m", "homm2.build.synth_pdb",
        "--exe", args.exe,
        "--csv", args.symbols,
        "--out", args.pdb,
        "--reloc-aliases", args.aliases,
    ))

    output_dir.parent.mkdir(parents=True, exist_ok=True)
    temporary = Path(tempfile.mkdtemp(
        prefix=output_dir.name + ".tmp-", dir=output_dir.parent))
    try:
        run((
            args.delinker,
            "--pdb-path", args.pdb,
            "--exe-path", args.exe,
            "--output-path", temporary,
            "--engine-path", "c:\\proj\\",
        ))
        assert_no_metadata_symbols(temporary)
        (temporary / OUTPUT_MARKER).write_text(
            "aliases_sha256=%s\npdb_sha256=%s\naliases=%d\n" %
            (digest(args.aliases), digest(args.pdb), len(aliases)))
        if output_dir.exists():
            shutil.rmtree(output_dir)
        temporary.rename(output_dir)
    except BaseException:
        shutil.rmtree(temporary, ignore_errors=True)
        raise
    print("[reloc-target] %d aliases -> %s" % (len(aliases), output_dir))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

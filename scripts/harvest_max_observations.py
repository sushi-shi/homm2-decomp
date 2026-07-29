#!/usr/bin/env python3
"""Harvest banked-MAX evidence into tracked artifacts.

Walks build/tu-state-noise/*/manifest.json for runs whose --record-max updated
config/match_baseline.tsv and preserves, under version control:

  docs/matching-matrices/max-observations.tsv
      one row per observation: when, unit, symbol, source hash, score, seed,
      trial, family, insertion, probe tag, run directory. These are the exact
      coordinates `tu_state_noise --seed ... --only-trial ...` needs to replay
      the island that produced the banked maximum.

  docs/matching-matrices/max-asm/<unit>__<symbol-hash>__<src-hash>.asm
      Intel-syntax disassembly of the winning candidate bytes (plus the raw
      hex and ordered relocation stream in the header) so the best-known
      codegen SHAPE stays diffable even after the build tree or TU state
      moves on. This is the structural reference for future source-shape work.

The ledger (config/match_baseline.tsv) stays observational: this harvester
adds evidence, never scores.
"""
import hashlib
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
NOISE = ROOT / "build" / "tu-state-noise"
OUT_TSV = ROOT / "docs" / "matching-matrices" / "max-observations.tsv"
OUT_ASM = ROOT / "docs" / "matching-matrices" / "max-asm"
HEADER = ("recorded_utc\tunit\tsymbol\tsrc_hash\tscore\tseed\ttrial\tfamily\t"
          "insertion\ttag\trun_dir\n")


def disassemble(text_hex: str) -> str:
    blob = bytes.fromhex(text_hex)
    tmp = OUT_ASM / ".tmp.bin"
    tmp.write_bytes(blob)
    try:
        out = subprocess.run(
            ["objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel", str(tmp)],
            capture_output=True, text=True)
        text = out.stdout
        cut = text.find("<.data>:")
        if cut != -1:
            text = text[text.index("\n", cut) + 1:]
        return "\n".join(ln.rstrip() for ln in text.splitlines()) + "\n"
    finally:
        tmp.unlink(missing_ok=True)


def main() -> int:
    OUT_ASM.mkdir(parents=True, exist_ok=True)
    seen = set()
    if OUT_TSV.exists():
        for line in OUT_TSV.read_text().splitlines()[1:]:
            parts = line.split("\t")
            if len(parts) >= 10:
                seen.add((parts[1], parts[2], parts[3], parts[4]))
    rows = []
    for manifest_path in sorted(NOISE.glob("*/manifest.json")):
        try:
            m = json.loads(manifest_path.read_text())
        except (json.JSONDecodeError, OSError):
            continue
        rec = m.get("record_max") or {}
        if not rec.get("updated"):
            continue
        unit, symbol = rec.get("unit", ""), rec.get("symbol", "")
        src_hash = rec.get("source_hash", "")
        score = f"{rec.get('new_max', rec.get('observed_score', 0)):.4f}"
        key = (unit, symbol, src_hash, score)
        if key in seen:
            continue
        # winning trial: exact_closure if present, else best_retained/best trial
        win = m.get("exact_closure") or (m.get("best_retained") or {})
        trial_no = win.get("trial")
        cand = None
        for t in m.get("trials") or []:
            if t.get("trial") == trial_no:
                cand = t.get("candidate") or {}
                break
        when = datetime.fromtimestamp(manifest_path.stat().st_mtime,
                                      tz=timezone.utc).isoformat(timespec="seconds")
        rows.append((when, unit, symbol, src_hash, score, str(m.get("seed")),
                     str(trial_no), win.get("family", ""), m.get("insertion", ""),
                     win.get("tag", ""), manifest_path.parent.name))
        seen.add(key)
        if cand and cand.get("text_hex"):
            sym_tag = hashlib.sha1(symbol.encode()).hexdigest()[:10]
            name = f"{unit.replace('/', '-')}__{sym_tag}__{src_hash}.asm"
            head = (f"; {symbol}\n; unit {unit}  src_hash {src_hash}  "
                    f"score {score}  seed {m.get('seed')}  trial {trial_no}\n"
                    f"; size {cand.get('size')}  text_sha {cand.get('text_sha')}\n"
                    f"; relocs {json.dumps(cand.get('reloc_stream'))[:2000]}\n"
                    f"; hex {cand.get('text_hex')}\n\n")
            (OUT_ASM / name).write_text(head + disassemble(cand["text_hex"]))
    if rows:
        if not OUT_TSV.exists():
            OUT_TSV.write_text(HEADER)
        with OUT_TSV.open("a") as f:
            for r in rows:
                f.write("\t".join(r) + "\n")
    print(f"harvested {len(rows)} new observation(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

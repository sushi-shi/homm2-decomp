"""homm2 status - run objdiff over the base<->target objs and print match %."""
import json, os, subprocess, sys
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))

def main(argv=None):
    od = REPO / "build/objdiff"; rep = od / "report.json"
    subprocess.run(["objdiff-cli", "report", "generate", "-p", str(od), "-o", str(rep)],
                   cwd=REPO, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if not rep.exists():
        print("[status] no report (run 'homm2 build' first)"); return 1
    data = json.loads(rep.read_text())
    units = data.get("units", [])
    def pct(u):  # objdiff omits matched_* entirely at 0%
        m = u.get("measures", {}) or {}
        return float(m.get("matched_code_percent", 0) or 0)
    started = sorted((u for u in units if pct(u) > 0), key=pct, reverse=True)
    for u in started[:25]:
        print(f"  {pct(u):6.2f}%  {u.get('name')}")
    overall = float((data.get("measures", {}) or {}).get("matched_code_percent", 0) or 0)
    print(f"[status] units: {len(units)}  with-progress: {len(started)}  overall: {overall:.2f}%")
    return 0

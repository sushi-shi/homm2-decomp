#!/usr/bin/env python3
"""cc_wrap.py - the `wine cl` compiler wrapper ninja's `cl` rule invokes.

ninja drives the base/recompile side natively on Linux; the compiler is VC4.2
CL.EXE under wine. This wrapper translates paths (winepath -w), sets INCLUDE to
the MSVC + repo headers, runs `wine cl <flags> /Fo<obj.w> <src.w>`, and exits
non-zero if the .obj wasn't produced (wine noise can mask cl's real exit).

Toolchain + prefix come from `nix develop .#build` (MSVC_DIR, WINEPREFIX).
Usage (emitted into build.ninja by configure.py):
    cc_wrap.py --out <obj> --src <src> -- <cl flags...>
"""
import argparse, os, shutil, signal, subprocess, sys, tempfile
from pathlib import Path
SCRIPT_DIR = Path(__file__).resolve().parent
HOMM2_DIR = next((p for p in SCRIPT_DIR.parents if (p / "flake.nix").exists()), SCRIPT_DIR)

def die(m): print(f"[cc_wrap] ERROR: {m}", file=sys.stderr); sys.exit(1)
def find_ci(d, name):
    return next((p for p in d.iterdir() if p.name.lower() == name.lower()), None) if d.is_dir() else None
def msvc_dir():
    # Honor a VALID $MSVC_DIR override (e.g. a hosted toolchain); otherwise anchor on
    # the repo's own build/toolchain. A stale/wrong MSVC_DIR or HOMM2_TOOLCHAIN
    # lingering in the env (shell entered from a subdir) thus can't break the build.
    env = os.environ.get("MSVC_DIR")
    if env and find_ci(Path(env) / "bin", "cl.exe"):
        return Path(env)
    return HOMM2_DIR / "build/toolchain/msvc"
def winepath_w(p):
    return subprocess.check_output(["winepath", "-w", str(p)], text=True, stderr=subprocess.DEVNULL).strip()
def ensure_wineserver():
    ws = shutil.which("wineserver")
    if ws: subprocess.run([ws, "-p"], check=False, stdin=subprocess.DEVNULL,
                          stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def _run_cl(cmd, out):
    timeout = float(os.environ.get("HOMM2_CL_TIMEOUT", "300"))
    with tempfile.TemporaryFile() as logf:
        proc = subprocess.Popen(cmd, cwd=str(out.parent), stdin=subprocess.DEVNULL,
                                stdout=logf, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            proc.wait(timeout=timeout); rc = proc.returncode
        except subprocess.TimeoutExpired:
            try: os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
            except (ProcessLookupError, PermissionError): pass
            proc.wait(); rc = 0 if out.exists() else 1
        logf.seek(0); return logf.read().decode("latin1", "replace"), rc

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True); ap.add_argument("--src", required=True)
    ap.add_argument("flags", nargs=argparse.REMAINDER)
    a = ap.parse_args()
    flags = a.flags[1:] if a.flags and a.flags[0] == "--" else a.flags
    msvc = msvc_dir(); cl = find_ci(msvc / "bin", "cl.exe")
    if not cl: die(f"CL.EXE not under {msvc}/bin - run inside `nix develop .#build`.")
    if shutil.which("wine") is None: die("wine not found - run inside `nix develop .#build`.")
    src = Path(a.src).resolve(); out = Path(a.out).resolve()
    if not src.exists(): die(f"source missing: {src}")
    out.parent.mkdir(parents=True, exist_ok=True)
    if out.exists(): out.unlink()
    os.environ.setdefault("WINEDEBUG", "fixme-all,err-kerberos")
    if not Path(os.environ.get("WINEPREFIX", "")).is_dir():   # same anti-stale anchor
        os.environ["WINEPREFIX"] = str(HOMM2_DIR / "build/wineprefix")
    ensure_wineserver()
    # INCLUDE = MSVC headers + repo include/ (cl reads INCLUDE for <...> system headers).
    incs = [msvc / "include"]
    if (HOMM2_DIR / "include").is_dir(): incs.append(HOMM2_DIR / "include")
    os.environ["INCLUDE"] = ";".join(winepath_w(p) for p in incs)
    cmd = ["wine", str(cl), *flags, f"/Fo{winepath_w(out)}", winepath_w(src)]
    output, rc = _run_cl(cmd, out)
    if not out.exists():
        sys.stderr.write(f"[cc_wrap] FAILED {src.name} -> {out}\n" + "\n".join(output.strip().splitlines()[-15:]) + "\n")
        sys.exit(rc or 1)
    sys.exit(0)

if __name__ == "__main__": main()

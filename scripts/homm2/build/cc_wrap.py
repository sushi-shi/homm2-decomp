#!/usr/bin/env python3
"""cc_wrap.py - the `wine cl` compiler wrapper ninja's `cl` rule invokes.

ninja drives the base/recompile side natively on Linux; the compiler is the pinned VC6
CL.EXE under wine. This wrapper translates paths (winepath -w), sets INCLUDE to
the MSVC + repo headers, runs `wine cl <flags> /Fo<obj.w> <src.w>`, and exits
non-zero if the .obj wasn't produced (wine noise can mask cl's real exit).

Toolchain + prefix come from `nix develop .#build` (MSVC_DIR, WINEPREFIX).
Usage (emitted into build.ninja by configure.py):
    cc_wrap.py --out <obj> --src <src> -- <cl flags...>
"""
import argparse, os, re, sys
from pathlib import Path

# The wine plumbing lives in homm2.core.wine; these names are re-exported
# because this module was their historical home.
from homm2.core.paths import REPO as HOMM2_DIR
from homm2.core.wine import (  # noqa: F401  (re-exported)
    ensure_wineserver,
    find_ci,
    msvc_dir,
    prepare_env,
    run_hang_proof,
    winepath_w,
)

_INC_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*[<"]([^>"]+)[>"]', re.M)

def scan_header_deps(src, inc_root):
    """Recover header deps by scanning rather than trusting cl output under wine
    (VC6 does have /showIncludes; switching to it is a possible simplification):
    recursively resolve
    every `#include` against the repo include root (+ each file's own dir for "quoted"
    includes) and return the repo headers reached. System headers (<string.h> etc.) don't
    resolve under inc_root and are skipped — they never change. Over-approximates (ignores
    #if), which is SAFE for a depfile: worst case an extra rebuild, never a stale obj."""
    src = Path(src).resolve(); inc_root = Path(inc_root)
    seen = set(); stack = [src]
    while stack:
        f = stack.pop()
        if f in seen:
            continue
        seen.add(f)
        try:
            text = f.read_text(errors="replace")
        except OSError:
            continue
        for inc in _INC_RE.findall(text):
            for cand in (f.parent / inc, inc_root / inc):
                if cand.is_file():
                    stack.append(cand.resolve()); break
    return sorted(str(p) for p in seen if p != src)

def die(m): print(f"[cc_wrap] ERROR: {m}", file=sys.stderr); sys.exit(1)

def _run_cl(cmd, out, cl_timeout=None):
    """Historical name: the hang-proof runner now lives in homm2.core.wine."""
    return run_hang_proof(cmd, out, cwd=out.parent, timeout=cl_timeout)

_COMPILE_ENV_READY = False
_CL_PATH = None
def _prepare_compile_env():
    """Resolve the toolchain, prefix, wineserver, and INCLUDE once per process."""
    global _COMPILE_ENV_READY, _CL_PATH
    if _COMPILE_ENV_READY:
        return _CL_PATH
    msvc = msvc_dir(); cl = find_ci(msvc / "bin", "cl.exe")
    if not cl: raise RuntimeError(f"CL.EXE not under {msvc}/bin - run inside `nix develop .#build`.")
    prepare_env()
    incs = [msvc / "include"]
    if (HOMM2_DIR / "include").is_dir(): incs.append(HOMM2_DIR / "include")
    if (HOMM2_DIR / "vendor").is_dir():
        incs += sorted(d for d in (HOMM2_DIR / "vendor").iterdir() if d.is_dir())
    os.environ["INCLUDE"] = ";".join(winepath_w(p) for p in incs)
    _CL_PATH = cl
    _COMPILE_ENV_READY = True
    return cl

def run_compile(src, out, flags, *, depfile=True, depfile_target=None, cl_timeout=None):
    """Compile one TU in-process; returns (rc, log, timed_out).

    Shares one resolved toolchain/INCLUDE per process, so a search engine can
    call it from worker threads without paying an interpreter, winepath, or
    wineserver spawn per trial. `depfile=False` skips the ninja header-dep
    emission for disposable probe objects.
    """
    cl = _prepare_compile_env()
    src = Path(src).resolve(); out = Path(out).resolve()
    if not src.exists():
        return 1, f"source missing: {src}\n", False
    out.parent.mkdir(parents=True, exist_ok=True)
    if out.exists(): out.unlink()
    cmd = ["wine", str(cl), *flags, f"/Fo{winepath_w(out)}", winepath_w(src)]
    output, rc, timed_out = _run_cl(cmd, out, cl_timeout)
    if not out.exists():
        return (rc or 1), output, timed_out
    if depfile:
        deps = scan_header_deps(src, HOMM2_DIR / "include")
        dep_list = " ".join(d.replace(" ", "\\ ") for d in deps)
        Path(str(out) + ".d").write_text(f"{depfile_target or out}: {dep_list}\n")
    return 0, output, timed_out

def main():
    # INCLUDE = MSVC headers + repo include/ + vendored middleware SDK headers (cl reads
    # INCLUDE for <...> system headers). Each vendor/<sdk>/ dir (e.g. vendor/miles-3.6,
    # vendor/smacker-3.0g, vendor/wing-1.0) is added like the original toolchain's SDK
    # dirs so `#include <mss.h>` / `<smack.h>` / `<wing.h>` resolves; these are the
    # closed middleware DLLs HMM2PL.exe imports (mss32/smackw32/WING32) — headers only,
    # never built as units. See docs/vendor-middleware.md. The depfile target must be
    # the obj path AS ninja passed it (a.out); `deps=gcc` in build.ninja consumes it.
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True); ap.add_argument("--src", required=True)
    ap.add_argument("flags", nargs=argparse.REMAINDER)
    a = ap.parse_args()
    flags = a.flags[1:] if a.flags and a.flags[0] == "--" else a.flags
    src = Path(a.src); out = Path(a.out).resolve()
    try:
        rc, output, _timed_out = run_compile(
            src, out, flags, depfile=True, depfile_target=a.out)
    except RuntimeError as exc:
        die(str(exc))
    if rc:
        sys.stderr.write(f"[cc_wrap] FAILED {src.name} -> {out}\n" + "\n".join(output.strip().splitlines()[-15:]) + "\n")
        sys.exit(rc)
    sys.exit(0)

if __name__ == "__main__": main()

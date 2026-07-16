#!/usr/bin/env python3
"""clangd.py - generate a clangd compilation database for the HoMM2 tree.

Invoke via `homm2 clangd`. ADDITIVE editor tooling (go-to-def / completion /
hover / diagnostics in nvim) that runs ALONGSIDE the real matching build; it does
not touch it. The matching build compiles with MSVC 4.2's CL.EXE under wine
(scripts/homm2/build/cc_wrap.py), which clangd cannot invoke. So we emit our OWN
compilation database, in clang-cl form, that points clang at the toolchain's MSVC
headers and asks it to *emulate* MSVC 4.2 (cl 10.20 == _MSC_VER 1020). clang reads
those headers as plain files - no wine, parse-only, never a real compile.

Output: build/clangd/compile_commands.json   (git-ignored). The committed .clangd
points clangd here via CompileFlags.CompilationDatabase. Each [[unit]] in
config/units.toml gets one entry; the entry supplies `/I include` (so `<va.h>` /
`<EDITOR/fullMap.h>` resolve) and `/imsvc` for the MSVC system headers.

Run inside `nix develop .#build` (sets MSVC_DIR), or with MSVC_DIR pointed at the
provisioned toolchain. Re-run after adding units / headers.
"""
import json, os, shutil, sys, tomllib
from pathlib import Path

REPO = next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
            Path(__file__).resolve().parents[3])
MANIFEST = REPO / "config" / "units.toml"
OUT_DIR = REPO / "build" / "clangd"
OUT_FILE = OUT_DIR / "compile_commands.json"
MIRROR_DIR = OUT_DIR / "inc-lower"          # lowercase mirror of UPPERCASE MSVC headers

MSC_COMPAT = "10.20"                          # MSVC 4.2 == cl 10.20 == _MSC_VER 1020
TARGET = "i386-pc-windows-msvc"
DEFINES = ["/D_X86_", "/DWIN32", "/D_WINDOWS", "/D_MT"]   # 32-bit Win32 app, static CRT (/MT)


def resolve_msvc_include():
    msvc_dir = os.environ.get("MSVC_DIR")
    if msvc_dir and (Path(msvc_dir) / "include").is_dir():
        return Path(msvc_dir) / "include"
    cand = REPO / "build/toolchain/msvc/include"
    if cand.is_dir():
        return cand
    raise SystemExit("[clangd] MSVC headers not found - set MSVC_DIR (run inside "
                     "`nix develop .#build`) or provision build/toolchain/msvc/.")


def build_lowercase_mirror(real: Path, mirror: Path) -> Path:
    """Recursive lowercase-symlink mirror of `real`, so `<string.h>` resolves onto
    the on-disk `STRING.H`. Rebuilt only when `real` changes (marker-guarded)."""
    marker = mirror.parent / (mirror.name + ".src")
    if mirror.is_dir() and marker.is_file() and marker.read_text() == str(real):
        return mirror
    if mirror.exists():
        shutil.rmtree(mirror)
    for root, _dirs, files in os.walk(real):
        rel = os.path.relpath(root, real)
        low = mirror if rel == "." else mirror / rel.lower()
        low.mkdir(parents=True, exist_ok=True)
        for fn in files:
            link = low / fn.lower()
            if not link.exists():
                link.symlink_to(os.path.join(root, fn))
    marker.parent.mkdir(parents=True, exist_ok=True)
    marker.write_text(str(real))
    return mirror


def project_include_flags(repo: Path) -> list[str]:
    flags = ["/I", str(repo / "include")]
    vendor = repo / "vendor"
    if vendor.is_dir():
        for include in sorted(path for path in vendor.iterdir() if path.is_dir()):
            flags.extend(("/I", str(include)))
    return flags


def base_flags(msvc_inc: Path, msvc_low: Path):
    """clang-cl flags shared by every unit. `/imsvc` = system header (diagnostics
    inside MSVC headers are silenced); `/I include` keeps OUR headers non-system so
    real diagnostics in src/ still surface, and resolves `<va.h>`."""
    return [
        f"--target={TARGET}",
        f"-fms-compatibility-version={MSC_COMPAT}",
        "-fms-extensions",
        "-fdelayed-template-parsing",
        "/imsvc", str(msvc_low),     # lowercase mirror first (<string.h>)
        "/imsvc", str(msvc_inc),     # then exact-case (<STRING.H>)
        *project_include_flags(REPO),
        *DEFINES,
    ]


def main():
    msvc_inc = resolve_msvc_include()
    msvc_low = build_lowercase_mirror(msvc_inc, MIRROR_DIR / "msvc")
    units = tomllib.loads(MANIFEST.read_text()).get("unit", [])
    if not units:
        raise SystemExit(f"{MANIFEST}: no [[unit]] entries (run `homm2 init`/`configure`)")
    shared = base_flags(msvc_inc, msvc_low)

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    entries = [{"directory": str(REPO), "file": u["source"],
                "arguments": ["clang-cl", "/c", u["source"], *shared]} for u in units]
    OUT_FILE.write_text(json.dumps(entries, indent=2) + "\n")
    print(f"[clangd] wrote {OUT_FILE.relative_to(REPO)} ({len(entries)} units)")
    print(f"[clangd] MSVC headers: {msvc_inc}  (+ lowercase mirror {MIRROR_DIR}/msvc)")
    print(f"[clangd] our headers : {REPO / 'include'}  (resolves <va.h>)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

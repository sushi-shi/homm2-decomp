#!/usr/bin/env python3
"""homm2.core.wine - the era-toolchain wine plumbing, in one place.

Everything CL/LINK/LIB/RC/ML need to run under wine: toolchain lookup, path
translation, the persistent wineserver, the environment doctrine, and the two
runners. Callers above this module never spell "wine".

The doctrine, stated once:
  * ``WINEDEBUG`` defaults to ``fixme-all,err-kerberos``;
  * ``WINEPREFIX`` is anchored to ``build/wineprefix`` unless the environment
    names an existing directory (a stale prefix from another checkout must not
    win);
  * every child runs with ``TZ=UTC0`` - the historical LINK timestamps are
    UTC strings fed through faketime, and an ambient timezone once shifted
    the PE/PDB stamps with the machine;
  * hang-proof running kills the whole process group on timeout (a wedged
    wine CL otherwise survives its parent and starves later compiles).
"""

from __future__ import annotations

import os
import shutil
import signal
import subprocess
import tempfile
from pathlib import Path

from homm2.core.paths import REPO


def find_ci(directory: Path, name: str) -> Path | None:
    """Case-insensitive child lookup (the toolchain mixes CL.EXE / cl.exe)."""
    if not directory.is_dir():
        return None
    low = name.lower()
    return next((p for p in directory.iterdir() if p.name.lower() == low), None)


def msvc_dir() -> Path:
    """The pinned toolchain root, ignoring a stale $MSVC_DIR override."""
    env = os.environ.get("MSVC_DIR")
    if env and find_ci(Path(env) / "bin", "cl.exe"):
        return Path(env)
    return REPO / "build/toolchain/msvc"


def tool(name: str) -> Path:
    """The era tool `name` under the toolchain's bin, or a clear error."""
    path = find_ci(msvc_dir() / "bin", name)
    if path is None:
        raise RuntimeError(
            f"{name} not under {msvc_dir()}/bin - run inside `nix develop .#build`")
    return path


_Z_DRIVE_IS_ROOT: bool | None = None


def winepath_w(path) -> str:
    """Windows spelling of a path without spawning a wine client.

    The pinned prefixes map the whole filesystem as the standard ``z:``
    dosdevice, so translation is the pure ``Z:`` + backslash rewrite (exactly
    what ``winepath -w`` prints for these paths). The symlink is verified once
    per process; an exotic prefix without a root ``z:`` falls back to real
    winepath. Relative paths are resolved against the working directory first,
    matching ``winepath -w``.
    """
    global _Z_DRIVE_IS_ROOT
    if _Z_DRIVE_IS_ROOT is None:
        prefix = Path(os.environ.get("WINEPREFIX", ""))
        try:
            _Z_DRIVE_IS_ROOT = os.readlink(prefix / "dosdevices/z:") == "/"
        except OSError:
            _Z_DRIVE_IS_ROOT = False
    text = str(path)
    if not text.startswith("/"):
        text = str(Path(text).resolve())
    if _Z_DRIVE_IS_ROOT:
        return "Z:" + text.replace("/", "\\")
    return subprocess.check_output(
        ["winepath", "-w", text], text=True, stderr=subprocess.DEVNULL).strip()


_WINESERVER_ENSURED = False


def ensure_wineserver() -> None:
    global _WINESERVER_ENSURED
    if _WINESERVER_ENSURED:
        return
    ws = shutil.which("wineserver")
    if ws:
        subprocess.run([ws, "-p"], check=False, stdin=subprocess.DEVNULL,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    _WINESERVER_ENSURED = True


_ENV_READY = False


def prepare_env() -> None:
    """Apply the doctrine to this process once: wine presence, WINEDEBUG,
    the WINEPREFIX anchor, and the persistent wineserver."""
    global _ENV_READY
    if _ENV_READY:
        return
    if shutil.which("wine") is None:
        raise RuntimeError("wine not found - run inside `nix develop .#build`")
    os.environ.setdefault("WINEDEBUG", "fixme-all,err-kerberos")
    if not Path(os.environ.get("WINEPREFIX", "")).is_dir():
        os.environ["WINEPREFIX"] = str(REPO / "build/wineprefix")
    ensure_wineserver()
    _ENV_READY = True


def child_env(extra: dict[str, str] | None = None) -> dict[str, str]:
    """The doctrine environment for one wine child (TZ pinned to UTC0)."""
    env = {**os.environ, "TZ": "UTC0"}
    if extra:
        env.update(extra)
    return env


def run_hang_proof(command: list[str], produces: Path, *, cwd: Path,
                   timeout: float | None = None) -> tuple[str, int, bool]:
    """Run one wine child in its own session; SIGKILL the group on timeout.

    Returns (log, returncode, timed_out); after a timeout the return code is
    judged by whether `produces` exists, because wine noise can mask the real
    exit.
    """
    limit = timeout if timeout is not None else float(
        os.environ.get("HOMM2_CL_TIMEOUT", "300"))
    timed_out = False
    with tempfile.TemporaryFile() as logf:
        proc = subprocess.Popen(
            command, cwd=str(cwd), env=child_env(), stdin=subprocess.DEVNULL,
            stdout=logf, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            proc.wait(timeout=limit)
            rc = proc.returncode
        except subprocess.TimeoutExpired:
            timed_out = True
            try:
                os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
            except (ProcessLookupError, PermissionError):
                pass
            proc.wait()
            rc = 0 if produces.exists() else 1
        logf.seek(0)
        return logf.read().decode("latin1", "replace"), rc, timed_out


def run(program: Path | str, *args: str, cwd: Path | None = None,
        faketime_spec: str | None = None, log: Path | None = None,
        quiet: bool = False) -> str:
    """Run one era tool under wine; raise on failure.

    `faketime_spec` wraps the invocation in ``faketime -f <spec>`` (the
    historical-PDB link path). Output goes to `log` when given, to stdout
    otherwise (suppressed when `quiet`). Returns the captured output.
    """
    prepare_env()
    command = ["wine", str(program), *args]
    if faketime_spec is not None:
        faketime = shutil.which("faketime")
        if faketime is None:
            raise RuntimeError("faketime is required; enter `nix develop .#build`")
        command = [faketime, "-f", faketime_spec, *command]
    completed = subprocess.run(
        command, cwd=None if cwd is None else str(cwd), text=True,
        env=child_env(), stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if log is not None:
        log.write_text(completed.stdout)
    elif completed.stdout and not quiet:
        print(completed.stdout, end="")
    if completed.returncode:
        tail = "\n".join(completed.stdout.strip().splitlines()[-15:])
        raise RuntimeError(
            f"command failed ({completed.returncode}): {' '.join(command)}\n{tail}")
    return completed.stdout

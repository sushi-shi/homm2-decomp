"""Append-only usage events shared by every tooling entry point.

Logging is best effort and never changes a command's result. Start events survive
crashes/termination; only a matching finish event establishes completion.
"""
from __future__ import annotations

from contextvars import ContextVar
from datetime import datetime, timezone
from functools import wraps
from pathlib import Path
import contextlib
import re
import subprocess
import threading
import traceback
import fcntl
import json
import os
import shlex
import sys
import time
import uuid

TAIL = 16384


class _Tee:
    """Pass writes through to `stream` and keep the last TAIL characters."""

    def __init__(self, stream):
        self.stream = stream
        self.tail = ""

    def write(self, text):
        self.tail = (self.tail + text)[-TAIL:]
        try:
            return self.stream.write(text)
        except BrokenPipeError:
            return len(text)

    def flush(self):
        with contextlib.suppress(BrokenPipeError):
            self.stream.flush()

    def __getattr__(self, name):
        return getattr(self.stream, name)


def classify_error(text: str) -> str:
    """Best-effort category from diagnostic text; never inferred from rc alone."""
    lower = text.lower()
    if any(s in lower for s in ("operation not permitted", "permission denied")):
        return "environment.permission"
    if any(s in lower for s in ("winepath", "wineserver:", "wine: could not",
                                "wine: failed", "wineboot")):
        return "environment.wine"
    if re.search(r"\b(?:fatal )?error C\d{4}\b", text) or "produced no object" in lower:
        return "compile.cpp"
    if "no report" in lower and "homm2 build" in lower:
        return "build.stale"
    if "unknown target" in lower:
        return "build.target"
    if "traceback (most recent call last)" in lower:
        return "tool.exception"
    if any(s in lower for s in ("unrecognized arguments", "invalid choice",
                                "usage:", "unknown command", "unknown verb",
                                "expected one argument", "is not a unit")):
        return "cli.arguments"
    if "regression" in lower or ": fail" in lower:
        return "gate.failed"
    return "command.failed"


def run_process(argv: list[str], *, cwd: Path) -> int:
    """Run a child with both pipes streamed through the current sys streams,
    so a logged invocation sees the child's diagnostics."""
    with subprocess.Popen(argv, cwd=cwd, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, text=True,
                          errors="replace") as process:
        def forward(pipe, stream):
            try:
                for line in pipe:
                    with contextlib.suppress(BrokenPipeError):
                        stream.write(line)
                        stream.flush()
            finally:
                pipe.close()
        threads = [threading.Thread(target=forward, args=pair, daemon=True)
                   for pair in ((process.stdout, sys.stdout),
                                (process.stderr, sys.stderr))]
        for thread in threads:
            thread.start()
        try:
            rc = process.wait()
        finally:
            if process.poll() is None:
                process.terminate()
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.wait()
            for thread in threads:
                thread.join()
        return rc



_active: ContextVar[str | None] = ContextVar("homm2_usage", default=None)
_installed = False
_warned = False


def _write(event: dict) -> None:
    global _warned
    try:
        from homm2.core.paths import REPO
        path = REPO / "build" / "homm2_usage.jsonl"
        path.parent.mkdir(parents=True, exist_ok=True)
        row = {"schema": 1, "time": datetime.now(timezone.utc).isoformat(),
               "pid": os.getpid(), "ppid": os.getppid(), **event}
        line = json.dumps(row, ensure_ascii=True) + "\n"
        with path.open("a", encoding="utf-8") as handle:
            fcntl.flock(handle, fcntl.LOCK_EX)
            handle.write(line)
            handle.flush()
    except Exception as exc:
        if not _warned:
            _warned = True
            try:
                print(f"[usage] cannot append usage log: {exc}", file=sys.stderr)
            except Exception:
                pass


def _text_completion(command: str, rc: int) -> None:
    """Copyable completion log alongside the structured entry-point events."""
    try:
        from homm2.core.paths import REPO
        now = datetime.now(timezone.utc)
        with (REPO / "build" / "homm2_usage.log").open("a", encoding="utf-8") as handle:
            fcntl.flock(handle, fcntl.LOCK_EX)
            handle.write(f"[{now:%Y-%m-%d}][{now:%H:%M:%S}][{rc}]: {command}\n")
    except Exception:
        pass


def _audit(event: str, args: tuple) -> None:
    # Python emits this for run/check_output/Popen alike. Do not log the
    # environment (which may contain credentials), stdin or file contents.
    if event != "subprocess.Popen" or _active.get() is None:
        return
    try:
        executable, argv, cwd, _env = args
        _write({"event": "subprocess", "parent_id": _active.get(),
                "executable": os.fsdecode(executable),
                "argv": ([os.fsdecode(a) for a in argv]
                         if isinstance(argv, (list, tuple)) else os.fsdecode(argv)),
                "cwd": os.fsdecode(cwd) if cwd is not None else os.getcwd()})
    except Exception:
        pass  # audit hooks must never prevent a subprocess from launching


def logged(fn):
    """Record a main(argv=None) entry, including nested and batch invocations."""
    spec = fn.__globals__.get("__spec__")
    module = spec.name if spec is not None else fn.__module__

    @wraps(fn)
    def invoke(*args, **kwargs):
        global _installed
        if not _installed:
            sys.addaudithook(_audit)
            _installed = True
        argv = args[0] if args else kwargs.get("argv")
        argv = list(sys.argv[1:] if argv is None else argv)
        prefix = ["homm2"] if module == "homm2.cli" else ["python3", "-m", module]
        invocation = uuid.uuid4().hex
        parent = _active.get()
        started = time.monotonic()
        _write({"event": "start", "id": invocation, "parent_id": parent,
                "module": module, "argv": argv, "cwd": os.getcwd(),
                "command": shlex.join([*prefix, *argv])})
        token = _active.set(invocation)
        stdout, stderr = _Tee(sys.stdout), _Tee(sys.stderr)
        query = (module == "homm2.cli" and argv and argv[0] in {"sema", "walls"}) or module.startswith(("homm2.sema", "homm2.walls"))
        failure_rc = 2 if query else 1
        rc, error = 1, None
        diagnostic = None
        try:
            with contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
                result = fn(*args, **kwargs)
            rc = 0 if result is None else int(result)
            return result
        except BaseException as exc:
            error = {"type": type(exc).__name__, "message": str(exc)}
            if not isinstance(exc, SystemExit):
                diagnostic = "".join(traceback.format_exception(type(exc), exc, exc.__traceback__))
            elif isinstance(exc.code, str):
                diagnostic = exc.code
            if isinstance(exc, SystemExit):
                rc = (0 if exc.code is None else exc.code
                      if isinstance(exc.code, int) else 1)
            elif isinstance(exc, KeyboardInterrupt):
                rc = 130
            raise
        finally:
            _active.reset(token)
            diagnostic = (diagnostic or "\n".join(t for t in (stderr.tail.strip(), stdout.tail.strip()) if t))[-TAIL:]
            failed = rc < 0 or rc >= failure_rc or (error is not None and error["type"] != "SystemExit")
            category = ("interrupted" if rc == 130 else "process.signal" if rc < 0
                        else classify_error(diagnostic)) if failed else None
            _write({"event": "finish", "id": invocation, "parent_id": parent,
                    "module": module, "exit_code": rc,
                    "duration_s": round(time.monotonic() - started, 6),
                    "error": error, "outcome": "error" if failed else "difference" if rc else "success",
                    "error_category": category,
                    "diagnostic": diagnostic if failed else None})
            _text_completion(shlex.join([*prefix, *argv]), rc)
    return invoke

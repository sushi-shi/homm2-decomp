#!/usr/bin/env python3
"""One-shot clangd queries over the generated compile_commands.json - source
navigation for humans and agents without an editor: where is this defined,
who references it, what is its type.

  python3 -m homm2.analysis.clangd_query symbol <fuzzy-name>    # workspace symbols
  python3 -m homm2.analysis.clangd_query def  <file> <line> [<col>]   # definition
  python3 -m homm2.analysis.clangd_query refs <file> <line> [<col>]   # references
  python3 -m homm2.analysis.clangd_query hover <file> <line> [<col>]  # type/doc at point
  python3 -m homm2.analysis.clangd_query rename <file> <line> [<col>] <new-name>
                                                                # rename the symbol at
                                                                # point tree-wide (add
                                                                # --dry-run to preview)
  python3 -m homm2.analysis.clangd_query index                 # build the background
                                                                # index, wait until done

Lines/columns are 1-based (as printed by grep/editors). When <col> is omitted,
identifiers on the line are probed right-to-left (the declared name sits
rightmost on C++ declaration lines) until one yields a result.

`rename` drives clangd's `textDocument/rename`: it returns a WorkspaceEdit -
the declaration plus EVERY reference in every TU (keyed on the symbol's USR, so
a same-named field of a different struct is NOT touched) - which the tool applies
bottom-to-top per file (only OUR src/**, include/** files; SDK edits are skipped).
Cross-file completeness needs clangd's background index, so `rename` waits for it
to settle first.

Cross-TU answers (symbol, refs into other TUs) come from clangd's background
index, built incrementally under build/clangd/.cache/ on first use - early runs may
return partial results until it fills (the tool prints indexing progress).
Definition/hover on an open file need no index. clang is a READER of this MSVC 4.2
dialect (it parses our reconstructed src/ against the recovered headers via
build/clangd/compile_commands.json): navigation is reliable, its diagnostics are
NOT build truth - the wine `cl` build + objdiff are. Regenerate the compdb with
`homm2 clangd` after adding a unit.
"""

import argparse
import json
import os
import re
import select
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
CDB_DIR = ROOT / "build" / "clangd"
CDB = CDB_DIR / "compile_commands.json"


class Clangd:
    def __init__(self):
        self.proc = subprocess.Popen(
            ["clangd", "--background-index", "--log=error",
             "--header-insertion=never",  # never let `rename` smuggle in an #include
             f"--compile-commands-dir={CDB_DIR}"],
            cwd=ROOT, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
        self._id = 0
        self._request("initialize", {
            "processId": os.getpid(),
            "rootUri": ROOT.as_uri(),
            "capabilities": {
                "window": {"workDoneProgress": True},
                "workspace": {"workspaceEdit": {"documentChanges": True}},
                "textDocument": {
                    "documentSymbol": {"hierarchicalDocumentSymbolSupport": True},
                    "rename": {"prepareSupport": True},
                },
            },
        })
        self._notify("initialized", {})

    def _send(self, payload: dict) -> None:
        body = json.dumps({"jsonrpc": "2.0", **payload}).encode()
        self.proc.stdin.write(f"Content-Length: {len(body)}\r\n\r\n".encode() + body)
        self.proc.stdin.flush()

    def _recv(self) -> dict:
        headers = b""
        while not headers.endswith(b"\r\n\r\n"):
            headers += self.proc.stdout.read(1)
        length = int(re.search(rb"Content-Length: (\d+)", headers).group(1))
        return json.loads(self.proc.stdout.read(length))

    def _notify(self, method: str, params: dict) -> None:
        self._send({"method": method, "params": params})

    def _request(self, method: str, params: dict, timeout: float = 120.0,
                 tolerant: bool = False):
        """`tolerant` returns None on a clangd error instead of exiting - used by
        `rename` so probing a non-symbol position just moves on to the next candidate."""
        self._id += 1
        self._send({"id": self._id, "method": method, "params": params})
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            msg = self._recv()
            if msg.get("method") == "window/workDoneProgress/create":
                self._send({"id": msg["id"], "result": None})
                continue
            if msg.get("id") == self._id:
                if "error" in msg:
                    if tolerant:
                        return None
                    sys.exit(f"clangd error: {msg['error'].get('message')}")
                return msg.get("result")
        sys.exit(f"clangd: no reply to {method} within {timeout}s")

    def wait_for_index(self, stream=None) -> None:
        """Block until background indexing reports 'end' (or stays idle). Progress goes to
        `stream` (default stdout; `rename` passes stderr to keep the edit listing clean)."""
        stream = stream or sys.stdout
        begun = False
        last = time.monotonic()
        while True:
            ready, _, _ = select.select([self.proc.stdout], [], [], 5.0)
            if not ready:
                if time.monotonic() - last > (30.0 if begun else 90.0):
                    print("index: idle - nothing (left) to index" if not begun
                          else "index: no progress for 30s - assuming done", file=stream)
                    return
                continue
            msg = self._recv()
            last = time.monotonic()
            if msg.get("method") == "window/workDoneProgress/create":
                self._send({"id": msg["id"], "result": None})
                continue
            if msg.get("method") != "$/progress":
                continue
            value = msg["params"]["value"]
            if value.get("kind") == "begin":
                begun = True
            elif value.get("kind") == "report":
                pct = value.get("percentage")
                msg_txt = value.get("message", "")
                print(f"\rindex: {pct}% {msg_txt[:60]:<60}", end="", flush=True, file=stream)
            elif value.get("kind") == "end" and begun:
                print("\nindex: done", file=stream)
                return

    def open_file(self, path: Path) -> None:
        self._notify("textDocument/didOpen", {"textDocument": {
            "uri": path.as_uri(), "languageId": "cpp", "version": 1,
            "text": path.read_text(errors="replace"),
        }})

    def rename(self, path: Path, line: int, char: int, new_name: str):
        """0-based line/char. Returns a WorkspaceEdit (decl + all refs) or None (tolerant)."""
        return self._request("textDocument/rename", {
            "textDocument": {"uri": path.as_uri()},
            "position": {"line": line, "character": char},
            "newName": new_name,
        }, tolerant=True)

    def close(self) -> None:
        try:
            self._request("shutdown", {}, timeout=5)
            self._notify("exit", {})
        except Exception:  # best-effort teardown of a one-shot
            pass
        self.proc.terminate()


def rel(uri_or_path: str) -> str:
    p = uri_or_path.removeprefix("file://")
    try:
        return str(Path(p).relative_to(ROOT))
    except ValueError:
        return p


def fmt_location(loc: dict) -> str:
    r = loc["range"]["start"]
    return f"{rel(loc['uri'])}:{r['line'] + 1}:{r['character'] + 1}"


def positions_to_probe(path: Path, line: int, col):
    """1-based -> 0-based; without a column, probe each identifier on the line."""
    if col is not None:
        yield line - 1, col - 1
        return
    text = path.read_text(errors="replace").splitlines()[line - 1]
    for m in reversed(list(re.finditer(r"[A-Za-z_][A-Za-z0-9_]*", text))):
        yield line - 1, m.start()


def at_point(method: str, path: Path, line: int, col, lsp: Clangd):
    lsp.open_file(path)
    for ln, ch in positions_to_probe(path, line, col):
        result = lsp._request(method, {
            "textDocument": {"uri": path.as_uri()},
            "position": {"line": ln, "character": ch},
            **({"context": {"includeDeclaration": True}}
               if method == "textDocument/references" else {}),
        })
        if result:
            return result
    return None


def _first_compdb_file() -> Path:
    entry = json.loads(CDB.read_text())[0]
    p = Path(entry["file"])
    if not p.is_absolute():
        p = (Path(entry.get("directory", ROOT)) / p)
    return p


# --- rename: WorkspaceEdit normalization + application ----------------------
def _uri_to_path(uri: str) -> Path:
    return Path(uri.removeprefix("file://"))


def _normalize_edits(ws_edit) -> dict:
    """WorkspaceEdit -> {Path: [TextEdit,...]} handling both the `changes` and the
    `documentChanges` response shapes clangd may return."""
    out = {}
    if not ws_edit:
        return out
    for uri, edits in (ws_edit.get("changes") or {}).items():
        out.setdefault(_uri_to_path(uri), []).extend(edits)
    for dc in ws_edit.get("documentChanges") or []:
        out.setdefault(_uri_to_path(dc["textDocument"]["uri"]), []).extend(dc.get("edits", []))
    return out


def _line_starts(text: str) -> list:
    starts = [0]
    for i, ch in enumerate(text):
        if ch == "\n":
            starts.append(i + 1)
    return starts


def _is_ours(path: Path) -> bool:
    """Only OUR reconstructed sources are writable; recovered SDK headers aren't."""
    try:
        parts = path.resolve().relative_to(ROOT).parts
    except ValueError:
        return False
    return bool(parts) and parts[0] in ("src", "include") and "vendor" not in parts


def do_rename(lsp: Clangd, path: Path, line: int, col, new_name: str, dry_run: bool) -> int:
    """Rename the symbol at (1-based line[/col]) to `new_name` across the tree. --dry-run
    prints the edit set (file:line: old -> new) and writes nothing; otherwise applies
    edits bottom-to-top per file and prints per-file counts."""
    lsp.open_file(path)
    lsp.wait_for_index(stream=sys.stderr)
    ws = None
    for ln, ch in positions_to_probe(path, line, col):
        ws = lsp.rename(path, ln, ch, new_name)
        if _normalize_edits(ws):
            break
    edits = _normalize_edits(ws)
    if not edits:
        print("(no rename result - not a renameable identifier at that position, "
              "or the index is still cold)")
        return 1

    foreign = [p for p in edits if not _is_ours(p)]
    total_files = total_sites = 0
    for path_i, tedits in sorted(edits.items(), key=lambda kv: str(kv[0])):
        if not _is_ours(path_i):
            continue
        content = path_i.read_text()
        starts = _line_starts(content)
        resolved = []
        for e in tedits:
            r = e["range"]
            s = starts[r["start"]["line"]] + r["start"]["character"]
            t = starts[r["end"]["line"]] + r["end"]["character"]
            resolved.append((s, t, e["newText"], r["start"]["line"]))
        resolved.sort()
        total_files += 1
        total_sites += len(resolved)
        if dry_run:
            for s, t, new_text, ln0 in resolved:
                print(f"  {rel(str(path_i))}:{ln0 + 1}: {content[s:t]} -> {new_text}")
        else:
            for s, t, new_text, _ in reversed(resolved):  # bottom-up keeps offsets valid
                content = content[:s] + new_text + content[t:]
            path_i.write_text(content)
            print(f"  {rel(str(path_i))}: {len(resolved)} edit(s)")

    verb = "would change" if dry_run else "changed"
    print(f"{verb} {total_sites} site(s) across {total_files} file(s).")
    for p in sorted(foreign, key=str):
        print(f"  (skipped foreign file: {rel(str(p))})", file=sys.stderr)
    return 0


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("index")
    sub.add_parser("symbol").add_argument("query")
    for name in ("def", "refs", "hover"):
        p = sub.add_parser(name)
        p.add_argument("file")
        p.add_argument("line", type=int)
        p.add_argument("col", type=int, nargs="?")
    r = sub.add_parser("rename")
    r.add_argument("file")
    r.add_argument("line", type=int)
    r.add_argument("col", type=int, nargs="?")  # optional; new_name is the required tail
    r.add_argument("new_name")
    r.add_argument("--dry-run", action="store_true",
                   help="preview the edit set (file:line: old -> new); write nothing")
    args = ap.parse_args()

    if not CDB.is_file():
        sys.exit("[clangd-query] build/clangd/compile_commands.json not found - "
                 "run `homm2 clangd` (inside `nix develop`)")

    lsp = Clangd()
    try:
        if args.cmd == "index":
            lsp.open_file(_first_compdb_file())
            lsp.wait_for_index()
            return

        if args.cmd == "symbol":
            lsp.open_file(_first_compdb_file())
            for _ in range(10):
                syms = lsp._request("workspace/symbol", {"query": args.query}) or []
                if syms:
                    break
                time.sleep(2)
            for s in syms:
                container = s.get("containerName") or ""
                print(f"{container}{'::' if container else ''}{s['name']}  "
                      f"{fmt_location(s['location'])}")
            if not syms:
                print("(no symbols - background index may still be building)")
            return

        path = (ROOT / args.file).resolve()
        if not path.is_file():
            sys.exit(f"no such file: {args.file}")

        if args.cmd == "rename":
            return do_rename(lsp, path, args.line, args.col, args.new_name, args.dry_run)

        method = {"def": "textDocument/definition",
                  "refs": "textDocument/references",
                  "hover": "textDocument/hover"}[args.cmd]
        result = at_point(method, path, args.line, args.col, lsp)
        if not result:
            print("(no result)")
        elif args.cmd == "hover":
            print(result["contents"]["value"])
        else:
            for loc in result if isinstance(result, list) else [result]:
                print(fmt_location(loc))
    finally:
        lsp.close()


if __name__ == "__main__":
    sys.exit(main())

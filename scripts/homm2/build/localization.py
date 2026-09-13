"""Static, byte-preserving Buka localization using master's IDs and PO format.

Authored Tr("semantic.id") expressions become literal macros, never runtime calls.
The compiler view preserves every source byte offset and newline: Clang's source
annotations and AST-based tools still point into the authored file. Only generated
files contain CP1251 byte escapes. Both Clang and VC6 see the same literal macros.
"""
from __future__ import annotations

import argparse
import ast
import hashlib
import json
import os
import re
import tempfile
from dataclasses import dataclass
from pathlib import Path


# One lexer shared by the renderer and the no-hidden-text gate. Comments, quoted
# backslashes and character literals must not be mistaken for localization calls.
TOKEN = re.compile(
    r'(?P<space>\s+)|(?P<comment>//[^\n]*|/\*.*?\*/)|'
    r'(?P<string>(?:u8|u|U|L)?"(?:\\.|[^"\\])*")|'
    r"(?P<char>(?:u|U|L)?'(?:\\.|[^'\\])*')|"
    r'(?P<identifier>[A-Za-z_]\w*)|(?P<punct>::|.)', re.S)
ID = re.compile(r'[a-z][a-z0-9_]*(?:\.[A-Za-z0-9_]+)+\Z')
NUMERIC_ESCAPE = re.compile(r'\\(?:x[0-9a-fA-F]+|[0-7]{1,3}|u[0-9a-fA-F]{4}|U[0-9a-fA-F]{8})')


def tokens(text):
    return [m for m in TOKEN.finditer(text) if m.lastgroup not in ('space', 'comment')]


def quoted(value):
    return json.dumps(value, ensure_ascii=False)


def literal(value: str) -> str:
    """ASCII C literal encoding exact Windows-1251 bytes (fixed-width octal)."""
    out = []
    for byte in value.encode('cp1251'):
        if byte in (34, 92):
            out.append('\\' + chr(byte))
        elif 32 <= byte < 127:
            out.append(chr(byte))
        else:
            out.append('\\%03o' % byte)
    return '"' + ''.join(out) + '"'


def parse_registry(text):
    ts = tokens(text)
    entries = {}
    i = 0
    while i < len(ts):
        if ts[i].group() != 'HOMM2_MESSAGE' or i + 4 >= len(ts):
            raise ValueError('expected HOMM2_MESSAGE(id, English text)')
        if ts[i + 1].group() != '(' or ts[i + 3].group() != ',':
            raise ValueError('malformed HOMM2_MESSAGE')
        key = ast.literal_eval(ts[i + 2].group())
        i += 4
        parts = []
        while i < len(ts) and ts[i].lastgroup == 'string':
            parts.append(ast.literal_eval(ts[i].group()))
            i += 1
        if not parts or i >= len(ts) or ts[i].group() != ')':
            raise ValueError(f'{key}: expected English string literal')
        if not ID.fullmatch(key) or key in entries:
            raise ValueError(f'invalid or duplicate message ID: {key}')
        entries[key] = ''.join(parts)
        i += 1
    return entries


def parse_po(text):
    entries, fields, active, fuzzy = [], {}, None, False
    for line in [*text.splitlines(), '']:
        line = line.strip()
        if not line:
            if fields:
                if fuzzy:
                    raise ValueError('fuzzy translations are not allowed in the retail catalog')
                entries.append(fields)
            fields, active, fuzzy = {}, None, False
        elif line.startswith('#,'):
            fuzzy |= 'fuzzy' in line[2:].split(',') or 'fuzzy' in line[2:].split()
        elif line.startswith('#'):
            continue
        elif line.startswith('"') and active:
            fields[active] += ast.literal_eval(line)
        else:
            match = re.fullmatch(r'(msgctxt|msgid|msgstr)\s+(".*")', line)
            if match is None or match[1] in fields:
                raise ValueError(f'unsupported or duplicate PO field: {line}')
            active = match[1]
            fields[active] = ast.literal_eval(match[2])
    return entries


# Compare conversion types, width/precision star arguments, and length modifiers.
# Clang checks the actual call-site C++ argument types after expansion.
FORMAT = re.compile(r'(?<![0-9])%(?:[-+ #0]*)(?:\*|\d+)?(?:\.(?:\*|\d+))?(?:hh|ll|I64|[hlLjzt])?[diouxXfFeEgGaAcspn%]')


def format_signature(text):
    result = []
    for match in FORMAT.finditer(text):
        spec = match.group()
        if spec == '%%':
            continue
        result.extend('*' for _ in range(spec.count('*')))
        kind = re.search(r'(hh|ll|I64|[hlLjzt])?([diouxXfFeEgGaAcspn%])$', spec)
        result.append((kind[1] or '') + kind[2])
    return result


@dataclass(frozen=True)
class Catalog:
    english: dict[str, str]
    russian: dict[str, str]

    @classmethod
    def load(cls, repo):
        root = Path(repo) / 'locales'
        registry_text = (root / 'messages.def').read_text(encoding='utf-8')
        po_text = (root / 'ru.po').read_text(encoding='utf-8')
        for name, text in (('messages.def', registry_text), ('ru.po', po_text)):
            errors = hidden_text_errors(text, allow_unicode=True)
            if errors:
                raise ValueError(f'{name}:{errors[0][0]}: {errors[0][1]}')
        english = parse_registry(registry_text)
        russian = {}
        for entry in parse_po(po_text):
            key = entry.get('msgctxt')
            if key is None and entry.get('msgid') == '':
                continue
            if key not in english or key in russian or entry.get('msgid') != english[key]:
                raise ValueError(f'{key}: unknown, duplicate or stale PO entry')
            value = entry.get('msgstr')
            if not value:
                raise ValueError(f'{key}: missing Russian translation')
            if format_signature(value) != format_signature(english[key]):
                raise ValueError(f'{key}: English/Russian printf placeholders differ')
            literal(value)  # Fail on unrepresentable Unicode, never replace it.
            russian[key] = value
        if russian.keys() != english.keys():
            raise ValueError('missing Russian translations: ' + ', '.join(english.keys() - russian.keys()))
        return cls(english, russian)

    def macro(self, key):
        if key not in self.russian:
            raise ValueError(f'unknown localization ID: {key}')
        return 'H2L' + hashlib.sha256(key.encode('ascii')).hexdigest()[:8]

    def header(self):
        macros = [self.macro(key) for key in self.russian]
        if len(set(macros)) != len(macros):
            raise ValueError('localization macro hash collision')
        return ''.join(f'#define {self.macro(key)} {literal(value)}\n'
                       for key, value in sorted(self.russian.items()))

    def calls(self, text):
        ts = tokens(text)
        for i, token in enumerate(ts):
            if token.group() != 'localization':
                continue
            tail = ts[i:i + 6]
            if len(tail) < 3 or [t.group() for t in tail[:3]] != ['localization', '::', 'Tr']:
                continue
            if (len(tail) != 6 or tail[3].group() != '(' or
                    tail[4].lastgroup != 'string' or tail[5].group() != ')'):
                raise ValueError('localization::Tr requires one literal semantic ID')
            key = ast.literal_eval(tail[4].group())
            self.macro(key)
            yield token.start(), tail[5].end(), key

    def render(self, text, *, expanded=False):
        out = text
        for start, end, key in reversed(list(self.calls(text))):
            if expanded:
                replacement = literal(self.russian[key])
            else:
                # Macro names are shorter than "localization". Padding retains
                # original UTF-8 byte positions, even for multiline calls.
                old = text[start:end].encode('utf-8')
                macro = self.macro(key)
                replacement = macro + ''.join('\n' if c == 10 else ' '
                                               for c in old[len(macro):])
            out = out[:start] + replacement + out[end:]
        return out


def hidden_text_errors(text, *, allow_unicode=False):
    """Reject numeric escapes; spell controls as \n, \t, etc. (NUL as \0).

    Escaped backslashes are consumed as a pair, so a path containing the text
    ``\\\\x`` is not mistaken for a byte escape. Checks strings AND characters.
    Cyrillic belongs in the catalog, not in inline game literals.
    """
    errors = []
    for token in tokens(text):
        if token.lastgroup not in ('string', 'char'):
            continue
        raw = token.group()
        for escape in re.finditer(r'\\(?:x[0-9a-fA-F]+|[0-7]{1,3}|u[0-9a-fA-F]{4}|U[0-9a-fA-F]{8}|.)', raw, re.S):
            value = escape.group()
            if not NUMERIC_ESCAPE.fullmatch(value):
                continue
            if value != '\\0':
                errors.append((text.count('\n', 0, token.start()) + 1, 'numeric text escape ' + value))
        if not allow_unicode and any(ord(c) >= 128 for c in raw):
            errors.append((text.count('\n', 0, token.start()) + 1, 'inline non-ASCII text; use a catalog ID'))
    return errors


def _write_generated(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = text.encode('utf-8')
    if path.is_file() and path.read_bytes() == payload:
        return
    with tempfile.NamedTemporaryFile(dir=path.parent, delete=False) as stream:
        stream.write(payload)
        temporary = stream.name
    os.replace(temporary, path)


def prepare(repo, source):
    """Return (generated source, literal header, Clang VFS overlay, dependencies).

    Content-addressed files are safe for concurrent TU builds and probes. The
    overlay keeps authored paths AND offsets in libclang, including headers.
    """
    repo, source = Path(repo).resolve(), Path(source).resolve()
    if not (repo / 'locales/messages.def').is_file():
        return source, None, None, []  # Small standalone tool-test fixtures.
    from homm2.build.cc_wrap import scan_header_deps
    catalog = Catalog.load(repo)
    generated = repo / 'build/localization'
    header_text = catalog.header()
    digest = hashlib.sha256(header_text.encode()).hexdigest()
    header = generated / (digest + '.h')
    _write_generated(header, header_text)
    dependencies = [repo / 'locales/messages.def', repo / 'locales/ru.po', Path(__file__)]
    roots, compiled = [], source
    views = {}
    for path in [source, *(Path(p) for p in scan_header_deps(source, repo / 'include'))]:
        text = path.read_text(encoding='utf-8')
        rendered = catalog.render(text)
        views[path] = rendered
        if rendered == text:
            continue
        target = generated / hashlib.sha256((str(path) + rendered).encode()).hexdigest() / path.name
        _write_generated(target, rendered)
        roots.append({'type': 'file', 'name': str(path), 'external-contents': str(target)})
        if path == source:
            compiled = target
    if any(Path(entry['name']) != source for entry in roots):
        # VC6 has no VFS support. A self-contained reachable-header mirror also
        # handles a quoted sibling include reached through an unchanged header.
        fingerprint = json.dumps([(str(p), value) for p, value in sorted(views.items())])
        mirror = generated / hashlib.sha256((header_text + fingerprint).encode()).hexdigest()
        header = mirror / 'messages.h'
        _write_generated(header, header_text)
        for path, rendered in views.items():
            relative = path.relative_to(repo) if path.is_relative_to(repo) else Path('probe') / path.name
            target = mirror / relative
            _write_generated(target, rendered)
            if path == source:
                compiled = target
    overlay_text = json.dumps({'version': 0, 'use-external-names': False, 'roots': roots})
    overlay = generated / (hashlib.sha256(overlay_text.encode()).hexdigest() + '.json')
    _write_generated(overlay, overlay_text)
    return compiled, header, overlay, dependencies


def clang_args(repo, source):
    _, header, overlay, _ = prepare(repo, source)
    return ['-ivfsoverlay', str(overlay), '-include', str(header)] if header else []


def check_formats(repo, source):
    """Clang checks real CRT calls; tolerate VC6 STL parse debt as inventories do."""
    import clang.cindex as ci
    from homm2.build.annotated_data import _clang_args, configure_libclang
    from homm2.clang_options import ClangMode
    configure_libclang()
    translation = ci.Index.create().parse(
        str(source), args=[*_clang_args(Path(repo), Path(source), mode=ClangMode.RETAIL_ANALYSIS),
                           '-Wformat=2', '-Werror=format',
                           '-Wno-error=format-security', '-Wno-error=format-nonliteral'])
    errors = []
    for diagnostic in translation.diagnostics:
        if diagnostic.severity < ci.Diagnostic.Error:
            continue
        file = diagnostic.location.file
        path = Path(str(file)).resolve() if file else None
        own = path is not None and (path == Path(source).resolve() or any(
            path.is_relative_to(Path(repo).resolve() / directory) for directory in ('src', 'include')))
        if own or diagnostic.option.startswith('-Wformat') or diagnostic.severity == ci.Diagnostic.Fatal:
            errors.append(str(diagnostic))
    return errors


def check_tree(repo):
    catalog = Catalog.load(repo)
    errors, used = [], set()
    for directory in ('src', 'include'):
        for path in sorted((Path(repo) / directory).rglob('*')):
            if path.suffix not in ('.cpp', '.h', '.c', '.hpp', '.inc'):
                continue
            text = path.read_text(encoding='utf-8')
            errors.extend(f'{path}:{line}: {message}' for line, message in hidden_text_errors(text))
            try:
                used.update(key for _, _, key in catalog.calls(text))
            except ValueError as exc:
                errors.append(f'{path}: {exc}')
    return errors, used


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path.cwd())
    args = parser.parse_args(argv)
    try:
        errors, used = check_tree(args.root)
    except (ValueError, UnicodeError) as exc:
        errors, used = [str(exc)], set()
    for error in errors:
        print(error)
    print(f'[localization] {len(used)} used IDs; {len(errors)} errors')
    return int(bool(errors))


if __name__ == '__main__':
    raise SystemExit(main())

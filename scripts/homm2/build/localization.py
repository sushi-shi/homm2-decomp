"""Static, byte-preserving Buka localization using master's IDs and PO format.

Authored Tr("semantic.id") expressions become literal macros, never runtime calls.
The compiler view preserves every source byte offset and newline: Clang's source
annotations and AST-based tools still point into the authored file. Only generated
files contain CP1251 byte escapes. Both Clang and VC6 see the same literal macros.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import tempfile
from pathlib import Path


# Re-export the portable catalog API for existing analysis/build callers.
from homm2.build.catalog import (Catalog, tokens, quoted, literal, parse_registry,
                                 parse_po, format_signature, hidden_text_errors)


def _write_generated(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = text.encode('utf-8')
    if path.is_file() and path.read_bytes() == payload:
        return
    with tempfile.NamedTemporaryFile(dir=path.parent, delete=False) as stream:
        stream.write(payload)
        temporary = stream.name
    os.replace(temporary, path)


def prepare(repo, source, *, locale='ru'):
    """Return (generated source, literal header, Clang VFS overlay, dependencies).

    Content-addressed files are safe for concurrent TU builds and probes. The
    overlay keeps authored paths AND offsets in libclang, including headers.
    """
    repo, source = Path(repo).resolve(), Path(source).resolve()
    if not (repo / 'locales/messages.def').is_file():
        return source, None, None, []  # Small standalone tool-test fixtures.
    from homm2.build.cc_wrap import scan_header_deps
    catalog = Catalog.load(repo)
    generated = repo / ('build/localization' if locale == 'ru' else 'build/ordinary/en/localization')
    header_text = catalog.header(locale)
    digest = hashlib.sha256(header_text.encode()).hexdigest()
    header = generated / (digest + '.h')
    _write_generated(header, header_text)
    dependencies = [repo / 'locales/messages.def', repo / 'locales/ru.po', Path(__file__),
                    Path(__file__).with_name('catalog.py')]
    roots, compiled = [], source
    views = {}
    for path in [source, *(Path(p) for p in scan_header_deps(source, repo / 'include'))]:
        text = path.read_text(encoding='utf-8')
        rendered = catalog.render(text, locale=locale)
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


def clang_args(repo, source, *, locale='ru'):
    _, header, overlay, _ = prepare(repo, source, locale=locale)
    return ['-ivfsoverlay', str(overlay), '-include', str(header)] if header else []


def check_formats(repo, source, *, locale='ru'):
    """Clang checks real CRT calls; tolerate VC6 STL parse debt as inventories do."""
    import clang.cindex as ci
    from homm2.build.annotated_data import _clang_args, configure_libclang
    from homm2.clang_options import ClangMode
    configure_libclang()
    translation = ci.Index.create().parse(
        str(source), args=[*_clang_args(Path(repo), Path(source), mode=ClangMode.RETAIL_ANALYSIS,
                                       locale=locale),
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

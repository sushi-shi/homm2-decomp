"""Inventory parameter roles and reconcile declarations using Clang identities.

Names are reconstruction evidence: PoL's publics-only CodeView has no argument
names, and Buka is stripped. A shared type is a review index, not a naming rule.
"""
from __future__ import annotations

import argparse
from concurrent.futures import ProcessPoolExecutor
import csv
import importlib
import json
from pathlib import Path
import sys

import clang.cindex as ci

from homm2.audit.bool_fields import _entries, _project_relative
from homm2.build.annotated_data import _clang_args, configure_libclang
from homm2.clang_options import ClangMode
from homm2.core.paths import REPO

CALLABLES = {ci.CursorKind.FUNCTION_DECL, ci.CursorKind.CXX_METHOD,
             ci.CursorKind.CONSTRUCTOR, ci.CursorKind.DESTRUCTOR,
             ci.CursorKind.CONVERSION_FUNCTION, ci.CursorKind.FUNCTION_TEMPLATE}
CALLBACKS = {ci.CursorKind.TYPEDEF_DECL, ci.CursorKind.TYPE_ALIAS_DECL,
             ci.CursorKind.FIELD_DECL, ci.CursorKind.VAR_DECL, ci.CursorKind.PARM_DECL}
INDIRECT = {ci.TypeKind.POINTER, ci.TypeKind.LVALUEREFERENCE,
            ci.TypeKind.RVALUEREFERENCE, ci.TypeKind.MEMBERPOINTER}
ARRAYS = {ci.TypeKind.CONSTANTARRAY, ci.TypeKind.INCOMPLETEARRAY,
          ci.TypeKind.VARIABLEARRAY, ci.TypeKind.DEPENDENTSIZEDARRAY}
REVIEW_FIELDS = ('file', 'function_usr', 'index', 'name', 'decision', 'reason')


def location(loc, repo):
    return {'file': _project_relative(str(loc.file), repo) if loc.file else None,
            'line': loc.line, 'offset': loc.offset}


def qualified(cursor):
    names = [cursor.spelling]
    parent = cursor.semantic_parent
    while parent and parent.kind != ci.CursorKind.TRANSLATION_UNIT:
        if parent.spelling:
            names.append(parent.spelling)
        parent = parent.semantic_parent
    return '::'.join(reversed(names))


def domains(type_, repo, route='value'):
    type_ = type_.get_canonical()
    if type_.kind in INDIRECT:
        return domains(type_.get_pointee(), repo, route + '/' + type_.kind.name)
    if type_.kind in ARRAYS:
        return domains(type_.get_array_element_type(), repo, route + '/array')
    if type_.kind == ci.TypeKind.FUNCTIONPROTO:
        result = domains(type_.get_result(), repo, route + '/return')
        for index, arg in enumerate(type_.argument_types()):
            result.extend(domains(arg, repo, route + f'/argument:{index}'))
        return result
    if type_.kind not in (ci.TypeKind.ENUM, ci.TypeKind.RECORD):
        return []
    declaration = type_.get_declaration()
    if declaration.spelling in ('H2EnumStorage', 'H2SteppedEnumStorage'):
        domain = domains(type_.get_template_argument_type(0), repo, route + '/storage')
        if domain:
            return domain
    return [{'name': declaration.spelling or type_.spelling,
             'usr': declaration.get_usr(), 'kind': declaration.kind.name,
             'linkage': declaration.linkage.name,
             'route': route, **location(declaration.location, repo)}]


def analyze(translation, repo):
    parameters, unowned, handled = [], [], set()
    blobs = {}

    def visit(cursor):
        project = location(cursor.location, repo)['file'] is not None
        if project and cursor.kind in CALLABLES | CALLBACKS:
            arguments = [c for c in cursor.get_children() if c.kind == ci.CursorKind.PARM_DECL]
            for index, parameter in enumerate(arguments):
                handled.add(parameter.hash)
                loc = location(parameter.location, repo)
                extent = parameter.extent
                file = loc['file']
                if not file:
                    continue
                if file not in blobs:
                    blobs[file] = (repo / file).read_bytes()
                start, end = extent.start.offset, extent.end.offset
                callback = cursor.kind in CALLBACKS
                usr = cursor.get_usr()
                # Some callback owners have empty USRs; preserve their site identity.
                if callback:
                    usr += f"@{location(cursor.location, repo)['file']}:{cursor.location.offset}"
                parameters.append({**loc, 'function': qualified(cursor),
                    'function_usr': usr, 'function_offset': cursor.location.offset,
                    'function_kind': cursor.kind.name, 'callback': callback,
                    'definition': cursor.is_definition(), 'index': index,
                    'parameter_count': len(arguments), 'name': parameter.spelling,
                    'type': parameter.type.spelling,
                    'canonical_type': parameter.type.get_canonical().spelling,
                    'domains': domains(parameter.type, repo),
                    'extent': {'start': start, 'end': end},
                    'excerpt': blobs[file][start:end].decode('utf-8', errors='replace'),
                    'linkage': cursor.linkage.name})
        if project and cursor.kind == ci.CursorKind.PARM_DECL and cursor.hash not in handled:
            unowned.append({**location(cursor.location, repo), 'name': cursor.spelling})
        for child in cursor.get_children():
            if not child.location.is_in_system_header:
                visit(child)

    visit(translation.cursor)
    return parameters, unowned


def _scan_entry(payload):
    repo, entry = payload
    configure_libclang()
    source = (Path(entry['directory']) / entry['file']).resolve()
    # Strict mode retains the enum domains erased in the VC6 ABI view. Pair
    # declarations within that mode by USR; do not guess overloads from names.
    translation = ci.Index.create().parse(str(source), args=_clang_args(
        repo, source, mode=ClangMode.STRICT))
    diagnostics = [d for d in translation.diagnostics if d.severity >= ci.Diagnostic.Error]
    errors = [str(d) for d in diagnostics if not d.location.is_in_system_header]
    sdk_errors = [str(d) for d in diagnostics if d.location.is_in_system_header]
    parameters, unowned = analyze(translation, repo)
    files = {source.relative_to(repo).as_posix()}
    for include in translation.get_includes():
        file = _project_relative(include.include.name, repo)
        if file:
            files.add(file)
    return {'source': source.relative_to(repo).as_posix(), 'parameters': parameters,
            'unowned': unowned, 'files': sorted(files), 'errors': errors, 'sdk_errors': sdk_errors}


def assemble(scans, repo):
    sites, included = {}, set()
    for scan in scans:
        included.update(scan['files'])
        occurrences = {}
        for parameter in scan['parameters']:
            key = (parameter['file'], parameter['function_offset'], parameter['function_usr'],
                   parameter['index'], parameter['offset'])
            occurrence = occurrences.get(key, 0)
            occurrences[key] = occurrence + 1
            key += (occurrence,)
            row = sites.setdefault(key, {**parameter, 'units': [], 'occurrence': occurrence})
            if scan['source'] not in row['units']:
                row['units'].append(scan['source'])
    parameters = []
    groups, by_type = {}, {}
    for key, row in sorted(sites.items()):
        row['id'] = f'P{len(parameters) + 1:05d}'
        row['selected'] = bool(row['domains'])
        parameters.append(row)
        # Internal functions in different TUs must not reconcile with each other.
        scope = row['units'][0] if row['linkage'] == 'INTERNAL' else ''
        group_key = (scope, row['function_usr'], row['index'])
        group = groups.setdefault(group_key, {'function': row['function'],
            'function_usr': row['function_usr'], 'index': row['index'], 'scope': scope,
            'names': set(), 'sites': [], 'selected': False})
        group['names'].add(row['name'])
        group['sites'].append(row['id'])
        group['selected'] |= row['selected']
        for domain in row['domains']:
            key = (domain['usr'] or domain['name'],
                   row['units'][0] if domain['linkage'] == 'INTERNAL' else '')
            group = by_type.setdefault(key, {'type': domain['name'],
                'kind': domain['kind'], 'declaration': {k: domain[k] for k in ('file', 'line', 'offset')},
                'uses': []})
            group['uses'].append({'site': row['id'], 'name': row['name'],
                'function': row['function'], 'file': row['file'], 'line': row['line'],
                'route': domain['route']})
    for group in groups.values():
        group['names'] = sorted(group['names'])
        group['unnamed_sites'] = '' in group['names']
        group['spelling_difference'] = len([n for n in group['names'] if n]) > 1
    headers = {p.relative_to(repo).as_posix() for p in (repo / 'include').rglob('*.h')}
    sources = {p.relative_to(repo).as_posix() for p in (repo / 'src').rglob('*.cpp')}
    return {'schema_version': 1, 'mode': 'strict', 'parameters': parameters,
        'groups': list(groups.values()), 'by_type': list(by_type.values()),
        'scans': [{k: v for k, v in scan.items() if k != 'parameters'} for scan in scans],
        'coverage': {'unseen_headers': sorted(headers - included),
            'unselected_sources': sorted(sources - {s['source'] for s in scans}),
            'limitations': ['Only active declarations in the selected compilation database are visible.',
                'SDK declarations are excluded; SDK types on project parameters remain visible.',
                'Strict enum domains do not assert retail ABI or original parameter spelling.',
                'VC6 system-header diagnostics are reported separately; SDK bodies are not certified.',
                'Snapshot IDs and byte offsets are not stable cross-edit keys.',
                'Different roles may use the same type; this inventory never chooses names.']}}


def scan(repo=REPO, filters=(), jobs=1):
    repo = repo.resolve()
    entries = _entries(repo, filters)
    if not entries:
        raise ValueError('no matching translation units')
    payloads = [(repo, entry) for entry in entries]
    if jobs == 1:
        scans = list(map(_scan_entry, payloads))
    else:
        with ProcessPoolExecutor(max_workers=jobs) as pool:
            worker = importlib.import_module('homm2.audit.parameters')._scan_entry
            scans = list(pool.map(worker, payloads))
    return assemble(scans, repo)


def check_review(report, path):
    """Check every selected site and reject stale, duplicate or unnamed decisions."""
    with path.open(newline='') as stream:
        reader = csv.DictReader(stream, delimiter='\t')
        if tuple(reader.fieldnames or ()) != REVIEW_FIELDS:
            raise ValueError('invalid parameter review columns')
        decisions = list(reader)
    expected = {(p['file'], p['function_usr'], str(p['index']), p['name'])
                for p in report['parameters'] if p['selected']}
    seen, errors = set(), []
    for row in decisions:
        key = tuple(row[k] for k in REVIEW_FIELDS[:4])
        if key in seen:
            errors.append(f'duplicate review: {key}')
        seen.add(key)
        if key not in expected:
            errors.append(f'stale review: {key}')
        if row['decision'] not in ('retain', 'rename') or not row['reason'].strip():
            errors.append(f'invalid disposition: {key}')
    errors.extend(f'missing review: {key}' for key in sorted(expected - seen))
    return errors


def write_tsv(report, stream, *, all_parameters=False, by_type=False):
    writer = csv.writer(stream, delimiter='\t', lineterminator='\n')
    if by_type:
        writer.writerow(('type', 'kind', 'name', 'function', 'file', 'line', 'site', 'route'))
        for group in report['by_type']:
            for use in group['uses']:
                writer.writerow((group['type'], group['kind'], *(use[k] for k in
                    ('name', 'function', 'file', 'line', 'site', 'route'))))
    else:
        writer.writerow(('id', 'file', 'line', 'function', 'index', 'name', 'type', 'domains', 'definition'))
        for row in report['parameters']:
            if all_parameters or row['selected']:
                writer.writerow(tuple(row[k] for k in ('id', 'file', 'line', 'function', 'index', 'name', 'type'))
                    + (';'.join(d['name'] for d in row['domains']), int(row['definition'])))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--tu', action='append', default=[])
    parser.add_argument('-j', '--jobs', type=int, default=1)
    parser.add_argument('--format', choices=('json', 'tsv'), default='tsv')
    parser.add_argument('--output', type=Path)
    parser.add_argument('--all', action='store_true')
    parser.add_argument('--by-type', action='store_true')
    parser.add_argument('--check', action='store_true', help='require complete review and consistent selected declarations')
    parser.add_argument('--review', type=Path, default=REPO / 'docs/parameter-naming-review.tsv')
    args = parser.parse_args(argv)
    if args.jobs < 1 or (args.check and args.tu):
        parser.error('--jobs must be positive; --check requires the complete inventory')
    try:
        report = scan(filters=args.tu, jobs=args.jobs)
        errors = [e for s in report['scans'] for e in s['errors']]
        errors += [f'unowned parameter: {p}' for s in report['scans'] for p in s['unowned']]
        if args.check:
            errors += check_review(report, args.review)
            errors += [f"declaration mismatch: {g['function']} argument {g['index']}: {g['names']}"
                       for g in report['groups'] if g['selected'] and g['spelling_difference']]
        if args.output:
            args.output.parent.mkdir(parents=True, exist_ok=True)
        stream = args.output.open('w') if args.output else sys.stdout
        try:
            if args.format == 'json':
                json.dump(report, stream, indent=2)
                stream.write('\n')
            else:
                write_tsv(report, stream, all_parameters=args.all, by_type=args.by_type)
        finally:
            if args.output:
                stream.close()
        print(f"parameters: {len(report['parameters'])} sites; "
              f"{sum(p['selected'] for p in report['parameters'])} enum/record; "
              f"{len(report['scans'])} TUs; {len(errors)} errors", file=sys.stderr)
        for error in errors:
            print(error, file=sys.stderr)
        return int(bool(errors))
    except (OSError, ValueError, RuntimeError, ci.TranslationUnitLoadError) as error:
        print(f'parameters: {error}', file=sys.stderr)
        return 2


if __name__ == '__main__':
    raise SystemExit(main())

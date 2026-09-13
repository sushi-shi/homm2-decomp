"""Compile and link an isolated locale build, without retail inputs or matching."""
from __future__ import annotations

import argparse
import io
import subprocess

from homm2.build import ninja_syntax
from homm2.build.configure.rules import emit_rules
from homm2.build.configure.link_graph import LINK_LIBRARIES
from homm2.build.fixed_asm import unit as fixed_asm_unit
from homm2.build.localization import _write_generated, check_tree
from homm2.core.manifest import load, unit_flags
from homm2.core.paths import REPO


def graph(manifest, locale):
    if locale not in ('ru', 'en'):
        raise ValueError(f'unsupported locale: {locale}')
    directory = f'build/ordinary/{locale}'
    stream = io.StringIO()
    w = ninja_syntax.Writer(stream)
    emit_rules(w, locale=locale, builddir=directory)
    source_objects, base_objects = [], []
    for unit in manifest['unit']:
        output = f"{directory}/obj/{unit['unit']}.obj"
        if fixed_asm_unit(unit['unit'], unit['source']):
            w.build(output, 'ml_omf', inputs=unit['source'],
                    implicit='scripts/homm2/build/ml_wrap.py')
        else:
            w.build(output, 'cl', inputs=unit['source'],
                    implicit=['scripts/homm2/build/cc_wrap.py',
                              'scripts/homm2/build/localization.py',
                              'scripts/homm2/build/catalog.py',
                              'locales/messages.def', 'locales/ru.po'],
                    variables={'flags': ' '.join(unit_flags(unit, manifest)),
                               'unit': unit['unit']})
        (base_objects if unit['unit'].startswith('BASE/') else source_objects).append(output)
    midi = base_objects.index(f'{directory}/obj/BASE/Midi.obj')
    archives = []
    for name, members in (('BASE-prefix', base_objects[:midi]),
                          ('Midi', base_objects[midi:midi + 1]),
                          ('BASE-suffix', base_objects[midi + 1:])):
        if members:
            output = f'{directory}/{name}.lib'
            w.build(output, 'archive', inputs=list(reversed(members)))
            archives.append(output)
    imports = []
    for name in ('wing32', 'netapi32', 'mss32', 'smackw32', 'audiere'):
        output = f'{directory}/imports/{name}.lib'
        rule = ('legacy_implib' if name == 'wing32' else
                'definition_vendor_implib' if name in ('netapi32', 'smackw32') else
                'definition_implib')
        w.build(output, rule, inputs=f'imports/{name}.def',
                implicit=['scripts/homm2/build/legacy_import_lib.py',
                          'scripts/homm2/build/regular_import_lib.py',
                          'scripts/homm2/build/regular_vendor_import_lib.py'],
                variables={'dll': name + '.dll', 'options':
                           '--symbol _Netbios@4 --lookup Netbios --hint 180'
                           if name == 'netapi32' else ''})
        imports.append(output)
    libraries = [value.replace('build/link/', directory + '/imports/')
                 for value in LINK_LIBRARIES]
    # One ordinary LINK pass, raw objects and source-backed import manifests.
    flags = ['/NOLOGO', '/MACHINE:IX86', '/BASE:0x400000',
             '/SUBSYSTEM:WINDOWS,4.0', '/STACK:66112,4096', '/HEAP:1048576,4096',
             '/INCREMENTAL:NO', '/OPT:NOREF', '/DEBUG',
             f'/PDB:{directory}/HMM2PL.pdb', f'/MAP:{directory}/HMM2PL.map',
             '/LIBPATH:build/toolchain/msvc/lib', '/OUT:$out',
             '/NODEFAULTLIB:LIBCMT', '/NODEFAULTLIB:LIBCPMT', '/NODEFAULTLIB:OLDNAMES',
             *source_objects, 'OLDNAMES.LIB', *libraries, *archives,
             'MSVCPRT.LIB', 'LIBCMT.LIB']
    w.rule('ordinary_link', command='wine "$$MSVC_DIR/bin/LINK.EXE" @$out.rsp',
           rspfile='$out.rsp', rspfile_content=' '.join(flags),
           description=f'LINK {locale} $out')
    w.build(f'{directory}/HMM2PL.exe', 'ordinary_link',
            inputs=source_objects + archives + imports,
            implicit=['build/toolchain/msvc/bin/LINK.EXE',
                      'build/toolchain/msvc/lib/LIBCMT.LIB',
                      'build/toolchain/msvc/lib/MSVCPRT.LIB'])
    w.default(f'{directory}/HMM2PL.exe')
    return stream.getvalue()


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    locales = parser.add_mutually_exclusive_group()
    locales.add_argument('--ru', dest='locale', action='store_const', const='ru')
    locales.add_argument('--en', dest='locale', action='store_const', const='en')
    parser.set_defaults(locale='ru')
    parser.add_argument('-j', type=int, help='parallel Ninja jobs (0 means unlimited)')
    parser.add_argument('-v', action='store_true', help='verbose Ninja commands')
    args = parser.parse_args(argv)
    if args.j is not None and args.j < 0:
        parser.error('-j must be nonnegative')
    errors, _ = check_tree(REPO)
    if errors:
        parser.error('\n'.join(errors))
    path = REPO / f'build/ordinary/{args.locale}/build.ninja'
    _write_generated(path, graph(load(), args.locale))
    command = ['ninja', '-f', str(path)]
    if args.j is not None:
        command += ['-j', str(args.j)]
    if args.v:
        command.append('-v')
    return subprocess.run(command, cwd=REPO).returncode


if __name__ == '__main__':
    raise SystemExit(main())

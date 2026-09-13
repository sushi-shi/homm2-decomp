#!/usr/bin/env python3
"""Build the generated Windows source tree (Russian by default)."""
import argparse
from pathlib import Path
import subprocess

from tools.catalog import Catalog

ROOT = Path(__file__).resolve().parent


def prepare(locale):
    catalog = Catalog.load(ROOT)
    for directory in ('src', 'include'):
        for source in sorted((ROOT / directory).rglob('*')):
            if source.suffix not in ('.cpp', '.h'):
                continue
            output = ROOT / 'build' / locale / 'localized' / source.relative_to(ROOT)
            output.parent.mkdir(parents=True, exist_ok=True)
            value = catalog.render(source.read_text(encoding='utf-8'),
                                   locale=locale, expanded=True)
            # Keep unchanged generated headers stable for incremental builds.
            if not output.exists() or output.read_text(encoding='utf-8') != value:
                output.write_text(value, encoding='utf-8')


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    locales = parser.add_mutually_exclusive_group()
    locales.add_argument('--ru', dest='locale', action='store_const', const='ru')
    locales.add_argument('--en', dest='locale', action='store_const', const='en')
    parser.set_defaults(locale='ru')
    parser.add_argument('--prepare', action='store_true', help=argparse.SUPPRESS)
    parser.add_argument('-j', type=int, help='parallel Ninja jobs')
    parser.add_argument('-v', action='store_true')
    args = parser.parse_args(argv)
    if args.j is not None and args.j < 0:
        parser.error('-j must be nonnegative')
    if args.prepare:
        prepare(args.locale)
        return 0
    command = ['ninja', '-f', 'build-en.ninja' if args.locale == 'en' else 'build.ninja']
    if args.j is not None:
        command += ['-j', str(args.j)]
    if args.v:
        command.append('-v')
    return subprocess.run(command, cwd=ROOT).returncode


if __name__ == '__main__':
    raise SystemExit(main())

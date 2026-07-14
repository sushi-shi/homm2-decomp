"""Command-line entry point for the integer-domain recovery workflow."""

import argparse

from . import census, strict, symbols


def main(argv=None):
    parser = argparse.ArgumentParser(prog="homm2 enum-types")
    commands = parser.add_subparsers(dest="command", required=True)

    census_parser = commands.add_parser("census", help="write enum inventory reports")
    census_parser.add_argument("--check-manifest", action="store_true")

    strict_parser = commands.add_parser("strict", help="run whole-tree strict Clang checks")
    strict_parser.add_argument("-j", "--jobs", type=int, default=0)
    strict_parser.add_argument("--tu", action="append", default=[], help="limit to a unit/source")

    commands.add_parser("symbols", help="audit retail and production enum mangling")
    commands.add_parser("validate", help="validate config/integer_domains.toml")

    args = parser.parse_args(argv)
    if args.command == "census":
        return census.run(check_manifest=args.check_manifest)
    if args.command == "strict":
        return strict.run(jobs=args.jobs, filters=args.tu)
    if args.command == "symbols":
        return symbols.run()
    return census.validate_manifest()

"""C36/S37: signed flag bytes, unsigned coordinates and retained local extents.

Run in the build environment with PYTHONPATH=scripts. The checks do not
claim successful/complete file reads or validate external map dimensions.
"""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root = Path.cwd()
unit, = [u for u in units() if u['unit'] == 'SOURCE/GAME']
probe = Path('docs/reconstruction/loadmap-byte-domain-probe.cpp')
code, log, timed_out = run_compile(probe, Path('build/c36-byte-domain.obj'),
                                  unit_flags(unit), depfile=False, cl_timeout=60)
print(log)
assert code == 0 and not timed_out
print('PASS VC6 signed char/i8, unsigned sentinel and destination-field types')

for raw in range(256):
    signed = raw if raw < 128 else raw-256
    assert signed % 256 == raw
    assert (signed % 256 != 255) == (raw != 255)
    assert (signed < 0) == bool(raw & 128)
    assert (signed & 127) == (raw & 127)
print('PASS all256 coordinate, sentinel, signed flag and masked type byte values')

configure_libclang()
source = root/'src/SOURCE/GAME.cpp'
body = source.read_text() + '\n' + probe.read_text()
for mode in ClangMode:
    tu = ci.Index.create().parse(str(source), args=_clang_args(root, source, mode=mode)+['-Dregister='],
                                unsaved_files=[(str(source), body)])
    own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error and d.location.file
           and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src', 'include'))]
    errors = unreviewed_diagnostics(own, root)
    assert not errors, [str(d) for d in errors]
    fn, = [c for c in tu.cursor.walk_preorder() if c.kind == ci.CursorKind.CXX_METHOD
           and c.spelling == 'LoadMap' and c.is_definition()]
    locals = {c.spelling: c.type for c in fn.walk_preorder() if c.kind == ci.CursorKind.VAR_DECL}
    for name, size, kind in [('x',1,ci.TypeKind.UCHAR), ('y',1,ci.TypeKind.UCHAR),
                             ('type',8,ci.TypeKind.SCHAR), ('junk',8,ci.TypeKind.CHAR_S)]:
        typ = locals[name]
        assert typ.get_size() == size, (mode, name, typ.spelling, typ.get_size())
        assert typ.get_array_element_type().get_canonical().kind == kind, (mode, name, typ.spelling)
    print('PASS', mode.value, 'LoadMap local x/y u8[1], type i8[8], junk char[8] and source diagnostics')

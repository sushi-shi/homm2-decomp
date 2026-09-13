"""U07/S24: actual VC6 layout plus both supported source dialects."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root = Path.cwd()
unit, = [u for u in units() if u['unit'] == 'SOURCE/netwin']
code, log, timed_out = run_compile(
    Path('docs/reconstruction/netbios-layout-probe.cpp'), Path('build/u07-layout.obj'),
    unit_flags(unit), depfile=False, cl_timeout=60)
print(log)
assert code == 0 and not timed_out
print('PASS VC6 payload4096/array28672/events36/NCB64')
configure_libclang()
source = root/'src/SOURCE/netwin.cpp'
body = source.read_text() + '\n' + (root/'docs/reconstruction/netbios-layout-probe.cpp').read_text()
for mode in ClangMode:
    tu = ci.Index.create().parse(str(source), args=_clang_args(root, source, mode=mode)+['-Dregister='],
                                unsaved_files=[(str(source), body)])
    own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error and d.location.file
           and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src', 'include'))]
    errors = unreviewed_diagnostics(own, root)
    assert not errors, (mode, [str(d) for d in errors])
    print('PASS netwin', mode.value, 'zero unreviewed project errors and layout')

"""C04/C08/C09/C11: both source dialects; C10's cast is a required modern boundary."""
from pathlib import Path
import subprocess
import clang.cindex as ci
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root = Path.cwd()
configure_libclang()
for name in ('src/BASE/WINMGR.cpp', 'src/SOURCE/SMACKMGR.cpp', 'src/SOURCE/wingraph.cpp'):
    source = root/name
    parent = subprocess.check_output(['git', 'show', '638cb091:'+name], text=True)
    for mode in ClangMode:
        for label, body in [('parent', parent), ('retained', source.read_text())]:
            tu = ci.Index.create().parse(str(source), args=_clang_args(root, source, mode=mode)+['-Dregister='],
                                        unsaved_files=[(str(source), body)])
            own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error and d.location.file
                   and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include'))]
            errors = unreviewed_diagnostics(own, root)
            assert not errors, (name, mode, label, [str(d) for d in errors])
            print('PASS', name, mode.value, label, 'zero unreviewed project errors')

source = root/'src/SOURCE/x_arena.cpp'
body = source.read_text()
cast = 'const_cast<char*>(gStatNames[widgetIndex1])'
assert body.count(cast) == 1
for mode in ClangMode:
    for label, text in [('retained', body), ('removed', body.replace(cast, 'gStatNames[widgetIndex1]'))]:
        tu = ci.Index.create().parse(str(source), args=_clang_args(root, source, mode=mode)+['-Dregister='],
                                    unsaved_files=[(str(source), text)])
        own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error and d.location.file
               and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include'))]
        errors = unreviewed_diagnostics(own, root)
        expected = 1 if mode == ClangMode.STRICT and label == 'removed' else 0
        assert len(errors) == expected, (mode, label, [str(d) for d in errors])
        if errors:
            assert Path(errors[0].location.file.name).resolve() == source
            assert 'no matching constructor' in errors[0].spelling
        print('PASS C10', mode.value, label, len(errors), 'expected project errors')

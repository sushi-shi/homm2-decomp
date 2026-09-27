"""C07: validate both source dialects and packed startup-record contracts."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root = Path.cwd()
configure_libclang()
for name in ('src/SOURCE/dpnetwin.cpp', 'src/SOURCE/Wsnetwin.cpp'):
    source = root/name
    body = source.read_text() + '''
#include <stddef.h>
typedef char C07DPSize[sizeof(DirectPlayStartupMessage) == 26 ? 1 : -1];
typedef char C07DPMap[offsetof(DirectPlayStartupMessage, playerIds) == 2 ? 1 : -1];
'''
    if 'Wsnetwin' in name:
        body += '''
typedef char C07WSSize[sizeof(WinsockStartupMessage) == 26 ? 1 : -1];
typedef char C07WSMap[offsetof(WinsockStartupMessage, playerAddresses) == 2 ? 1 : -1];
'''
    for mode in ClangMode:
        tu = ci.Index.create().parse(str(source), args=_clang_args(root, source, mode=mode)+['-Dregister='],
                                    unsaved_files=[(str(source), body)])
        own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error and d.location.file
               and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src', 'include'))]
        errors = unreviewed_diagnostics(own, root)
        assert not errors, (name, mode, [str(d) for d in errors])
        print('PASS', name, mode.value, 'zero unreviewed project errors; packed record contracts')

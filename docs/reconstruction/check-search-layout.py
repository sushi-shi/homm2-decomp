"""U02/U03/U04: VC6 layout and both language dialects for all changed TUs."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root=Path.cwd()
unit,=[u for u in units() if u['unit']=='SOURCE/SEARCH']
code,log,timed_out=run_compile(Path('docs/reconstruction/search-layout-probe.cpp'),Path('build/u02-layout.obj'),unit_flags(unit),depfile=False,cl_timeout=60)
print(log)
assert code==0 and not timed_out
print('PASS VC6 grid-pointer/directions and all searchArray fields')
configure_libclang()
for name in ('ADVMGR','AI','ARMY','FINDPATH','PHILAI','SEARCH'):
    source=root/'src/SOURCE'/(name+'.cpp')
    body=source.read_text()+'''
#include <stddef.h>
typedef char U02Size[sizeof(searchArray)==0x2518 ? 1 : -1];
typedef char U02Directions[offsetof(searchStorage,directions)==4 ? 1 : -1];
void U03UnsignedCount(u32&);
void U03CheckCount(searchArray* search) { U03UnsignedCount(search->m_queueCount); }
'''
    for mode in ClangMode:
        tu=ci.Index.create().parse(str(source),args=_clang_args(root,source,mode=mode)+['-Dregister='],unsaved_files=[(str(source),body)])
        own=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error and d.location.file
             and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include'))]
        errors=unreviewed_diagnostics(own,root)
        assert not errors,(name,mode,[str(d) for d in errors])
        print('PASS',name,mode.value,'zero unreviewed project errors; layout and u32 count',flush=True)

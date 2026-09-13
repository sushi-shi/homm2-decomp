"""U06: actual VC6 layout plus complete changed TUs in both source dialects."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root=Path.cwd()
unit,=[u for u in units() if u['unit']=='SOURCE/ARMYGRP']
code,log,timed_out=run_compile(Path('docs/reconstruction/army-group-layout-probe.cpp'),Path('build/u06-layout.obj'),unit_flags(unit),depfile=False,cl_timeout=60)
print(log)
assert code==0 and not timed_out
print('PASS VC6 packed army-group15/type5@0/count10@5, hero250, town100')
configure_libclang()
for name in ('EVENTS','GAME','PHILAI','ARMYGRP'):
    source=root/'src/SOURCE'/(name+'.cpp')
    body=source.read_text()+'''
#include <stddef.h>
typedef char U06Size[sizeof(armyGroup)==15 ? 1 : -1];
typedef char U06CountOffset[offsetof(armyGroup,m_creatureCounts)==5 ? 1 : -1];
void U06SignedCount(i16&);
void U06CheckSignedCount(armyGroup* group) { U06SignedCount(group->m_creatureCounts[0]); }
'''
    for mode in ClangMode:
        tu=ci.Index.create().parse(str(source),args=_clang_args(root,source,mode=mode)+['-Dregister='],unsaved_files=[(str(source),body)])
        own=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error and d.location.file
             and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include'))]
        errors=unreviewed_diagnostics(own,root)
        assert not errors,(name,mode,[str(d) for d in errors])
        print('PASS',name,mode.value,'layout and actual i16 reference; zero unreviewed project errors',flush=True)

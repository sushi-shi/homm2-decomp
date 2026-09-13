"""Check the actual town-handler owners in VC6 and both full source dialects."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root=Path.cwd()
unit,=[u for u in units() if u['unit']=='SOURCE/TOWNMGR']
probe=Path('docs/reconstruction/town-handler-types-probe.cpp')
code,log,timed_out=run_compile(probe,Path('build/town-handler-types.obj'),unit_flags(unit),depfile=False,cl_timeout=60)
print(log)
assert code==0 and not timed_out
print('PASS VC6 signed int/long/tick widths, byte/count/timer owners and army size')
configure_libclang()
source=root/'src/SOURCE/TOWNMGR.cpp'
body=source.read_text()+'\n'+probe.read_text()
for mode in ClangMode:
    tu=ci.Index.create().parse(str(source),args=_clang_args(root,source,mode=mode)+['-Dregister='],
                                unsaved_files=[(str(source),body)])
    own=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error and d.location.file
         and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include','docs/reconstruction'))]
    errors=unreviewed_diagnostics(own,root)
    assert not errors,(mode,[str(d) for d in errors])
    print('PASS TOWNMGR',mode.value,'complete source plus type/owner assertions; zero unreviewed project errors',flush=True)

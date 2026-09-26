"""U09/U14: VC6 wire layout and all affected native-TU include closures in both dialects."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root=Path.cwd()
all_units=units()
game,=[u for u in all_units if u['unit']=='SOURCE/GAME']
probe=Path('docs/reconstruction/remote-save-layout-probe.cpp')
code,log,timed_out=run_compile(probe,Path('build/u14-layout.obj'),unit_flags(game),depfile=False,cl_timeout=60)
print(log)
assert code==0 and not timed_out
print('PASS VC6: init16/chunk247/data245/send200/buffer256/adventure247/message256/KB256; offsets and member types')
configure_libclang()
affected={root/'include/SOURCE'/name for name in ('REMOTE_TYPES.h','REMOTE.h','REMOTE_SAVE.h','KB.h','ADVMGR.h','ADVMGR_TYPES.h')}
checked={mode:set() for mode in ClangMode}
for unit in all_units:
    source=root/'src'/(unit['unit']+'.cpp')
    if not source.exists():
        continue
    for mode in ClangMode:
        tu=ci.Index.create().parse(str(source),args=_clang_args(root,source,mode=mode)+['-Dregister='])
        closure={Path(item.include.name).resolve() for item in tu.get_includes()}
        if not closure & affected:
            continue
        own=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error and d.location.file
             and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include'))]
        errors=unreviewed_diagnostics(own,root)
        assert not errors,(unit['unit'],mode.value,[str(d) for d in errors])
        checked[mode].add(unit['unit'])
        print('PASS',unit['unit'],mode.value,'affected include closure; zero unreviewed project errors',flush=True)
for mode,names in checked.items():
    assert {'SOURCE/GAME','SOURCE/ADVMGR','SOURCE/KB','SOURCE/REMOTE','SOURCE/CURSOR'} <= names
    print('PASS',mode.value,len(names),'affected native-TU include closures',flush=True)

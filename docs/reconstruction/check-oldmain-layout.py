"""U11/S33: actual VC6 packet sizes and both dialects' six-byte local flags."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root=Path.cwd()
unit,=[u for u in units() if u['unit']=='SOURCE/KB']
code,log,timed_out=run_compile(Path('docs/reconstruction/oldmain-layout-probe.cpp'),
                             Path('build/u11-layout.obj'),unit_flags(unit),depfile=False,cl_timeout=60)
print(log)
assert code==0 and not timed_out
print('PASS VC6 setup212/buffer256/player count6')
configure_libclang()
source=root/'src/SOURCE/KB.cpp'
for mode in ClangMode:
    tu=ci.Index.create().parse(str(source),args=_clang_args(root,source,mode=mode)+['-Dregister='])
    own=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error and d.location.file
         and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include'))]
    errors=unreviewed_diagnostics(own,root)
    assert not errors,(mode,[str(d) for d in errors])
    function,=[c for c in tu.cursor.get_children() if c.kind==ci.CursorKind.FUNCTION_DECL
               and c.spelling=='oldmain' and c.is_definition()]
    locals={c.spelling:c for c in function.walk_preorder() if c.kind==ci.CursorKind.VAR_DECL}
    for name in ('matchedNetPlayers_d','matchedGamePlayers_c'):
        assert locals[name].type.get_size()==6,(mode,name,locals[name].type.spelling)
    for name in ('unusedMainState_o','unusedMenuState','unusedPlayerState_f'):
        assert locals[name].type.get_size()==4,(mode,name)
    assert locals['netBuffer_f'].type.get_size()==256
    print('PASS',mode.value,'zero unreviewed errors; flags6, retained scalars4 and buffer256')

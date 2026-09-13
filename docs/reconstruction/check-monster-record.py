"""U05: unchanged66-record retail table and both source-dialect layout contracts."""
from pathlib import Path
import clang.cindex as ci
from homm2.core.coff import CoffObject
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root = Path.cwd()
control = root/'build/orig/HMM2PL.exe'
verify_retail(control)
_, _, _, read_bytes = _pe_layout(control)
obj = CoffObject((root/'build/objdiff/base/SOURCE/KB.obj').read_bytes())
symbol, = [s for s in obj.symbols.values() if s.name == '?gMonsterDatabase@@3PAUtag_monsterInfo@@A']
section = obj.symbol_section(symbol)
data = obj.section_bytes(section)[symbol.value:symbol.value+66*26]
assert data == read_bytes(0xfa460, 66*26)
assert not [r for r in obj.relocations if r.section == symbol.section and symbol.value <= r.site < symbol.value+66*26]
print('PASS all66 monster records,1716 retail bytes at004fa460; no embedded relocations')

configure_libclang()
sources = ['AI', 'ARMY', 'CMBTMGR', 'COMMAND', 'DRAWING', 'FLY', 'GAME', 'KB', 'PHILAI', 'SPELLAI', 'SPELLS']
for name in sources:
    source = root/'src/SOURCE'/(name+'.cpp')
    body = source.read_text() + '''
#include <stddef.h>
typedef char U05Size[sizeof(tag_monsterInfo) == 26 ? 1 : -1];
typedef char U05Cost[offsetof(tag_monsterInfo, cost) == 0 ? 1 : -1];
typedef char U05Value[offsetof(tag_monsterInfo, fightValue) == 2 ? 1 : -1];
typedef char U05Icon[offsetof(tag_monsterInfo, iconIndex) == 6 ? 1 : -1];
typedef char U05Sprite[offsetof(tag_monsterInfo, spriteName) == 17 ? 1 : -1];
typedef char U05Flags[offsetof(tag_monsterInfo, attributes) == 22 ? 1 : -1];
typedef char U05FlagSize[sizeof(MonsterFlags) == 4 ? 1 : -1];
'''
    for mode in ClangMode:
        tu = ci.Index.create().parse(str(source), args=_clang_args(root, source, mode=mode)+['-Dregister='],
                                    unsaved_files=[(str(source), body)])
        own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error and d.location.file
               and any(Path(d.location.file.name).resolve().is_relative_to(root/p) for p in ('src','include'))]
        errors = unreviewed_diagnostics(own, root)
        assert not errors, (name, mode, [str(d) for d in errors])
        print('PASS', name, mode.value, 'all fields/size; zero unreviewed project errors', flush=True)

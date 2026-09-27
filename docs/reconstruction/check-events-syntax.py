"""EVENTS source dialects and actual serialized owners, not packet validation."""
import subprocess
from pathlib import Path
import clang.cindex as ci
from homm2.permute.generate_ast_variants import configure_libclang,clang_args

root=Path.cwd();source=root/'src/SOURCE/EVENTS.cpp'
parent=subprocess.check_output(['git','show','3a80a103:src/SOURCE/EVENTS.cpp'],text=True)
header=root/'include/SOURCE/EVENTS.h'
parent_header=subprocess.check_output(['git','show','3a80a103:include/SOURCE/EVENTS.h'],text=True)
configure_libclang();args=clang_args(root,source)
for mode in ('c++98','c++20'):
 parse_args=[('-std='+mode) if arg.startswith('-std=') else arg for arg in args]+['-Dregister=']
 evidence=[]
 for label,body in [('parent',parent),('retained',source.read_text())]:
  unsaved=[(str(source),body)]
  if label=='parent':unsaved.append((str(header),parent_header))
  tu=ci.Index.create().parse(str(source),args=parse_args,unsaved_files=unsaved)
  errors=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error]
  local=[d for d in errors if d.location.file and Path(d.location.file.name)==source]
  assert not local,(mode,label,[str(d) for d in local])
  evidence.append(sorted((d.location.file.name if d.location.file else '',d.location.line,d.spelling) for d in errors))
  if label=='retained':
   for owner,size,fields in [
    ('hero',250,{'m_spells':148}),
    ('armyGroup',15,{}),('town',100,{}),
    ('combatRemoteData',155,{'fragment':0,'randomSeed':8,'firstGold':16,'secondGold':21,'firstArmy':25,'secondArmy':40,'combatTown':55}),
    ('combatRemoteHeroFragment',201,{'fragment':0,'data':1}),
    ('mapEventExtra',137,{'active':0,'resources':1,'artifact':29,'answerCount':31,'answers':32,'riddle':136})]:
    record,=[c for c in tu.cursor.get_children() if c.spelling==owner and c.is_definition() and c.kind in (ci.CursorKind.CLASS_DECL,ci.CursorKind.STRUCT_DECL)]
    assert record.type.get_size()==size,(mode,owner,record.type.get_size())
    actual={c.spelling:c.get_field_offsetof()//8 for c in record.get_children() if c.kind==ci.CursorKind.FIELD_DECL}
    assert all(actual.get(k)==v for k,v in fields.items()),(mode,owner,actual)
 assert evidence[0]==evidence[1],(mode,evidence)
 print('PASS',mode,'hero250/spells148, army15, town100, battle155, fragment201, event137 layouts; no source errors; identical',len(evidence[0]),'external diagnostics')

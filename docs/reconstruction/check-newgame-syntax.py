"""C76/U18/O07: dialect diagnostics and actual packet/game member layout."""
import subprocess
from pathlib import Path
import clang.cindex as ci
from homm2.permute.generate_ast_variants import configure_libclang,clang_args
root=Path.cwd();source=root/'src/SOURCE/Newgame.cpp'
parent=subprocess.check_output(['git','show','f84e0b7c:src/SOURCE/Newgame.cpp'],text=True)
configure_libclang();args=clang_args(root,source)
for mode in ('c++98','c++20'):
 parse_args=[('-std='+mode) if arg.startswith('-std=') else arg for arg in args]+['-Dregister=']
 evidence=[]
 for label,body in [('parent',parent),('retained',source.read_text())]:
  tu=ci.Index.create().parse(str(source),args=parse_args,unsaved_files=[(str(source),body)])
  errors=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error]
  local=[d for d in errors if d.location.file and Path(d.location.file.name)==source]
  assert not local,(mode,label,[str(d) for d in local])
  evidence.append(sorted((d.location.file.name if d.location.file else '',d.location.line,d.spelling) for d in errors))
  if label=='retained':
   for owner,size,fields in [('NewGameRemotePacket',256,{'sender':0,'id':1,'type':5,'command':6,'payloadSize':7,'payload':9}),
      ('SMapHeader',420,{'name':58,'description':118}),
      ('game',None,{'m_mapHeader':0x2a9,'m_setupPlayerColor':0x44d,'m_mapFilename':0x466,'_pad_0x47c':0x47c,'m_playerCount':0x48e})]:
    record,=[c for c in tu.cursor.get_children() if c.spelling==owner and c.is_definition()]
    if size is not None:assert record.type.get_size()==size,(mode,owner,record.type.get_size())
    actual={c.spelling:c.get_field_offsetof()//8 for c in record.get_children() if c.kind==ci.CursorKind.FIELD_DECL}
    assert all(actual.get(k)==v for k,v in fields.items()),(mode,owner,actual)
 assert evidence[0]==evidence[1],(mode,evidence)
 print('PASS',mode,'packet256/map420/game offsets; no source errors and identical',len(evidence[0]),'external diagnostic identities')

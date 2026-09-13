"""C78/O08: both source dialects and actual packed campaign/choice owners."""
import subprocess
from pathlib import Path
import clang.cindex as ci
from homm2.permute.generate_ast_variants import configure_libclang,clang_args
root=Path.cwd();source=root/'src/SOURCE/X_CAMPGN.cpp'
parent=subprocess.check_output(['git','show','2abf1283:src/SOURCE/X_CAMPGN.cpp'],text=True)
header=root/'include/SOURCE/ExpCampaign.h'
parent_header=subprocess.check_output(['git','show','2abf1283:include/SOURCE/ExpCampaign.h'],text=True)
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
   for owner,size,fields in [('ExpCampaign',79,{'m_campaignId':0,'m_currentMap':4,'m_mapCount':8,'m_mapChoices':12,'m_mapsPlayed':20,'m_mapDays':28,'m_awards':44,'m_bonusChoices':55,'m_pad_0x3f':63,'m_viewMap':67,'m_window':71,'m_viewOnly':75}),('SCampaignChoice',5,{'type':0,'amount':3})]:
    record,=[c for c in tu.cursor.get_children() if c.spelling==owner and c.is_definition() and c.kind in (ci.CursorKind.CLASS_DECL,ci.CursorKind.STRUCT_DECL)]
    assert record.type.get_size()==size,(mode,owner,record.type.get_size())
    actual={c.spelling:c.get_field_offsetof()//8 for c in record.get_children() if c.kind==ci.CursorKind.FIELD_DECL}
    assert all(actual.get(k)==v for k,v in fields.items()),(mode,owner,actual)
 assert evidence[0]==evidence[1],(mode,evidence)
 print('PASS',mode,'packed campaign79/choice5 layout; no source errors and identical',len(evidence[0]),'external diagnostics')

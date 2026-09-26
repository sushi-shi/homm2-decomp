"""C75: no new C++98/C++20 diagnostics relative to reviewed parent aab81af5."""
import subprocess
from pathlib import Path
import clang.cindex as ci
from homm2.permute.generate_ast_variants import configure_libclang,clang_args
root=Path.cwd();source=root/'src/SOURCE/DRAWING.cpp'
parent=subprocess.check_output(['git','show','aab81af5:src/SOURCE/DRAWING.cpp'],text=True)
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
 assert evidence[0]==evidence[1],(mode,evidence)
 print('PASS',mode,'no source errors; identical',len(evidence[0]),'external diagnostic identities')

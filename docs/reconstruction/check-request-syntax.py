"""Compare REQUEST source diagnostics with the reviewed pre-edit parent.

Retail VC6 compatibility is not enough: preserve both C++98 analysis and the
modern strong-enum model. Existing external-header diagnostics must be exactly
the same set, not accepted solely because an error count stays constant.
"""
import subprocess
from pathlib import Path
import clang.cindex as ci
from homm2.permute.generate_ast_variants import configure_libclang,clang_args

root=Path.cwd()
source=root/'src/SOURCE/REQUEST.cpp'
parent=subprocess.check_output(['git','show','13e6af40:src/SOURCE/REQUEST.cpp'],text=True)
configure_libclang()
args=clang_args(root,source)
for mode in ('c++98','c++20'):
    parse_args=[('-std='+mode) if arg.startswith('-std=') else arg for arg in args]+['-Dregister=']
    evidence=[]
    for label,body in [('parent',parent),('retained',source.read_text())]:
        tu=ci.Index.create().parse(str(source),args=parse_args,unsaved_files=[(str(source),body)])
        errors=[d for d in tu.diagnostics if d.severity>=ci.Diagnostic.Error]
        local=[d for d in errors if d.location.file and Path(d.location.file.name)==source]
        assert not local,(mode,label,[str(d) for d in local])
        external=sorted((d.location.file.name if d.location.file else '',d.location.line,d.spelling) for d in errors)
        evidence.append(external)
    assert evidence[0]==evidence[1],(mode,evidence)
    print('PASS',mode,'no source errors; identical',len(evidence[0]),'pre-existing external diagnostic identities')

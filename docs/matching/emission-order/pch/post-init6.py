import json
from pathlib import Path
m=json.loads(Path('build/probe/post-init4.json').read_text())
m["files"]["node.h"]="@@wpre@@"+m["files"]["node.h"]+"@@wpost@@"
m["defaults"].update({"wpre":"","wpost":""})
m["axes"]=m["axes"][:2]+[{"name":"win","options":[{"name":"none"},
  {"name":"pch_before","slots":{"wpre":"#include <windows.h>\n"}},
  {"name":"pch_after","slots":{"wpost":"#include <windows.h>\n"}},
  {"name":"tu_after","slots":{"after":"#include <windows.h>\n"}},
  {"name":"pch_before_tu_again","slots":{"wpre":"#include <windows.h>\n","after":"#include <windows.h>\n"}},
  {"name":"winbase_pch","slots":{"wpre":"#include <windef.h>\n"}}]},
 {"name":"pch","options":[{"name":"pch"},{"name":"nopch","units":["eff.cpp"],"unit_flags":{"eff.cpp":[]}}]}]
Path('build/probe/post-init6.json').write_text(json.dumps(m,indent=1))

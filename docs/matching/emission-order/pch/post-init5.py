import json
from pathlib import Path
m=json.loads(Path('build/probe/post-init4.json').read_text())
hs=["windows.h","SOURCE/armyGroup.h","SOURCE/hero.h","SOURCE/REMOTE_TYPES.h","SOURCE/KB_TYPES.h","BASE/message.h","SOURCE/KBForward.h","BASE/WINMGR.h","BASE/dialog.h","SOURCE/GAME.h","SOURCE/town.h","SOURCE/KBDeclarations.h","SOURCE/KB.h"]
m["axes"]=m["axes"][:2]+[{"name":"after","options":[{"name":"none"}]+[{"name":h.replace('/','_'),"slots":{"after":f"#include <{h}>\n"}} for h in hs]}]
Path('build/probe/post-init5.json').write_text(json.dumps(m,indent=1))

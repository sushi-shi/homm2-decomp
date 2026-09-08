import json, shlex, subprocess
from pathlib import Path
entry = next(x for x in json.loads(Path('build/compile_commands.json').read_text()) if x['file'].endswith('/EXECUTABLE/POSIX/Main.cpp'))
args = shlex.split(entry['command'])
args[args.index('-o')+1] = '/tmp/ironfist-engine-probe.o'
args[-1] = str(Path('tools/review/ironfist/engine_probe.cpp').resolve())
args.append('-fno-access-control')
subprocess.run(args, cwd='build', check=True)
link = subprocess.check_output(['ninja','-C','build','-t','commands','homm2'], text=True).splitlines()[-1]
link = link.replace('CMakeFiles/homm2.dir/src/EXECUTABLE/POSIX/Main.cpp.o', '/tmp/ironfist-engine-probe.o')
link = link.replace(' -o homm2 ', ' -o /tmp/ironfist-engine-probe ')
subprocess.run(link, cwd='build', shell=True, check=True)

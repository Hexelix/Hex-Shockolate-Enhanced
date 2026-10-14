#!/usr/bin/env python3
"""Run focused tests using a configured build's actual compiler settings.
Usage: python3 tests/run.py build
Requires cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON.
"""
import json, pathlib, shlex, subprocess, sys
root=pathlib.Path(__file__).resolve().parents[1]
build=(root/(sys.argv[1] if len(sys.argv)>1 else 'build')).resolve()
commands=json.loads((build/'compile_commands.json').read_text())
out=build/'regression-tests'; out.mkdir(exist_ok=True)
def settings(suffix):
    item=next(c for c in commands if c['file'].endswith(suffix))
    args=shlex.split(item['command']); clean=[]; skip=False
    for a in args:
        if skip: skip=False; continue
        if a=='-o': skip=True; continue
        if a in ('-c','-DNDEBUG') or a==item['file']: continue
        clean.append(a)
    return clean+['-UNDEBUG','-ffunction-sections','-fdata-sections'],item
for test,source in [('yaw','EDMS/Source/interfac.cc'),('medic','GameSrc/drugs.c')]:
    args,item=settings(source)
    obj=out/(test+'.o')
    subprocess.run(args+['-c',item['file'],'-o',str(obj)],cwd=item['directory'],check=True)
    harness=root/'tests'/(test+('.cpp' if test=='yaw' else '.c'))
    exe=out/test
    subprocess.run(args+[str(harness),str(obj),'-Wl,--gc-sections','-o',str(exe)],cwd=item['directory'],check=True)
    subprocess.run([str(exe)],check=True)

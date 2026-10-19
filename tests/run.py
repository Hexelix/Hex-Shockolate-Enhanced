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
log_args,log_item=settings('LOG/src/log.c')
log_obj=out/'logging.o'
subprocess.run(log_args+['-c',log_item['file'],'-o',str(log_obj)],cwd=log_item['directory'],check=True)
for test,source in [('yaw','EDMS/Source/interfac.cc'),('medic','GameSrc/drugs.c'),('berserk','GameSrc/drugs.c'),('berserk_integration','GameSrc/drugs.c'),('player_save','RES/Source/resacc.c'),('inventory','GameSrc/invent.c'),('ecology','GameSrc/ecology.c')]:
    args,item=settings(source)
    obj=out/(test+'.o')
    if test not in ('inventory','ecology'):
        subprocess.run(args+['-c',item['file'],'-o',str(obj)],cwd=item['directory'],check=True)
    if test=='player_save': args,_=settings('GameSrc/drugs.c')
    harness=root/'tests'/(test+('.cpp' if test=='yaw' else '.c'))
    exe=out/test
    subprocess.run(args+[str(harness)]+([] if test in ('inventory','ecology') else [str(obj)])+[str(log_obj),'-Wl,--gc-sections','-o',str(exe)],cwd=item['directory'],check=True)
    subprocess.run([str(exe)],check=True)

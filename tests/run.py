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
for test,source in [('yaw','EDMS/Source/interfac.cc'),('medic','GameSrc/drugs.c'),('berserk','GameSrc/drugs.c'),('berserk_integration','GameSrc/drugs.c'),('player_save','RES/Source/resacc.c'),('inventory','GameSrc/invent.c'),('ecology','GameSrc/ecology.c'),('save_preflight','GameSrc/gamewrap.c'),('crash_report','MacSrc/crash_report.c')]:
    args,item=settings(source)
    obj=out/(test+'.o')
    if test not in ('inventory','ecology'):
        subprocess.run(args+['-c',item['file'],'-o',str(obj)],cwd=item['directory'],check=True)
    if test=='player_save': args,_=settings('GameSrc/drugs.c')
    harness=root/'tests'/(test+('.cpp' if test=='yaw' else '.c'))
    exe=out/test
    subprocess.run(args+[str(harness)]+([] if test in ('inventory','ecology') else [str(obj)])+[str(log_obj),'-Wl,--gc-sections','-o',str(exe)],cwd=item['directory'],check=True)
    subprocess.run([str(exe)],check=True)

# Link the real screen allocator and conversion/projection code to SDL's dummy
# video backend; no proprietary game assets or display server are needed.
args,item=settings('MacSrc/ShockBitmap.c')
link_args=shlex.split((build/'CMakeFiles/systemshock.dir/link.txt').read_text())
sdl_library=next((a for a in link_args if 'libSDL2' in a and 'mixer' not in a and 'main' not in a), '-lSDL2')
gl_library=next((a for a in link_args if 'libGL.' in a), '-lGL')
objects=[]
for suffix in ['MacSrc/ShockBitmap.c','GameSrc/gr2ss.c','2D/Source/mode.c','3D/Source/fov.c']:
    extra,entry=settings(suffix)
    obj=out/('video-'+pathlib.Path(suffix).stem+'.o')
    subprocess.run(extra+['-c',entry['file'],'-o',str(obj)],cwd=entry['directory'],check=True)
    objects.append(str(obj))
exe=out/'video'
subprocess.run(args+[str(root/'tests/video.c')]+objects+[str(log_obj),
    str(build/'src/Libraries/libFIX_LIB.a'),sdl_library,
    '-lm','-Wl,--gc-sections','-o',str(exe)],cwd=item['directory'],check=True)
subprocess.run([str(exe)],check=True)

if '--opengl' in sys.argv:
    # Optional: needs SDL's offscreen OpenGL backend (Mesa on the build host).
    args,item=settings('MacSrc/OpenGL.cc')
    exe=out/'video-gl'
    subprocess.run(args+[str(root/'tests/video_gl.cpp'),str(log_obj),
        str(build/'src/Libraries/lib2D_LIB.a'),str(build/'src/Libraries/libLG_LIB.a'),
        sdl_library,gl_library,
        '-Wl,--gc-sections','-o',str(exe)],cwd=item['directory'],check=True)
    subprocess.run([str(exe)],cwd=root,check=True)

#!/usr/bin/env python3
"""Copy original game assets into this build's res folder; never modify originals.
Usage: python3 import-data.py /path/to/sshock.kpf
   or: python3 import-data.py /path/to/SSHOCK
"""
import pathlib, shutil, sys, zipfile
if len(sys.argv)!=2:
    sys.exit(__doc__)
source=pathlib.Path(sys.argv[1]).expanduser().resolve()
base=pathlib.Path(__file__).resolve().parent/'res'
required={'cybstrng.res','gamescr.res','mfdart.res','obj3d.res','citmat.res','digifx.res','objprop.dat'}
entries={}
def identify(name):
    parts=pathlib.PurePosixPath(name.replace('\\','/').lower()).parts
    if '..' in parts or name.startswith(('/', '\\')): return None
    # Known original PC asset layouts; exclude Mac/remake resources.
    for prefix,priority in [('res/pc/cdrom/',0),('res/pc/hd/',1),('res/',0),('',0)]:
        for kind in ('data','sound'):
            start=tuple((prefix+kind).strip('/').split('/'))
            if parts[:len(start)]==start and len(parts)>len(start):
                return (kind,)+parts[len(start):],priority
    return None
if source.is_file() and zipfile.is_zipfile(source):
    with zipfile.ZipFile(source) as z:
        for info in z.infolist():
            result=identify(info.filename)
            if info.is_dir() or result is None: continue
            rel,priority=result
            if rel not in entries or priority>entries[rel][0]: entries[rel]=(priority,info)
        missing=required-{p[-1] for p in entries if p[0]=='data'}
        if missing: sys.exit('Archive lacks recognized PC DATA files: '+', '.join(sorted(missing)))
        for rel,(_,info) in entries.items():
            dest=base.joinpath(*rel); dest.parent.mkdir(parents=True,exist_ok=True)
            with z.open(info) as src, dest.open('wb') as dst: shutil.copyfileobj(src,dst)
elif source.is_dir():
    # Accept the DATA folder itself or a game/res directory containing DATA and SOUND.
    scan=source.parent if source.name.lower()=='data' else source
    for p in scan.rglob('*'):
        if not p.is_file() or p.is_symlink(): continue
        result=identify(p.relative_to(scan).as_posix())
        if result:
            rel,priority=result
            if rel not in entries or priority>entries[rel][0]: entries[rel]=(priority,p)
    missing=required-{p[-1] for p in entries if p[0]=='data'}
    if missing: sys.exit('Directory lacks required DATA files: '+', '.join(sorted(missing)))
    for rel,(_,p) in entries.items():
        dest=base.joinpath(*rel); dest.parent.mkdir(parents=True,exist_ok=True)
        if p.resolve()!=dest.resolve(): shutil.copy2(p,dest)
else: sys.exit('Expected an original game directory or a ZIP-format sshock.kpf.')
print(f'Copied {len(entries)} files into {base}. Originals were not modified.')
if not any(p[0]=='sound' for p in entries): print('No SOUND folder was found; original music assets are still needed for music.')

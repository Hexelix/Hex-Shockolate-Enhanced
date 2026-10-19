#!/usr/bin/env python3
"""Package a built executable with shaders, launcher, and corresponding source."""
import argparse
import pathlib
import shutil
import subprocess
import tarfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('build', type=pathlib.Path)
parser.add_argument('destination', type=pathlib.Path)
parser.add_argument('--mixer-source', required=True, type=pathlib.Path)
args = parser.parse_args()
root = pathlib.Path(__file__).resolve().parents[1]
binary = args.build.resolve() / 'systemshock'
if binary.read_bytes()[:4] != b'\x7fELF':
    parser.error('systemshock is not an ELF executable')
dest = args.destination.resolve()
if dest.exists():
    parser.error('destination already exists; choose a new package directory')
dest.mkdir(parents=True)
shutil.copy2(binary, dest / 'systemshock')
subprocess.run(['objcopy', '--only-keep-debug', str(binary), str(dest / 'systemshock.debug')], check=True)
subprocess.run(['strip', str(dest / 'systemshock')], check=True)
subprocess.run(['objcopy', '--add-gnu-debuglink=systemshock.debug', 'systemshock'], cwd=dest, check=True)
(dest / 'systemshock').chmod(0o755)
(dest / 'systemshock.debug').chmod(0o644)
shutil.copytree(root / 'shaders', dest / 'shaders')
for name in ['launch.sh', 'import-data.py', 'README-recovery.txt']:
    shutil.copy2(root / 'linux' / name, dest / name)
shutil.copy2(root / 'COPYING.txt', dest / 'COPYING.txt')
source = dest / 'source' / 'systemshock-enhanced'
# Use current tracked content, never stale build directories or game assets.
tracked = subprocess.check_output(['git', '-C', str(root), 'ls-files', '-z'])
for name in tracked.decode().split('\0'):
    if not name:
        continue
    src = root / name
    if src.is_file():
        target = source / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, target)
shutil.copytree(args.mixer_source, dest / 'source' / 'SDL_mixer',
                ignore=shutil.ignore_patterns('.git', 'build', 'CMakeFiles'))
with tarfile.open(str(dest) + '.tar.gz', 'w:gz') as archive:
    archive.add(dest, arcname=dest.name)
print(str(dest) + '.tar.gz')

#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$HERE"
if [ ! -f shaders/main.vert ] || [ ! -f shaders/texture.frag ] || \
   [ ! -f shaders/color.frag ] || [ ! -f shaders/star.frag ] || \
   [ ! -f shaders/hdr.frag ]; then
    echo 'Shader files are missing. Extract the complete package, including the shaders directory.' >&2
    exit 1
fi
if [ ! -f res/data/cybstrng.res ] || [ ! -f res/data/objprop.dat ]; then
    echo 'Game data is missing. Run import-data.py with your sshock.kpf or original game directory first.' >&2
    exit 1
fi
exec ./systemshock "$@" >> run.log 2>&1

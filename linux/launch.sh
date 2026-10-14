#!/bin/sh
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$HERE"
if [ ! -f res/data/cybstrng.res ] || [ ! -f res/data/objprop.dat ]; then
    echo 'Game data is missing. Run import-data.py with your sshock.kpf or original game directory first.' >&2
    exit 1
fi
exec ./systemshock "$@" >> run.log 2>&1

#!/bin/sh
# Standard Linux build using distribution-provided development packages.
# Debian/Ubuntu: build-essential cmake libsdl2-dev libsdl2-mixer-dev libgl-dev pkg-config
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cmake -S "$ROOT" -B "$ROOT/build" \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_LINK_DEPENDS_USE_LINKER=FALSE \
  -DENABLE_SDL2=ON -DENABLE_SOUND=ON -DENABLE_FLUIDSYNTH=OFF \
  -DCMAKE_DISABLE_FIND_PACKAGE_ALSA=TRUE -DCMAKE_SKIP_RPATH=ON
cmake --build "$ROOT/build" --parallel
python3 "$ROOT/tests/run.py" "$ROOT/build"

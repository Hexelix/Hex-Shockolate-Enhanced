#!/bin/sh
# From the supplied source bundle, build SDL_mixer into the game executable.
# Requires gcc/g++, cmake, pkg-config, libsdl2-dev and libgl-dev.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
MIXSRC=$(CDPATH= cd -- "$ROOT/../SDL_mixer" && pwd)
MIXBUILD="$ROOT/mixer-build"
cmake -S "$MIXSRC" -B "$MIXBUILD" -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF -DSDL2MIXER_SAMPLES=OFF \
  -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_MOD=OFF -DSDL2MIXER_MIDI=OFF \
  -DSDL2MIXER_OPUS=OFF -DSDL2MIXER_VORBIS=OFF -DSDL2MIXER_WAVPACK=OFF
cmake --build "$MIXBUILD" --parallel
cmake -S "$ROOT" -B "$ROOT/build" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_LINK_DEPENDS_USE_LINKER=FALSE \
  -DENABLE_SDL2=ON -DENABLE_SOUND=BUNDLED -DENABLE_FLUIDSYNTH=OFF \
  -DSDL2_MIXER_LIBRARY="$MIXBUILD/libSDL2_mixer.a" \
  -DCMAKE_C_FLAGS="-I$MIXSRC/include" -DCMAKE_CXX_FLAGS="-I$MIXSRC/include" \
  -DCMAKE_DISABLE_FIND_PACKAGE_ALSA=TRUE -DCMAKE_SKIP_RPATH=ON
cmake --build "$ROOT/build" --parallel
python3 "$ROOT/tests/run.py" "$ROOT/build"

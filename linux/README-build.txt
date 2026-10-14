SYSTEM SHOCK ENHANCED — LINUX TEST BUILD

Based on MeeperTheGreat99/systemshock-enhanced commit
87ba0a0f4505675c6f96a53f3bc2d6add8c48960 (GPL-3.0-or-later).
This is a separate native source-port build, not a replacement for Nightdive's
sekhmet_x64.exe. Original purchased game data is required and is not included.

CHANGES
* Horizontal mouselook changes only the player's yaw. It no longer uses the
  teleport operation that cleared angular velocities and interrupted leaning
  and stance changes. Cyberspace controls retain their original path.
* Medi-patches: double regeneration at each stage, including stacked-dose
  decay and wearoff cleanup. The original taper, random expiration, fatigue
  influence, health cap and rounding remain. Total healing is not fixed.
* Plasma gun: twice the loaded base damage before charge/armor/random damage.
* Railgun: explosion damage 100 -> 200 and distance decrement 80 -> 160.
  Radius and ammunition use are unchanged. Splash damage to you also rises.
  Its direct-hit calculation, if used, receives the same base multiplier.
  Armor, difficulty, charge and rounding mean displayed damage need not be
  exactly twice on every individual hit.
* General-item and weapon inventory limits are unchanged in this build.

STEAM DECK SETUP (DESKTOP MODE)
1. Extract the archive somewhere separate, e.g. /home/deck/Games/SS-Enhanced.
   The folder containing this README must also contain systemshock, launch.sh
   and import-data.py. Do not replace the installed Nightdive executable.
2. In Steam, browse the original game's installed files to locate sshock.kpf.
3. Open a terminal in this extracted folder and run:
     python3 import-data.py "/full/path/to/sshock.kpf"
   Alternatively provide the original CD/Classic SSHOCK folder containing
   DATA and SOUND. The importer copies into res/data and res/sound with
   lowercase filenames. It does not alter the originals. HD data overrides
   matching CD-ROM data inside a KPF. If the layout is not recognized, it
   stops with a missing-files message rather than starting an incomplete game.
4. Run:
     chmod +x systemshock launch.sh
     ./launch.sh -fullscreen
5. Add launch.sh as a Non-Steam Game. Set Start In to this extracted folder.
   Leave forced Proton compatibility OFF: this is a native Linux executable.
   Set Steam Input to keyboard/mouse bindings. Useful bindings: F toggles
   mouselook, Q/E lean, T/G/B stand/crouch/prone, WASD move, C quick pickup.
   Use a trackpad/right stick as mouse and map the remaining keys as desired.

The launch script writes output to run.log in this folder. Saves/preferences
belong to this port; begin a new game. Nightdive save compatibility is not
promised. To undo, remove this Non-Steam shortcut and folder; the original
Steam installation is untouched.

BUILD FEATURES / REQUIREMENTS
Linux x86-64; glibc >= 2.34; a libstdc++ providing GLIBCXX_3.4.29; SDL2;
OpenGL runtime. SDL_mixer 2.8.0 is statically linked into the executable.
Sound effects and the built-in AdLib music synthesizer are included.
FluidSynth soundfonts and native ALSA MIDI output are disabled in this package.
No Steam Deck hardware, Gamescope, or controller playtest was available.
Use the host SteamOS runtime first; older Steam Linux Runtime containers may
provide a glibc older than this binary requires.

VALIDATION
* Release build completed; final executable has no unresolved shared libraries
  in the Linux build environment and no build-directory RPATH.
* Tests compiled the actual EDMS implementation: 1,000 yaw updates preserve
  all other physics state, including lean/stance derivatives. Invalid handles
  and non-player physics models are ignored.
* Tests compiled actual medic functions: all ten rates are twice baseline;
  single and overlapping doses return to the prior regeneration value.
* Asset importer checked with a synthetic KPF for HD/CD-ROM precedence,
  filename normalization, sound paths and traversal rejection.
* Weapon paths were reviewed in source; no in-game damage measurement yet.
* Startup reached original resource loading, but game assets were unavailable
  for this session. Running the underlying binary without assets crashes in
  the upstream resource loader; launch.sh checks for required data first.
  This is an experimental build, not a completed gameplay validation.

FIRST PLAYTEST
Start a new game. Hold Q or E and move the mouse continuously left/right;
lean should still develop. Repeat with T/G/B stance changes. Check returning
to neutral, collision near walls and toggling mouselook. For healing, test
while sufficiently injured to avoid the health cap; let the effect finish
and confirm regeneration stops. For weapons, test the same enemy/difficulty
and plasma charge level; allow for the game's normal random damage.

SOURCE / REBUILD
source/systemshock-enhanced contains the complete modified port source,
focused regression tests, and linux/build.sh. See source/changes.patch for
tracked-source changes against the commit above. Added tests/ and linux/
files are supplied in full. No proprietary game assets are included.

On a normal Debian/Ubuntu development system install build-essential, cmake,
pkg-config, libsdl2-dev, libsdl2-mixer-dev and libgl-dev, then run:
  sh source/systemshock-enhanced/linux/build.sh
That convenient rebuild uses the system SDL_mixer shared library. To reproduce
the packaged dependency arrangement, use the included SDL_mixer source:
  cmake -S source/SDL_mixer -B mixer-build -DBUILD_SHARED_LIBS=OFF \
    -DSDL2MIXER_SAMPLES=OFF -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_MOD=OFF \
    -DSDL2MIXER_MIDI=OFF -DSDL2MIXER_OPUS=OFF -DSDL2MIXER_VORBIS=OFF \
    -DSDL2MIXER_WAVPACK=OFF -DCMAKE_BUILD_TYPE=Release
  cmake --build mixer-build --parallel
The complete static-mixer build is also automated by:
  sh source/systemshock-enhanced/linux/build-static-mixer.sh
Use a fresh build directory when switching between build methods/compilers.

The delivered binary was compiled with GCC 13.3 (C), GCC 11.4 (C++), and
Ubuntu 22.04 glibc development headers to avoid newer library symbol demands.
It links dynamically to system SDL2/OpenGL/libstdc++/glibc. A rebuild on a
newer distribution may require newer runtime versions.

LICENSES
The port is GPL-3.0-or-later; see COPYING.txt and source file notices.
SDL_mixer is zlib-licensed; its source and LICENSE.txt are included.
Bundled third-party code retains its original notices and licenses.

HEX-SHOCKOLATE-ENHANCED — TEN-WEAPON LINUX BUILD (v0.3.3)

Repository: https://github.com/Hexelix/Hex-Shockolate-Enhanced
Based on MeeperTheGreat99/systemshock-enhanced, derived from Shockolate.
GPL-3.0-or-later. Purchased original game data is required and not included.

CHANGES IN THIS BUILD
* Ecology reinforcements wait a random 30-120 seconds of game time after a
  population deficit is detected (plus the scheduler's fraction-of-a-second
  rounding). Each ecology trigger has at most one pending event. More kills
  do not stack timers or continually restart an existing timer.
* When the event fires, population is counted again; a recovered population
  or removed/replaced trigger cancels that event. If placement fails or a
  deficit remains, another fresh 30-120 second timer is scheduled. Finite-use
  triggers still stop when exhausted. Enemy types, spawn regions, quotas and
  batch sizes retain their existing rules. Scripted ambushes and other
  non-ecology spawns keep their original timing.
* Pending ecology events are stored in the existing level event queue and
  survive saves. They use game time, so pausing does not spend the delay.
* General inventory: 28 slots (formerly 14), shown as two pages of 14 items.
* Weapons: ten slots (formerly seven), shown as seven weapons then three.
* Use Page Up / Page Down, scroll the wheel over the inventory, or click the
  active Weapons/General category button again to switch pages. Titles show
  1/2 or 2/2. Grenades and patches remain visible on both weapon pages.
  Selecting a weapon or general item reveals its page automatically.
  Tab / Shift+Tab and weapon-wheel cycling include all ten weapon slots.
* Medi-patches: 1.5x original regeneration (reduced from the earlier 2x).
  The initial regeneration value is 645 rather than 430. The cumulative taper
  is rounded to whole units at each stage and cleanup returns to zero.
  Original random expiration, rounding and maximum-health cap still apply.
* Berserk patches: 60 seconds of game time, unaffected by random decay or
  fatigue. Another dose refreshes the remaining duration to 60 seconds.
  Pausing does not spend game time; saving/loading preserves remaining time.
  Detox can still remove berserk early, as before.
* Retains the mouselook leaning/stance fix and 2x railgun/plasma base damage.
  Railgun splash damage to you is increased too; radius/ammo cost unchanged.

SAVE COMPATIBILITY
This ten-weapon build requires a new game. The player save layout now stores
one more weapon than v0.3.2. Saves from earlier builds, Nightdive and upstream
Shockolate are rejected before the game world or working file is changed.
Keep old saves in a separate folder.

STEAM DECK SETUP — DESKTOP MODE
1. Extract this archive into its own folder, such as
   /home/deck/Games/Hex-Shockolate-Enhanced-v0.3.3.
2. Browse the Steam Enhanced Edition installation to locate sshock.kpf.
   Open a terminal in the extracted folder and run:
     python3 import-data.py "/full/path/to/sshock.kpf"
   Alternatively provide the original CD/Classic SSHOCK directory containing
   DATA and SOUND. The importer copies files into res/data and res/sound;
   it does not modify the originals. You can also copy the res folder from
   your previous source-port setup. Start a new game; do not copy old saves.
3. Run:
     chmod +x systemshock launch.sh
     ./launch.sh -fullscreen
4. Add launch.sh as a Non-Steam Game. Set Start In to this extracted folder.
   Leave forced Proton compatibility OFF; this is a native Linux executable.
   Use Steam Input keyboard/mouse bindings: WASD movement, F mouselook,
   Q/E lean, T/G/B stand/crouch/prone, C quick pickup, Tab/Shift+Tab weapon
   cycling. Bind Page Up/Down to spare buttons for the new inventory pages.
5. Begin a new game and test before a long session.

The launcher writes run.log in this folder. If required game data is missing,
it stops with a message. The original Steam installation remains separate.

BUILD / DEPENDENCIES
Linux x86-64. Requires glibc >= 2.34, libstdc++ with GLIBCXX_3.4.29, SDL2 and
OpenGL. SDL_mixer 2.8.0 is statically linked. Built-in AdLib music and effects
are included; FluidSynth and native ALSA MIDI output are disabled. Original
SOUND assets are needed for music. The binary uses the same dependency
arrangement as the earlier package and has no build-directory RPATH.

Source is included under source/systemshock-enhanced. Standard Debian/Ubuntu
build prerequisites: build-essential, cmake, pkg-config, libsdl2-dev,
libsdl2-mixer-dev and libgl-dev. Run:
  sh source/systemshock-enhanced/linux/build.sh
This uses the system SDL_mixer shared library. To build the static SDL_mixer
arrangement supplied here, instead run:
  sh source/systemshock-enhanced/linux/build-static-mixer.sh
The included sibling source/SDL_mixer directory supplies its source.
Use a fresh build directory when switching build methods/compilers.
The delivered binary uses GCC 11.4 (C and C++) and Ubuntu 22.04
libc development headers for its runtime baseline. Rebuilding on another
system can change the required runtime versions.

VALIDATION / LIMITS
Release build completed. Focused tests compile production code and cover:
* Lean/stance physics state surviving 1,000 mouse yaw updates.
* All 1.5x medic taper stages and cleanup of overlapping doses.
* Berserk's one-minute boundary, refreshed doses, timer wraparound, and its
  exemption from random/fatigue decay.
* Encoding/decoding all 28 general slots, ten weapons, active selections,
  remaining berserk time, and the declared player-save resource size.
* Inventory page wrapping, row mapping through slot 28, selecting the final
  slots, blank rows on weapon page two, and forward/backward weapon cycling.
* Ecology delay bounds, timer deduplication and saved event bytes, restored
  populations, removed/replaced traps, delayed placement retries, finite-use
  triggers, and scheduler timestamp wraparound. The test isolates production
  ecology code with a mock spawn callback and queue insertion.
Tests are rerunnable with:
  python3 source/systemshock-enhanced/tests/run.py /absolute/path/to/build

No in-game or Steam Deck hardware playtest was available. These checks do
not replace one. First test acquiring items/weapons past the original limits,
switching pages, using/dropping the last slots, changing level, and saving/
loading them. Then time a berserk dose, save halfway through it and reload;
it should expire after the remaining game time. Test healing below the cap.
For ecology, kill a respawning enemy and wait: its ecology trigger should
wait 30-120 seconds before trying to replenish. Save partway through a wait
and reload to check the pending timer. The larger player record from v0.2
intentionally ends compatibility with the earlier seven-weapon saves.

LICENSES
The port is GPL-3.0-or-later: see COPYING.txt and source-file notices.
SDL_mixer is zlib-licensed; source/SDL_mixer/LICENSE.txt is included.
Other bundled code retains its original notices and licenses.

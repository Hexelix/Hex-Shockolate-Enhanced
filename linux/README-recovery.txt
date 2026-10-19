Hex-Shockolate-Enhanced v0.4.1 — texture-cache fixes and crash logging

CRASH INVESTIGATION
The reported v0.4.0 crash ended without a fault message; its precise cause is
not established. This build fixes reproduced OpenGL texture-cache key collisions
on 64-bit systems, owns cached bitmap pixels instead of retaining borrowed
resource/unpack pointers, and uses the converted image pitch for GPU uploads.

Common native faults now record the signal, instruction address and memory
mappings in run.log. The launcher records nonzero exit status too, including
process termination that cannot be handled inside the game. Include the complete
new run.log if a crash recurs. Keep systemshock.debug: it matches this executable
and allows the crash location to be decoded. Normal gameplay does not need it.

VIDEO OPTIONS
Open Video to find Resolution, Fullscreen, and the FOV slider.
Resolution offers four entries per page; click More for the next page.
Added: 1280x720, 1280x800 (Steam Deck native), 1600x900, 1920x1080,
1920x1200, 2560x1440, 2560x1600, and 3840x2160 (4K UHD).
The five original resolutions remain available.

FOV ranges from 70 to 135 degrees horizontally. It works with Software and
OpenGL and is saved in prefs.txt. Close the options to apply the view change.
New installations default to 1280x800 and 80-degree horizontal FOV.
Existing prefs.txt settings retain their selected resolution and FOV.

The world projection uses the chosen viewport aspect ratio. The classic HUD
and menus scale to fill the selected resolution. Original cutscenes retain
their existing scaling behavior. 4K describes render resolution, not texture
resolution. OpenGL is recommended for higher resolutions; select it under
Video > Rendering. Actual performance depends on your hardware.

In desktop fullscreen, the physical output follows the desktop/Steam display
mode; the chosen resolution controls rendering and may be scaled to that output.
For native 4K output, use a 4K display and desktop/Steam display mode.

SAVE COMPATIBILITY
v0.3.3 and v0.4.0 ten-weapon saves remain compatible. Nine-weapon and earlier saves are
incompatible and are rejected before the world or working file is changed.
All balance changes and the 28 general / 10 weapon slots are retained.

INSTALLATION
Extract the entire package into a new folder, copy your res folder, and run
./launch.sh. You can also copy v0.3.3 saves and prefs.txt. Add launch.sh to
Steam with forced Proton compatibility off. If Steam limits the available
output size, set the shortcut's Game Resolution to Native or the desired size.
For a fresh data import:
  python3 import-data.py "/full/path/to/sshock.kpf"

Source and licenses are included in source/. Purchased data is not included.
